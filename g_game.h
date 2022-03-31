#ifndef G_GAME_H
#define G_GAME_H

#include <cstdint>

#include "d_net.h"
#include "d_player.h"
#include "r_defs.h"
#include "r_main.h"
#include "w_wad.h"
#include "p_local.h"

class Game
{
	struct demo_header_t
	{
		std::uint8_t m_version;
		skill_t m_skill;
		std::uint8_t m_episode;
		std::uint8_t m_map;
		std::uint8_t m_deathmatch;
		std::uint8_t m_respawnparm;
		std::uint8_t m_fastparm;
		std::uint8_t m_nomonsters;
		std::uint8_t m_consoleplayer;

		std::array< std::uint8_t, 4> m_playeringame;
	};
	static_assert(std::has_unique_object_representations_v<demo_header_t> && sizeof(demo_header_t) == 13, "Bug");

public:
	static const inline std::uint8_t VERSION { 109 };
	static const inline std::size_t MAX_PLAYER{ 4 };

	enum class gamestate_t
	{
		GS_LEVEL,
		GS_INTERMISSION,
		GS_FINALE,
		GS_DEMOSCREEN,
		GS_RESET
	};

	enum class gameaction_t
	{
		ga_nothing,
		ga_loadlevel,
		ga_newgame,
		ga_loadgame,
		ga_savegame,
		ga_playdemo,
		ga_completed,
		ga_victory,
		ga_worlddone,
		ga_screenshot
	};

	using player_t = generic_player_t<MAX_PLAYER>;

private:
	const WAD& m_wad;
	const Renderer& m_renderer;
	const Net& m_net;
	gamestate_t m_gamestate{ gamestate_t::GS_DEMOSCREEN };
	gameaction_t m_gameaction{ gameaction_t::ga_nothing };
	Net::nettic_key_t m_game_ticker;
	PlayMap m_play_map;

	// Demo
	const char* m_defdemoname{ nullptr };

	// Current game skill
	skill_t m_gameskill{ skill_t::sk_baby };
	std::uint8_t m_gameepisode;
	std::uint8_t m_gamemap;
	std::size_t m_skytexture;

	// player info
	std::array <bool, MAX_PLAYER> m_playeringame;
	std::array <player_t, MAX_PLAYER> m_players;

	safe_list_t<std::function<void(gamestate_t)>> m_gamestate_observers;

public:
	Game(const WAD& wad, const Renderer& renderer, Net& net);

	gamestate_t current_state() const
	{
		return m_gamestate;
	}

	gameaction_t current_action() const
	{
		return m_gameaction;
	}

	void G_ShowDemoScreen();
	void G_DeferedPlayDemo(const char* name);

	bool demoplayback() const { return m_defdemoname != nullptr; }

	using gamestate_key_t = list_key_t<decltype(m_gamestate_observers)>;

	template<typename Function, typename = std::enable_if_t<std::is_invocable_v<Function, gamestate_t>>>
	gamestate_key_t RegisterStateObserver(Function&& f)
	{
		m_gamestate_observers.emplace_front(std::forward<Function>(f));
		return gamestate_key_t{ &m_gamestate_observers, std::begin(m_gamestate_observers) };
	}

private:

	void G_Ticker();
	void G_DoPlayDemo();
	void G_InitNew(skill_t skill, std::uint8_t episode, std::uint8_t map);
	void G_DoLoadLevel();
	void G_SetGamestate(gamestate_t newstate);

};

#endif
