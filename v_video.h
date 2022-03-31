#ifndef V_VIDEO_H
#define V_VIDEO_H

#include <cstdint>
#include <array>
#include "r_defs.h"

template <std::size_t SCREENWIDTH, std::size_t SCREENHEIGHT>
struct screen_column_t
{
    struct iterator
    {
        // iterate over columns

        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = std::uint8_t;
        using pointer = std::uint8_t*;
        using reference = std::uint8_t&;

        iterator(std::uint8_t* ptr) : m_column{ ptr } {}

        reference operator*() const { return *m_column; }
        pointer operator->() const { return m_column; }
        iterator& operator++() {
            m_column += SCREENWIDTH;
            return *this;
        }
        iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

        friend bool operator== (const iterator& a, const iterator& b) { return a.m_column == b.m_column; };
        friend bool operator!= (const iterator& a, const iterator& b) { return a.m_column != b.m_column; };
    private:

        std::uint8_t* m_column;
    };

    screen_column_t(std::uint8_t* column) : m_column{ column } {}

    screen_column_t& operator++() { ++m_column; return *this; }
    friend bool operator== (const screen_column_t& a, const screen_column_t& b) { return a.m_column == b.m_column; };
    friend bool operator!= (const screen_column_t& a, const screen_column_t& b) { return a.m_column != b.m_column; };

    iterator begin() { return iterator(m_column); }
    iterator end() { return iterator(m_column + SCREENWIDTH * SCREENHEIGHT); }
private:
    std::uint8_t *m_column;
};

struct screen_t
{
	static const inline std::uint32_t SCREENWIDTH{ 320 };
	static const inline std::uint32_t SCREENHEIGHT{ 200 };

    struct iterator
    {
        // iterate over columns

        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = screen_column_t<SCREENWIDTH, SCREENHEIGHT>;
        using pointer = screen_column_t<SCREENWIDTH, SCREENHEIGHT>*;
        using reference = screen_column_t<SCREENWIDTH, SCREENHEIGHT>&;

        iterator(std::uint8_t * ptr) : m_column{ ptr } {}

        reference operator*() { return m_column; }
        pointer operator->() { return &m_column; }
        iterator& operator++() {
            ++m_column;
            return *this;
        }
        iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

        friend bool operator== (const iterator& a, const iterator& b) { return a.m_column == b.m_column; };
        friend bool operator!= (const iterator& a, const iterator& b) { return a.m_column != b.m_column; };
    private:

        screen_column_t<SCREENWIDTH, SCREENHEIGHT> m_column;
    };

    iterator begin() {return iterator(m_data.data());}
    iterator end() { return iterator(m_data.data() + SCREENWIDTH); }
    const std::uint8_t *data() const { return m_data.data(); }
    const std::size_t size() const { return m_data.size(); }

    const std::size_t ScreenWidth() const { return SCREENWIDTH; }
    const std::size_t ScreenHeight() const { return SCREENHEIGHT; }

private:
    std::array<std::uint8_t, SCREENWIDTH*SCREENHEIGHT> m_data;
};

class Video
{
public:
	void V_DrawPatch(std::int32_t x, std::int32_t y, screen_t& scrn, const patch_t& patch);
};

#endif
