#pragma once
#include <memory>
#include <string>
namespace AsioKCP
{
	enum class eEventType 
	{
		eConnect,
		eDisconnect,
		eRcvMsg,
		eLagNotify,
		eTimeout,
		eConnectFaile,

	};

	typedef void(event_callback_t)(uint32_t /*conv*/, eEventType /*event_type*/, std::shared_ptr<std::string>& /*msg*/);

	inline const char* eventTypeStr(eEventType eventType)
	{
		switch (eventType)
		{
		case eEventType::eConnect: return "eConnect";
		case eEventType::eDisconnect: return "eDisconnect";
		case eEventType::eRcvMsg: return "eRcvMsg";
		case eEventType::eLagNotify: return "eLagNotify";
		case eEventType::eTimeout: return "eTimeout";
		case eEventType::eConnectFaile: return "eConnectFaile";
		default: return "unknown";
		}
	}

}