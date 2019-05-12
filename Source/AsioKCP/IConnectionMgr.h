#pragma once
#include "Connection.h"

namespace asio_kcp 
{
	class IConnectionMgr
	{
	public:
		IConnectionMgr() {}
		virtual ~IConnectionMgr() {}
		virtual std::shared_ptr<Connection>& Find(uint32_t conv) = 0;
		virtual std::shared_ptr<Connection> Add(std::weak_ptr<ConnectionSocket> manager_ptr,
			const uint32_t& conv, const asio::ip::udp::endpoint& udp_sender_endpoint) = 0;
		virtual uint32_t GetNewConv() { return 1001; }
	};

}
