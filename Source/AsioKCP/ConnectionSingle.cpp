#include "ConnectionSingle.h"

namespace asio_kcp 
{

	std::shared_ptr<Connection>& ConnectionSingle::Find(uint32_t conv)
	{
		return Connect;
	}

	std::shared_ptr<Connection> ConnectionSingle::Add(std::weak_ptr<ConnectionSocket> manager_ptr, const uint32_t & conv, const asio::ip::udp::endpoint & udp_sender_endpoint)
	{
		Connect = Connection::Create(manager_ptr, conv, udp_sender_endpoint);
		return Connect;
	}


}
