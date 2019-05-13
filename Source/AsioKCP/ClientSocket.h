#pragma once
#include "ConnectionSocket.h"

namespace asio_kcp
{
	class Connection;
	class ClientSocket : public ConnectionSocket, public std::enable_shared_from_this<ClientSocket>
	{
	public:
		ClientSocket(asio::io_service& service, bool ipv6 = false);

		void Connect(const std::string& host, uint32_t port);

		int SendMsg(const std::string& msg);

		bool IsConnected() { return Connected && ConnectionPtr; }

		void Diconnect();

		std::string RemoteAddress();

		uint32_t RemotePort();

		virtual void SendPackage(const char * data, size_t len, const asio::ip::udp::endpoint & endpoint) override;
	protected:

		virtual void OnUpdate() override;

	private:
		void HandleUdpSend(const std::error_code& error, size_t bytes_recvd);

		void HandleUdpReceiveFrom(const std::error_code& error, size_t bytes_recvd);

		void HookUdpAsyncReceive(void);

		void HandleKcpPacket(size_t bytes_recvd);

	private:
		bool Connected = false;
		asio::io_service* Service = nullptr;
		asio::ip::udp::socket Socket;
		std::shared_ptr<Connection> ConnectionPtr;
	};
}