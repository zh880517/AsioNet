#include "ConnectionSocket.h"
#include "Connection.h"
#include "ConnectPacket.h"
#include <iostream>
#include "ikcp.h"
namespace AsioKCP 
{
	void ConnectionSocket::Update(uint64_t clock)
	{
		Clock = clock;
		OnUpdate();
	}

	void ConnectionSocket::SetCallback(const std::function<event_callback_t>& func)
	{
		EventCallback = func;
	}

	void ConnectionSocket::OnEvent(uint32_t conv, eEventType event_type, std::shared_ptr<std::string> msg)
	{
		EventCallback(conv, event_type, msg);
	}

}
