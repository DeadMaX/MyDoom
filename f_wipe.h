#ifndef F_WIPE_H
#define F_WIPE_H

#include "v_video.h"

class Wipe
{
	screen_t m_start_screen;
	screen_t m_end_screen;

public:
	void wipe_StartScreen(const screen_t& s) { m_start_screen = s; }
};

#endif
