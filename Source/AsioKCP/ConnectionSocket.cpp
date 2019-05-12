#include "ConnectionSocket.h"
#include "Connection.h"
#include "ConnectPacket.hpp"
#include <iostream>
#include "ikcp.h"
namespace asio_kcp 
{
	ConnectionSocket::ConnectionSocket(IConnectionMgr*mgr, asio::io_service& service, const std::string & address, int udp_port)
		:ConnectionMgr(mgr) 
		,Socket(service, asio::ip::udp::endpoint(asio::ip::address::from_string(address), udp_port))
	{
		HookUdpAsyncReceive();
	}


	void ConnectionSocket::Update(uint64_t clock)
	{
		Clock = clock;
	}

	void ConnectionSocket::Stop()
	{
		Stopped = true;
		Socket.cancel();
		Socket.close();
	}


	void ConnectionSocket::SetCallback(const std::function<event_callback_t>& func)
	{
		EventCallback = func;
	}

	void ConnectionSocket::OnEvent(uint32_t conv, eEventType event_type, std::shared_ptr<std::string> msg)
	{
		EventCallback(conv, event_type, msg);
	}

	void ConnectionSocket::SendPackage(const char*data, size_t len, const asio::ip::udp::endpoint & endpoint)
	{
		Socket.async_send_to(asio::buffer(data, len), EndPoint,
			std::bind(&ConnectionSocket::HandleUdpSend, shared_from_this(),
				std::placeholders::_1,
				std::placeholders::_2));
		//Socket.send_to(asio::buffer(data, len), endpoint);
	}

	void ConnectionSocket::HandleUdpReceiveFrom(const std::error_code & error, size_t bytes_recvd)
	{
		do 
		{
			if (error || bytes_recvd <=0)
			{
				printf("\nhandle_udp_receive_from error end! error: %s, bytes_recvd: %ld\n", error.message().c_str(), bytes_recvd);
				break;
			}
			if (asio_kcp::is_connect_packet(udp_data_, bytes_recvd))
			{
				HandleConnectPacket();
				break;
			}

			HandleKcpPacket(bytes_recvd);
		} while (false);
		HookUdpAsyncReceive();
	}

	void ConnectionSocket::HandleUdpSend(const std::error_code & error, size_t bytes_recvd)
	{
	}

	void ConnectionSocket::HookUdpAsyncReceive(void)
	{
		if (Stopped)
			return;
		Socket.async_receive_from(
			asio::buffer(udp_data_, sizeof(udp_data_)), EndPoint,
			std::bind(&ConnectionSocket::HandleUdpReceiveFrom, shared_from_this(),
				std::placeholders::_1,
				std::placeholders::_2));
	}


	void ConnectionSocket::HandleConnectPacket()
	{
		if (ConnectionMgr != nullptr)
		{
			uint32_t conv = ConnectionMgr->GetNewConv();
			static std::string send_back_msg = asio_kcp::making_send_back_conv_packet(conv);
			auto connect = ConnectionMgr->Add(shared_from_this(), conv, EndPoint);
			connect->SendMsg(send_back_msg);
		}
	}

	void ConnectionSocket::HandleKcpPacket(size_t bytes_recvd)
	{
		if (ConnectionMgr == nullptr)
			return;

		IUINT32 conv;
		int ret = ikcp_get_conv(udp_data_, bytes_recvd, &conv);
		if (ret == 0)
		{
			throw std::exception("ikcp_get_conv return 0");
		}

		auto conn_ptr = ConnectionMgr->Find(conv);
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
