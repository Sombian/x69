#pragma once

#include <bit>

#include <cassert>
#include <cstdint>
#include <cassert>

#include <type_traits>

template<typename _, typename T> class new_type_t
{
	friend _; std::decay_t<T> V; // thin? wrapper

public:

	constexpr new_type_t(T _V) noexcept : V {_V} {}

	constexpr  new_type_t() noexcept = default;
	constexpr ~new_type_t() noexcept = default;

	constexpr operator const T&() const noexcept { return V; }
	constexpr operator /*&*/ T&() /*&*/ noexcept { return V; }

	// unary op (pre & post)

	constexpr auto operator+() const noexcept -> _ requires requires { +V; } { return {+V}; }
	constexpr auto operator-() const noexcept -> _ requires requires { -V; } { return {-V}; }
	constexpr auto operator~() const noexcept -> _ requires requires { ~V; } { return {~V}; }

	constexpr auto operator++(int) noexcept -> _ requires requires { V++; } { return {V++}; }
	constexpr auto operator--(int) noexcept -> _ requires requires { V--; } { return {V--}; }

	constexpr auto operator++(   ) noexcept -> _& requires requires { ++V; } { ++V; return static_cast<_&>(*this); }
	constexpr auto operator--(   ) noexcept -> _& requires requires { --V; } { --V; return static_cast<_&>(*this); }

	// binary op (T {op} T)

	friend constexpr auto operator<(_ lhs, _ rhs) noexcept -> bool requires requires { lhs.V < rhs.V; } { return lhs.V < rhs.V; }
	friend constexpr auto operator>(_ lhs, _ rhs) noexcept -> bool requires requires { lhs.V > rhs.V; } { return lhs.V > rhs.V; }

	friend constexpr auto operator==(_ lhs, _ rhs) noexcept -> bool requires requires { lhs.V == rhs.V; } { return lhs.V == rhs.V; }
	friend constexpr auto operator!=(_ lhs, _ rhs) noexcept -> bool requires requires { lhs.V != rhs.V; } { return lhs.V != rhs.V; }
	friend constexpr auto operator<=(_ lhs, _ rhs) noexcept -> bool requires requires { lhs.V <= rhs.V; } { return lhs.V <= rhs.V; }
	friend constexpr auto operator>=(_ lhs, _ rhs) noexcept -> bool requires requires { lhs.V >= rhs.V; } { return lhs.V >= rhs.V; }

	friend constexpr auto operator+(_ lhs, _ rhs) noexcept -> _ requires requires { lhs.V + rhs.V; } { return _(lhs.V + rhs.V); }
	friend constexpr auto operator-(_ lhs, _ rhs) noexcept -> _ requires requires { lhs.V - rhs.V; } { return _(lhs.V - rhs.V); }
	friend constexpr auto operator*(_ lhs, _ rhs) noexcept -> _ requires requires { lhs.V * rhs.V; } { return _(lhs.V * rhs.V); }
	friend constexpr auto operator/(_ lhs, _ rhs) noexcept -> _ requires requires { lhs.V / rhs.V; } { return _(lhs.V / rhs.V); }
	friend constexpr auto operator%(_ lhs, _ rhs) noexcept -> _ requires requires { lhs.V % rhs.V; } { return _(lhs.V % rhs.V); }

	friend constexpr auto operator&(_ lhs, _ rhs) noexcept -> _ requires requires { lhs.V & rhs.V; } { return _(lhs.V & rhs.V); }
	friend constexpr auto operator|(_ lhs, _ rhs) noexcept -> _ requires requires { lhs.V | rhs.V; } { return _(lhs.V | rhs.V); }
	friend constexpr auto operator^(_ lhs, _ rhs) noexcept -> _ requires requires { lhs.V ^ rhs.V; } { return _(lhs.V ^ rhs.V); }

	friend constexpr auto operator<<(_ lhs, _ rhs) noexcept -> _ requires requires { lhs.V << rhs.V; } { return _(lhs.V << rhs.V); }
	friend constexpr auto operator>>(_ lhs, _ rhs) noexcept -> _ requires requires { lhs.V >> rhs.V; } { return _(lhs.V >> rhs.V); }

	// binary op (T {op} ?)

	friend constexpr auto operator<(_ lhs, auto rhs) noexcept -> bool requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V < rhs; }) { return lhs.V < rhs; }
	friend constexpr auto operator>(_ lhs, auto rhs) noexcept -> bool requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V > rhs; }) { return lhs.V > rhs; }

	friend constexpr auto operator==(_ lhs, auto rhs) noexcept -> bool requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V == rhs; }) { return lhs.V == rhs; }
	friend constexpr auto operator!=(_ lhs, auto rhs) noexcept -> bool requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V != rhs; }) { return lhs.V != rhs; }
	friend constexpr auto operator<=(_ lhs, auto rhs) noexcept -> bool requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V <= rhs; }) { return lhs.V <= rhs; }
	friend constexpr auto operator>=(_ lhs, auto rhs) noexcept -> bool requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V >= rhs; }) { return lhs.V >= rhs; }

	friend constexpr auto operator+(_ lhs, auto rhs) noexcept -> _ requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V + rhs; }) { return _(lhs.V + rhs); }
	friend constexpr auto operator-(_ lhs, auto rhs) noexcept -> _ requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V - rhs; }) { return _(lhs.V - rhs); }
	friend constexpr auto operator*(_ lhs, auto rhs) noexcept -> _ requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V * rhs; }) { return _(lhs.V * rhs); }
	friend constexpr auto operator/(_ lhs, auto rhs) noexcept -> _ requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V / rhs; }) { return _(lhs.V / rhs); }
	friend constexpr auto operator%(_ lhs, auto rhs) noexcept -> _ requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V % rhs; }) { return _(lhs.V % rhs); }

	friend constexpr auto operator&(_ lhs, auto rhs) noexcept -> _ requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V & rhs; }) { return _(lhs.V & rhs); }
	friend constexpr auto operator|(_ lhs, auto rhs) noexcept -> _ requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V | rhs; }) { return _(lhs.V | rhs); }
	friend constexpr auto operator^(_ lhs, auto rhs) noexcept -> _ requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V ^ rhs; }) { return _(lhs.V ^ rhs); }

	friend constexpr auto operator<<(_ lhs, auto rhs) noexcept -> _ requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V << rhs; }) { return _(lhs.V << rhs); }
	friend constexpr auto operator>>(_ lhs, auto rhs) noexcept -> _ requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V >> rhs; }) { return _(lhs.V >> rhs); }

	// binary op (? {op} T)

	friend constexpr auto operator<(auto lhs, _ rhs) noexcept -> bool requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs < rhs.V; }) { return lhs < rhs.V; }
	friend constexpr auto operator>(auto lhs, _ rhs) noexcept -> bool requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs > rhs.V; }) { return lhs > rhs.V; }

	friend constexpr auto operator==(auto lhs, _ rhs) noexcept -> bool requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs == rhs.V; }) { return lhs == rhs.V; }
	friend constexpr auto operator!=(auto lhs, _ rhs) noexcept -> bool requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs != rhs.V; }) { return lhs != rhs.V; }
	friend constexpr auto operator<=(auto lhs, _ rhs) noexcept -> bool requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs <= rhs.V; }) { return lhs <= rhs.V; }
	friend constexpr auto operator>=(auto lhs, _ rhs) noexcept -> bool requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs >= rhs.V; }) { return lhs >= rhs.V; }

	friend constexpr auto operator+(auto lhs, _ rhs) noexcept -> decltype(lhs) requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs + rhs.V; }) { return lhs + rhs.V; }
	friend constexpr auto operator-(auto lhs, _ rhs) noexcept -> decltype(lhs) requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs - rhs.V; }) { return lhs - rhs.V; }
	friend constexpr auto operator*(auto lhs, _ rhs) noexcept -> decltype(lhs) requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs * rhs.V; }) { return lhs * rhs.V; }
	friend constexpr auto operator/(auto lhs, _ rhs) noexcept -> decltype(lhs) requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs / rhs.V; }) { return lhs / rhs.V; }
	friend constexpr auto operator%(auto lhs, _ rhs) noexcept -> decltype(lhs) requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs % rhs.V; }) { return lhs % rhs.V; }

	friend constexpr auto operator&(auto lhs, _ rhs) noexcept -> decltype(lhs) requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs & rhs.V; }) { return lhs & rhs.V; }
	friend constexpr auto operator|(auto lhs, _ rhs) noexcept -> decltype(lhs) requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs | rhs.V; }) { return lhs | rhs.V; }
	friend constexpr auto operator^(auto lhs, _ rhs) noexcept -> decltype(lhs) requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs ^ rhs.V; }) { return lhs ^ rhs.V; }

	friend constexpr auto operator<<(auto lhs, _ rhs) noexcept -> decltype(lhs) requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs << rhs.V; }) { return lhs << rhs.V; }
	friend constexpr auto operator>>(auto lhs, _ rhs) noexcept -> decltype(lhs) requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs >> rhs.V; }) { return lhs >> rhs.V; }

	// assign op (T {op}= T)

	friend constexpr auto operator+=(_& lhs, _ rhs) noexcept -> _& requires requires { lhs.V += rhs.V; } { lhs.V += rhs.V; return lhs; }
	friend constexpr auto operator-=(_& lhs, _ rhs) noexcept -> _& requires requires { lhs.V -= rhs.V; } { lhs.V -= rhs.V; return lhs; }
	friend constexpr auto operator*=(_& lhs, _ rhs) noexcept -> _& requires requires { lhs.V *= rhs.V; } { lhs.V *= rhs.V; return lhs; }
	friend constexpr auto operator/=(_& lhs, _ rhs) noexcept -> _& requires requires { lhs.V /= rhs.V; } { lhs.V /= rhs.V; return lhs; }
	friend constexpr auto operator%=(_& lhs, _ rhs) noexcept -> _& requires requires { lhs.V %= rhs.V; } { lhs.V %= rhs.V; return lhs; }

	friend constexpr auto operator&=(_& lhs, _ rhs) noexcept -> _& requires requires { lhs.V &= rhs.V; } { lhs.V &= rhs.V; return lhs; }
	friend constexpr auto operator|=(_& lhs, _ rhs) noexcept -> _& requires requires { lhs.V |= rhs.V; } { lhs.V |= rhs.V; return lhs; }
	friend constexpr auto operator^=(_& lhs, _ rhs) noexcept -> _& requires requires { lhs.V ^= rhs.V; } { lhs.V ^= rhs.V; return lhs; }

	friend constexpr auto operator<<=(_& lhs, _ rhs) noexcept -> _& requires requires { lhs.V <<= rhs.V; } { lhs.V <<= rhs.V; return lhs; }
	friend constexpr auto operator>>=(_& lhs, _ rhs) noexcept -> _& requires requires { lhs.V >>= rhs.V; } { lhs.V >>= rhs.V; return lhs; }

	// assign op (T {op}= ?)

	friend constexpr auto operator+=(_& lhs, auto rhs) noexcept -> _& requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V += rhs; }) { lhs.V += rhs; return lhs; }
	friend constexpr auto operator-=(_& lhs, auto rhs) noexcept -> _& requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V -= rhs; }) { lhs.V -= rhs; return lhs; }
	friend constexpr auto operator*=(_& lhs, auto rhs) noexcept -> _& requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V *= rhs; }) { lhs.V *= rhs; return lhs; }
	friend constexpr auto operator/=(_& lhs, auto rhs) noexcept -> _& requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V /= rhs; }) { lhs.V /= rhs; return lhs; }
	friend constexpr auto operator%=(_& lhs, auto rhs) noexcept -> _& requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V %= rhs; }) { lhs.V %= rhs; return lhs; }

	friend constexpr auto operator&=(_& lhs, auto rhs) noexcept -> _& requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V &= rhs; }) { lhs.V &= rhs; return lhs; }
	friend constexpr auto operator|=(_& lhs, auto rhs) noexcept -> _& requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V |= rhs; }) { lhs.V |= rhs; return lhs; }
	friend constexpr auto operator^=(_& lhs, auto rhs) noexcept -> _& requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V ^= rhs; }) { lhs.V ^= rhs; return lhs; }

	friend constexpr auto operator<<=(_& lhs, auto rhs) noexcept -> _& requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V <<= rhs; }) { lhs.V <<= rhs; return lhs; }
	friend constexpr auto operator>>=(_& lhs, auto rhs) noexcept -> _& requires (!std::is_same_v<std::remove_cvref_t<decltype(rhs)>, _> && requires { lhs.V >>= rhs; }) { lhs.V >>= rhs; return lhs; }

	// assign op (? {op}= T)

	friend constexpr auto operator+=(auto& lhs, _ rhs) noexcept -> decltype(lhs)& requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs += rhs.V; }) { lhs += rhs.V; return lhs; }
	friend constexpr auto operator-=(auto& lhs, _ rhs) noexcept -> decltype(lhs)& requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs -= rhs.V; }) { lhs -= rhs.V; return lhs; }
	friend constexpr auto operator*=(auto& lhs, _ rhs) noexcept -> decltype(lhs)& requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs *= rhs.V; }) { lhs *= rhs.V; return lhs; }
	friend constexpr auto operator/=(auto& lhs, _ rhs) noexcept -> decltype(lhs)& requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs /= rhs.V; }) { lhs /= rhs.V; return lhs; }
	friend constexpr auto operator%=(auto& lhs, _ rhs) noexcept -> decltype(lhs)& requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs %= rhs.V; }) { lhs %= rhs.V; return lhs; }

	friend constexpr auto operator&=(auto& lhs, _ rhs) noexcept -> decltype(lhs)& requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs &= rhs.V; }) { lhs &= rhs.V; return lhs; }
	friend constexpr auto operator|=(auto& lhs, _ rhs) noexcept -> decltype(lhs)& requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs |= rhs.V; }) { lhs |= rhs.V; return lhs; }
	friend constexpr auto operator^=(auto& lhs, _ rhs) noexcept -> decltype(lhs)& requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs ^= rhs.V; }) { lhs ^= rhs.V; return lhs; }

	friend constexpr auto operator<<=(auto& lhs, _ rhs) noexcept -> decltype(lhs)& requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs <<= rhs.V; }) { lhs <<= rhs.V; return lhs; }
	friend constexpr auto operator>>=(auto& lhs, _ rhs) noexcept -> decltype(lhs)& requires (!std::is_same_v<std::remove_cvref_t<decltype(lhs)>, _> && requires { lhs >>= rhs.V; }) { lhs >>= rhs.V; return lhs; }
};

