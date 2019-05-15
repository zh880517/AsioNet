#pragma once
#include <memory>
#include <functional>
#include "kcpTypedef.h"
namespace asio {
	class io_context;
	typedef io_context io_service;
}

namespace AsioKCP
{
	class ClientSocket;

	class Client
	{
	public:
		Client(asio::io_service& service, bool ipv6 = false);
		Client(const Client&) = delete;
		const Client& operator=(const Client&) = delete;

		void Connect(const std::string& host, uint32_t port);

		void Update(int64_t clock);

		void SetCallback(const std::function<event_callback_t>& func);

		int SendMsg(const std::string& msg);

		int SendMsg(uint32_t peerConv, const std::string& msg);

		void AddPeer(uint32_t conv, const std::string& address, uint32_t port);

		void RemovePeer(uint32_t conv);

		void Diconnect();

		std::string RemoteAddress();

		uint32_t RemotePort();

		std::string PeerRemoteAddress(uint32_t conv);

		uint32_t PreerRemotePort(uint32_t conv);

	private:
		std::shared_ptr<ClientSocket> Socket;
	};
	using ClientPtr = std::shared_ptr<Client>;
}

