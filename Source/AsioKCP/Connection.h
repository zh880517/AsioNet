#pragma once
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#include <asio.hpp>
#include <memory>

struct IKCPCB;
typedef struct IKCPCB ikcpcb;

namespace asio_kcp 
{
	class ConnectionSocket;

	class Connection : private asio::detail::noncopyable
	{
	public:
		Connection(const std::weak_ptr<ConnectionSocket>& manager_ptr);
		~Connection();

		static std::shared_ptr<Connection> Create(const std::weak_ptr<ConnectionSocket>& manager_ptr,
			const uint32_t& conv, const asio::ip::udp::endpoint& udp_remote_endpoint);

		void SetUdpRemoteEndpoint(const asio::ip::udp::endpoint& udp_remote_endpoint);

		void Update(uint64_t clock);

		void Input(char* udp_data, size_t bytes_recvd, const asio::ip::udp::endpoint& udp_remote_endpoint);

		bool IsTimeout() const;
		void DoTimeout();
		void SendMsg(const std::string& msg);

		std::string RemoteAddress()const { return RemoteEndpoint.address().to_string(); }
		uint32_t RemotePort()const { return RemoteEndpoint.port(); }

	private:
		void Init(const uint32_t& conv);

		void Clean();

		//KCP callback
		static int UdpOutput(const char *buf, int len, ikcpcb *kcp, void *user);

		void SendPackage(const char *buf, int len);

		uint64_t GetClock() const;

		uint32_t GetTimeoutTime() const;
	private:
		std::weak_ptr<ConnectionSocket> Manager;
		uint32_t Conv = 0;
		uint64_t StartClock = 0;
		ikcpcb* Kcp = nullptr;
		asio::ip::udp::endpoint RemoteEndpoint;
		uint64_t LastPacketRecvTime = 0;
	};
	using ConnectionPtr = std::shared_ptr<Connection>;
}

