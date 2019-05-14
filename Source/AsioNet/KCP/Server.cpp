#include "Server.h"
#include "ServerSocket.h"
namespace AsioKCP 
{
	Server::Server(asio::io_service& service, const std::string & address, const uint32_t port)
		:Socket(std::make_shared<ServerSocket>(service, address, port))
		,Port(port)
	{
	}

	void Server::Update(uint64_t clock)
	{
		Socket->Update(clock);
	}

	void Server::SetCallback(const std::function<event_callback_t>& func)
	{
		Socket->SetCallback(func);
	}

	int Server::SendMsg(uint32_t conv, const std::string& msg)
	{
		return Socket->SendMsg(conv, msg);
	}

	void Server::ForceDisconnect(uint32_t conv)
	{
		Socket->ForceDisconnect(conv);
	}

	void Server::Stop()
	{
		Socket->Stop();
	}

	std::string Server::RemoteAddress(uint32_t conv)
	{
		return Socket->RemoteAddress(conv);
	}

	uint32_t Server::RemotePort(uint32_t conv)
	{
		return Socket->RemotePort(conv);
	}

}

