#include "ClientSocket.h"
#include "ikcp.h"
#include "ConnectPacket.h"
#include <iostream>

namespace AsioKCP
{
	ClientSocket::ClientSocket(asio::io_service & service, bool ipv6)
		: Service(&service)
		, Socket(service)
	{
		Socket.open(ipv6 ? asio::ip::udp::v6() : asio::ip::udp::v4());
	}

	void ClientSocket::Connect(const std::string & host, uint32_t port)
	{
		if (Stage == ClientSocketStage::eNone)
		{
			asio::error_code ec;
			auto address = asio::ip::address::from_string(host);
			if (ec)
			{
				auto resolver = asio::ip::udp::resolver(*Service);
				EndPoint = *(resolver.resolve(host).begin());
			}
			else
			{
				EndPoint = asio::ip::udp::endpoint(address, port);
			}
			//ConnectionPtr = Connection::Create(weak_from_this(), 1001, EndPoint);
			DoConnect();
		}
	}

	int ClientSocket::SendMsg(const std::string & msg)
	{
		if (Stage != ClientSocketStage::eConnected || !ConnectionPtr)
			return -1;
		ConnectionPtr->SendMsg(msg);
		return 0;
	}

	int ClientSocket::SendMsg(uint32_t peerConv, const std::string & msg)
	{
		if (auto ptr = PeerConnections.Find(peerConv))
		{
			ptr->SendMsg(msg);
		}
		return -1;
	}

	void ClientSocket::Diconnect()
	{
		asio::error_code ec;
		Socket.cancel(ec);
		Socket.close(ec);
		if (ConnectionPtr)
		{
			static std::shared_ptr<std::string> msg(new std::string("server force disconnect"));
			OnEvent(ConnectionPtr->GetConv(), eEventType::eDisconnect, msg);
			ConnectionPtr.reset();
		}
	}

	void ClientSocket::AddPeer(uint32_t conv, const std::string & address, uint32_t port)
	{
		asio::ip::udp::endpoint endpoint(asio::ip::address::from_string(address), port);
		PeerConnections.Add(weak_from_this(), conv, endpoint);
	}

	void ClientSocket::RemovePeer(uint32_t conv)
	{
		PeerConnections.Remove(conv);
	}

	std::string ClientSocket::RemoteAddress()
	{
		if (ConnectionPtr)
			return ConnectionPtr->RemoteAddress();
		return std::string();
	}

	uint32_t ClientSocket::RemotePort()
	{
		if (ConnectionPtr)
			return ConnectionPtr->RemotePort();
		return 0;
	}

	std::string ClientSocket::PeerRemoteAddress(uint32_t conv)
	{
		if (auto ptr = PeerConnections.Find(conv))
		{
			return ptr->RemoteAddress();
		}
		return std::string();
	}

	uint32_t ClientSocket::PreerRemotePort(uint32_t conv)
	{
		if (auto ptr = PeerConnections.Find(conv))
		{
			return ptr->RemotePort();
		}
		return 0;
	}

	void ClientSocket::SendPackage(const char * data, size_t len, const asio::ip::udp::endpoint & endpoint)
	{
// 		Socket.async_send_to(asio::buffer(data, len), EndPoint,
// 			std::bind(&ClientSocket::HandleUdpSend, shared_from_this(),
// 				std::placeholders::_1,
// 				std::placeholders::_2));
		Socket.send_to(asio::buffer(data, len), EndPoint);
	}

	void ClientSocket::OnUpdate()
	{
		if (Stage == ClientSocketStage::eConnecting)
		{
			if (NextReConnectTime != -1 && ConnectStartClock != -1)
			{
				//5 second timeout
				if (Clock - ConnectStartClock > 5000)
				{
					Stage = ClientSocketStage::eNone;
					ConnectStartClock = 0;
					NextReConnectTime = 0;
					std::stringstream stream;
					stream << "Connect Faile -> " << EndPoint.address().to_string() << " : "<<EndPoint.port();
					std::shared_ptr<std::string> msg = std::make_shared<std::string>(stream.str());
					OnEvent(0, eEventType::eConnectFaile, msg);
				}
				else if(NextReConnectTime < Clock)
				{
					asio::error_code ec;
					Socket.cancel(ec);
					DoConnect();
				}
			}
			return;
		}
		if (ConnectionPtr)
		{
			ConnectionPtr->Update(Clock);
			if (ConnectionPtr->IsTimeout())
			{
				ConnectionPtr->DoTimeout();
				ConnectionPtr.reset();
				Stage = ClientSocketStage::eNone;
			}
		}
		PeerConnections.Update(Clock);
	}

	void ClientSocket::HandleUdpSend(const std::error_code & error, size_t bytes_recvd)
	{
	}

	void ClientSocket::HandleUdpReceiveFrom(const std::error_code & error, size_t bytes_recvd)
	{
		do
		{
			if (error || bytes_recvd <= 0)
			{
				printf("\nhandle_udp_receive_from error end! error: %s, bytes_recvd: %ld\n", error.message().c_str(), bytes_recvd);
				break;
			}
			if (Stage == ClientSocketStage::eConnecting)
			{
				if (AsioKCP::is_send_back_conv_packet(udp_data_, bytes_recvd))
				{
					Stage = ClientSocketStage::eConnected;
					uint32_t conv = grab_conv_from_send_back_conv_packet(udp_data_, bytes_recvd);
					ConnectionPtr = Connection::Create(weak_from_this(), conv, EndPoint);
					static std::shared_ptr<std::string> msg = std::make_shared<std::string>("Connect Server Success");
					OnEvent(ConnectionPtr->GetConv(), eEventType::eConnect, msg);
				}
				break;
			}

			HandleKcpPacket(bytes_recvd);
		} while (false);
		HookUdpAsyncReceive();
	}

	void ClientSocket::HookUdpAsyncReceive(void)
	{
		Socket.async_receive_from(
			asio::buffer(udp_data_, sizeof(udp_data_)), EndPoint,
			std::bind(&ClientSocket::HandleUdpReceiveFrom, shared_from_this(),
				std::placeholders::_1,
				std::placeholders::_2));
	}
	void ClientSocket::HandleKcpPacket(size_t bytes_recvd)
	{
		IUINT32 conv;
		int ret = ikcp_get_conv(udp_data_, bytes_recvd, &conv);
		if (ret == 0)
		{
			return;
		}
		if (ConnectionPtr && ConnectionPtr->GetConv() == conv)
		{
			ConnectionPtr->Input(udp_data_, bytes_recvd, EndPoint);
		}
		else
		{
			if (auto ptr = PeerConnections.Find(conv))
			{
				ptr->Input(udp_data_, bytes_recvd, EndPoint);
			}
			else
			{
				std::cout << "connection not exist with conv: " << conv << " " << Socket.local_endpoint().port() << std::endl;
			}
		}
	}
	void ClientSocket::DoConnect()
	{
		std::string connectMsg = making_connect_packet();
		SendPackage(connectMsg.data(), connectMsg.length(), EndPoint);
		Stage = ClientSocketStage::eConnecting;
		if (ConnectStartClock == 0)
			ConnectStartClock = Clock;
		NextReConnectTime = Clock + 1000;
		HookUdpAsyncReceive();
	}
}