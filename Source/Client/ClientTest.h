#pragma once
#include "AsioService.h"
class ClientTest
{
public:
	ClientTest(AsioKCP::ClientPtr& client);

private:
	void EventCallBack(uint32_t conv, AsioKCP::eEventType event_type, std::shared_ptr<std::string>& msg);

private:
	AsioKCP::ClientPtr Client;
};
