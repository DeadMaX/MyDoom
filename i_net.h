#ifndef I_NET_H
#define I_NET_H

#include <cstdint>

class INet
{
public:
	static const inline std::uint32_t DOOMCOM_ID = 0x12345678;

	const bool netgame {false};
	const std::uint32_t id{ DOOMCOM_ID };
	const std::size_t numplayers{ 1 };
	const std::size_t numnodes{ 1 };
	const bool deathmatch{ false };
	const std::size_t consoleplayer{ 0 };
};

#endif