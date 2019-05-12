#include "Server.h"
#include "ConnectionSocket.h"
#include "ConnectionContainer.h"

namespace asio_kcp 
{
	Server::Server(asio::io_service& service, const std::string & address, const uint32_t port)
		:Connections(std::make_shared<ConnectionContainer>())
		,Socket(std::make_shared<ConnectionSocket>(Connections.get(),service, address, port))
	{
		
	}

	void Server::Update(uint64_t clock)
	{
		Connections->Update(clock);
	}

	void Server::SetCallback(const std::function<event_callback_t>& func)
	{
		Socket->SetCallback(func);
	}

	int Server::SendMsg(uint32_t conv, const std::string& msg)
	{
		auto connection_ptr = Connections->Find(conv);
		if (!connection_ptr)
			return -1;
		connection_ptr->SendMsg(msg);
		return 0;
	}

	void Server::ForceDisconnect(uint32_t conv)
	{
		if (!Connections->Find(conv))
			return;
		std::shared_ptr<std::string> msg(new std::string("server force disconnect"));
		Socket->OnEvent(conv, eEventType::eDisconnect, msg);
		Connections->Remove(conv);
	}

	void Server::Stop()
	{
		Connections->StopAll();
		Socket->Stop();
	}

	std::string Server::RemoteAddress(uint32_t conv)
	{
		if (auto connection = Connections->Find(conv))
		{
			return connection->RemoteAddress();
		}
		return std::string();
	}

	uint32_t Server::RemotePort(uint32_t conv)
	{
		if (auto connection = Connections->Find(conv))
		{
			return connection->RemotePort();
		}
		return 0;
	}

}

