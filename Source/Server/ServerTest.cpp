#include "ServerTest.h"
#include <iostream>

ServerTest::ServerTest(AsioKCP::ServerPtr & server)
	:Server(server)
{
	server->SetCallback(
		std::bind(&ServerTest::EventCallBack, this,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3));
}

void ServerTest::EventCallBack(uint32_t conv, AsioKCP::eEventType event_type, std::shared_ptr<std::string>& msg)
{
	std::cout << AsioKCP::eventTypeStr(event_type) << " conv : " << conv << " msg:" << *msg << std::endl;
	switch (event_type)
	{
	case AsioKCP::eEventType::eConnect:
	{
		std::string msg1("hello world");
		Server->SendMsg(conv, msg1);
	}
		break;
	case AsioKCP::eEventType::eDisconnect:
		break;
	case AsioKCP::eEventType::eRcvMsg:
	{
		std::string msg2("pong");
		Server->SendMsg(conv, msg2);
	}
		break;
	case AsioKCP::eEventType::eLagNotify:
		break;
	case AsioKCP::eEventType::eTimeout:
		break;
	case AsioKCP::eEventType::eConnectFaile:
		break;
	default:
		break;
	}
}
