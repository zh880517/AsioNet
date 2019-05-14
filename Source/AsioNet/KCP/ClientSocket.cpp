#include "ClientSocket.h"
#include "ikcp.h"
#include "ConnectPacket.h"
#include <iostream>

namespace asio_kcp
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
			auto resolver = asio::ip::udp::resolver(*Service);
			EndPoint = *(resolver.resolve(host).begin());
			//ConnectionPtr = Connection::Create(weak_from_this(), 1001, EndPoint);
			DoConnect();
		}
	}

	int ClientSocket::SendMsg(const std::string & msg)
	{
		if (!Connected || !ConnectionPtr)
			return -1;
		ConnectionPtr->SendMsg(msg);
		return 0;
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
			if (NextReConnectTime > 0 && ConnectStartClock > 0)
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
				Connected = false;
			}
		}
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
				if (asio_kcp::is_send_back_conv_packet(udp_data_, bytes_recvd))
				{
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
		if (ConnectionPtr)
			return;
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
		if (!ConnectionPtr || ConnectionPtr->GetConv() != conv)
		{
			std::cout << "connection not exist with conv: " << conv << std::endl;
			return;
		}
		ConnectionPtr->Input(udp_data_, bytes_recvd, EndPoint);
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