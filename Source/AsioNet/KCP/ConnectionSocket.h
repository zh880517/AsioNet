#pragma once
#include <functional>
#include "kcpTypedef.h"
#include "ConnectionContainer.h"

namespace AsioKCP 
{
	class ConnectionSocket
	{
	public:
		virtual ~ConnectionSocket() {}

		void Update(uint64_t clock);

		void SetCallback(const std::function<event_callback_t>& func);

		// this func should be multithread safe if running UdpPacketHandler in work thread pool.  can implement by io_service.dispatch
		void OnEvent(uint32_t conv, eEventType event_type, std::shared_ptr<std::string> msg);
		// this func should be multithread safe if running UdpPacketHandler in work thread pool.  can implement by io_service.dispatch
		virtual void SendPackage(const char*data, size_t len, const asio::ip::udp::endpoint& endpoint) = 0;

		uint64_t GetClock(void) const { return Clock; }
	protected:

		virtual void OnUpdate() = 0;

	protected:
		std::function<event_callback_t> EventCallback;
		char udp_data_[1024 * 32] = {0};
		uint64_t Clock = 0;
		asio::ip::udp::endpoint EndPoint;
	};
}

