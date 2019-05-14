#include "ServerSocket.h"
#include "ConnectPacket.h"
#include "ikcp.h"
#include <iostream>

namespace AsioKCP
{
	ServerSocket::ServerSocket(asio::io_service & service, const std::string & address, int udp_port)
		: Socket(service, asio::ip::udp::endpoint(asio::ip::address::from_string(address), udp_port))
	{
		
	}
	void ServerSocket::Stop()
	{
		Stopped = true;
		Socket.cancel();
		Socket.close();
		Connections.StopAll();
	}

	void ServerSocket::Start()
	{
		HookUdpAsyncReceive();
	}

	int ServerSocket::SendMsg(uint32_t conv, const std::string & msg)
	{
		auto connection_ptr = Connections.Find(conv);
		if (!connection_ptr)
			return -1;
		connection_ptr->SendMsg(msg);
		return 0;
	}

	void ServerSocket::ForceDisconnect(uint32_t conv)
	{
		if (!Connections.Find(conv))
			return;
		static std::shared_ptr<std::string> msg(new std::string("server force disconnect"));
		OnEvent(conv, eEventType::eDisconnect, msg);
		Connections.Remove(conv);
	}

	std::string ServerSocket::RemoteAddress(uint32_t conv)
	{
		if (auto connection = Connections.Find(conv))
		{
			return connection->RemoteAddress();
		}
		return std::string();
	}

	uint32_t ServerSocket::RemotePort(uint32_t conv)
	{
		if (auto connection = Connections.Find(conv))
		{
			return connection->RemotePort();
		}
		return 0;
	}

	void ServerSocket::SendPackage(const char * data, size_t len, const asio::ip::udp::endpoint & endpoint)
	{
// 		Socket.async_send_to(asio::buffer(data, len), EndPoint,
// 			std::bind(&ServerSocket::HandleUdpSend, shared_from_this(),
// 				std::placeholders::_1,
// 				std::placeholders::_2));
		Socket.send_to(asio::buffer(data, len), EndPoint);
	}

	void ServerSocket::OnUpdate()
	{
		Connections.Update(Clock);
	}

	void ServerSocket::HandleUdpReceiveFrom(const std::error_code & error, size_t bytes_recvd)
	{
		do
		{
			if (error || bytes_recvd <= 0)
			{
				printf("\nhandle_udp_receive_from error end! error: %s, bytes_recvd: %ld\n", error.message().c_str(), bytes_recvd);
				break;
			}
			if (AsioKCP::is_connect_packet(udp_data_, bytes_recvd))
			{
				HandleConnectPacket();
				break;
			}

			HandleKcpPacket(bytes_recvd);
		} while (false);
		HookUdpAsyncReceive();
	}

	void ServerSocket::HandleUdpSend(const std::error_code & error, size_t bytes_recvd)
	{
	}

	void ServerSocket::HookUdpAsyncReceive(void)
	{
		if (Stopped)
			return;
		auto ptr = shared_from_this();
		Socket.async_receive_from(
			asio::buffer(udp_data_, sizeof(udp_data_)), EndPoint,
			std::bind(&ServerSocket::HandleUdpReceiveFrom, shared_from_this(),
				std::placeholders::_1,
				std::placeholders::_2));
	}

	void ServerSocket::HandleConnectPacket()
	{
		uint32_t conv = Connections.GetNewConv();
		std::string send_back_msg = AsioKCP::making_send_back_conv_packet(conv);
		auto connect = Connections.Add(shared_from_this(), conv, EndPoint);
		SendPackage(send_back_msg.data(), send_back_msg.length(), EndPoint);
	}

	void ServerSocket::HandleKcpPacket(size_t bytes_recvd)
	{
		IUINT32 conv;
		int ret = ikcp_get_conv(udp_data_, bytes_recvd, &conv);
		if (ret == 0)
		{
			return;
		}

		auto conn_ptr = Connections.Find(conv);
		if (!conn_ptr)
		{
			std::cout << "connection not exist with conv: " << conv << std::endl;
			return;
		}

		if (conn_ptr)
			conn_ptr->Input(udp_data_, bytes_recvd, EndPoint);
		else
			std::cout << "add_new_connection failed! can not connect!" << std::endl;
	}
}