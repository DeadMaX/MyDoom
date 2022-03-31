#include "v_video.h"

void Video::V_DrawPatch(std::int32_t x, std::int32_t y, screen_t &scrn, const patch_t& patch)
{
	y -= patch.m_topoffset;
	x -= patch.m_leftoffset;

	auto dest_column = scrn.begin();
	std::advance(dest_column, x);

	for (auto &column : patch)
	{
		for (auto &post : column)
		{
			auto dest_row = std::begin(*dest_column);
			std::advance(dest_row, y + post.m_topdelta);
			std::copy(std::begin(post), std::end(post), dest_row);
		}

		++dest_column;
	}
}