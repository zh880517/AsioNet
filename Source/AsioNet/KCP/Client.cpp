#include "Client.h"
#include "ClientSocket.h"

namespace AsioKCP
{
	Client::Client(asio::io_service & service, bool ipv6)
		: Socket(std::make_shared<ClientSocket>(service, ipv6))
	{
	}

	void Client::Update(uint64_t clock)
	{
		Socket->Update(clock);
	}
	void Client::SetCallback(const std::function<event_callback_t>& func)
	{
		Socket->SetCallback(func);
	}
	int Client::SendMsg(const std::string & msg)
	{
		return Socket->SendMsg(msg);
	}
	void Client::Diconnect()
	{
		Socket->Diconnect();
	}
	std::string Client::RemoteAddress()
	{
		return Socket->RemoteAddress();
	}
	uint32_t Client::RemotePort()
	{
		return Socket->RemotePort();
	}
}