#include "g_game.h"

Game::Game(const WAD& wad, const Renderer& renderer, Net& net)
	: m_wad{ wad }
	, m_renderer{ renderer }
	, m_net{net}
	, m_game_ticker{ net.RegisterTicObserver(std::bind(&Game::G_Ticker, this)) }
{
}

void Game::G_SetGamestate(gamestate_t newstate)
{
	m_gamestate = newstate;
	if (newstate == gamestate_t::GS_DEMOSCREEN)
		m_gameaction = gameaction_t::ga_nothing;
	for (auto& observer : m_gamestate_observers)
		observer(newstate);
}

void Game::G_Ticker()
{
	// TODO

	while (m_gameaction != gameaction_t::ga_nothing)
	{
		switch (m_gameaction)
		{
		case gameaction_t::ga_loadlevel:
			// G_DoLoadLevel();
			break;
		case gameaction_t::ga_newgame:
			// G_DoNewGame();
			break;
		case gameaction_t::ga_loadgame:
			// G_DoLoadGame();
			break;
		case gameaction_t::ga_savegame:
			// G_DoSaveGame();
			break;
		case gameaction_t::ga_playdemo:
			G_DoPlayDemo();
			break;
		case gameaction_t::ga_completed:
			// G_DoCompleted();
			break;
		case gameaction_t::ga_victory:
			// F_StartFinale();
			break;
		case gameaction_t::ga_worlddone:
			// G_DoWorldDone();
			break;
		case gameaction_t::ga_screenshot:
			// M_ScreenShot();
			m_gameaction = gameaction_t::ga_nothing;
			break;
		case gameaction_t::ga_nothing:
			break;
		}
	}

	// TODO
}

void Game::G_ShowDemoScreen()
{
	G_SetGamestate(gamestate_t::GS_DEMOSCREEN);
}

void Game::G_DeferedPlayDemo(const char* name)
{
	m_defdemoname = name;
	m_gameaction = gameaction_t::ga_playdemo;
}

void Game::G_DoPlayDemo()
{
	m_gameaction = gameaction_t::ga_nothing;
	auto &demo_p = m_wad.W_CacheLumpName<demo_header_t>(m_defdemoname);

	if (demo_p.m_version != VERSION)
		return;

	std::ranges::transform(demo_p.m_playeringame, std::begin(m_playeringame), [](const std::uint8_t& in_game) { return !!in_game; });
	// TODO if (playeringame[1]) 

	G_InitNew(demo_p.m_skill, demo_p.m_episode, demo_p.m_map);
}

void Game::G_InitNew(skill_t skill, std::uint8_t episode, std::uint8_t map)
{
	// TODO M_ClearRandom();

	if (skill == skill_t::sk_nightmare && m_gameskill != skill_t::sk_nightmare)
	{
		// TODO
		/*
		for (i = S_SARG_RUN1; i <= S_SARG_PAIN2; i++)
			states[i].tics >>= 1;
		mobjinfo[MT_BRUISERSHOT].speed = 20 * FRACUNIT;
		mobjinfo[MT_HEADSHOT].speed = 20 * FRACUNIT;
		mobjinfo[MT_TROOPSHOT].speed = 20 * FRACUNIT;
		*/
	}
	else if (skill != skill_t::sk_nightmare && m_gameskill == skill_t::sk_nightmare)
	{
		/*
		for (i = S_SARG_RUN1; i <= S_SARG_PAIN2; i++)
			states[i].tics <<= 1;
		mobjinfo[MT_BRUISERSHOT].speed = 15 * FRACUNIT;
		mobjinfo[MT_HEADSHOT].speed = 10 * FRACUNIT;
		mobjinfo[MT_TROOPSHOT].speed = 10 * FRACUNIT;
		*/
	}

	std::ranges::for_each(m_players, [](player_t &player) {
		player.m_playerstate = playerstate_t::PST_REBORN;
	});

	m_gameepisode = episode;
	m_gamemap = map;
	m_gameskill = skill;

	G_DoLoadLevel();
}

void Game::G_DoLoadLevel()
{
	switch (m_gameepisode)
	{
	case 1:
		m_skytexture = m_renderer.R_TextureNumForName("SKY1");
		break;
	case 2:
		m_skytexture = m_renderer.R_TextureNumForName("SKY2");
		break;
	case 3:
		m_skytexture = m_renderer.R_TextureNumForName("SKY3");
		break;
	case 4:
		m_skytexture = m_renderer.R_TextureNumForName("SKY4");
		break;
	default:
		throw std::runtime_error("Unknown episode");
	}

	G_SetGamestate(gamestate_t::GS_LEVEL);

	for (size_t i = 0; auto& player : m_players)
	{
		if (m_playeringame[i++] && player.m_playerstate == playerstate_t::PST_DEAD)
			player.m_playerstate = playerstate_t::PST_REBORN;
		std::ranges::fill(player.frags, 0);
	}

	m_play_map.P_SetupLevel(m_gameepisode, m_gamemap, 0, m_gameskill);

	// TODO set current displayed player

	m_gameaction = gameaction_t::ga_nothing;

	// TODO clear controls
}