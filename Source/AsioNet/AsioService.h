#pragma once
#include <memory>
#include <map>
#include "KCP/Client.h"
#include "KCP/Server.h"

namespace AsioNet
{
	class AsioService
	{
	public:
		AsioService();

		void Update();

		std::shared_ptr<AsioKCP::Server> CreateKCPServer(const std::string& address, const uint32_t port);

		std::shared_ptr<AsioKCP::Client> CreateKCPClent();

		void DestoryServer(const std::shared_ptr<AsioKCP::Server>& server);

		void DestoryClient(const std::shared_ptr<AsioKCP::Client>& client);

	private:
		std::shared_ptr<asio::io_service> Service;
		std::map<uint32_t, std::shared_ptr<AsioKCP::Server>> Servers;
		std::map<AsioKCP::Client*, std::shared_ptr<AsioKCP::Client>> Clients;
	};
	
}
