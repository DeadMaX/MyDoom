#ifndef I_VIDEO_H
#define I_VIDEO_H

#include <array>
#include <cstdint>

#include "v_video.h"

#include <QWidget>

struct color_t
{
	std::uint8_t red;
	std::uint8_t green;
	std::uint8_t blue;
};

static_assert(std::has_unique_object_representations_v<color_t> && sizeof(color_t) == 3, "Bug");

static const inline std::size_t palette_size{ 256 };
using palette_t = std::array<color_t, palette_size>;

class IVideo : public QWidget
{
	Q_OBJECT

	static const std::array<std::array<std::uint8_t, 256>, 5> s_gammatable;

	QList<QRgb> m_palette;
	std::uint8_t m_usegamma{0};

	const screen_t& m_screen;

	virtual void	paintEvent(QPaintEvent* event) override;

public:
	IVideo(QWidget* parent, const screen_t& screen);
	void I_SetPalette(const palette_t &);
	void I_FinishUpdate();
};

#endif
