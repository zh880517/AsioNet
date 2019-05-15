#pragma once
#include "ConnectionSocket.h"
#include "ConnectionContainer.h"

namespace AsioKCP
{
	enum class ClientSocketStage
	{
		eNone,
		eConnecting,
		eConnected,
		eTimeOut,
	};

	class Connection;
	class ClientSocket : public ConnectionSocket, public std::enable_shared_from_this<ClientSocket>
	{
	public:
		ClientSocket(asio::io_service& service, bool ipv6 = false);

		void Connect(const std::string& host, uint32_t port);

		int SendMsg(const std::string& msg);

		int SendMsg(uint32_t peerConv, const std::string& msg);

		bool IsConnected() { return Stage != ClientSocketStage::eConnected && ConnectionPtr; }

		void Diconnect();

		void AddPeer(uint32_t conv, const std::string& address, uint32_t port);

		void RemovePeer(uint32_t conv);

		std::string RemoteAddress();

		uint32_t RemotePort();

		std::string PeerRemoteAddress(uint32_t conv);

		uint32_t PreerRemotePort(uint32_t conv);

		virtual void SendPackage(const char * data, size_t len, const asio::ip::udp::endpoint & endpoint) override;
	protected:

		virtual void OnUpdate() override;

	private:
		void HandleUdpSend(const std::error_code& error, size_t bytes_recvd);

		void HandleUdpReceiveFrom(const std::error_code& error, size_t bytes_recvd);

		void HookUdpAsyncReceive(void);

		void HandleKcpPacket(size_t bytes_recvd);

		void DoConnect();

	private:
		ClientSocketStage Stage = ClientSocketStage::eNone;
		int64_t ConnectStartClock = -1;
		int64_t NextReConnectTime = -1;
		asio::io_service* Service = nullptr;
		asio::ip::udp::socket Socket;
		std::shared_ptr<Connection> ConnectionPtr;
		ConnectionContainer PeerConnections;
	};
}