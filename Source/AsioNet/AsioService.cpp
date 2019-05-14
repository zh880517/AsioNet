#include "AsioService.h"
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#include "asio.hpp"
#include <chrono>
namespace AsioNet
{

	AsioService::AsioService()
		: Service(std::make_shared<asio::io_service>())
	{

	}

	void AsioService::Update()
	{
		uint64_t clock = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
		for (auto& kv : Servers)
		{
			kv.second->Update(clock);
		}
		for (auto& kv : Clients)
		{
			kv.second->Update(clock);
		}
	}

	std::shared_ptr<AsioKCP::Server> AsioService::CreateKCPServer(const std::string & address, const uint32_t port)
	{
		auto it = Servers.find(port);
		if (it != Servers.end())
			return it->second;
		auto server = std::make_shared<AsioKCP::Server>(*(Service.get()), address, port);
		Servers.insert(std::make_pair(port, server));
		return server;
	}

	std::shared_ptr<AsioKCP::Client> AsioService::CreateKCPClent()
	{
		std::shared_ptr<AsioKCP::Client> client = std::make_shared<AsioKCP::Client>(*(Service.get()));
		Clients.insert(std::make_pair(client.get(), client));
		return client;
	}

	void AsioService::DestoryServer(const std::shared_ptr<AsioKCP::Server>& server)
	{
		if (server)
		{
			Servers.erase(server->GetPort());
		}
	}

	void AsioService::DestoryClient(const std::shared_ptr<AsioKCP::Client>& client)
	{
		Clients.erase(client.get());
	}

}