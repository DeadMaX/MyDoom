#ifndef I_SYSTEM_H
#define I_SYSTEM_H

#include <chrono>

class ISystem
{
	static const inline std::uint32_t TICKRATE{ 35 }; // run at 35 FPS

public:
	using gameticks = std::chrono::duration<std::int32_t, std::ratio<1, TICKRATE>>;
	gameticks I_GetTime() const;

protected:
	const std::chrono::time_point<std::chrono::system_clock> m_start_time{std::chrono::system_clock::now()};
};

#endif
