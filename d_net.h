#ifndef D_NET_H
#define D_NET_H

#include <array>
#include <list>
#include <functional>
#include <type_traits>

#include "i_system.h"
#include "i_net.h"
#include "r_defs.h"

class Net
{
	static const inline std::size_t MAXNETNODES{ 8 };

	struct net_info_t
	{
		bool m_nodeingame{ false };
		bool m_remoteresend{ false };
		ISystem::gameticks m_nettics{ 0 };
		ISystem::gameticks m_resendto{ 0 };
		std::uint32_t resendcount{ 0 };
	};

	const ISystem& m_system;
	const INet& m_inet;

	ISystem::gameticks m_oldentertics{ 0 };
	ISystem::gameticks m_gametic{0};

	void NetUpdate(const ISystem::gameticks& now, const ISystem::gameticks& count);
	void GetPackets(const ISystem::gameticks& now);
	std::array<net_info_t, MAXNETNODES> m_netnodes;

	safe_list_t<std::function<void()>> m_tic_observer;
	safe_list_t<std::function<void()>> m_stall_tic_observer;

public:
	using nettic_key_t = list_key_t<decltype(m_tic_observer)>;
	using stall_nettic_key_t = list_key_t<decltype(m_stall_tic_observer)>;


	Net(const ISystem& system, const INet& inet);
	void TryRunTics();
	ISystem::gameticks current_gametic() const {return m_gametic;}

	template<typename Function, typename = std::enable_if_t<std::is_invocable_v<Function>>>
	nettic_key_t RegisterTicObserver(Function &&f)
	{
		return nettic_key_t{ &m_tic_observer, m_tic_observer.emplace(std::end(m_tic_observer), std::forward<Function>(f)) };
	}

	template<typename Function, typename = std::enable_if_t<std::is_invocable_v<Function>>>
	stall_nettic_key_t RegisterStallTicObserver(Function&& f)
	{
		;
		return stall_nettic_key_t{&m_stall_tic_observer, m_stall_tic_observer.emplace(std::end(m_stall_tic_observer), std::forward<Function>(f)) };
	}
};

#endif
