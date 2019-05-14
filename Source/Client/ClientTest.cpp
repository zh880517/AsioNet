#include "ClientTest.h"
#include <iostream>

ClientTest::ClientTest(AsioKCP::ClientPtr & client)
	:Client(client)
{
	client->SetCallback(
		std::bind(&ClientTest::EventCallBack, this, 
			std::placeholders::_1, 
			std::placeholders::_2, 
			std::placeholders::_3));
	client->Connect("127.0.0.1", 4567);
}

void ClientTest::EventCallBack(uint32_t conv, AsioKCP::eEventType event_type, std::shared_ptr<std::string>& msg)
{
	std::string pingMsg("ping");
	std::cout << AsioKCP::eventTypeStr(event_type) << " conv : " << conv << " msg:" << *msg << std::endl;
	switch (event_type)
	{
	case AsioKCP::eEventType::eConnect:
		Client->SendMsg(pingMsg);
		break;
	case AsioKCP::eEventType::eDisconnect:
		break;
	case AsioKCP::eEventType::eRcvMsg:
		Client->SendMsg(pingMsg);
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
