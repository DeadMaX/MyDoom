#ifndef M_FIXED_H
#define M_FIXED_H

#include <cstdint>
#include <type_traits>
#include <limits>

class fixed_t
{
	std::int32_t m_value;

	constexpr fixed_t(std::int32_t value)
		: m_value{value}
	{}

public:

	constexpr fixed_t &operator +=(const fixed_t &lhs)
	{
		m_value += lhs.m_value;
		return *this;
	}

	constexpr fixed_t &operator -=(const fixed_t& lhs)
	{
		m_value -= lhs.m_value;
		return *this;
	}

	constexpr fixed_t &operator *=(const fixed_t& lhs)
	{
		std::int64_t a = std::int64_t{ m_value };
		std::int64_t b = std::int64_t{ lhs.m_value };
		std::int64_t c = a * b;
		if (c > std::int64_t{ std::numeric_limits<std::int32_t>::max() })
			m_value = std::numeric_limits<std::int32_t>::max();
		else if (c < std::int64_t{ std::numeric_limits<std::int32_t>::lowest() })
			m_value = std::numeric_limits<std::int32_t>::lowest();
		else
			m_value = static_cast<std::int32_t>(c);
		return *this;
	}

	constexpr fixed_t &operator /=(const fixed_t& lhs)
	{
		std::int64_t a = std::int64_t{ m_value } << 16;
		std::int64_t b = std::int64_t{ lhs.m_value };
		std::int64_t c = a / b;
		if (c > std::int64_t{ std::numeric_limits<std::int32_t>::max()})
			m_value = std::numeric_limits<std::int32_t>::max();
		else if (c < std::int64_t{ std::numeric_limits<std::int32_t>::lowest() })
			m_value = std::numeric_limits<std::int32_t>::lowest();
		else
			m_value = static_cast<std::int32_t>(c);
		return *this;
	}

	constexpr bool operator> (const fixed_t& rhs) const
	{
		return m_value > rhs.m_value;
	}

	constexpr bool operator< (const fixed_t& rhs) const
	{
		return m_value < rhs.m_value;
	}

	constexpr bool operator>= (const fixed_t& rhs) const
	{
		return m_value >= rhs.m_value;
	}

	constexpr bool operator<= (const fixed_t& rhs) const
	{
		return m_value <= rhs.m_value;
	}

	constexpr bool operator== (const fixed_t& rhs) const
	{
		return m_value == rhs.m_value;
	}

	constexpr bool operator!= (const fixed_t& rhs) const
	{
		return m_value != rhs.m_value;
	}

	std::int32_t integer() const
	{
		return m_value >> 16;
	}

	constexpr static fixed_t from_num(std::int16_t integer, std::uint16_t fract)
	{
		return fixed_t{ std::int32_t{integer} * (1 << 16) + fract };
	}

	constexpr static fixed_t from_num(std::int32_t num)
	{
		return fixed_t{ num };
	}
};

static_assert(std::has_unique_object_representations_v<fixed_t>, "Invalid compillation");
static_assert(sizeof(fixed_t) == sizeof(std::int32_t), "Invalid compillation");

constexpr fixed_t operator+ (const fixed_t &lhs, const fixed_t &rhs)
{
	fixed_t res{ lhs };
	res += rhs;
	return res;
}

constexpr fixed_t operator- (const fixed_t &lhs, const fixed_t &rhs)
{
	fixed_t res{ lhs };
	res += rhs;
	return res;
}

constexpr fixed_t operator* (const fixed_t &lhs, const fixed_t &rhs)
{
	fixed_t res{ lhs };
	res *= rhs;
	return res;
}

constexpr fixed_t operator/ (const fixed_t &lhs, const fixed_t &rhs)
{
	fixed_t res{ lhs };
	res /= rhs;
	return res;
}

#endif
