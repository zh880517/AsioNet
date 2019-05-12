#pragma once
#include <memory>
#include <string>
namespace asio_kcp
{
	enum eEventType
	{
		eConnect,
		eDisconnect,
		eRcvMsg,
		eLagNotify,
		eTimeout,

		eCountOfEventType
	};


	typedef void(event_callback_t)(uint32_t /*conv*/, eEventType /*event_type*/, std::shared_ptr<std::string> /*msg*/);

	inline const char* eventTypeStr(eEventType eventType)
	{
		switch (eventType)
		{
		case eConnect: return "eConnect";
		case eDisconnect: return "eDisconnect";
		case eRcvMsg: return "eRcvMsg";
		case eLagNotify: return "eLagNotify";
		default: return "unknown";
		}
	}

}