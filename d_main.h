#ifndef D_MAIN_H
#define D_MAIN_H

#include <QByteArray>
#include <QTimer>
#include <QObject>
#include <QLabel>

#include "r_main.h"
#include "w_wad.h"
#include "g_game.h"
#include "d_net.h"
#include "v_video.h"
#include "i_video.h"
#include "f_wipe.h"

class Doom : public QObject
{
	Q_OBJECT

	QTimer m_refresh_timer{this};

	screen_t m_front_screen;
	screen_t m_back_screen;

	ISystem m_isystem;
	INet m_inet;
	Video m_video;
	IVideo m_ivideo;
	WAD m_wad;
	Renderer m_renderer{ m_wad };
	Net m_net{ m_isystem, m_inet };
	Game m_game{m_wad, m_renderer, m_net};
	Wipe m_wipe{};

	/* Demo */
	Net::nettic_key_t m_advancedemo{ m_net.RegisterTicObserver(std::bind(&Doom::D_DoAdvanceDemo, this)) };
	std::uint8_t m_demosequence{ std::numeric_limits<std::uint8_t>::max() };
	Net::nettic_key_t m_pagetickey;
	std::int16_t m_pagetic;
	const char* m_pagename{nullptr};

	/* Level */
	ISystem::gameticks m_levelstarttic;

	Game::gamestate_t m_oldgamestate{ Game::gamestate_t::GS_RESET };
	Game::gamestate_t m_wipegamestate{ m_game.current_state() };
	Game::gamestate_key_t m_gamechanges{ m_game.RegisterStateObserver(std::bind(&Doom::D_GameChange, this, std::placeholders::_1)) };
	void D_Display();


	void D_PageDrawer();
	void D_DoAdvanceDemo();
	void D_PageTicker();
	void D_AdvanceDemo();
	void D_GameChange(Game::gamestate_t);

public:
	Doom(QWidget*, const QByteArray&);

private slots:
	void D_DoomLoop();
};

#endif
