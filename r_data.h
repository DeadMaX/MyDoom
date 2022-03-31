#ifndef R_DATA_H
#define R_DATA_H

#include <stdexcept>
#include <cstdint>

#include "m_fixed.h"

#pragma pack(push, 1)
struct mappatch_t
{
	std::int16_t m_originx;
	std::int16_t m_originy;
	std::uint16_t m_patch;
	std::int16_t m_stepdir;
	std::uint16_t m_colormap;
};

struct texture_t
{
	std::array<char, 8> m_name;
	std::uint32_t m_masked;
	std::uint16_t m_width;
	std::uint16_t m_height;
	std::uint32_t m_columndirectory;
	std::uint16_t m_patchcount;

	struct iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = const mappatch_t;
		using pointer = const mappatch_t*;
		using reference = const mappatch_t&;

		iterator(const texture_t* ptr, std::uint16_t idx) : m_me{ ptr }, m_idx{ idx } {}

		reference operator*() const { return *operator->(); }
		pointer operator->() const {
			return &reinterpret_cast<const mappatch_t*>(m_me + 1)[m_idx];
		}
		iterator& operator++() {
			++m_idx;
			return *this;
		}
		iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

		friend bool operator== (const iterator& a, const iterator& b) { return a.m_me == b.m_me && a.m_idx == b.m_idx; };
		friend bool operator!= (const iterator& a, const iterator& b) { return a.m_me != b.m_me || a.m_idx != b.m_idx; };
	private:

		const texture_t* const m_me;
		std::uint16_t m_idx;
	};

	iterator begin() const { return iterator{ this, 0 }; }
	iterator end() const { return iterator{ this, m_patchcount }; }

	const mappatch_t& operator[](std::uint16_t idx) const {
		return reinterpret_cast<const mappatch_t*>(this + 1)[idx];
	}
};
#pragma pack(pop)

static_assert(std::has_unique_object_representations_v<mappatch_t> && sizeof(mappatch_t) == 10, "Bug");
static_assert(std::has_unique_object_representations_v<texture_t> && sizeof(texture_t) == 22, "Bug");

struct ptexture_t
{
	std::uint32_t m_numtextures;

	struct iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = const texture_t;
		using pointer = const texture_t*;
		using reference = const texture_t&;

		iterator(const ptexture_t* ptr, std::uint32_t idx) : m_me{ ptr }, m_idx{ idx } {}

		reference operator*() const { return *operator->(); }
		pointer operator->() const {
			return reinterpret_cast<const texture_t*>(reinterpret_cast<const char*>(m_me) + reinterpret_cast<const uint32_t*>(m_me + 1)[m_idx]);
		}
		iterator& operator++() {
			++m_idx;
			return *this;
		}
		iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

		friend bool operator== (const iterator& a, const iterator& b) { return a.m_me == b.m_me && a.m_idx == b.m_idx; };
		friend bool operator!= (const iterator& a, const iterator& b) { return a.m_me != b.m_me || a.m_idx != b.m_idx; };
	private:

		const ptexture_t* const m_me;
		std::uint32_t m_idx;
	};

	iterator begin() const { return iterator{ this, 0 }; }
	iterator end() const { return iterator{ this, m_numtextures }; }

	const texture_t& operator[](std::size_t idx) const {
		return *reinterpret_cast<const texture_t*>(reinterpret_cast<const char*>(this) + reinterpret_cast<const uint32_t*>(this + 1)[idx]);
	}
};

static_assert(std::has_unique_object_representations_v<ptexture_t> && sizeof(ptexture_t) == 4, "Bug");

struct pname_t
{
	std::uint32_t m_numpatchesnames;

	struct iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = const std::array<char, 8>;
		using pointer = const std::array<char, 8>*;
		using reference = const std::array<char, 8>&;

		iterator(const pname_t* ptr, std::uint32_t idx) : m_me{ ptr }, m_idx{ idx } {}

		reference operator*() const { return *operator->(); }
		pointer operator->() const {
			return &reinterpret_cast<const std::array<char, 8>*>(m_me + 1)[m_idx];
		}
		iterator& operator++() {
			++m_idx;
			return *this;
		}
		iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

		friend bool operator== (const iterator& a, const iterator& b) { return a.m_me == b.m_me && a.m_idx == b.m_idx; };
		friend bool operator!= (const iterator& a, const iterator& b) { return a.m_me != b.m_me || a.m_idx != b.m_idx; };
	private:

		const pname_t* const m_me;
		std::uint32_t m_idx;
	};

	iterator begin() const { return iterator{ this, 0 }; }
	iterator end() const { return iterator{ this, m_numpatchesnames }; }

	const std::array<char, 8>& operator[](std::uint32_t idx) const
	{
		return reinterpret_cast<const std::array<char, 8>*>(this + 1)[idx];
	}
};

static_assert(std::has_unique_object_representations_v<pname_t> && sizeof(pname_t) == 4, "Bug");

struct texture_data_t
{
	// std::uint32_t m_texturewidthmask{0};
	// needed for texture pegging
	fixed_t m_textureheight{fixed_t::from_num(0)};
	//std::uint32_t m_texturecompositesize{0};
	std::vector<column_t> m_texturecolumns;
	std::vector<std::unique_ptr<composite_patch_t>> m_texturecomposite;
};

#endif