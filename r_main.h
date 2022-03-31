#ifndef R_MAIN_H
#define R_MAIN_H

#include <cstdint>
#include <array>
#include <map>

#include "w_wad.h"
#include "m_fixed.h"
#include "tables.h"
#include "r_defs.h"
#include "r_data.h"

class Renderer
{
	static const inline std::uint32_t SCREENWIDTH{ 320 };
	static const inline std::uint32_t SCREENHEIGHT{ 200 };
	static const inline fineangle_t FIELDOFVIEW{ ANG45 * 3 }; // 135 degree

	template <size_t idx, size_t COUNT>
	std::uint32_t compute_viewangletox(const fixed_t &focallength)
	{
		constexpr fixed_t tangent = fineangle_t::from_fraction<idx, COUNT>().finetangent();
		if constexpr (tangent > fixed_t::from_num(2, 0))
			return 0;
		if constexpr (tangent < fixed_t::from_num(-2, 0))
			return SCREENWIDTH;

		auto t = m_centerxfrac - (tangent * focallength) + fixed_t::from_num(1, 0);
		if (t <= fixed_t::from_num(-1, 0))
			return 0;
		if (t >= fixed_t::from_num(SCREENWIDTH+1, 0))
			return SCREENWIDTH;
		return t.integer();
	}

	template<std::size_t... Indices>
	std::array<std::uint32_t, sizeof...(Indices)> init_viewangletox(const fixed_t &focallength, std::index_sequence<Indices...>)
	{
		return {{ compute_viewangletox<Indices, sizeof...(Indices)>(focallength)... }};
	}

	fineangle_t compute_xtoviewangle(fixed_t focallength, size_t)
	{
		return fineangle_t::from_num(0u);
	}

	template<std::size_t... Indices>
	std::array<fineangle_t, sizeof...(Indices)> init_xtoviewangle(fixed_t focallength, std::index_sequence<Indices...>)
	{
		return {{ compute_xtoviewangle(focallength, Indices)... }};
	}

	std::uint32_t	m_setblocks{ 11 };
	std::uint32_t	m_setdetail{ 0 };
	std::uint32_t	m_scaledviewwidth{ SCREENWIDTH };
	std::uint32_t	m_detailshift{ 0 };
	std::uint32_t	m_viewwidth{ SCREENWIDTH };
	std::uint32_t	m_viewheight{ SCREENHEIGHT };

	std::uint32_t	m_centery{ m_viewheight / 2};
	std::uint32_t	m_centerx{ m_viewwidth / 2};
	fixed_t m_centerxfrac{ fixed_t::from_num(m_centerx, 0u) };
	fixed_t m_centeryfrac{ fixed_t::from_num(m_centery, 0u) };
	fixed_t m_projection{ m_centerxfrac };
	std::array<std::uint32_t, 4096> viewangletox{ init_viewangletox(m_centerxfrac / FIELDOFVIEW.finetangent(), std::make_index_sequence<sizeof(viewangletox)/sizeof(viewangletox[0])>{})};
	std::array<fineangle_t, SCREENWIDTH + 1> xtoviewangle{ init_xtoviewangle(m_centerxfrac / FIELDOFVIEW.finetangent(), std::make_index_sequence<sizeof(xtoviewangle) / sizeof(xtoviewangle[0])>{}) };

	const pname_t* m_pname{nullptr};
	const ptexture_t* m_maptex1{ nullptr };
	const ptexture_t* m_maptex2{ nullptr };
	std::map<std::array<char, 8>, std::size_t> m_texture_index;
	std::vector<texture_data_t> m_texturelookup;

	WAD::wad_index_t m_f_start;
	WAD::wad_index_t m_f_end;
	WAD::wad_index_t m_s_start;
	WAD::wad_index_t m_s_end;
	WAD::wad_index_t m_p_start;
	WAD::wad_index_t m_p_end;
	WAD::wad_index_t m_skyflatnum; // special flat

	void R_InitTextures(const WAD& wad);
	void R_GenerateLookup(const WAD& wad, std::size_t idx);
	void R_GenerateComposite(const WAD& wad, const texture_t& texture, texture_data_t& texture_data, const std::vector<bool>& composite_cols) const;
	std::size_t R_CheckTextureNumForName(const char* name) const;
	const texture_t& R_GetTexture(std::size_t) const;

public:
	Renderer(const WAD& wad);
	bool setsizeneeded{ true };

	void R_ExecuteSetViewSize();
	std::size_t R_TextureNumForName(const char* name) const;
};

#endif
