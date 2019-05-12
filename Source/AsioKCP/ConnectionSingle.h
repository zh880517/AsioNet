#pragma once
#include "IConnectionMgr.h"
namespace asio_kcp
{
	class ConnectionSingle : public IConnectionMgr , private asio::detail::noncopyable
	{
	public:

		virtual std::shared_ptr<Connection>& Find(uint32_t conv) override;

		virtual std::shared_ptr<Connection> Add(std::weak_ptr<ConnectionSocket> manager_ptr, const uint32_t & conv, const asio::ip::udp::endpoint & udp_sender_endpoint) override;

		std::shared_ptr<Connection>& GetConnection() { return Connect; }
	private:
		std::shared_ptr<Connection> Connect;
	};

}