#define NEW_TYPE(NAME, T, ...) struct NAME final : public new_type_t<NAME, T> \
{                                                                             \
    /* inherit constructor from super T */                                    \
    using new_type_t<NAME, T>::new_type_t;                                    \
                                                                              \
    __VA_ARGS__                                                               \
}                                                                             \

namespace x69
{
	constexpr bool IS_LE {std::endian::native == std::endian::little};
	constexpr bool IS_BE {std::endian::native != std::endian::little};

	// std::size_t
	typedef intptr_t isize;
	typedef uintptr_t usize;
	
	// floating point
	typedef float f32;
	typedef double f64;
	
	// signed integer
	typedef std::int8_t i8;
	typedef std::int16_t i16;
	typedef std::int32_t i32;
	typedef std::int64_t i64;
	
	// unsigned integer
	typedef std::uint8_t u8;
	typedef std::uint16_t u16;
	typedef std::uint32_t u32;
	typedef std::uint64_t u64;
	
	// for LLDB debugger
	typedef unsigned long long WORD;
	typedef unsigned      char BYTE;
	
	static_assert(sizeof(i8) == 1);
	static_assert(sizeof(i16) == 2);
	static_assert(sizeof(i32) == 4);
	static_assert(sizeof(i64) == 8);
	
	static_assert(sizeof(u8) == 1);
	static_assert(sizeof(u16) == 2);
	static_assert(sizeof(u32) == 4);
	static_assert(sizeof(u64) == 8);
	
