#pragma once
#include "AsioService.h"
class ServerTest
{
public:
	ServerTest(AsioKCP::ServerPtr& server);


private:
	void EventCallBack(uint32_t conv, AsioKCP::eEventType event_type, std::shared_ptr<std::string>& msg);
private:
	AsioKCP::ServerPtr Server;
};
