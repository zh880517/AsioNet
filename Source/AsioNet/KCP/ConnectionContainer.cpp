#include "ConnectionContainer.h"

namespace AsioKCP 
{

	ConnectionContainer::ConnectionContainer()
	{
	}

	std::shared_ptr<Connection>& ConnectionContainer::Find(uint32_t conv)
	{
		auto iter = Connections.find(conv);
		if (iter == Connections.end())
		{
			static std::shared_ptr<Connection> none;
			return none;
		}
		else
			return iter->second;
	}

	std::shared_ptr<Connection> ConnectionContainer::Add(std::weak_ptr<ConnectionSocket> manager_ptr, const uint32_t & conv, const asio::ip::udp::endpoint& udp_sender_endpoint)
	{
		auto ptr = Connection::Create(manager_ptr, conv, udp_sender_endpoint);
		Connections[conv] = ptr;
		return ptr;
	}

	uint32_t ConnectionContainer::GetNewConv()
	{
		while (Connections.find(CurrConv) != Connections.end())
		{
			if (++CurrConv == 0)
				CurrConv = 1001;
		}
		return CurrConv;
	}

	void ConnectionContainer::Update(uint64_t clock)
	{
		for (auto iter = Connections.begin(); iter != Connections.end();)
		{
			auto& ptr = iter->second;

			ptr->Update(clock);

			// check timeout
			if (ptr->IsTimeout())
			{
				ptr->DoTimeout();
				Connections.erase(iter++);
				continue;
			}

			iter++;
		}
	}

	void ConnectionContainer::StopAll()
	{
		Connections.clear();
	}

	void ConnectionContainer::Remove(uint32_t conv)
	{
		Connections.erase(conv);
	}

}

