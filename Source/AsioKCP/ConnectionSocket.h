#pragma once
#include <functional>
#include "kcpTypedef.h"
#include "ConnectionContainer.h"

namespace asio_kcp 
{
	class ConnectionSocket : public std::enable_shared_from_this<ConnectionSocket>
	{
	public:
		ConnectionSocket(IConnectionMgr*mgr, asio::io_service& service, const std::string& address, int udp_port);

		void SetConnectionMgr(IConnectionMgr* mgr) { ConnectionMgr = mgr; }

		void Update(uint64_t clock);

		void Stop();
		void SetCallback(const std::function<event_callback_t>& func);

		// this func should be multithread safe if running UdpPacketHandler in work thread pool.  can implement by io_service.dispatch
		void OnEvent(uint32_t conv, eEventType event_type, std::shared_ptr<std::string> msg);
		// this func should be multithread safe if running UdpPacketHandler in work thread pool.  can implement by io_service.dispatch
		void SendPackage(const char*data, size_t len, const asio::ip::udp::endpoint& endpoint);

		uint64_t GetClock(void) const { return Clock; }

	private:
		void HandleUdpReceiveFrom(const std::error_code& error, size_t bytes_recvd);
		void HandleUdpSend(const std::error_code& error, size_t bytes_recvd);
		void HookUdpAsyncReceive(void);

		void HandleConnectPacket();
		void HandleKcpPacket(size_t bytes_recvd);

	private:
		bool Stopped = false;
		std::function<event_callback_t> EventCallback;
		char udp_data_[1024 * 32] = {0};
		uint64_t Clock = 0;
		IConnectionMgr* ConnectionMgr = nullptr;

		asio::ip::udp::socket Socket;
		asio::ip::udp::endpoint EndPoint;
	};
}

