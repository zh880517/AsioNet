#pragma once
#include "ConnectionSocket.h"
#include "ConnectionContainer.h"
namespace AsioKCP 
{
	class ServerSocket : public ConnectionSocket, public std::enable_shared_from_this<ServerSocket>
	{
	public:
		ServerSocket(asio::io_service& service, const std::string& address, int udp_port);

		void Stop();

		void Start();

		int SendMsg(uint32_t conv, const std::string& msg);

		void ForceDisconnect(uint32_t conv);

		std::string RemoteAddress(uint32_t conv);

		uint32_t RemotePort(uint32_t conv);

		virtual void SendPackage(const char*data, size_t len, const asio::ip::udp::endpoint& endpoint) override;
	protected:

		virtual void OnUpdate() override;

	private:
		void HandleUdpReceiveFrom(const std::error_code& error, size_t bytes_recvd);

		void HandleUdpSend(const std::error_code& error, size_t bytes_recvd);

		void HookUdpAsyncReceive(void);

		void HandleConnectPacket();

		void HandleKcpPacket(size_t bytes_recvd);
	private:
		bool Stopped = false;
		ConnectionContainer Connections;
		asio::ip::udp::socket Socket;
	};
}

