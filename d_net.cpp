#include "d_net.h"

#include <stdexcept>
#include <algorithm>

Net::Net(const ISystem& system, const INet& inet)
	: m_system{ system }
	, m_inet{ inet }
{
	if (m_inet.id != INet::DOOMCOM_ID || m_inet.numnodes > m_netnodes.size())
		throw std::runtime_error("Doomcom buffer invalid!");

	for (std::size_t i = 0; i < m_inet.numnodes; i++)
		m_netnodes[i].m_nodeingame = true;
}

void Net::NetUpdate(const ISystem::gameticks& now, const ISystem::gameticks& count)
{
	if (count.count() == 0)
		return GetPackets(now);

	// TODO process events

	GetPackets(now);
}

void Net::GetPackets(const ISystem::gameticks& now)
{
	// TODO

	// for now advance local player to current time
	m_netnodes[m_inet.consoleplayer].m_nettics = now;

}

void Net::TryRunTics()
{
	auto NetInfo_by_nettics = [](const net_info_t& a, const net_info_t& b)
	{
		if (!b.m_nodeingame) return true;
		if (!a.m_nodeingame) return false;
		return a.m_nettics < b.m_nettics;
	};

	auto entertic = m_system.I_GetTime();
	auto realtics = entertic - m_oldentertics;
	m_oldentertics = entertic;

	// TODO
	NetUpdate(entertic, realtics);
	ISystem::gameticks lowtic = std::ranges::min_element(m_netnodes, NetInfo_by_nettics)->m_nettics;
	
	auto availabletics = lowtic - m_gametic;
	ISystem::gameticks counts;
	// decide how many tics to run
	if (realtics < availabletics - ISystem::gameticks{1})
		counts = realtics + ISystem::gameticks{ 1 };
	else if (realtics < availabletics)
		counts = realtics;
	else
		counts = availabletics;

	if (counts < ISystem::gameticks{ 1 })
		counts = ISystem::gameticks{ 1 };

	// wait for the remote player update (may freeze the game)
	while (lowtic < m_gametic + counts)
	{
		NetUpdate(entertic, realtics);
		lowtic = std::ranges::min_element(m_netnodes, NetInfo_by_nettics)->m_nettics;

		if (lowtic < m_gametic) // we must not go back in time ...
			throw std::runtime_error("TryRunTics: lowtic < gametic");

		// no updates ... give hand
		if ((m_system.I_GetTime() - entertic).count() >= 20)
		{
			for (auto& f : m_stall_tic_observer)
				f();
			return;
		}
	}

	for (ISystem::gameticks i{}; i < counts; i += ISystem::gameticks{ 1 })
	{
		if (m_gametic > lowtic) // we must no go too far in time
			throw std::runtime_error("gametic>lowtic");

		for (auto& f : m_tic_observer)
			f();

		m_gametic += ISystem::gameticks{ 1 };
	}

	NetUpdate(entertic, realtics);
}
