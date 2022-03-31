#include "i_system.h"

ISystem::gameticks ISystem::I_GetTime() const
{
	auto now{std::chrono::system_clock::now()};
	return std::chrono::duration_cast<gameticks>(now - m_start_time);
}