	static_assert(sizeof(f32) == 4);
	static_assert(sizeof(f64) == 8);

#ifdef POISON
	#if defined(__GNUC__)
		#pragma GCC poison size_t
		#pragma GCC poison int8_t
		#pragma GCC poison int16_t
		#pragma GCC poison int32_t
		#pragma GCC poison int64_t
		#pragma GCC poison uint8_t
		#pragma GCC poison uint16_t
		#pragma GCC poison uint32_t
		#pragma GCC poison uint64_t
	#elif defined(__clang__)
		#pragma clang poison size_t
		#pragma clang poison int8_t
		#pragma clang poison int16_t
		#pragma clang poison int32_t
		#pragma clang poison int64_t
		#pragma clang poison uint8_t
		#pragma clang poison uint16_t
		#pragma clang poison uint32_t
		#pragma clang poison uint64_t
	#elif defined(_MSC_VER)
		#pragma warning(error : 4995)
		#pragma deprecated(size_t)
		#pragma deprecated(int8_t)
		#pragma deprecated(int16_t)
		#pragma deprecated(int32_t)
		#pragma deprecated(int64_t)
		#pragma deprecated(uint8_t)
		#pragma deprecated(uint16_t)
		#pragma deprecated(uint32_t)
		#pragma deprecated(uint64_t)
	#endif
#endif

	enum class magic : u8
	{
		N, // range's tail
	};

	inline namespace sentinel
	{
		NEW_TYPE(clamp, usize);

		inline constexpr auto operator-([[maybe_unused]] magic lhs,
		                                [[maybe_unused]] usize rhs) noexcept -> clamp { return {0x0 + rhs}; }

		inline constexpr auto operator-([[maybe_unused]] clamp lhs,
		                                [[maybe_unused]] usize rhs) noexcept -> clamp { return {lhs + rhs}; }
	}
}
