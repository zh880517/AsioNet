#pragma once
#include <memory>
#include <functional>
#include "kcpTypedef.h"
namespace asio {
	class io_context;
	typedef io_context io_service;
}

namespace asio_kcp
{
	class ConnectionSocket;
	class ConnectionSingle;

	class Client
	{
	public:
		Client(asio::io_service& service);
		Client(const Client&) = delete;
		const Client& operator=(const Client&) = delete;

		void Update(uint64_t clock);
		void SetCallback(const std::function<event_callback_t>& func);
		int SendMsg(const std::string& msg);
		void Stop();

		std::string RemoteAddress(uint32_t conv);
		uint32_t RemotePort(uint32_t conv);

	private:
		std::shared_ptr<ConnectionSingle> ConnectionMgr;
		std::shared_ptr<ConnectionSocket> Socket;
	};
}

