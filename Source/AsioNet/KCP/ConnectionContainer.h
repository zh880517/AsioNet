#pragma once
#include <unordered_map>
#include "Connection.h"

namespace AsioKCP 
{
	class ConnectionContainer
	{
	public:
		ConnectionContainer();

		std::shared_ptr<Connection>& Find(uint32_t conv);

		std::shared_ptr<Connection> Add(std::weak_ptr<ConnectionSocket> manager_ptr,
			const uint32_t& conv, const asio::ip::udp::endpoint& udp_sender_endpoint);

		uint32_t GetNewConv();

		void Update(uint64_t clock);

		void StopAll();

		void Remove(uint32_t conv);

	private:
		uint32_t CurrConv = 1001;
		std::unordered_map<uint32_t, std::shared_ptr<Connection>> Connections;
	};

}

