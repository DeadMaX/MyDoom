#include "r_main.h"

#include <array>
#include <stdexcept>
#include <cctype>

const texture_t& Renderer::R_GetTexture(std::size_t idx) const
{
	if (idx >= m_maptex1->m_numtextures)
	{
		return (*m_maptex2)[idx - m_maptex1->m_numtextures];
	}
	return (*m_maptex1)[idx];
}

static void R_DrawColumnInCache(std::span<std::uint8_t> &destination, const column_t &source, std::int16_t originy, std::uint16_t cacheheight)
{
	for (auto& patch : source)
	{
		std::int16_t count = patch.m_length;
		std::int16_t position = originy + patch.m_topdelta;
		if (position < 0)
		{
			count += position;
			position = 0;
		}

		if (position + count > cacheheight)
			count = cacheheight - position;

		if (count > 0)
		{
			auto dst = std::begin(destination);
			std::advance(dst, position);
			std::copy_n(std::begin(patch), count, dst);
		}
	}
}

void Renderer::R_GenerateComposite(const WAD& wad, const texture_t& texture, texture_data_t& texture_data, const std::vector<bool> &composite_cols) const
{
	std::vector<std::span<std::uint8_t>> composition_buffers;
	composition_buffers.resize(composite_cols.size());

	for (auto& mappatch : texture)
	{
		auto& realpatch = wad.W_CacheLumpName<patch_t>((*m_pname)[mappatch.m_patch]);
		auto x1 = mappatch.m_originx;
		decltype(x1) x2 = x1 + realpatch.m_width;
		auto x = x1;

		if (x1 < 0)
			x = 0;

		if (x2 > texture.m_width)
			x2 = texture.m_width;

		for (; x < x2; ++x)
		{
			if (!composite_cols[x])
				continue;

			if (composition_buffers[x].empty())
			{
				auto& composite_patch = texture_data.m_texturecomposite.emplace_back(new (texture.m_height) composite_patch_t);
				texture_data.m_texturecolumns[x] = composite_patch->column();
				composition_buffers[x] = composite_patch->data();
			}

			R_DrawColumnInCache(composition_buffers[x], realpatch[x - x1], mappatch.m_originy, texture.m_height);
		}
	}
}

void Renderer::R_GenerateLookup(const WAD& wad, std::size_t idx)
{
	bool have_composite{ false };
	auto& texture_data = m_texturelookup[idx];
	auto& texture = R_GetTexture(idx);

	texture_data.m_textureheight = fixed_t::from_num(texture.m_height, 0);
	texture_data.m_texturecolumns.resize(texture.m_width);
	std::vector<bool> composite_columns(texture.m_width, false);

	for (auto& mappatch : texture)
	{
		auto& realpatch = wad.W_CacheLumpName<patch_t>((*m_pname)[mappatch.m_patch]);
		auto x1 = mappatch.m_originx;
		decltype(x1) x2 = x1 + realpatch.m_width;
		auto x = x1;

		if (x1 < 0)
			x = 0;

		if (x2 > texture.m_width)
			x2 = texture.m_width;

		for (; x < x2; ++x)
		{
			if (texture_data.m_texturecolumns[x])
			{
				composite_columns[x] = have_composite = true;
			}
			else
				texture_data.m_texturecolumns[x] = realpatch[x - x1];
		}
	}

	for(auto& col : texture_data.m_texturecolumns)
		if (!col)
			throw std::runtime_error("R_GenerateLookup: column without a patch");

	if (have_composite)
		R_GenerateComposite(wad, texture, texture_data, composite_columns);

}

void Renderer::R_InitTextures(const WAD& wad)
{
	m_pname = &wad.W_CacheLumpName<pname_t>("PNAMES");
	m_maptex1 = &wad.W_CacheLumpName<ptexture_t>("TEXTURE1");
	
	std::size_t idx{ 0 };

	std::for_each(std::begin(*m_maptex1), std::end(*m_maptex1), [&](const texture_t& texture) {
			m_texture_index[texture.m_name] = idx++;
		});

	if (wad.W_LumpNameExists("TEXTURE2"))
	{
		m_maptex2 = &wad.W_CacheLumpName<ptexture_t>("TEXTURE2");
		std::for_each(std::begin(*m_maptex2), std::end(*m_maptex2), [&](const texture_t& texture) {
			m_texture_index[texture.m_name] = idx++;
		});
	}

	m_texturelookup.resize(idx);
	for (std::size_t i = 0; i < idx; i++)
		R_GenerateLookup(wad, i);
}

std::size_t Renderer::R_CheckTextureNumForName(const char* name) const
{
	std::array<char, 8> search{};

	for (size_t i = 0; name[i]; ++i)
		search[i] = std::toupper(name[i]);

	auto idx = m_texture_index.find(search);
	if (idx == std::end(m_texture_index))
		return std::numeric_limits<std::uint32_t>::max();
	return idx->second;
}

std::size_t Renderer::R_TextureNumForName(const char* name) const
{
	auto res = R_CheckTextureNumForName(name);
	if (res == std::numeric_limits<std::size_t>::max())
		throw std::runtime_error("Unknown texture");
	return res;
}
