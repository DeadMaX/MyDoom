#include "d_main.h"

#include <chrono>
#include <stdexcept>

#include "i_system.h"

Doom::Doom(QWidget* parent, const QByteArray& data)
	: m_ivideo{ parent, m_front_screen }
	, m_wad{ data }
{
	connect(&m_refresh_timer, &QTimer::timeout, this, &Doom::D_DoomLoop);
	m_refresh_timer.setInterval(std::chrono::duration_cast<std::chrono::milliseconds>(ISystem::gameticks{1}));
	m_refresh_timer.setSingleShot(false);
	m_refresh_timer.start();
}

void Doom::D_DoomLoop()
{
	m_net.TryRunTics();

	// TODO

	D_Display();

	// TODO
}

void Doom::D_Display()
{
	bool wipe{ false };
	auto gamestate = m_game.current_state();

	if (m_renderer.setsizeneeded)
	{
		m_renderer.R_ExecuteSetViewSize();
		m_oldgamestate = Game::gamestate_t::GS_RESET; // force background redraw
	}

	if (gamestate != m_wipegamestate)
	{
		wipe = true;
		m_wipe.wipe_StartScreen(m_front_screen);
	}

	switch (gamestate)
	{
	case Game::gamestate_t::GS_RESET:
		throw std::runtime_error("Invalide state");

	case Game::gamestate_t::GS_LEVEL:
		/*
			HU_Erase();
			if (automapactive)
				AM_Drawer();

			ST_Drawer(viewheight == 200, redrawsbar);
		*/
		I_UpdateNoBlit();
		if (!automapactive)
			R_RenderPlayerView(&players[displayplayer]);
		HU_Drawer();
		if (!automapactive && scaledviewwidth != 320)


		break;

	case Game::gamestate_t::GS_INTERMISSION:
		// TODO
		WI_Drawer();
		I_UpdateNoBlit();
		break;

	case Game::gamestate_t::GS_FINALE:
		// TODO
		F_Drawer();
		I_UpdateNoBlit();
		break;

	case Game::gamestate_t::GS_DEMOSCREEN:
		D_PageDrawer();
		I_UpdateNoBlit();
		break;
	}



	if (!wipe)
	{
		m_ivideo.I_FinishUpdate();
		return;
	}

	// TODO
}

void Doom::D_PageTicker()
{
	if (--m_pagetic < 0)
	{
		D_AdvanceDemo();
	}
}

void Doom::D_PageDrawer()
{
	m_video.V_DrawPatch(0, 0, m_front_screen, m_wad.W_CacheLumpName<patch_t>(m_pagename));
}

void Doom::D_AdvanceDemo()
{
	m_pagetickey.release();
	m_advancedemo = m_net.RegisterTicObserver(std::bind(&Doom::D_DoAdvanceDemo, this));
}

void Doom::D_GameChange(Game::gamestate_t newstate)
{
	switch (newstate)
	{
	case Game::gamestate_t::GS_LEVEL:
		m_levelstarttic = m_net.current_gametic();
		m_wipegamestate = Game::gamestate_t::GS_RESET; // force a wipe
		if (m_oldgamestate != Game::gamestate_t::GS_LEVEL)
			R_FillBackScreen();
		break;

	default:
		break;
	}

	// make sure to use correct palette
	if (newstate != m_oldgamestate && newstate != Game::gamestate_t::GS_LEVEL)
		m_ivideo.I_SetPalette(m_wad.W_CacheLumpName<palette_t>("PLAYPAL"));
}

void Doom::D_DoAdvanceDemo()
{
	m_advancedemo.release(); // wait for the end of the new demo action

	m_demosequence = std::uint8_t(m_demosequence + 1) % 7;

	switch(m_demosequence)
	{
	case 0:
		m_game.G_ShowDemoScreen();
		m_pagename = "TITLEPIC";
		m_pagetic = 170;
		m_pagetickey = m_net.RegisterTicObserver(std::bind(&Doom::D_PageTicker, this));
		break;
	case 1:
		m_game.G_DeferedPlayDemo("demo1");
		break;
	case 2:
		m_pagetic = 200;
		m_game.G_ShowDemoScreen();
		m_pagename = "CREDIT";
		m_pagetickey = m_net.RegisterTicObserver(std::bind(&Doom::D_PageTicker, this));
		break;
	case 3:
		m_game.G_DeferedPlayDemo("demo2");
		break;
	case 4:
		m_pagetic = 200;
		m_game.G_ShowDemoScreen();
		m_pagename = "CREDIT";
		m_pagetickey = m_net.RegisterTicObserver(std::bind(&Doom::D_PageTicker, this));
		break;
	case 5:
		m_game.G_DeferedPlayDemo("demo3");
		break;
	case 6:
		m_game.G_DeferedPlayDemo("demo4");
		break;
	default:
		throw std::runtime_error("Invalid demo sequence");
		break;
	}

}