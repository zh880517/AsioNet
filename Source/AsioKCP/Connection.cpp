#include "Connection.h"
#include "ConnectPacket.hpp"
#include "ConnectionSocket.h"
#include "kcpTypedef.h"
#include <iostream>
#include "ikcp.h"

namespace asio_kcp 
{
	Connection::Connection(const std::weak_ptr<ConnectionSocket>& manager_ptr)
		:Manager(manager_ptr)
	{
		StartClock = GetClock();
	}

	Connection::~Connection()
	{
		Clean();
	}

	std::shared_ptr<Connection> Connection::Create(const std::weak_ptr<ConnectionSocket>& manager_ptr, const uint32_t & conv, const asio::ip::udp::endpoint & udp_remote_endpoint)
	{
		std::shared_ptr<Connection> ptr = std::make_shared<Connection>(manager_ptr);
		if (ptr)
		{
			ptr->Init(conv);
			ptr->SetUdpRemoteEndpoint(udp_remote_endpoint);
		}
		return ptr;
	}

	void Connection::SetUdpRemoteEndpoint(const asio::ip::udp::endpoint & udp_remote_endpoint)
	{
		RemoteEndpoint = udp_remote_endpoint;
	}

	void Connection::Update(uint64_t clock)
	{
		ikcp_update(Kcp, static_cast<uint32_t>(clock - StartClock));
	}

	void Connection::Input(char * udp_data, size_t bytes_recvd, const asio::ip::udp::endpoint & udp_remote_endpoint)
	{
		LastPacketRecvTime = GetClock();
		RemoteEndpoint = udp_remote_endpoint;

		ikcp_input(Kcp, udp_data, bytes_recvd);
		{
			char kcp_buf[1024 * 1000] = "";
			int kcp_recvd_bytes = ikcp_recv(Kcp, kcp_buf, sizeof(kcp_buf));
			if (kcp_recvd_bytes > 0)
			{
				std::shared_ptr<std::string> package = std::make_shared<std::string>(kcp_buf, kcp_recvd_bytes);
				if (auto ptr = Manager.lock())
				{
					ptr->OnEvent(Conv, eRcvMsg, package);
				}
			}
		}
	}

	bool Connection::IsTimeout() const
	{
		if (LastPacketRecvTime == 0)
			return false;

		return GetClock() - LastPacketRecvTime > GetTimeoutTime();
	}

	void Connection::DoTimeout()
	{
		if (auto ptr = Manager.lock())
		{
			std::shared_ptr<std::string> msg(new std::string("timeout"));
			ptr->OnEvent(Conv, eEventType::eTimeout, msg);
		}
	}

	void Connection::SendMsg(const std::string & msg)
	{
		int send_ret = ikcp_send(Kcp, msg.data(), msg.size());
		if (send_ret < 0)
		{
			std::cout << "send_ret<0: " << send_ret << std::endl;
		}
	}

	void Connection::Init(const uint32_t & conv)
	{
		Conv = conv;
		Kcp = ikcp_create(conv, (void*)this);
		Kcp->output = &Connection::UdpOutput;
		// 启动快速模式
		// 第二个参数 nodelay-启用以后若干常规加速将启动
		// 第三个参数 interval为内部处理时钟，默认设置为 10ms
		// 第四个参数 resend为快速重传指标，设置为2
		// 第五个参数 为是否禁用常规流控，这里禁止
		//ikcp_nodelay(p_kcp_, 1, 10, 2, 1);
		ikcp_nodelay(Kcp, 1, 5, 1, 1); // 设置成1次ACK跨越直接重传, 这样反应速度会更快. 内部时钟5毫秒.
	}

	void Connection::Clean()
	{
		std::string disconnect_msg = asio_kcp::making_disconnect_packet(Conv);
		SendPackage(disconnect_msg.c_str(), disconnect_msg.size());
		ikcp_release(Kcp);
		Kcp = nullptr;
		Conv = 0;
	}

	int Connection::UdpOutput(const char * buf, int len, ikcpcb * kcp, void * user)
	{
		((Connection*)user)->SendPackage(buf, len);
		return 0;
	}

	void Connection::SendPackage(const char * buf, int len)
	{
		if (auto ptr = Manager.lock())
		{
			ptr->SendPackage(buf, len, RemoteEndpoint);
		}
	}

	uint64_t Connection::GetClock() const
	{
		if (auto ptr = Manager.lock())
		{
			return ptr->GetClock();
		}
		return 0;
	}

	uint32_t Connection::GetTimeoutTime() const
	{
		return 10000;
	}
}


