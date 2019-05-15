#include "Client.h"
#include "ClientSocket.h"

namespace AsioKCP
{
	Client::Client(asio::io_service & service, bool ipv6)
		: Socket(std::make_shared<ClientSocket>(service, ipv6))
	{
	}

	void Client::Connect(const std::string & host, uint32_t port)
	{
		Socket->Connect(host, port);
	}

	void Client::Update(int64_t clock)
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
	int Client::SendMsg(uint32_t peerConv, const std::string & msg)
	{
		return Socket->SendMsg(peerConv, msg);
	}
	void Client::AddPeer(uint32_t conv, const std::string & address, uint32_t port)
	{
		Socket->AddPeer(conv, address, port);
	}
	void Client::RemovePeer(uint32_t conv)
	{
		Socket->RemovePeer(conv);
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
	std::string Client::PeerRemoteAddress(uint32_t conv)
	{
		return Socket->PeerRemoteAddress(conv);
	}
	uint32_t Client::PreerRemotePort(uint32_t conv)
	{
		return Socket->PreerRemotePort(conv);
	}
}