#pragma once
#include <string>
#include <functional>
#include "kcpTypedef.h"
namespace asio {
	class io_context;
	typedef io_context io_service;
}
namespace AsioKCP 
{
	class ServerSocket;
	class ConnectionContainer;
	class Server
	{
	public:
		Server(asio::io_service& service, const std::string& address, const uint32_t port);
		Server(const Server&) = delete;
		const Server& operator=(const Server&) = delete;

		void Update(uint64_t clock);
		void SetCallback(const std::function<event_callback_t>& func);
		int SendMsg(uint32_t conv, const std::string& msg);
		void ForceDisconnect(uint32_t conv);
		void Stop();

		std::string RemoteAddress(uint32_t conv);
		uint32_t RemotePort(uint32_t conv);
		uint32_t GetPort()const { return Port; }

	private:
		uint32_t Port;
		std::shared_ptr<ServerSocket> Socket;
	};

}

