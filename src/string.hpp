#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>

#include <bit>
#include <ios>
#include <tuple>
#include <memory>
#include <vector>
#include <variant>
#include <utility>
#include <ostream>
#include <fstream>
#include <iterator>
#include <optional>
#include <algorithm>
#include <filesystem>
#include <type_traits>

// assumption; utf-8 terminal code page
inline auto operator<<(std::ostream& os, char32_t code) noexcept -> decltype(os)
{
	char out[4]; short unit {0};

	if (code <= 0x00007F)
	{
		out[unit++] = static_cast<char>(code /* safe to truncate */);
	}
	else if (code <= 0x0007FF)
	{
		out[unit++] = static_cast<char>(0xC0 | ((code >> 06) & 0x1F));
		out[unit++] = static_cast<char>(0x80 | ((code >> 00) & 0x3F));
	}
	else if (code <= 0x00FFFF)
	{
		out[unit++] = static_cast<char>(0xE0 | ((code >> 12) & 0x0F));
		out[unit++] = static_cast<char>(0x80 | ((code >> 06) & 0x3F));
		out[unit++] = static_cast<char>(0x80 | ((code >> 00) & 0x3F));
	}
	else if (code <= 0x10FFFF)
	{
		out[unit++] = static_cast<char>(0xF0 | ((code >> 18) & 0x07));
		out[unit++] = static_cast<char>(0x80 | ((code >> 12) & 0x3F));
		out[unit++] = static_cast<char>(0x80 | ((code >> 06) & 0x3F));
		out[unit++] = static_cast<char>(0x80 | ((code >> 00) & 0x3F));
	}
	return os.write(out, unit);
}

namespace utf
{

#define COPY_CONSTRUCTOR($T) constexpr $T(const $T&  other) noexcept
#define MOVE_CONSTRUCTOR($T) constexpr $T(/*&*/ $T&& other) noexcept

#define COPY_ASSIGNMENT($T) constexpr auto operator=(const $T&  rhs) noexcept -> $T&
#define MOVE_ASSIGNMENT($T) constexpr auto operator=(/*&*/ $T&& rhs) noexcept -> $T&

//┌──────────────────────────────────────────────────────────────┐
//│ special thanks to facebook's folly::FBString.                │
//│                                                              │
//│ SSO mode uses every bytes of heap string struct using union  │
//│ this was achievable thanks to the very clever memory layout. │
//│                                                              │
//│ for more, watch https://www.youtube.com/watch?v=kPR8h4-qZdk. │
//└──────────────────────────────────────────────────────────────┘

#define __OWNED__(name) const str<alien,arena>   &   name
#define __SLICE__(name) /*&*/ txt<alien /*&*/> /*&*/ name
#define __EQSTR__(name) const unit_t   (&name)[N]
#define __08STR__(name) const char8_t  (&name)[N]
#define __16STR__(name) const char16_t (&name)[N]
#define __32STR__(name) const char32_t (&name)[N]

template <size_t N> struct format_t { /**/ char _[N];
constexpr format_t(const char (&str)[N]) noexcept
{ std::ranges::copy(/**/ str /**/, this->_); }
[[nodiscard("?")]] constexpr auto operator==(const
format_t&) const noexcept -> bool = default; /**/
[[nodiscard("?")]] constexpr auto operator!=(const
format_t&) const noexcept -> bool = default; /**/
template <size_t U> constexpr auto operator==(const
format_t<U>&) const noexcept -> bool { return 0; };
template <size_t U> constexpr auto operator!=(const
format_t<U>&) const noexcept -> bool { return 1; }; };


enum class range : uint8_t {N};
struct clamp { const size_t _;
inline constexpr /**/ operator
size_t() const { return _; } };
inline constexpr auto operator-
(range, size_t offset) noexcept
-> clamp { return { offset }; }

template <format_t name> struct codec
{
	static_assert(false, "segfault");
};

// https://en.wikipedia.org/wiki/ASCII
template <> struct codec<"ASCII">
{
	static constexpr const bool is_variable {static_cast<bool>(0)};
	static constexpr const bool is_stateful {static_cast<bool>(0)};
	static constexpr const bool is_windable {static_cast<bool>(1)};

	using unit_t = char;

	constexpr  codec() noexcept = delete;
	constexpr ~codec() noexcept = delete;

	static constexpr auto size(const char32_t code) noexcept -> int8_t;
	static constexpr auto next(const unit_t* data) noexcept -> int8_t;
	static constexpr auto back(const unit_t* data) noexcept -> int8_t;

	static constexpr auto // transform a code point into code units.
	encode(const char32_t in, unit_t* out, int8_t step) noexcept -> void;
	static constexpr auto // transform code units into a code point.
	decode(const unit_t* in, char32_t& out, int8_t step) noexcept -> void;
};

// https://en.wikipedia.org/wiki/UTF-8
template <> struct codec<"UTF-8">
{
	static constexpr const bool is_variable {static_cast<bool>(1)};
	static constexpr const bool is_stateful {static_cast<bool>(0)};
	static constexpr const bool is_windable {static_cast<bool>(1)};

	using unit_t = char8_t;

	constexpr  codec() noexcept = delete;
	constexpr ~codec() noexcept = delete;

	static constexpr auto size(const char32_t code) noexcept -> int8_t;
	static constexpr auto next(const unit_t* data) noexcept -> int8_t;
	static constexpr auto back(const unit_t* data) noexcept -> int8_t;

	static constexpr auto // transform a code point into code units.
	encode(const char32_t in, unit_t* out, int8_t step) noexcept -> void;
	static constexpr auto // transform code units into a code point.
	decode(const unit_t* in, char32_t& out, int8_t step) noexcept -> void;
};

// https://en.wikipedia.org/wiki/UTF-16
template <> struct codec<"UTF-16">
{
	static constexpr const bool is_variable {static_cast<bool>(1)};
	static constexpr const bool is_stateful {static_cast<bool>(0)};
	static constexpr const bool is_windable {static_cast<bool>(1)};

	using unit_t = char16_t;

	constexpr  codec() noexcept = delete;
	constexpr ~codec() noexcept = delete;

	static constexpr auto size(const char32_t code) noexcept -> int8_t;
	static constexpr auto next(const unit_t* data) noexcept -> int8_t;
	static constexpr auto back(const unit_t* data) noexcept -> int8_t;

	static constexpr auto // transform a code point into code units.
	encode(const char32_t in, unit_t* out, int8_t step) noexcept -> void;
	static constexpr auto // transform code units into a code point.
	decode(const unit_t* in, char32_t& out, int8_t step) noexcept -> void;
};

// https://en.wikipedia.org/wiki/UTF-32
template <> struct codec<"UTF-32">
{
	static constexpr const bool is_variable {static_cast<bool>(0)};
	static constexpr const bool is_stateful {static_cast<bool>(0)};
	static constexpr const bool is_windable {static_cast<bool>(1)};

	using unit_t = char32_t;

	constexpr  codec() noexcept = delete;
	constexpr ~codec() noexcept = delete;

	static constexpr auto size(const char32_t code) noexcept -> int8_t;
	static constexpr auto next(const unit_t* data) noexcept -> int8_t;
	static constexpr auto back(const unit_t* data) noexcept -> int8_t;

	static constexpr auto // transform a code point into code units.
	encode(const char32_t in, unit_t* out, int8_t step) noexcept -> void;
	static constexpr auto // transform code units into a code point.
	decode(const unit_t* in, char32_t& out, int8_t step) noexcept -> void;
};

template <format_t local, typename alloc = std::allocator<typename codec<local>::unit_t>> class str;
template <format_t local /* unicode aware std::string_view; encourages pass by value! */> class txt;

//┌───────┬───────┬────────────┬─────────────────┐
//│ class │ owns? │ null-term? │ use-after-free? │
//├───────┼───────┼────────────┼─────────────────┤
//│ [str] │   T   │   always   │      safe       │
//├───────┼───────┼────────────┼─────────────────┤
//│ [txt] │   F   │   maybe?   │      [UB]       │
//└───────┴───────┴────────────┴─────────────────┘

template <typename model, format_t local> class API
{
	template <typename, format_t> friend class API;

	//┌────────────────────────────────────────────────┐
	//│ note: helper funcs are not encapsulated within │
	//│       and it was deliberate; to cut bin bloat. │
	//└────────────────────────────────────────────────┘

	using unit_t = typename codec<local>::unit_t;

	constexpr auto head() const noexcept -> const unit_t*;
	constexpr auto tail() const noexcept -> const unit_t*;

	template <typename, typename> class concat;
	                              class const_forward_iterator;
	                              class const_reverse_iterator;

public:
	
	// returns the number of code units, excluding NULL-TERMINATOR.
	constexpr auto size() const noexcept -> size_t;
	// returns the number of code points, excluding NULL-TERMINATOR.
	constexpr auto length() const noexcept -> size_t;

	// *self explanatory* returns whether or not it starts with *parameter*.
	template <format_t alien, typename arena>
	constexpr auto starts_with(__OWNED__(value)) const noexcept -> bool;
	template <format_t alien /* can't own */>
	constexpr auto starts_with(__SLICE__(value)) const noexcept -> bool;
	template <size_t                       N>
	constexpr auto starts_with(__EQSTR__(value)) const noexcept -> bool requires (std::is_same_v<unit_t, char>);
	template <size_t                       N>
	constexpr auto starts_with(__08STR__(value)) const noexcept -> bool /* encoding of char8_t is trivial */;
	template <size_t                       N>
	constexpr auto starts_with(__16STR__(value)) const noexcept -> bool /* encoding of char16_t is trivial */;
	template <size_t                       N>
	constexpr auto starts_with(__32STR__(value)) const noexcept -> bool /* encoding of char32_t is trivial */;

	// *self explanatory* returns whether or not it ends with *parameter*.
	template <format_t alien, typename arena>
	constexpr auto ends_with(__OWNED__(value)) const noexcept -> bool;
	template <format_t alien /* can't own */>
	constexpr auto ends_with(__SLICE__(value)) const noexcept -> bool;
	template <size_t                       N>
	constexpr auto ends_with(__EQSTR__(value)) const noexcept -> bool requires (std::is_same_v<unit_t, char>);
	template <size_t                       N>
	constexpr auto ends_with(__08STR__(value)) const noexcept -> bool /* encoding of char8_t is trivial */;
	template <size_t                       N>
	constexpr auto ends_with(__16STR__(value)) const noexcept -> bool /* encoding of char16_t is trivial */;
	template <size_t                       N>
	constexpr auto ends_with(__32STR__(value)) const noexcept -> bool /* encoding of char32_t is trivial */;

	// *self explanatory* returns whether or not it contains *parameter*.
	template <format_t alien, typename arena>
	constexpr auto contains(__OWNED__(value)) const noexcept -> size_t;
	template <format_t alien /* can't own */>
	constexpr auto contains(__SLICE__(value)) const noexcept -> size_t;
	template <size_t                       N>
	constexpr auto contains(__EQSTR__(value)) const noexcept -> size_t requires (std::is_same_v<unit_t, char>);
	template <size_t                       N>
	constexpr auto contains(__08STR__(value)) const noexcept -> size_t /* encoding of char8_t is trivial */;
	template <size_t                       N>
	constexpr auto contains(__16STR__(value)) const noexcept -> size_t /* encoding of char16_t is trivial */;
	template <size_t                       N>
	constexpr auto contains(__32STR__(value)) const noexcept -> size_t /* encoding of char32_t is trivial */;

	// returns a list of string slice, of which is a product of split aka division.
	template <format_t alien, typename arena>
	constexpr auto split(__OWNED__(value)) const noexcept -> std::vector<txt<local>>;
	template <format_t alien /* can't own */>
	constexpr auto split(__SLICE__(value)) const noexcept -> std::vector<txt<local>>;
	template <size_t                       N>
	constexpr auto split(__EQSTR__(value)) const noexcept -> std::vector<txt<local>> requires (std::is_same_v<unit_t, char>);
	template <size_t                       N>
	constexpr auto split(__08STR__(value)) const noexcept -> std::vector<txt<local>> /* encoding of char8_t is trivial */;
	template <size_t                       N>
	constexpr auto split(__16STR__(value)) const noexcept -> std::vector<txt<local>> /* encoding of char16_t is trivial */;
	template <size_t                       N>
	constexpr auto split(__32STR__(value)) const noexcept -> std::vector<txt<local>> /* encoding of char32_t is trivial */;

	// returns a list of string slice, of which is a product of search occurrence.
	template <format_t alien, typename arena>
	constexpr auto match(__OWNED__(value)) const noexcept -> std::vector<txt<local>>;
	template <format_t alien /* can't own */>
	constexpr auto match(__SLICE__(value)) const noexcept -> std::vector<txt<local>>;
	template <size_t                       N>
	constexpr auto match(__EQSTR__(value)) const noexcept -> std::vector<txt<local>> requires (std::is_same_v<unit_t, char>);
	template <size_t                       N>
	constexpr auto match(__08STR__(value)) const noexcept -> std::vector<txt<local>> /* encoding of char8_t is trivial */;
	template <size_t                       N>
	constexpr auto match(__16STR__(value)) const noexcept -> std::vector<txt<local>> /* encoding of char16_t is trivial */;
	template <size_t                       N>
	constexpr auto match(__32STR__(value)) const noexcept -> std::vector<txt<local>> /* encoding of char32_t is trivial */;

	// returns a slice, of which is a product of substring. N is a sentinel value.
	constexpr auto substr(clamp  start, clamp  until) const noexcept -> txt<local>;
	constexpr auto substr(clamp  start, range  until) const noexcept -> txt<local>;
	constexpr auto substr(size_t start, clamp  until) const noexcept -> txt<local>;
	constexpr auto substr(size_t start, range  until) const noexcept -> txt<local>;
	constexpr auto substr(size_t start, size_t until) const noexcept -> txt<local>;

	// iterator

	constexpr auto begin() const noexcept -> const_forward_iterator;
	constexpr auto end() const noexcept -> const_forward_iterator;

	constexpr auto rbegin() const noexcept -> const_reverse_iterator;
	constexpr auto rend() const noexcept -> const_reverse_iterator;

	// operators

	constexpr auto operator[](size_t value) const noexcept -> decltype(auto);
	constexpr auto operator[](size_t value) /*&*/ noexcept -> decltype(auto);

	template <format_t alien, typename arena>
	constexpr auto operator==(__OWNED__(rhs)) const noexcept -> bool;
	template <format_t alien /* can't own */>
	constexpr auto operator==(__SLICE__(rhs)) const noexcept -> bool;
	template <size_t                       N>
	constexpr auto operator==(__EQSTR__(rhs)) const noexcept -> bool requires (std::is_same_v<unit_t, char>);
	template <size_t                       N>
	constexpr auto operator==(__08STR__(rhs)) const noexcept -> bool /* encoding of char8_t is trivial */;
	template <size_t                       N>
	constexpr auto operator==(__16STR__(rhs)) const noexcept -> bool /* encoding of char16_t is trivial */;
	template <size_t                       N>
	constexpr auto operator==(__32STR__(rhs)) const noexcept -> bool /* encoding of char32_t is trivial */;

	template <format_t alien, typename arena>
	constexpr auto operator!=(__OWNED__(rhs)) const noexcept -> bool;
	template <format_t alien /* can't own */>
	constexpr auto operator!=(__SLICE__(rhs)) const noexcept -> bool;
	template <size_t                       N>
	constexpr auto operator!=(__EQSTR__(rhs)) const noexcept -> bool requires (std::is_same_v<unit_t, char>);
	template <size_t                       N>
	constexpr auto operator!=(__08STR__(rhs)) const noexcept -> bool /* encoding of char8_t is trivial */;
	template <size_t                       N>
	constexpr auto operator!=(__16STR__(rhs)) const noexcept -> bool /* encoding of char16_t is trivial */;
	template <size_t                       N>
	constexpr auto operator!=(__32STR__(rhs)) const noexcept -> bool /* encoding of char32_t is trivial */;

	template <format_t alien, typename arena>
	constexpr auto operator+(__OWNED__(rhs)) const noexcept -> concat<txt<local>, txt<alien>>;
	template <format_t alien /* can't own */>
	constexpr auto operator+(__SLICE__(rhs)) const noexcept -> concat<txt<local>, txt<alien>>;
	template <size_t                       N>
	constexpr auto operator+(__EQSTR__(rhs)) const noexcept -> concat<txt<local>, txt<local>> requires (std::is_same_v<unit_t, char>);
	template <size_t                       N>
	constexpr auto operator+(__08STR__(rhs)) const noexcept -> concat<txt<local>, txt<"UTF-8">> /* encoding of char8_t is trivial */;
	template <size_t                       N>
	constexpr auto operator+(__16STR__(rhs)) const noexcept -> concat<txt<local>, txt<"UTF-16">> /* encoding of char16_t is trivial */;
	template <size_t                       N>
	constexpr auto operator+(__32STR__(rhs)) const noexcept -> concat<txt<local>, txt<"UTF-32">> /* encoding of char32_t is trivial */;

	// reverse operators

	template <size_t N, format_t alien, typename arena>
	friend constexpr auto operator+(__08STR__(lhs), __OWNED__(rhs)) noexcept -> concat<txt<"UTF-8">, txt<alien>>
	{
		return {{lhs}, {rhs}};
	}

	template <size_t N, format_t alien, typename arena>
	friend constexpr auto operator+(__16STR__(lhs), __OWNED__(rhs)) noexcept -> concat<txt<"UTF-16">, txt<alien>>
	{
		return {{lhs}, {rhs}};
	}

	template <size_t N, format_t alien, typename arena>
	friend constexpr auto operator+(__32STR__(lhs), __OWNED__(rhs)) noexcept -> concat<txt<"UTF-32">, txt<alien>>
	{
		return {{lhs}, {rhs}};
	}

	template <size_t N, format_t alien /* can't own */>
	friend constexpr auto operator+(__08STR__(lhs), __SLICE__(rhs)) noexcept -> concat<txt<"UTF-8">, txt<alien>>
	{
		return {{lhs}, {rhs}};
	}

	template <size_t N, format_t alien /* can't own */>
	friend constexpr auto operator+(__16STR__(lhs), __SLICE__(rhs)) noexcept -> concat<txt<"UTF-16">, txt<alien>>
	{
		return {{lhs}, {rhs}};
	}

	template <size_t N, format_t alien /* can't own */>
	friend constexpr auto operator+(__32STR__(lhs), __SLICE__(rhs)) noexcept -> concat<txt<"UTF-32">, txt<alien>>
	{
		return {{lhs}, {rhs}};
	}

	template <size_t N, typename lhs_t, typename rhs_t>
	friend constexpr auto operator+(__08STR__(lhs), const concat<lhs_t, rhs_t>& rhs) noexcept -> concat<txt<"UTF-8">, concat<lhs_t, rhs_t>>
	{
		return {{lhs}, {rhs}};
	}

	template <size_t N, typename lhs_t, typename rhs_t>
	friend constexpr auto operator+(__16STR__(lhs), const concat<lhs_t, rhs_t>& rhs) noexcept -> concat<txt<"UTF-16">, concat<lhs_t, rhs_t>>
	{
		return {{lhs}, {rhs}};
	}

	template <size_t N, typename lhs_t, typename rhs_t>
	friend constexpr auto operator+(__32STR__(lhs), const concat<lhs_t, rhs_t>& rhs) noexcept -> concat<txt<"UTF-32">, concat<lhs_t, rhs_t>>
	{
		return {{lhs}, {rhs}};
	}

private:

	template <typename lhs_t, typename rhs_t> class concat
	{
		const lhs_t lhs;
		const rhs_t rhs;

	public:

		constexpr concat
		(
			decltype(lhs) lhs,
			decltype(rhs) rhs
		)
		noexcept : lhs {lhs},
		           rhs {rhs}
		{}

		template <format_t alien, typename arena>
		constexpr /**/ operator str<alien, arena>() const noexcept;

		// operators

		template <format_t alien, typename arena>
		constexpr auto operator+(__OWNED__(rhs)) noexcept -> concat<concat, txt<alien>>;

		template <format_t alien /* can't own */>
		constexpr auto operator+(__SLICE__(rhs)) noexcept -> concat<concat, txt<alien>>;

		template <size_t                       N>
		constexpr auto operator+(__08STR__(rhs)) noexcept -> concat<concat, txt<"UTF-8">>;

		template <size_t                       N>
		constexpr auto operator+(__16STR__(rhs)) noexcept -> concat<concat, txt<"UTF-16">>;

		template <size_t                       N>
		constexpr auto operator+(__32STR__(rhs)) noexcept -> concat<concat, txt<"UTF-32">>;

	private:

		constexpr auto __for_each__(const auto&& fun) const noexcept -> void;
	};

	class const_forward_iterator
	{
		const unit_t* ptr;

	public:

		using iterator_category = std::bidirectional_iterator_tag;
		using iterator_concept = std::bidirectional_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = char32_t;
		using reference = char32_t;

		constexpr const_forward_iterator
		(
			decltype(ptr) ptr
		)
		noexcept : ptr {ptr}
		{}

		constexpr operator const unit_t*() const noexcept;
		constexpr operator const unit_t*() /*&*/ noexcept;

		// stl compat; default constructible
		constexpr  const_forward_iterator() noexcept = default;
		constexpr ~const_forward_iterator() noexcept = default;

		constexpr auto operator*() const noexcept -> value_type;

		constexpr auto operator++(   ) noexcept -> const_forward_iterator&;
		constexpr auto operator++(int) noexcept -> const_forward_iterator;

		constexpr auto operator--(   ) noexcept -> const_forward_iterator&;
		constexpr auto operator--(int) noexcept -> const_forward_iterator;

		constexpr auto operator+(size_t value) noexcept -> const_forward_iterator;
		constexpr auto operator-(size_t value) noexcept -> const_forward_iterator;

		constexpr auto operator+=(size_t value) noexcept -> const_forward_iterator&;
		constexpr auto operator-=(size_t value) noexcept -> const_forward_iterator&;

		constexpr auto operator==(const const_forward_iterator& rhs) const noexcept -> bool = default;
		constexpr auto operator!=(const const_forward_iterator& rhs) const noexcept -> bool = default;
	};

	class const_reverse_iterator
	{
		const unit_t* ptr;

	public:

		using iterator_category = std::bidirectional_iterator_tag;
		using iterator_concept = std::bidirectional_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = char32_t;
		using reference = char32_t;

		constexpr const_reverse_iterator
		(
			decltype(ptr) ptr
		)
		noexcept : ptr {ptr}
		{}

		constexpr operator const unit_t*() const noexcept;
		constexpr operator const unit_t*() /*&*/ noexcept;

		// stl compat; default constructible
		constexpr  const_reverse_iterator() noexcept = default;
		constexpr ~const_reverse_iterator() noexcept = default;

		constexpr auto operator*() const noexcept -> value_type;

		constexpr auto operator++(   ) noexcept -> const_reverse_iterator&;
		constexpr auto operator++(int) noexcept -> const_reverse_iterator;

		constexpr auto operator--(   ) noexcept -> const_reverse_iterator&;
		constexpr auto operator--(int) noexcept -> const_reverse_iterator;

		constexpr auto operator+(size_t value) noexcept -> const_reverse_iterator;
		constexpr auto operator-(size_t value) noexcept -> const_reverse_iterator;

		constexpr auto operator+=(size_t value) noexcept -> const_reverse_iterator&;
		constexpr auto operator-=(size_t value) noexcept -> const_reverse_iterator&;

		constexpr auto operator==(const const_reverse_iterator& rhs) const noexcept -> bool = default;
		constexpr auto operator!=(const const_reverse_iterator& rhs) const noexcept -> bool = default;
	};
};

template <format_t local, typename alloc> class str : public API<str<local,alloc>, local>
{
	template <typename,format_t> friend class API;
	template <format_t,typename> friend class str;
	template <format_t /*none*/> friend class txt;

	using unit_t = typename codec<local>::unit_t;

	#define IS_LITTLE       \
	(                       \
	    std::endian::native \
	             ==         \
	    std::endian::little \
	)                       \

	enum mode_t : uint8_t
	{
		SMALL = IS_LITTLE
		        ?
		        0b00000000
		        :
		        0b00000000,

		LARGE = IS_LITTLE
		        ?
		        0b10000000
		        :
		        0b00000001,
	};

	struct buffer
	{
		unit_t* head;
		unit_t* last;
		size_t  size : (sizeof(size_t) * 8) - (sizeof(mode_t) * 8);
		size_t  meta : (sizeof(mode_t) * 8) - (sizeof(mode_t) * 0);

		[[nodiscard]] constexpr operator const unit_t*() const noexcept;
		[[nodiscard]] constexpr operator /*&*/ unit_t*() /*&*/ noexcept;
	};

	static constexpr const uint8_t MAX {            (sizeof(buffer) - 1) / (sizeof(unit_t) * 1)};
	static constexpr const uint8_t RMB {            (sizeof(buffer) - 1) * (         1        )};
	static constexpr const uint8_t SFT {IS_LITTLE ? (         0        ) : (         1        )};
	static constexpr const uint8_t MSK {IS_LITTLE ? (0b10000000        ) : (0b00000001        )};

	//┌───────────────────────────┐
	//│           small           │
	//├──────┬──────┬──────┬──────┤
	//│ head │ last │ size │ meta │
	//├──────┴──────┴──────┴──────┤
	//│           bytes           │
	//└───────────────────────────┘

	struct storage : public alloc
	{
		union
		{
			buffer large;

			unit_t small
			[sizeof(buffer) / sizeof(unit_t)];

			uint8_t bytes
			[sizeof(buffer) / sizeof(uint8_t)];
		}
		__union__ { .small {0} };

		constexpr  storage() noexcept;
		constexpr ~storage() noexcept;

		// single source of truth; category.
		constexpr auto mode() const noexcept -> mode_t;
		constexpr auto mode() /*&*/ noexcept -> mode_t;
	};
	#undef IS_LITTLE

	// returns ptr to buffer's 1st element.
	constexpr auto __head__() const noexcept -> const unit_t*;
	constexpr auto __head__() /*&*/ noexcept -> /*&*/ unit_t*;

	// returns ptr to buffer's last element.
	constexpr auto __tail__() const noexcept -> const unit_t*;
	constexpr auto __tail__() /*&*/ noexcept -> /*&*/ unit_t*;

	// returns ptr to buffer's last = capacity.
	constexpr auto __last__() const noexcept -> const unit_t*;
	constexpr auto __last__() /*&*/ noexcept -> /*&*/ unit_t*;

	// fixes invariant; use it after internal manipulation.
	constexpr auto __size__(size_t value) noexcept -> void;

	storage store;

	class reader; friend reader;
	class writer; friend writer;

	class forward_iterator; friend forward_iterator;
	class reverse_iterator; friend reverse_iterator;

	static_assert(sizeof(storage) == sizeof(buffer));
	static_assert(std::is_standard_layout_v<buffer>);
	static_assert(std::is_trivially_copyable_v<buffer>);
	static_assert(sizeof(buffer) == sizeof(size_t) * 3);
	static_assert(alignof(buffer) == alignof(size_t) * 1);
	static_assert(offsetof(buffer, head) == sizeof(size_t) * 0);
	static_assert(offsetof(buffer, last) == sizeof(size_t) * 1);

	typedef struct { unit_t* dest; bool does_shift; bool does_alloc; } __insert__t;
	typedef struct {                                                 } __assign__t;
	typedef struct {                                                 } __concat__t;

	// 2x capacity growth
	constexpr auto __insert__(unit_t* dest, char32_t code, int8_t step) noexcept -> __insert__t;

	template <format_t alien>
	constexpr auto __assign__(const typename codec<alien>::unit_t* rhs_0,
	                          const typename codec<alien>::unit_t* rhs_N) noexcept -> __assign__t;

	template <format_t alien>
	constexpr auto __concat__(const typename codec<alien>::unit_t* rhs_0,
	                          const typename codec<alien>::unit_t* rhs_N) noexcept -> __concat__t;

public:

	// optional; returns the content of a file with CRLF/CR to LF normalization.
	template <typename STRING> friend auto fileof(const STRING& path) noexcept
	->
	std::optional
	<
		std::variant
		<
			str<"UTF-8">
			,
			str<"UTF-16">
			,
			str<"UTF-32">
		>
	>;

	[[deprecated]] constexpr operator const unit_t*() const noexcept;
	[[deprecated]] constexpr operator /*&*/ unit_t*() /*&*/ noexcept;

	// rule of 5

	COPY_CONSTRUCTOR(str);
	MOVE_CONSTRUCTOR(str);

	COPY_ASSIGNMENT(str);
	MOVE_ASSIGNMENT(str);

	// constructors

	constexpr  str() noexcept = default;
	constexpr ~str() noexcept = default;

	template <format_t alien, typename arena>
	constexpr str(__OWNED__(str)) noexcept;
	template <format_t alien /* can't own */>
	constexpr str(__SLICE__(str)) noexcept;
	template <size_t                       N>
	constexpr str(__EQSTR__(str)) noexcept requires (std::is_same_v<unit_t, char>);
	template <size_t                       N>
	constexpr str(__08STR__(str)) noexcept /* encoding of char8_t is trivial */;
	template <size_t                       N>
	constexpr str(__16STR__(str)) noexcept /* encoding of char16_t is trivial */;
	template <size_t                       N>
	constexpr str(__32STR__(str)) noexcept /* encoding of char32_t is trivial */;

	// returns the number of code units it can hold, excluding NULL-TERMINATOR.
	constexpr auto capacity(/* getter */) const noexcept -> size_t;
	// changes the number of code units it can hold, excluding NULL-TERMINATOR.
	constexpr auto capacity(size_t value) /*&*/ noexcept -> void;

	// iterator

	using API<str<local, alloc>, local>::begin; // fix; name hiding
	using API<str<local, alloc>, local>::end; // fix; name hiding

	constexpr auto begin() /*&*/ noexcept -> forward_iterator;
	constexpr auto end() /*&*/ noexcept -> forward_iterator;

	using API<str<local, alloc>, local>::rbegin; // fix; name hiding
	using API<str<local, alloc>, local>::rend; // fix; name hiding

	constexpr auto rbegin() /*&*/ noexcept -> reverse_iterator;
	constexpr auto rend() /*&*/ noexcept -> reverse_iterator;

	// operators

	template <format_t alien, typename arena>
	constexpr auto operator=(__OWNED__(rhs))& noexcept -> str&;
	template <format_t alien /* can't own */>
	constexpr auto operator=(__SLICE__(rhs))& noexcept -> str&;
	template <size_t                       N>
	constexpr auto operator=(__EQSTR__(rhs))& noexcept -> str& requires (std::is_same_v<unit_t, char>);
	template <size_t                       N>
	constexpr auto operator=(__08STR__(rhs))& noexcept -> str& /* encoding of char8_t is trivial */;
	template <size_t                       N>
	constexpr auto operator=(__16STR__(rhs))& noexcept -> str& /* encoding of char16_t is trivial */;
	template <size_t                       N>
	constexpr auto operator=(__32STR__(rhs))& noexcept -> str& /* encoding of char32_t is trivial */;

	template <format_t alien, typename arena>
	constexpr auto operator=(__OWNED__(rhs))&& noexcept -> str&&;
	template <format_t alien /* can't own */>
	constexpr auto operator=(__SLICE__(rhs))&& noexcept -> str&&;
	template <size_t                       N>
	constexpr auto operator=(__EQSTR__(rhs))&& noexcept -> str&& requires (std::is_same_v<unit_t, char>);
	template <size_t                       N>
	constexpr auto operator=(__08STR__(rhs))&& noexcept -> str&& /* encoding of char8_t is trivial */;
	template <size_t                       N>
	constexpr auto operator=(__16STR__(rhs))&& noexcept -> str&& /* encoding of char16_t is trivial */;
	template <size_t                       N>
	constexpr auto operator=(__32STR__(rhs))&& noexcept -> str&& /* encoding of char32_t is trivial */;

	template <format_t alien, typename arena>
	constexpr auto operator+=(__OWNED__(rhs))& noexcept -> str&;
	template <format_t alien /* can't own */>
	constexpr auto operator+=(__SLICE__(rhs))& noexcept -> str&;
	template <size_t                       N>
	constexpr auto operator+=(__EQSTR__(rhs))& noexcept -> str& requires (std::is_same_v<unit_t, char>);
	template <size_t                       N>
	constexpr auto operator+=(__08STR__(rhs))& noexcept -> str& /* encoding of char8_t is trivial */;
	template <size_t                       N>
	constexpr auto operator+=(__16STR__(rhs))& noexcept -> str& /* encoding of char16_t is trivial */;
	template <size_t                       N>
	constexpr auto operator+=(__32STR__(rhs))& noexcept -> str& /* encoding of char32_t is trivial */;

	template <format_t alien, typename arena>
	constexpr auto operator+=(__OWNED__(rhs))&& noexcept -> str&&;
	template <format_t alien /* can't own */>
	constexpr auto operator+=(__SLICE__(rhs))&& noexcept -> str&&;
	template <size_t                       N>
	constexpr auto operator+=(__EQSTR__(rhs))&& noexcept -> str&& requires (std::is_same_v<unit_t, char>);
	template <size_t                       N>
	constexpr auto operator+=(__08STR__(rhs))&& noexcept -> str&& /* encoding of char8_t is trivial */;
	template <size_t                       N>
	constexpr auto operator+=(__16STR__(rhs))&& noexcept -> str&& /* encoding of char16_t is trivial */;
	template <size_t                       N>
	constexpr auto operator+=(__32STR__(rhs))&& noexcept -> str&& /* encoding of char32_t is trivial */;

private:

	class reader
	{
		using self_t = str;

		const self_t* src;
		const size_t  arg;

	public:

		constexpr reader
		(
			decltype(src) src,
			decltype(arg) arg
		)
		noexcept : src {src},
		           arg {arg}
		{}

		[[nodiscard]] constexpr operator char32_t() const noexcept;

		constexpr auto operator==(char32_t code) const noexcept -> bool;
		constexpr auto operator!=(char32_t code) const noexcept -> bool;
	};

	class writer
	{
		using self_t = str;

		/*&*/ self_t* src;
		const size_t  arg;

	public:

		constexpr writer
		(
			decltype(src) src,
			decltype(arg) arg
		)
		noexcept : src {src},
		           arg {arg}
		{}

		constexpr auto operator=(char32_t code) noexcept -> writer&;
		
		[[nodiscard]] constexpr operator char32_t() const noexcept;

		constexpr auto operator==(char32_t code) const noexcept -> bool;
		constexpr auto operator!=(char32_t code) const noexcept -> bool;
	};

	// self-healing iterator; allows mutation
	template <typename iterator> class cursor
	{
		using self_t = str;

		// std::views::reverse is impl in Clang/GCC/MSVC:
		//
		// ┌─────────────────────────────────────────────────────┐
		// │ template <typename iterator> class reverse_iterator │
		// │ {                                                   │
		// │     iterator current;                               │
		// │                                                     │
		// │     constexpr auto operator*() const -> value_type  │
		// │     {                                               │
		// │         iterator temporal {current};                │
		// │                                                     │
		// │         --temporal;                                 │
		// │                                                     │
		// │         return *temporal; // dangling               │
		// │     }                                               │
		// │ }                                                   │
		// └─────────────────────────────────────────────────────┘
		//
		// due to proxy is made from temporal <it> clone
		// self healing attempt that involves deref is UB
		// →
		// <std::shared_ptr> is perfect solution to this!

		enum class it_offset_relative_tag : uint8_t
		{
			HEAD,
			TAIL,
		};

		enum class it_cursor_category_tag : uint8_t
		{
			LTOR,
			RTOL,
		};

		struct state : std::enable_shared_from_this<state>
		{
			/*&&&*/ self_t* src;
			mutable unit_t* ptr;

			constexpr state
			(
				decltype(src) src,
				decltype(ptr) ptr
			)
			noexcept : src {src},
			           ptr {ptr}
			{}

			class proxy
			{
				std::shared_ptr<state> common;
				unit_t*                needle;
				it_offset_relative_tag offset_tag;
				it_cursor_category_tag cursor_tag;

			public:

				constexpr proxy
				(
					decltype(common) common,
					decltype(needle) needle,
					decltype(offset_tag) offset_tag,
					decltype(cursor_tag) cursor_tag
				)
				noexcept : common {common},
				           needle {needle},
				           offset_tag {offset_tag},
				           cursor_tag {cursor_tag}
				{}

				[[nodiscard]] constexpr operator char32_t() const noexcept;
				constexpr auto operator=(char32_t code) noexcept -> proxy&;

				constexpr auto operator==(char32_t code) const noexcept -> bool;
				constexpr auto operator!=(char32_t code) const noexcept -> bool;
			};
		};

		friend str;

		std::shared_ptr<state> common;
		size_t                 offset;
		size_t                 weight;
		it_offset_relative_tag offset_tag;
		it_cursor_category_tag cursor_tag;

		constexpr auto __needle__() const noexcept -> unit_t*;

	public:

		using iterator_category = std::bidirectional_iterator_tag;
		using iterator_concept = std::bidirectional_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = typename state::proxy;
		using reference = typename state::proxy;

		constexpr cursor
		(
			self_t* src,
			unit_t* ptr,
			decltype(offset) offset,
			decltype(weight) weight,
			decltype(offset_tag) offset_tag,
			decltype(cursor_tag) cursor_tag
		)
		noexcept : common {std::make_shared<state>(src, ptr)},
		           offset {offset},
		           weight {weight},
		           offset_tag {offset_tag},
		           cursor_tag {cursor_tag}
		{}

		constexpr operator const unit_t*() const noexcept;
		constexpr operator const unit_t*() /*&*/ noexcept;

		// stl compat; default constructible
		constexpr  cursor() noexcept = default;
		constexpr ~cursor() noexcept = default;

		constexpr auto operator*() const noexcept -> value_type;

		constexpr auto operator++(   ) noexcept -> iterator&;
		constexpr auto operator++(int) noexcept -> iterator;

		constexpr auto operator--(   ) noexcept -> iterator&;
		constexpr auto operator--(int) noexcept -> iterator;

		constexpr auto operator+(size_t value) noexcept -> iterator;
		constexpr auto operator-(size_t value) noexcept -> iterator;

		constexpr auto operator+=(size_t value) noexcept -> iterator&;
		constexpr auto operator-=(size_t value) noexcept -> iterator&;

		constexpr auto operator==(const iterator& rhs) const noexcept -> bool;
		constexpr auto operator!=(const iterator& rhs) const noexcept -> bool;
	};

	class forward_iterator : public cursor<forward_iterator> { public: using cursor<forward_iterator>::cursor; };
	class reverse_iterator : public cursor<reverse_iterator> { public: using cursor<reverse_iterator>::cursor; };
};

template <format_t local /* can't own */> class txt : public API<txt<local /*##*/>, local>
{
	template <typename,format_t> friend class API;
	template <format_t,typename> friend class str;
	template <format_t /*none*/> friend class txt;

	using unit_t = typename codec<local>::unit_t;

	const unit_t* __head__;
	const unit_t* __tail__;

	class reader; friend reader;
	class writer; friend writer;

public:

	constexpr txt
	(
		decltype(__head__) head,
		decltype(__tail__) tail
	)
	noexcept : __head__ {head},
	           __tail__ {tail}
	{}

	template <size_t N>
	constexpr txt
	(
		const unit_t (&str)[N]
	)
	noexcept : __head__ {&str[N - N]},
	           __tail__ {&str[N - 1]}
	{}

	template <size_t N>
	constexpr txt
	(
		/*&*/ unit_t (&str)[N]
	)
	noexcept : __head__ {&str[N - N]},
	           __tail__ {&str[N - 1]}
	{}

	template <typename arena>
	constexpr txt
	(
		const str<local, arena>& str
	)
	noexcept : __head__ {str.__head__()},
	           __tail__ {str.__tail__()}
	{}

	template <typename arena>
	constexpr txt
	(
		/*&*/ str<local, arena>& str
	)
	noexcept : __head__ {str.__head__()},
	           __tail__ {str.__tail__()}
	{}

	COPY_CONSTRUCTOR(txt) = default;
	MOVE_CONSTRUCTOR(txt) = default;

	COPY_ASSIGNMENT(txt) = default;
	MOVE_ASSIGNMENT(txt) = default;

	constexpr  txt() noexcept = delete;
	constexpr ~txt() noexcept = default;

private:

	class reader
	{
		using self_t = txt;

		const self_t* src;
		const size_t  arg;

	public:

		constexpr reader
		(
			decltype(src) src,
			decltype(arg) arg
		)
		noexcept : src {src},
		           arg {arg}
		{}

		[[nodiscard]] constexpr operator char32_t() const noexcept;

		constexpr auto operator==(char32_t code) const noexcept -> bool;
		constexpr auto operator!=(char32_t code) const noexcept -> bool;
	};

	class writer
	{
		using self_t = txt;

		/*&*/ self_t* src;
		const size_t  arg;

	public:

		constexpr writer
		(
			decltype(src) src,
			decltype(arg) arg
		)
		noexcept : src {src},
		           arg {arg}
		{}

		// constexpr auto operator=(char32_t code) noexcept -> writer&;
		
		[[nodiscard]] constexpr operator char32_t() const noexcept;

		constexpr auto operator==(char32_t code) const noexcept -> bool;
		constexpr auto operator!=(char32_t code) const noexcept -> bool;
	};
};

namespace detail
{
	template <format_t source>
	static constexpr auto __units__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail) noexcept -> size_t;

	template <format_t source>
	static constexpr auto __codes__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail) noexcept -> size_t;

	template <format_t source,
	          format_t target>
	static constexpr auto __fcopy__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail,
	                                                                            /*&*/ typename codec<target>::unit_t* dest) noexcept -> size_t;

	template <format_t source,
	          format_t target>
	static constexpr auto __rcopy__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail,
	                                                                            /*&*/ typename codec<target>::unit_t* dest) noexcept -> size_t;

	template <format_t source,
	          format_t target>
	static constexpr auto __equal__(const typename codec<source>::unit_t* lhs_0, const typename codec<source>::unit_t* lhs_N,
	                                const typename codec<target>::unit_t* rhs_0, const typename codec<target>::unit_t* rhs_N) noexcept -> bool;

	template <format_t source,
	          format_t target>
	static constexpr auto __nqual__(const typename codec<source>::unit_t* lhs_0, const typename codec<source>::unit_t* lhs_N,
	                                const typename codec<target>::unit_t* rhs_0, const typename codec<target>::unit_t* rhs_N) noexcept -> bool;

	template <format_t source,
	          format_t target>
	static constexpr auto __scan__(const typename codec<source>::unit_t* lhs_0, const typename codec<source>::unit_t* lhs_N,
	                               const typename codec<target>::unit_t* rhs_0, const typename codec<target>::unit_t* rhs_N,
	                                                                            const /* (head, tail) -> VOID! */ auto& fun) noexcept -> void;

	template <format_t source,
	          format_t target>
	static constexpr auto __swith__(const typename codec<source>::unit_t* lhs_0, const typename codec<source>::unit_t* lhs_N,
	                                const typename codec<target>::unit_t* rhs_0, const typename codec<target>::unit_t* rhs_N) noexcept -> bool;

	template <format_t source,
	          format_t target>
	static constexpr auto __ewith__(const typename codec<source>::unit_t* lhs_0, const typename codec<source>::unit_t* lhs_N,
	                                const typename codec<target>::unit_t* rhs_0, const typename codec<target>::unit_t* rhs_N) noexcept -> bool;

	template <format_t source,
	          format_t target>
	static constexpr auto __split__(const typename codec<source>::unit_t* lhs_0, const typename codec<source>::unit_t* lhs_N,
	                                const typename codec<target>::unit_t* rhs_0, const typename codec<target>::unit_t* rhs_N) noexcept -> std::vector<txt<source>>;

	template <format_t source,
	          format_t target>
	static constexpr auto __match__(const typename codec<source>::unit_t* lhs_0, const typename codec<source>::unit_t* lhs_N,
	                                const typename codec<target>::unit_t* rhs_0, const typename codec<target>::unit_t* rhs_N) noexcept -> std::vector<txt<source>>;

	template <format_t source>
	static constexpr auto __substr__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail, clamp  start, clamp  until) noexcept -> txt<source>;

	template <format_t source>
	static constexpr auto __substr__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail, clamp  start, range  until) noexcept -> txt<source>;

	template <format_t source>
	static constexpr auto __substr__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail, size_t start, clamp  until) noexcept -> txt<source>;

	template <format_t source>
	static constexpr auto __substr__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail, size_t start, range  until) noexcept -> txt<source>;

	template <format_t source>
	static constexpr auto __substr__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail, size_t start, size_t until) noexcept -> txt<source>;
};

#pragma region iostream

template <format_t alien, typename arena> inline auto operator<<(std::ostream& os, __OWNED__(str)) noexcept -> decltype(auto)
{
	typedef decltype(os) out;

	for (const auto code : str)
	{
		::operator<<(os, code);
	}
	return std::forward<out>(os);
}

template <format_t alien /* can't own */> inline auto operator<<(std::ostream& os, __SLICE__(str)) noexcept -> decltype(auto)
{
	typedef decltype(os) out;

	for (const auto code : str)
	{
		::operator<<(os, code);
	}
	return std::forward<out>(os);
}

#pragma endregion iostream
#pragma region codec<"ASCII">

constexpr auto codec<"ASCII">::size([[maybe_unused]] const char32_t code) noexcept -> int8_t
{
	return 1;
}

constexpr auto codec<"ASCII">::next([[maybe_unused]] const unit_t* data) noexcept -> int8_t
{
	return +1;
}

constexpr auto codec<"ASCII">::back([[maybe_unused]] const unit_t* data) noexcept -> int8_t
{
	return -1;
}

constexpr auto codec<"ASCII">::encode(const char32_t in, unit_t* out, int8_t step) noexcept -> void
{
	switch (step)
	{
		case +1:
		{
			out[+0] = static_cast<unit_t>(in);
			break;
		}
		case -1:
		{
			out[-1] = static_cast<unit_t>(in);
			break;
		}
		// 💢 ugh..? cannot recover
		default: { assert(!"corrupt");
		           std::unreachable(); }
	}
}

constexpr auto codec<"ASCII">::decode(const unit_t* in, char32_t& out, int8_t step) noexcept -> void
{
	switch (step)
	{
		case +1:
		{
			out = static_cast<char32_t>(in[+0]);
			break;
		}
		case -1:
		{
			out = static_cast<char32_t>(in[-1]);
			break;
		}
		// 💢 ugh..? cannot recover
		default: { assert(!"corrupt");
		           std::unreachable(); }
	}
}

#pragma endregion codec<"ASCII">
#pragma region codec<"UTF-8">

constexpr auto codec<"UTF-8">::size([[maybe_unused]] const char32_t code) noexcept -> int8_t
{
	const size_t N (std::bit_width
	(static_cast<uint32_t>(code)));

	//┌───────────────────────┐
	//│ U+000000 ... U+00007F │ -> 1 code unit
	//│ U+000080 ... U+0007FF │ -> 2 code unit
	//│ U+000800 ... U+00FFFF │ -> 3 code unit
	//│ U+010000 ... U+10FFFF │ -> 4 code unit
	//└───────────────────────┘

	return 1 + (8 <= N) + (12 <= N) + (17 <= N);
}

constexpr auto codec<"UTF-8">::next([[maybe_unused]] const unit_t* data) noexcept -> int8_t
{
	constexpr static const int8_t table[]
	{
		//┌─────────────────────────────┐
		//│ 0x0 ... 0xB -> single units │
		//└─────────────────────────────┘
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		//┌─────────────────────────────┐
		//│ 0xC ... 0xF -> [[variable]] │
		//└─────────────────────────────┘
		2, 2,
		3, 4,
	};

	return table[(data[0] >> 0x4) & 0x0F];
}

constexpr auto codec<"UTF-8">::back([[maybe_unused]] const unit_t* data) noexcept -> int8_t
{
	int8_t i {-1};
	
	// until start byte sequence is found...
	for (; (data[i] & 0xC0) == 0x80; --i) {}
	
	return i;
}

constexpr auto codec<"UTF-8">::encode(const char32_t in, unit_t* out, int8_t step) noexcept -> void
{
	switch (step)
	{
		case +1:
		{
			out[+0] = static_cast<unit_t>(in);
			break;
		}
		case -1:
		{
			out[-1] = static_cast<unit_t>(in);
			break;
		}
		case +2:
		{
			out[+0] = 0xC0 | ((in >> 06) & 0x1F);
			out[+1] = 0x80 | ((in >> 00) & 0x3F);
			break;
		}
		case -2:
		{
			out[-2] = 0xC0 | ((in >> 06) & 0x1F);
			out[-1] = 0x80 | ((in >> 00) & 0x3F);
			break;
		}
		case +3:
		{
			out[+0] = 0xE0 | ((in >> 12) & 0x0F);
			out[+1] = 0x80 | ((in >> 06) & 0x3F);
			out[+2] = 0x80 | ((in >> 00) & 0x3F);
			break;
		}
		case -3:
		{
			out[-3] = 0xE0 | ((in >> 12) & 0x0F);
			out[-2] = 0x80 | ((in >> 06) & 0x3F);
			out[-1] = 0x80 | ((in >> 00) & 0x3F);
			break;
		}
		case +4:
		{
			out[+0] = 0xF0 | ((in >> 18) & 0x07);
			out[+1] = 0x80 | ((in >> 12) & 0x3F);
			out[+2] = 0x80 | ((in >> 06) & 0x3F);
			out[+3] = 0x80 | ((in >> 00) & 0x3F);
			break;
		}
		case -4:
		{
			out[-4] = 0xF0 | ((in >> 18) & 0x07);
			out[-3] = 0x80 | ((in >> 12) & 0x3F);
			out[-2] = 0x80 | ((in >> 06) & 0x3F);
			out[-1] = 0x80 | ((in >> 00) & 0x3F);
			break;
		}
		// 💢 ugh..? cannot recover
		default: { assert(!"corrupt");
		           std::unreachable(); }
	}
}

constexpr auto codec<"UTF-8">::decode(const unit_t* in, char32_t& out, int8_t step) noexcept -> void
{
	switch (step)
	{
		case +1:
		{
			out = static_cast<char32_t>(in[+0]);
			break;
		}
		case -1:
		{
			out = static_cast<char32_t>(in[-1]);
			break;
		}
		case +2:
		{
			out = ((in[+0] & 0x1F) << 06)
			      |
			      ((in[+1] & 0x3F) << 00);
			break;
		}
		case -2:
		{
			out = ((in[-2] & 0x1F) << 06)
			      |
			      ((in[-1] & 0x3F) << 00);
			break;
		}
		case +3:
		{
			out = ((in[+0] & 0x0F) << 12)
			      |
			      ((in[+1] & 0x3F) << 06)
			      |
			      ((in[+2] & 0x3F) << 00);
			break;
		}
		case -3:
		{
			out = ((in[-3] & 0x0F) << 12)
			      |
			      ((in[-2] & 0x3F) << 06)
			      |
			      ((in[-1] & 0x3F) << 00);
			break;
		}
		case +4:
		{
			out = ((in[+0] & 0x07) << 18)
			      |
			      ((in[+1] & 0x3F) << 12)
			      |
			      ((in[+2] & 0x3F) << 06)
			      |
			      ((in[+3] & 0x3F) << 00);
			break;
		}
		case -4:
		{
			out = ((in[-4] & 0x07) << 18)
			      |
			      ((in[-3] & 0x3F) << 12)
			      |
			      ((in[-2] & 0x3F) << 06)
			      |
			      ((in[-1] & 0x3F) << 00);
			break;
		}
		// 💢 ugh..? cannot recover
		default: { assert(!"corrupt");
		           std::unreachable(); }
	}
}

#pragma endregion codec<"UTF-8">
#pragma region codec<"UTF-16">

constexpr auto codec<"UTF-16">::size([[maybe_unused]] const char32_t code) noexcept -> int8_t
{
	//┌───────────────────────┐
	//│ U+000000 ... U+00D7FF │ -> 1 code unit
	//│ U+00E000 ... U+00FFFF │ -> 1 code unit
	//│ U+000000 ... U+10FFFF │ -> 2 code unit
	//└───────────────────────┘

	return 1 + (0xFFFF /* pair? */ < code);
}

constexpr auto codec<"UTF-16">::next([[maybe_unused]] const unit_t* data) noexcept -> int8_t
{
	constexpr static const int8_t table[]
	{
		//┌──────────────────────────────────┐
		//│ 0x0000 ... 0xD7FF -> head of BMP │
		//└──────────────────────────────────┘
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		1, 1,
		//┌──────────────────────────────────┐
		//│ 0xD800 ... 0xDBFF -> H surrogate │
		//└──────────────────────────────────┘
		2, 2,
		//┌──────────────────────────────────┐
		//│ 0xDC00 ... 0xDFFF -> L surrogate │
		//└──────────────────────────────────┘
		0, 0,
		//┌──────────────────────────────────┐
		//│ 0xE000 ... 0xFFFF -> rest of BMP │
		//└──────────────────────────────────┘
		1, 1,
		1, 1,
		1, 1,
		1, 1,
	};

	return table[(data[0] >> 10) & 0x3F];
}

constexpr auto codec<"UTF-16">::back([[maybe_unused]] const unit_t* data) noexcept -> int8_t
{
	int8_t i {-1};
	
	// until start byte sequence is found...
	for (; (data[i] >> 0xA) == 0x37; --i) {}
	
	return i;
}

constexpr auto codec<"UTF-16">::encode(const char32_t in, unit_t* out, int8_t step) noexcept -> void
{
	switch (step)
	{
		case +1:
		{
			out[+0] = static_cast<unit_t>(in);
			break;
		}
		case -1:
		{
			out[-1] = static_cast<unit_t>(in);
			break;
		}
		case +2:
		{
			const char32_t code {in - 0x10000};
			out[+0] = 0xD800 | (code / 0x400);
			out[+1] = 0xDC00 | (code & 0x3FF);
			break;
		}
		case -2:
		{
			const char32_t code {in - 0x10000};
			out[-2] = 0xD800 | (code / 0x400);
			out[-1] = 0xDC00 | (code & 0x3FF);
			break;
		}
		// 💢 ugh..? cannot recover
		default: { assert(!"corrupt");
		           std::unreachable(); }
	}
}

constexpr auto codec<"UTF-16">::decode(const unit_t* in, char32_t& out, int8_t step) noexcept -> void
{
	switch (step)
	{
		case +1:
		{
			out = static_cast<char32_t>(in[+0]);
			break;
		}
		case -1:
		{
			out = static_cast<char32_t>(in[-1]);
			break;
		}
		case +2:
		{
			out = 0x10000 // supplymentary
			      |
			      ((in[+0] - 0xD800) << 10)
			      |
			      ((in[+1] - 0xDC00) << 00);
			break;
		}
		case -2:
		{
			out = 0x10000 // supplymentary
			      |
			      ((in[-2] - 0xD800) << 10)
			      |
			      ((in[-1] - 0xDC00) << 00);
			break;
		}
		// 💢 ugh..? cannot recover
		default: { assert(!"corrupt");
		           std::unreachable(); }
	}
}

#pragma endregion codec<"UTF-16">
#pragma region codec<"UTF-32">

constexpr auto codec<"UTF-32">::size([[maybe_unused]] const char32_t code) noexcept -> int8_t
{
	return 1;
}

constexpr auto codec<"UTF-32">::next([[maybe_unused]] const unit_t* data) noexcept -> int8_t
{
	return +1;
}

constexpr auto codec<"UTF-32">::back([[maybe_unused]] const unit_t* data) noexcept -> int8_t
{
	return -1;
}

constexpr auto codec<"UTF-32">::encode(const char32_t in, unit_t* out, int8_t step) noexcept -> void
{
	switch (step)
	{
		case +1:
		{
			out[+0] = static_cast<unit_t>(in);
			break;
		}
		case -1:
		{
			out[-1] = static_cast<unit_t>(in);
			break;
		}
		// 💢 ugh..? cannot recover
		default: { assert(!"corrupt");
		           std::unreachable(); }
	}
}

constexpr auto codec<"UTF-32">::decode(const unit_t* in, char32_t& out, int8_t step) noexcept -> void
{
	switch (step)
	{
		case +1:
		{
			out = static_cast<char32_t>(in[+0]);
			break;
		}
		case -1:
		{
			out = static_cast<char32_t>(in[-1]);
			break;
		}
		// 💢 ugh..? cannot recover
		default: { assert(!"corrupt");
		           std::unreachable(); }
	}
}

#pragma endregion codec<"UTF-32">
#pragma region core

template <typename model, format_t local> constexpr auto API<model, local>::head() const noexcept -> const unit_t*
{
	if constexpr (requires { static_cast<const model*>(this)->__head__(); })
	     return static_cast<const model*>(this)->__head__();
	else return static_cast<const model*>(this)->__head__  ;
}

template <typename model, format_t local> constexpr auto API<model, local>::tail() const noexcept -> const unit_t*
{
	if constexpr (requires { static_cast<const model*>(this)->__tail__(); })
	     return static_cast<const model*>(this)->__tail__();
	else return static_cast<const model*>(this)->__tail__  ;
}

template <typename model, format_t local> constexpr auto API<model, local>::size() const noexcept -> size_t
{
	return detail::__units__<local>(this->head(), this->tail());
}

template <typename model, format_t local> constexpr auto API<model, local>::length() const noexcept -> size_t
{
	return detail::__codes__<local>(this->head(), this->tail());
}

template <typename model, format_t local>
template <format_t alien, typename arena> constexpr auto API<model, local>::starts_with(__OWNED__(value)) const noexcept -> bool
{
	return detail::__swith__<local, alien>(this->head(), this->tail(), value.head(), value.tail());
}

template <typename model, format_t local>
template <format_t alien /* can't own */> constexpr auto API<model, local>::starts_with(__SLICE__(value)) const noexcept -> bool
{
	return detail::__swith__<local, alien>(this->head(), this->tail(), value.head(), value.tail());
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::starts_with(__EQSTR__(value)) const noexcept -> bool requires (std::is_same_v<unit_t, char>)
{
	return detail::__swith__<local, local>(this->head(), this->tail(), &value[N - N], &value[N - 1]);
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::starts_with(__08STR__(value)) const noexcept -> bool /* encoding of char8_t is trivial */
{
	return detail::__swith__<local, "UTF-8">(this->head(), this->tail(), &value[N - N], &value[N - 1]);
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::starts_with(__16STR__(value)) const noexcept -> bool /* encoding of char16_t is trivial */
{
	return detail::__swith__<local, "UTF-16">(this->head(), this->tail(), &value[N - N], &value[N - 1]);
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::starts_with(__32STR__(value)) const noexcept -> bool /* encoding of char32_t is trivial */
{
	return detail::__swith__<local, "UTF-32">(this->head(), this->tail(), &value[N - N], &value[N - 1]);
}

template <typename model, format_t local>
template <format_t alien, typename arena> constexpr auto API<model, local>::ends_with(__OWNED__(value)) const noexcept -> bool
{
	return detail::__ewith__<local, alien>(this->head(), this->tail(), value.head(), value.tail());
}

template <typename model, format_t local>
template <format_t alien /* can't own */> constexpr auto API<model, local>::ends_with(__SLICE__(value)) const noexcept -> bool
{
	return detail::__ewith__<local, alien>(this->head(), this->tail(), value.head(), value.tail());
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::ends_with(__EQSTR__(value)) const noexcept -> bool requires (std::is_same_v<unit_t, char>)
{
	return detail::__ewith__<local, local>(this->head(), this->tail(), &value[N - N], &value[N - 1]);
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::ends_with(__08STR__(value)) const noexcept -> bool /* encoding of char8_t is trivial */
{
	return detail::__ewith__<local, "UTF-8">(this->head(), this->tail(), &value[N - N], &value[N - 1]);
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::ends_with(__16STR__(value)) const noexcept -> bool /* encoding of char16_t is trivial */
{
	return detail::__ewith__<local, "UTF-16">(this->head(), this->tail(), &value[N - N], &value[N - 1]);
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::ends_with(__32STR__(value)) const noexcept -> bool /* encoding of char32_t is trivial */
{
	return detail::__ewith__<local, "UTF-32">(this->head(), this->tail(), &value[N - N], &value[N - 1]);
}

template <typename model, format_t local>
template <format_t alien, typename arena> constexpr auto API<model, local>::contains(__OWNED__(value)) const noexcept -> size_t
{
	return detail::__match__<local, alien>(this->head(), this->tail(), value.head(), value.tail()).size();
}

template <typename model, format_t local>
template <format_t alien /* can't own */> constexpr auto API<model, local>::contains(__SLICE__(value)) const noexcept -> size_t
{
	return detail::__match__<local, alien>(this->head(), this->tail(), value.head(), value.tail()).size();
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::contains(__EQSTR__(value)) const noexcept -> size_t requires (std::is_same_v<unit_t, char>)
{
	return detail::__match__<local, local>(this->head(), this->tail(), &value[N - N], &value[N - 1]).size();
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::contains(__08STR__(value)) const noexcept -> size_t /* encoding of char8_t is trivial */
{
	return detail::__match__<local, "UTF-8">(this->head(), this->tail(), &value[N - N], &value[N - 1]).size();
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::contains(__16STR__(value)) const noexcept -> size_t /* encoding of char16_t is trivial */
{
	return detail::__match__<local, "UTF-16">(this->head(), this->tail(), &value[N - N], &value[N - 1]).size();
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::contains(__32STR__(value)) const noexcept -> size_t /* encoding of char32_t is trivial */
{
	return detail::__match__<local, "UTF-32">(this->head(), this->tail(), &value[N - N], &value[N - 1]).size();
}

template <typename model, format_t local>
template <format_t alien, typename arena> constexpr auto API<model, local>::split(__OWNED__(value)) const noexcept -> std::vector<txt<local>>
{
	return detail::__split__<local, alien>(this->head(), this->tail(), value.head(), value.tail());
}

template <typename model, format_t local>
template <format_t alien /* can't own */> constexpr auto API<model, local>::split(__SLICE__(value)) const noexcept -> std::vector<txt<local>>
{
	return detail::__split__<local, alien>(this->head(), this->tail(), value.head(), value.tail());
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::split(__EQSTR__(value)) const noexcept -> std::vector<txt<local>> requires (std::is_same_v<unit_t, char>)
{
	return detail::__split__<local, local>(this->head(), this->tail(), &value[N - N], &value[N - 1]);
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::split(__08STR__(value)) const noexcept -> std::vector<txt<local>> /* encoding of char8_t is trivial */
{
	return detail::__split__<local, "UTF-8">(this->head(), this->tail(), &value[N - N], &value[N - 1]);
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::split(__16STR__(value)) const noexcept -> std::vector<txt<local>> /* encoding of char16_t is trivial */
{
	return detail::__split__<local, "UTF-16">(this->head(), this->tail(), &value[N - N], &value[N - 1]);
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::split(__32STR__(value)) const noexcept -> std::vector<txt<local>> /* encoding of char32_t is trivial */
{
	return detail::__split__<local, "UTF-32">(this->head(), this->tail(), &value[N - N], &value[N - 1]);
}

template <typename model, format_t local>
template <format_t alien, typename arena> constexpr auto API<model, local>::match(__OWNED__(value)) const noexcept -> std::vector<txt<local>>
{
	return detail::__match__<local, alien>(this->head(), this->tail(), value.head(), value.tail());
}

template <typename model, format_t local>
template <format_t alien /* can't own */> constexpr auto API<model, local>::match(__SLICE__(value)) const noexcept -> std::vector<txt<local>>
{
	return detail::__match__<local, alien>(this->head(), this->tail(), value.head(), value.tail());
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::match(__EQSTR__(value)) const noexcept -> std::vector<txt<local>> requires (std::is_same_v<unit_t, char>)
{
	return detail::__match__<local, local>(this->head(), this->tail(), &value[N - N], &value[N - 1]);
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::match(__08STR__(value)) const noexcept -> std::vector<txt<local>> /* encoding of char8_t is trivial */
{
	return detail::__match__<local, "UTF-8">(this->head(), this->tail(), &value[N - N], &value[N - 1]);
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::match(__16STR__(value)) const noexcept -> std::vector<txt<local>> /* encoding of char16_t is trivial */
{
	return detail::__match__<local, "UTF-16">(this->head(), this->tail(), &value[N - N], &value[N - 1]);
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::match(__32STR__(value)) const noexcept -> std::vector<txt<local>> /* encoding of char32_t is trivial */
{
	return detail::__match__<local, "UTF-32">(this->head(), this->tail(), &value[N - N], &value[N - 1]);
}

template <typename model, format_t local> constexpr auto API<model, local>::substr(clamp  start, clamp  until) const noexcept -> txt<local>
{
	return detail::__substr__<local>(this->head(), this->tail(), start, until);
}

template <typename model, format_t local> constexpr auto API<model, local>::substr(clamp  start, range  until) const noexcept -> txt<local>
{
	return detail::__substr__<local>(this->head(), this->tail(), start, until);
}

template <typename model, format_t local> constexpr auto API<model, local>::substr(size_t start, clamp  until) const noexcept -> txt<local>
{
	return detail::__substr__<local>(this->head(), this->tail(), start, until);
}

template <typename model, format_t local> constexpr auto API<model, local>::substr(size_t start, range  until) const noexcept -> txt<local>
{
	return detail::__substr__<local>(this->head(), this->tail(), start, until);
}

template <typename model, format_t local> constexpr auto API<model, local>::substr(size_t start, size_t until) const noexcept -> txt<local>
{
	return detail::__substr__<local>(this->head(), this->tail(), start, until);
}

template <typename model, format_t local> constexpr auto API<model, local>::begin() const noexcept -> const_forward_iterator
{
	return {this->head()};
}

template <typename model, format_t local> constexpr auto API<model, local>::end() const noexcept -> const_forward_iterator
{
	return {this->tail()};
}

template <typename model, format_t local> constexpr auto API<model, local>::rbegin() const noexcept -> const_reverse_iterator
{
	return {this->tail()};
}

template <typename model, format_t local> constexpr auto API<model, local>::rend() const noexcept -> const_reverse_iterator
{
	return {this->head()};
}

template <typename model, format_t local> constexpr auto API<model, local>::operator[](size_t value) const noexcept -> decltype(auto)
{
	return typename model::reader {static_cast<model*>(this), value};
}

template <typename model, format_t local> constexpr auto API<model, local>::operator[](size_t value) /*&*/ noexcept -> decltype(auto)
{
	return typename model::writer {static_cast<model*>(this), value};
}

template <typename model, format_t local>
template <format_t alien, typename arena> constexpr auto API<model, local>::operator==(__OWNED__(rhs)) const noexcept -> bool
{
	return detail::__equal__<local, alien>(this->head(), this->tail(), rhs.head(), rhs.tail());
}

template <typename model, format_t local>
template <format_t alien /* can't own */> constexpr auto API<model, local>::operator==(__SLICE__(rhs)) const noexcept -> bool
{
	return detail::__equal__<local, alien>(this->head(), this->tail(), rhs.head(), rhs.tail());
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::operator==(__EQSTR__(rhs)) const noexcept -> bool requires (std::is_same_v<unit_t, char>)
{
	return detail::__equal__<local, local>(this->head(), this->tail(), &rhs[N - N], &rhs[N - 1]);
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::operator==(__08STR__(rhs)) const noexcept -> bool /* encoding of char8_t is trivial */
{
	return detail::__equal__<local, "UTF-8">(this->head(), this->tail(), &rhs[N - N], &rhs[N - 1]);
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::operator==(__16STR__(rhs)) const noexcept -> bool /* encoding of char16_t is trivial */
{
	return detail::__equal__<local, "UTF-16">(this->head(), this->tail(), &rhs[N - N], &rhs[N - 1]);
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::operator==(__32STR__(rhs)) const noexcept -> bool /* encoding of char32_t is trivial */
{
	return detail::__equal__<local, "UTF-32">(this->head(), this->tail(), &rhs[N - N], &rhs[N - 1]);
}

template <typename model, format_t local>
template <format_t alien, typename arena> constexpr auto API<model, local>::operator!=(__OWNED__(rhs)) const noexcept -> bool
{
	return detail::__nqual__<local, alien>(this->head(), this->tail(), rhs.head(), rhs.tail());
}

template <typename model, format_t local>
template <format_t alien /* can't own */> constexpr auto API<model, local>::operator!=(__SLICE__(rhs)) const noexcept -> bool
{
	return detail::__nqual__<local, alien>(this->head(), this->tail(), rhs.head(), rhs.tail());
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::operator!=(__EQSTR__(rhs)) const noexcept -> bool requires (std::is_same_v<unit_t, char>)
{
	return detail::__nqual__<local, local>(this->head(), this->tail(), &rhs[N - N], &rhs[N - 1]);
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::operator!=(__08STR__(rhs)) const noexcept -> bool /* encoding of char8_t is trivial */
{
	return detail::__nqual__<local, "UTF-8">(this->head(), this->tail(), &rhs[N - N], &rhs[N - 1]);
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::operator!=(__16STR__(rhs)) const noexcept -> bool /* encoding of char16_t is trivial */
{
	return detail::__nqual__<local, "UTF-16">(this->head(), this->tail(), &rhs[N - N], &rhs[N - 1]);
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::operator!=(__32STR__(rhs)) const noexcept -> bool /* encoding of char32_t is trivial */
{
	return detail::__nqual__<local, "UTF-32">(this->head(), this->tail(), &rhs[N - N], &rhs[N - 1]);
}

template <typename model, format_t local>
template <format_t alien, typename arena> constexpr auto API<model, local>::operator+(__OWNED__(rhs)) const noexcept -> concat<txt<local>, txt<alien>>
{
	return {{this->head(), this->tail()}, {rhs.head(), rhs.tail()}};
}

template <typename model, format_t local>
template <format_t alien /* can't own */> constexpr auto API<model, local>::operator+(__SLICE__(rhs)) const noexcept -> concat<txt<local>, txt<alien>>
{
	return {{this->head(), this->tail()}, {rhs.head(), rhs.tail()}};
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::operator+(__EQSTR__(rhs)) const noexcept -> concat<txt<local>, txt<local>> requires (std::is_same_v<unit_t, char>)
{
	return {{this->head(), this->tail()}, {&rhs[N - N], &rhs[N - 1]}};
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::operator+(__08STR__(rhs)) const noexcept -> concat<txt<local>, txt<"UTF-8">> /* encoding of char8_t is trivial */
{
	return {{this->head(), this->tail()}, {&rhs[N - N], &rhs[N - 1]}};
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::operator+(__16STR__(rhs)) const noexcept -> concat<txt<local>, txt<"UTF-16">> /* encoding of char16_t is trivial */
{
	return {{this->head(), this->tail()}, {&rhs[N - N], &rhs[N - 1]}};
}

template <typename model, format_t local>
template <size_t                       N> constexpr auto API<model, local>::operator+(__32STR__(rhs)) const noexcept -> concat<txt<local>, txt<"UTF-32">> /* encoding of char32_t is trivial */
{
	return {{this->head(), this->tail()}, {&rhs[N - N], &rhs[N - 1]}};
}

#pragma endregion core
#pragma region core::concat

template <typename model, format_t local>
template <typename lhs_t, typename rhs_t>
template <format_t alien, typename arena> constexpr API<model, local>::concat<lhs_t, rhs_t>::operator str<alien, arena>() const noexcept
{
	using T = typename codec<local>::unit_t;
	using U = typename codec<alien>::unit_t;

	size_t size {0};

	// phase 1: calc length
	this->__for_each__([&]<format_t exotic>(txt<exotic> _) constexpr noexcept
	{
		if constexpr (exotic == alien) { size += detail::__units__<alien>(_.head(), _.tail()); }
		if constexpr (exotic != alien) { for (auto code : _) size += codec<alien>::size(code); }
	});

	str<alien, arena> out;

	out.capacity(size);
	out.__size__(size);

	U* ptr {out.__head__()};

	// phase 2: copy buffer
	this->__for_each__([&]<format_t exotic>(txt<exotic> _) constexpr noexcept
	{
		if constexpr (exotic == alien) { ptr += detail::__fcopy__<exotic, alien>(_.head(), _.tail(), ptr); }
		if constexpr (exotic != alien) { ptr += detail::__fcopy__<exotic, alien>(_.head(), _.tail(), ptr); }
	});

	return out;
}

template <typename model, format_t local>
template <typename lhs_t, typename rhs_t> constexpr auto API<model, local>::concat<lhs_t, rhs_t>::__for_each__(const auto&& fun) const noexcept -> void
{
	if constexpr (requires(lhs_t l) { l.__for_each__(fun); })
	{ this->lhs.__for_each__(fun); } else { fun(this->lhs); }

	if constexpr (requires(rhs_t r) { r.__for_each__(fun); })
	{ this->rhs.__for_each__(fun); } else { fun(this->rhs); }
}

template <typename model, format_t local>
template <typename lhs_t, typename rhs_t>
template <format_t alien, typename arena> constexpr auto API<model, local>::concat<lhs_t, rhs_t>::operator+(__OWNED__(rhs)) noexcept -> concat<concat, txt<alien>>
{
	return {*this, {rhs}};
}

template <typename model, format_t local>
template <typename lhs_t, typename rhs_t>
template <format_t alien /* can't own */> constexpr auto API<model, local>::concat<lhs_t, rhs_t>::operator+(__SLICE__(rhs)) noexcept -> concat<concat, txt<alien>>
{
	return {*this, {rhs}};
}

template <typename model, format_t local>
template <typename lhs_t, typename rhs_t>
template <size_t                       N> constexpr auto API<model, local>::concat<lhs_t, rhs_t>::operator+(__08STR__(rhs)) noexcept -> concat<concat, txt<"UTF-8">>
{
	return {*this, {rhs}};
}

template <typename model, format_t local>
template <typename lhs_t, typename rhs_t>
template <size_t                       N> constexpr auto API<model, local>::concat<lhs_t, rhs_t>::operator+(__16STR__(rhs)) noexcept -> concat<concat, txt<"UTF-16">>
{
	return {*this, {rhs}};
}

template <typename model, format_t local>
template <typename lhs_t, typename rhs_t>
template <size_t                       N> constexpr auto API<model, local>::concat<lhs_t, rhs_t>::operator+(__32STR__(rhs)) noexcept -> concat<concat, txt<"UTF-32">>
{
	return {*this, {rhs}};
}

#pragma endregion core::concat
#pragma region core::const_forward_iterator

template <typename model, format_t local> constexpr API<model, local>::const_forward_iterator::operator const typename codec<local>::unit_t*() const noexcept
{
	return this->ptr;
}

template <typename model, format_t local> constexpr API<model, local>::const_forward_iterator::operator const typename codec<local>::unit_t*() /*&*/ noexcept
{
	return this->ptr;
}

template <typename model, format_t local> constexpr auto API<model, local>::const_forward_iterator::operator*() const noexcept -> value_type
{
	char32_t code;

	codec<local>::decode(this->ptr, code, codec<local>::next(this->ptr));

	return code;
}

template <typename model, format_t local> constexpr auto API<model, local>::const_forward_iterator::operator++(   ) noexcept -> const_forward_iterator&
{
	this->ptr += codec<local>::next(this->ptr);

	return *this;
}

template <typename model, format_t local> constexpr auto API<model, local>::const_forward_iterator::operator++(int) noexcept -> const_forward_iterator
{
	auto clone {*this};
	         ++(*this);

	return clone;
}

template <typename model, format_t local> constexpr auto API<model, local>::const_forward_iterator::operator--(   ) noexcept -> const_forward_iterator&
{
	this->ptr += codec<local>::back(this->ptr);

	return *this;
}

template <typename model, format_t local> constexpr auto API<model, local>::const_forward_iterator::operator--(int) noexcept -> const_forward_iterator
{
	auto clone {*this};
	         --(*this);

	return clone;
}

template <typename model, format_t local> constexpr auto API<model, local>::const_forward_iterator::operator+(size_t value) noexcept -> const_forward_iterator
{
	auto clone {*this};

	for (size_t i {0}; i < value; ++i) { ++clone; }

	return clone;
}

template <typename model, format_t local> constexpr auto API<model, local>::const_forward_iterator::operator-(size_t value) noexcept -> const_forward_iterator
{
	auto clone {*this};

	for (size_t i {0}; i < value; ++i) { --clone; }

	return clone;
}

template <typename model, format_t local> constexpr auto API<model, local>::const_forward_iterator::operator+=(size_t value) noexcept -> const_forward_iterator&
{
	// auto clone {*this};

	for (size_t i {0}; i < value; ++i) { ++(*this); }

	return *this;
}

template <typename model, format_t local> constexpr auto API<model, local>::const_forward_iterator::operator-=(size_t value) noexcept -> const_forward_iterator&
{
	// auto clone {*this};

	for (size_t i {0}; i < value; ++i) { --(*this); }

	return *this;
}

#pragma endregion core::const_forward_iterator
#pragma region core::const_reverse_iterator

template <typename model, format_t local> constexpr API<model, local>::const_reverse_iterator::operator const typename codec<local>::unit_t*() const noexcept
{
	return this->ptr;
}

template <typename model, format_t local> constexpr API<model, local>::const_reverse_iterator::operator const typename codec<local>::unit_t*() /*&*/ noexcept
{
	return this->ptr;
}

template <typename model, format_t local> constexpr auto API<model, local>::const_reverse_iterator::operator*() const noexcept -> value_type
{
	char32_t code;

	codec<local>::decode(this->ptr, code, codec<local>::back(this->ptr));

	return code;
}

template <typename model, format_t local> constexpr auto API<model, local>::const_reverse_iterator::operator++(   ) noexcept -> const_reverse_iterator&
{
	this->ptr += codec<local>::back(this->ptr);

	return *this;
}

template <typename model, format_t local> constexpr auto API<model, local>::const_reverse_iterator::operator++(int) noexcept -> const_reverse_iterator
{
	const auto clone {*this};
	               ++(*this);

	return clone;
}

template <typename model, format_t local> constexpr auto API<model, local>::const_reverse_iterator::operator--(   ) noexcept -> const_reverse_iterator&
{
	this->ptr += codec<local>::next(this->ptr);

	return *this;
}

template <typename model, format_t local> constexpr auto API<model, local>::const_reverse_iterator::operator--(int) noexcept -> const_reverse_iterator
{
	const auto clone {*this};
	               --(*this);

	return clone;
}

template <typename model, format_t local> constexpr auto API<model, local>::const_reverse_iterator::operator+(size_t value) noexcept -> const_reverse_iterator
{
	auto clone {*this};

	for (size_t i {0}; i < value; ++i) { ++clone; }

	return clone;
}

template <typename model, format_t local> constexpr auto API<model, local>::const_reverse_iterator::operator-(size_t value) noexcept -> const_reverse_iterator
{
	auto clone {*this};

	for (size_t i {0}; i < value; ++i) { --clone; }

	return clone;
}

template <typename model, format_t local> constexpr auto API<model, local>::const_reverse_iterator::operator+=(size_t value) noexcept -> const_reverse_iterator&
{
	for (size_t i {0}; i < value; ++i) { ++(*this); }

	return *this;
}

template <typename model, format_t local> constexpr auto API<model, local>::const_reverse_iterator::operator-=(size_t value) noexcept -> const_reverse_iterator&
{
	for (size_t i {0}; i < value; ++i) { --(*this); }

	return *this;
}

#pragma endregion core::const_reverse_iterator
#pragma region core::detail

template <format_t source> constexpr auto detail::__units__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail) noexcept -> size_t
{
	   using T = typename codec<source>::unit_t;
	// using U = typename codec<target>::unit_t;

	return tail - head;
}

template <format_t source> constexpr auto detail::__codes__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail) noexcept -> size_t
{
	   using T = typename codec<source>::unit_t;
	// using U = typename codec<target>::unit_t;

	if constexpr (!codec<source>::is_variable)
	{
		return __units__<source>(head, tail);
	}

	if constexpr (codec<source>::is_stateful)
	{
		// TODO: abstracts pattern matching
	}

	size_t out {0};

	for (const T* ptr {head}; ptr < tail;
	     ptr += codec<source>::next(ptr))
	{
		++out;
	}
	return out;
}

template <format_t source,
          format_t target> constexpr auto detail::__fcopy__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail,
                                                                                                        /*&*/ typename codec<target>::unit_t* dest) noexcept -> size_t
{
	using T = typename codec<source>::unit_t;
	using U = typename codec<target>::unit_t;

	if constexpr (source == target)
	{
		const U* ptr {head};
		/*&*/ T* out {dest};

		for (; ptr <= tail; ++ptr, ++out) *out = *ptr;

		return detail::__units__<target>(head, tail);
	}

	if constexpr (source != target)
	{
		U* out {dest};

		for (const T* ptr {head}; ptr < tail; )
		{
			char32_t code;

			const auto T_step {codec<source>::next(ptr)};
			codec<source>::decode(ptr, code, T_step);
			const auto U_step {codec<target>::size(code)};
			codec<target>::encode(code, out, U_step);

			ptr += T_step;
			out += U_step;
		}

		return detail::__units__<target>(dest, out);
	}
}

template <format_t source,
          format_t target> constexpr auto detail::__rcopy__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail,
                                                                                                        /*&*/ typename codec<target>::unit_t* dest) noexcept -> size_t
{
	using T = typename codec<source>::unit_t;
	using U = typename codec<target>::unit_t;

	if constexpr (source == target)
	{
		const U* ptr {head + (tail - head)};
		/*&*/ T* out {dest + (tail - head)};

		for (; head <= ptr; --ptr, --out) *out = *ptr;

		return detail::__units__<target>(head, tail);
	}

	if constexpr (source != target)
	{
		U* out {dest};

		for (const T* ptr {head}; ptr < tail; )
		{
			char32_t code;

			const auto T_step {codec<source>::next(ptr)};
			codec<source>::decode(ptr, code, T_step);
			const auto U_step {codec<target>::size(code)};

			ptr += T_step;
			out += U_step;
		}

		for (const T* ptr {tail}; head < ptr; )
		{
			char32_t code;

			const auto T_step {codec<source>::back(ptr)};
			codec<source>::decode(ptr, code, T_step);
			const auto U_step {codec<target>::size(code)};
			codec<target>::encode(code, out, U_step);

			ptr += T_step;
			out += U_step;
		}

		return detail::__units__<target>(dest, out);
	}
}

template <format_t source,
          format_t target> constexpr auto detail::__equal__(const typename codec<source>::unit_t* lhs_0, const typename codec<source>::unit_t* lhs_N,
                                                            const typename codec<target>::unit_t* rhs_0, const typename codec<target>::unit_t* rhs_N) noexcept -> bool
{
	using T = typename codec<source>::unit_t;
	using U = typename codec<target>::unit_t;

	if constexpr (source == target)
	{
		if (lhs_0 == rhs_0
		    &&
		    lhs_N == rhs_N)
		{
			return true;
		}

		return detail::__units__<source>(lhs_0, lhs_N)
		       ==
		       detail::__units__<target>(rhs_0, rhs_N)
		       &&
		       std::ranges::equal(lhs_0, lhs_N, rhs_0, rhs_N);
	}

	if constexpr (source != target)
	{
		const T* lhs_ptr {lhs_0};
		const U* rhs_ptr {rhs_0};

		for (; lhs_ptr < lhs_N && rhs_ptr < rhs_N; )
		{
			char32_t T_code;
			char32_t U_code;

			const auto T_step {codec<source>::next(lhs_ptr)};
			const auto U_step {codec<target>::next(rhs_ptr)};

			codec<source>::decode(lhs_ptr, T_code, T_step);
			codec<target>::decode(rhs_ptr, U_code, U_step);

			if (T_code != U_code)
			{
				return false;
			}

			lhs_ptr += T_step;
			rhs_ptr += U_step;
		}

		return lhs_ptr == lhs_N && rhs_ptr == rhs_N;
	}
}

template <format_t source,
          format_t target> constexpr auto detail::__nqual__(const typename codec<source>::unit_t* lhs_0, const typename codec<source>::unit_t* lhs_N,
                                                            const typename codec<target>::unit_t* rhs_0, const typename codec<target>::unit_t* rhs_N) noexcept -> bool
{
	using T = typename codec<source>::unit_t;
	using U = typename codec<target>::unit_t;

	if constexpr (source == target)
	{
		if (lhs_0 == rhs_0
		    &&
		    lhs_N == rhs_N)
		{
			return false;
		}

		return detail::__units__<source>(lhs_0, lhs_N)
		       !=
		       detail::__units__<target>(rhs_0, rhs_N)
		       ||
		       !std::ranges::equal(lhs_0, lhs_N, rhs_0, rhs_N);
	}

	if constexpr (source != target)
	{
		const T* lhs_ptr {lhs_0};
		const U* rhs_ptr {rhs_0};

		for (; lhs_ptr < lhs_N && rhs_ptr < rhs_N; )
		{
			char32_t T_code;
			char32_t U_code;

			const auto T_step {codec<source>::next(lhs_ptr)};
			const auto U_step {codec<target>::next(rhs_ptr)};

			codec<source>::decode(lhs_ptr, T_code, T_step);
			codec<target>::decode(rhs_ptr, U_code, U_step);

			if (T_code != U_code)
			{
				return true;
			}

			lhs_ptr += T_step;
			rhs_ptr += U_step;
		}

		return lhs_ptr != lhs_N || rhs_ptr != rhs_N;
	}
}

template <format_t source,
          format_t target> constexpr auto detail::__swith__(const typename codec<source>::unit_t* lhs_0, const typename codec<source>::unit_t* lhs_N,
                                                            const typename codec<target>::unit_t* rhs_0, const typename codec<target>::unit_t* rhs_N) noexcept -> bool
{
	using T = typename codec<source>::unit_t;
	using U = typename codec<target>::unit_t;

	if constexpr (source == target)
	{
		if (lhs_0 == rhs_0
		    &&
		    lhs_N == rhs_N)
		{
			return true;
		}

		const auto lhs_len {detail::__units__<source>(lhs_0, lhs_N)};
		const auto rhs_len {detail::__units__<target>(rhs_0, rhs_N)};

		return rhs_len == 0 // if rhs(delimeter) is an empty string (e.g. str.starts_with(""))
		       ||
		       (lhs_len >= rhs_len && std::ranges::equal(lhs_0, lhs_0 + rhs_len, rhs_0, rhs_N));
	}

	if constexpr (source != target)
	{
		const T* lhs_ptr {lhs_0};
		const U* rhs_ptr {rhs_0};

		for (; lhs_ptr < lhs_N && rhs_ptr < rhs_N; )
		{
			char32_t T_code;
			char32_t U_code;

			const auto T_step {codec<source>::next(lhs_ptr)};
			const auto U_step {codec<target>::next(rhs_ptr)};

			codec<source>::decode(lhs_ptr, T_code, T_step);
			codec<target>::decode(rhs_ptr, U_code, U_step);

			if (T_code != U_code)
			{
				return false;
			}

			lhs_ptr += T_step;
			rhs_ptr += U_step;
		}

		return rhs_ptr == rhs_N;
	}
}

template <format_t source,
          format_t target> constexpr auto detail::__ewith__(const typename codec<source>::unit_t* lhs_0, const typename codec<source>::unit_t* lhs_N,
                                                            const typename codec<target>::unit_t* rhs_0, const typename codec<target>::unit_t* rhs_N) noexcept -> bool
{
	using T = typename codec<source>::unit_t;
	using U = typename codec<target>::unit_t;

	if constexpr (source == target)
	{
		if (lhs_0 == rhs_0
		    &&
		    lhs_N == rhs_N)
		{
			return true;
		}

		const auto lhs_len {detail::__units__<source>(lhs_0, lhs_N)};
		const auto rhs_len {detail::__units__<target>(rhs_0, rhs_N)};

		return rhs_len == 0 // if rhs(delimeter) is an empty string (e.g. str.ends_with(""))
		       ||
		       (lhs_len >= rhs_len && std::ranges::equal(lhs_N - rhs_len, lhs_N, rhs_0, rhs_N));
	}

	if constexpr (source != target)
	{
		const T* lhs_ptr {lhs_N};
		const U* rhs_ptr {rhs_N};

		for (; lhs_0 < lhs_ptr && rhs_0 < rhs_ptr; )
		{
			char32_t T_code;
			char32_t U_code;

			const auto T_step {codec<source>::back(lhs_ptr)};
			const auto U_step {codec<target>::back(rhs_ptr)};

			codec<source>::decode(lhs_ptr, T_code, T_step);
			codec<target>::decode(rhs_ptr, U_code, U_step);

			if (T_code != U_code)
			{
				return false;
			}

			lhs_ptr += T_step;
			rhs_ptr += U_step;
		}

		return rhs_ptr == rhs_0;
	}
}

template <format_t source,
          format_t target> constexpr auto detail::__scan__(const typename codec<source>::unit_t* lhs_0, const typename codec<source>::unit_t* lhs_N,
                                                           const typename codec<target>::unit_t* rhs_0, const typename codec<target>::unit_t* rhs_N,
                                                                                                        const /* (head, tail) -> VOID! */ auto& fun) noexcept -> void
{
	using T = typename codec<source>::unit_t;
	using U = typename codec<target>::unit_t;

	if constexpr (source == target)
	{
		const auto lhs_len {detail::__units__<source>(lhs_0, lhs_N)};
		const auto rhs_len {detail::__units__<target>(rhs_0, rhs_N)};

		if (0 < lhs_len && 0 < rhs_len)
		{
			if (lhs_len == rhs_len)
			{
				if (lhs_0 == rhs_0
				    &&
				    lhs_N == rhs_N)
				{
					fun(lhs_0, lhs_N);
				}
				else if (detail::__equal__<source, target>(lhs_0, lhs_N,
				                                           rhs_0, rhs_N))
				{
					fun(lhs_0, lhs_N);
				}
			}
			else if (lhs_len < rhs_len)
			{
				// nothing to do...
			}
			else if (lhs_len > rhs_len)
			{
				const T* out;

				// uint32_t step;
				// char32_t code;
				size_t i;
				size_t j;

				typedef char32_t code_t;

				   std::vector<size_t> tbl (rhs_len, 0);
				// std::vector<code_t> rhs (rhs_len, 0);

				i = 0;
				j = 0;

				for (const U* ptr {rhs_0}; ptr < rhs_N; ++ptr, ++i)
				{
					// codec<target>::decode(ptr, rhs[i],
					// step = codec<target>::next(ptr));

					while (0 < j && rhs_0[i] != rhs_0[j])
					{
						j = tbl[j - 1];
					}

					if /* match */ (rhs_0[i] == rhs_0[j])
					{
						tbl[i] = ++j;
					}
				}

				i = 0;
				j = 0;

				for (const T* ptr {lhs_0}; ptr < lhs_N; ++ptr, ++i)
				{
					// codec<source>::decode(ptr, code,
					// step = codec<source>::next(ptr));

					while (0 < j && *ptr != rhs_0[j])
					{
						j = tbl[j - 1];
					}

					if /* match */ (*ptr == rhs_0[j])
					{
						if (j == (  0  )) { out = ptr; /* 1st pos */ }

						++j;

						if (j == rhs_len) { fun(out, ptr + 1); j = 0; }
					}
				}
			}
		}
	}
	
	if constexpr (source != target)
	{
		const auto lhs_len {detail::__codes__<source>(lhs_0, lhs_N)};
		const auto rhs_len {detail::__codes__<target>(rhs_0, rhs_N)};

		if (0 < lhs_len && 0 < rhs_len)
		{
			if (lhs_len == rhs_len)
			{
				if (detail::__equal__<source, target>(lhs_0, lhs_N,
				                                      rhs_0, rhs_N))
				{
					fun(lhs_0, lhs_N);
				}
			}
			else if (lhs_len < rhs_len)
			{
				// nothing to do...
			}
			else if (lhs_len > rhs_len)
			{
				const T* out;

				uint32_t step;
				char32_t code;
				size_t i;
				size_t j;

				typedef char32_t code_t;

				std::vector<size_t> tbl (rhs_len, 0);
				std::vector<code_t> rhs (rhs_len, 0);

				i = 0;
				j = 0;

				for (const U* ptr {rhs_0}; ptr < rhs_N; ptr += step, ++i)
				{
					codec<target>::decode(ptr, rhs[i],
					step = codec<target>::next(ptr));

					while (0 < j && rhs[i] != rhs[j])
					{
						j = tbl[j - 1];
					}

					if /* match */ (rhs[i] == rhs[j])
					{
						tbl[i] = ++j;
					}
				}

				i = 0;
				j = 0;

				for (const T* ptr {lhs_0}; ptr < lhs_N; ptr += step, ++i)
				{
					codec<source>::decode(ptr, code,
					step = codec<source>::next(ptr));

					while (0 < j && code != rhs[j])
					{
						j = tbl[j - 1];
					}

					if /* match */ (code == rhs[j])
					{
						if (j == (  0  )) { out = ptr; /* get 1st pos */ }

						++j;

						if (j == rhs_len) { j = 0; fun(out, ptr + step); }
					}
				}
			}
		}
	}
}

template <format_t source,
          format_t target> constexpr auto detail::__split__(const typename codec<source>::unit_t* lhs_0, const typename codec<source>::unit_t* lhs_N,
                                                            const typename codec<target>::unit_t* rhs_0, const typename codec<target>::unit_t* rhs_N) noexcept -> std::vector<txt<source>>
{
	using T = typename codec<source>::unit_t;
	using U = typename codec<target>::unit_t;

	std::vector<txt<source>> out;

	const T* last {lhs_0};

	detail::__scan__<source, target>(lhs_0, lhs_N,
	                                 rhs_0, rhs_N,
		// on every distinct L → R match
		[&](const T* head, const T* tail)
		{
			if (head != last)
			{
				out.emplace_back(last, head);

				last = tail; // update anchor
			}
		}
	);

	if (last != lhs_0 && last < lhs_N)
	{
		out.emplace_back(last, lhs_N);
	}

	return out;
}

template <format_t source,
          format_t target> constexpr auto detail::__match__(const typename codec<source>::unit_t* lhs_0, const typename codec<source>::unit_t* lhs_N,
                                                            const typename codec<target>::unit_t* rhs_0, const typename codec<target>::unit_t* rhs_N) noexcept -> std::vector<txt<source>>
{
	using T = typename codec<source>::unit_t;
	using U = typename codec<target>::unit_t;

	std::vector<txt<source>> out;

	// const T* last {lhs_0};

	detail::__scan__<source, target>(lhs_0, lhs_N,
	                                 rhs_0, rhs_N,
		// on every distinct L → R match
		[&](const T* head, const T* tail)
		{
			out.emplace_back(head, tail);
		}
	);

	// if (last != lhs_0 && last < lhs_N)
	// {
	// 	out.emplace_back(last, lhs_N);
	// }

	return out;
}

template <format_t source> constexpr auto detail::__substr__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail, [[maybe_unused]] clamp  start, [[maybe_unused]] clamp  until) noexcept -> txt<source>
{
	   using T = typename codec<source>::unit_t;
	// using U = typename codec<target>::unit_t;

	// e.g. str.substr(N - 1, N - 0);

	assert(until < start);

	const T* foo {tail};
	
	for (size_t i {  0  }; i < until && head < foo; ++i, foo += codec<source>::back(foo)) {}

	const T* bar {foo};

	for (size_t i {until}; i < start && head < bar; ++i, bar += codec<source>::back(bar)) {}

	assert(head <= foo && foo <= tail);
	assert(head <= bar && bar <= tail);

	return {bar, foo};
}

template <format_t source> constexpr auto detail::__substr__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail, [[maybe_unused]] clamp  start, [[maybe_unused]] range  until) noexcept -> txt<source>
{
	   using T = typename codec<source>::unit_t;
	// using U = typename codec<target>::unit_t;

	// e.g. str.substr(N - 1, N);

	const T* foo {tail};

	for (size_t i {  0  }; i < start && head < foo; ++i, foo += codec<source>::back(foo)) {}

	const T* bar {tail};

	assert(head <= foo && foo <= tail);
	assert(head <= bar && bar <= tail);

	return {foo, bar};
}

template <format_t source> constexpr auto detail::__substr__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail, [[maybe_unused]] size_t start, [[maybe_unused]] clamp  until) noexcept -> txt<source>
{
	   using T = typename codec<source>::unit_t;
	// using U = typename codec<target>::unit_t;

	// e.g. str.substr(0, N - 1);

	const T* foo {head};

	for (size_t i {  0  }; i < start && foo < tail; ++i, foo += codec<source>::next(foo)) {}

	const T* bar {tail};

	for (size_t i {  0  }; i < until && head < bar; ++i, bar += codec<source>::back(bar)) {}

	assert(head <= foo && foo <= tail);
	assert(head <= bar && bar <= tail);

	return {foo, bar};
}

template <format_t source> constexpr auto detail::__substr__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail, [[maybe_unused]] size_t start, [[maybe_unused]] range  until) noexcept -> txt<source>
{
	   using T = typename codec<source>::unit_t;
	// using U = typename codec<target>::unit_t;

	// e.g. str.substr(0, N);

	const T* foo {head};

	for (size_t i {  0  }; i < start && foo < tail; ++i, foo += codec<source>::next(foo)) {}

	const T* bar {tail};

	assert(head <= foo && foo <= tail);
	assert(head <= bar && bar <= tail);

	return {foo, bar};
}

template <format_t source> constexpr auto detail::__substr__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail, [[maybe_unused]] size_t start, [[maybe_unused]] size_t until) noexcept -> txt<source>
{
	   using T = typename codec<source>::unit_t;
	// using U = typename codec<target>::unit_t;

	// e.g. str.substr(6, 9);

	assert(start < until);

	const T* foo {head};

	for (size_t i {  0  }; i < start && foo < tail; ++i, foo += codec<source>::next(foo)) {}

	const T* bar {foo};

	for (size_t i {start}; i < until && bar < tail; ++i, bar += codec<source>::next(bar)) {}

	assert(head <= foo && foo <= tail);
	assert(head <= bar && bar <= tail);

	return {foo, bar};
}

#pragma endregion core::detail
#pragma region SSO23

template <format_t local, typename alloc> constexpr str<local, alloc>::buffer::operator const typename str<local, alloc>::unit_t*() const noexcept
{
	return this->head;
}

template <format_t local, typename alloc> constexpr str<local, alloc>::buffer::operator /*&*/ typename str<local, alloc>::unit_t*() /*&*/ noexcept
{
	return this->head;
}

template <format_t local, typename alloc> constexpr str<local, alloc>::storage::storage() noexcept
{
	   this->__union__.bytes[RMB] = MAX << SFT;
	// std::construct_at(&this->__union__.large);
}

template <format_t local, typename alloc> constexpr str<local, alloc>::storage::~storage() noexcept
{
	if (this->mode() == LARGE)
	{
		std::allocator_traits<alloc>::deallocate((*this),
		                                         (*this).__union__.large.head,
		                                         (*this).__union__.large.last
		                                         -
		                                         (*this).__union__.large.head);
	}
}

template <format_t local, typename alloc> constexpr auto str<local, alloc>::storage::mode() const noexcept -> mode_t
{
	return static_cast<mode_t>(this->__union__.bytes[RMB] & MSK);
}

template <format_t local, typename alloc> constexpr auto str<local, alloc>::storage::mode() /*&*/ noexcept -> mode_t
{
	return static_cast<mode_t>(this->__union__.bytes[RMB] & MSK);
}

template <format_t local, typename alloc> constexpr auto str<local, alloc>::__head__() const noexcept -> const unit_t*
{
	return this->store.mode() == SMALL
	       ?
	       this->store.__union__.small // ✨ roeses are red, violets are blue
	       :
	       this->store.__union__.large; // ✨ roeses are red, violets are blue
}

template <format_t local, typename alloc> constexpr auto str<local, alloc>::__head__() /*&*/ noexcept -> /*&*/ unit_t*
{
	return this->store.mode() == SMALL
	       ?
	       this->store.__union__.small // ✨ roeses are red, violets are blue
	       :
	       this->store.__union__.large; // ✨ roeses are red, violets are blue
}

template <format_t local, typename alloc> constexpr auto str<local, alloc>::__tail__() const noexcept -> const unit_t*
{
	return this->store.mode() == SMALL
	       ?
	       &this->store.__union__.small[MAX - (this->store.__union__.bytes[RMB] >> SFT)]
	       :
	       &this->store.__union__.large[this->store.__union__.large.size /* get as-is */];
}

template <format_t local, typename alloc> constexpr auto str<local, alloc>::__tail__() /*&*/ noexcept -> /*&*/ unit_t*
{
	return this->store.mode() == SMALL
	       ?
	       &this->store.__union__.small[MAX - (this->store.__union__.bytes[RMB] >> SFT)]
	       :
	       &this->store.__union__.large[this->store.__union__.large.size /* get as-is */];
}

template <format_t local, typename alloc> constexpr auto str<local, alloc>::__last__() const noexcept -> const unit_t*
{
	return this->store.mode() == SMALL
	       ?
	       &this->store.__union__.small[MAX] // ✨ roeses are red, violets are blue
	       :
	       this->store.__union__.large.last; // ✨ roeses are red, violets are blue
}

template <format_t local, typename alloc> constexpr auto str<local, alloc>::__last__() /*&*/ noexcept -> /*&*/ unit_t*
{
	return this->store.mode() == SMALL
	       ?
	       &this->store.__union__.small[MAX] // ✨ roeses are red, violets are blue
	       :
	       this->store.__union__.large.last; // ✨ roeses are red, violets are blue
}

template <format_t local, typename alloc> constexpr auto str<local, alloc>::capacity(/* getter */) const noexcept -> size_t
{
	return this->store.mode() == SMALL
	       ?
	       MAX // or calculate the ptrdiff_t just as large mode as shown down below
	       :
	       this->store.__union__.large.last - this->store.__union__.large.head - 1;
}

template <format_t local, typename alloc> constexpr auto str<local, alloc>::capacity(size_t value) /*&*/ noexcept -> void
{
	if (this->capacity() < value)
	{
		const auto size {detail::__units__<local>(this->__head__(), this->__tail__())};

		// phase 1: request
		unit_t* head {std::allocator_traits<alloc>::allocate(this->store, value + 1)};
		unit_t* last {/* half open ptrn; one-past-the-end */(head/*<&>*/+ value + 1)};

		// phase 2: migrate
		detail::__fcopy__<local, local>
		(
			this->__head__(), // const T*
			this->__tail__(), // const T*
			head              //       T*
		);

		// phase 3: release
		if (this->store.mode() == LARGE)
		{
			std::allocator_traits<alloc>::deallocate(this->store,
			                                         this->store.__union__.large.head,
			                                         this->store.__union__.large.last
			                                         -
			                                         this->store.__union__.large.head);
		}

		this->store.__union__.large.head = head;
		this->store.__union__.large.last = last;
		this->store.__union__.large.size = size;
		this->store.__union__.large.meta = LARGE;
	}
}

template <format_t local, typename alloc> constexpr auto str<local, alloc>::__size__(size_t value) noexcept -> void
{
	switch (this->store.mode())
	{
		case SMALL:
		{
			const auto slot {(MAX - value) << SFT};

			//┌────────────┐    ┌───────[LE]───────┐
			//│ 0b0XXXXXXX │ -> │ no need for skip │
			//└────────────┘    └──────────────────┘

			//┌────────────┐    ┌───────[BE]───────┐
			//│ 0bXXXXXXX0 │ -> │ skip right 1 bit │
			//└────────────┘    └──────────────────┘

			this->store.__union__.bytes[RMB] = slot;
			this->store.__union__.small[value] = '\0';
			break;
		}
		case LARGE:
		{
			this->store.__union__.large.size = value;
			this->store.__union__.large[value] = '\0';
			break;
		}
	}
}

#pragma endregion SSO23
#pragma region str

template <format_t local, typename alloc> constexpr str<local, alloc>::operator const unit_t*() const noexcept
{
	return this->__head__();
}

template <format_t local, typename alloc> constexpr str<local, alloc>::operator /*&*/ unit_t*() /*&*/ noexcept
{
	return this->__head__();
}

template <format_t local, typename alloc> constexpr str<local, alloc>::str(const str& other) noexcept
{
	// copy constructor
	if (this != &other)
	{
		this->capacity(other.size());

		detail::__fcopy__<local, local>
		(
			other.__head__(), // const T*
			other.__tail__(), // const T*
			this->__head__()  //       T*
		);

		this->__size__(other.size());
	}
}

template <format_t local, typename alloc> constexpr str<local, alloc>::str(/*&*/ str&& other) noexcept
{
	// move constructor
	if (this != &other)
	{
		std::swap(this->store.__union__.bytes,
		          other.store.__union__.bytes);
	}
}

template <format_t local, typename alloc> constexpr auto str<local, alloc>::operator=(const str& other) noexcept -> str&
{
	// copy assignment
	if (this != &other)
	{
		this->capacity(other.size());

		detail::__fcopy__<local, local>
		(
			other.__head__(), // const T*
			other.__tail__(), // const T*
			this->__head__()  //       T*
		);

		this->__size__(other.size());
	}
	return *this;
}

template <format_t local, typename alloc> constexpr auto str<local, alloc>::operator=(/*&*/ str&& other) noexcept -> str&
{
	// move assignment
	if (this != &other)
	{
		std::swap(this->store.__union__.bytes,
		          other.store.__union__.bytes);
	}
	return *this;
}

template <format_t local, typename alloc>
template <format_t alien, typename arena> constexpr str<local, alloc>::str(__OWNED__(str)) noexcept
{
	this->operator=(str);
}

template <format_t local, typename alloc>
template <format_t alien /* can't own */> constexpr str<local, alloc>::str(__SLICE__(str)) noexcept
{
	this->operator=(str);
}

template <format_t local, typename alloc>
template <size_t                       N> constexpr str<local, alloc>::str(__EQSTR__(str)) noexcept requires (std::is_same_v<unit_t, char>)
{
	this->operator=(str);
}

template <format_t local, typename alloc>
template <size_t                       N> constexpr str<local, alloc>::str(__08STR__(str)) noexcept /* encoding of char8_t is trivial */
{
	this->operator=(str);
}

template <format_t local, typename alloc>
template <size_t                       N> constexpr str<local, alloc>::str(__16STR__(str)) noexcept /* encoding of char16_t is trivial */
{
	this->operator=(str);
}

template <format_t local, typename alloc>
template <size_t                       N> constexpr str<local, alloc>::str(__32STR__(str)) noexcept /* encoding of char32_t is trivial */
{
	this->operator=(str);
}

template <format_t local, typename alloc> constexpr auto str<local, alloc>::begin() /*&*/ noexcept -> forward_iterator
{
	return {this, this->__head__(), 0, (     0     ), forward_iterator::it_offset_relative_tag::HEAD,
	                                                  forward_iterator::it_cursor_category_tag::LTOR};
}

template <format_t local, typename alloc> constexpr auto str<local, alloc>::end() /*&*/ noexcept -> forward_iterator
{
	return {this, this->__tail__(), 0, this->length(), forward_iterator::it_offset_relative_tag::TAIL,
	                                                   forward_iterator::it_cursor_category_tag::LTOR};
}

template <format_t local, typename alloc> constexpr auto str<local, alloc>::rbegin() /*&*/ noexcept -> reverse_iterator
{
	return {this, this->__tail__(), 0, (     0     ), reverse_iterator::it_offset_relative_tag::TAIL,
	                                                  reverse_iterator::it_cursor_category_tag::RTOL};
}

template <format_t local, typename alloc> constexpr auto str<local, alloc>::rend() /*&*/ noexcept -> reverse_iterator
{
	return {this, this->__head__(), 0, this->length(), reverse_iterator::it_offset_relative_tag::HEAD,
	                                                   reverse_iterator::it_cursor_category_tag::RTOL};
}

template <format_t local, typename alloc>
template <format_t alien, typename arena> constexpr auto str<local, alloc>::operator=(__OWNED__(rhs))& noexcept -> str&
{
	this->__assign__<alien>(rhs.__head__(), rhs.__tail__()); return *this;
}

template <format_t local, typename alloc>
template <format_t alien /* can't own */> constexpr auto str<local, alloc>::operator=(__SLICE__(rhs))& noexcept -> str&
{
	this->__assign__<alien>(rhs.__head__  , rhs.__tail__  ); return *this;
}

template <format_t local, typename alloc>
template <size_t                       N> constexpr auto str<local, alloc>::operator=(__EQSTR__(rhs))& noexcept -> str& requires (std::is_same_v<unit_t, char>)
{
	this->__assign__<local>(&rhs[N - N], &rhs[N - 1]); return *this;
}

template <format_t local, typename alloc>
template <size_t                       N> constexpr auto str<local, alloc>::operator=(__08STR__(rhs))& noexcept -> str& /* encoding of char8_t is trivial */
{
	this->__assign__<"UTF-8">(&rhs[N - N], &rhs[N - 1]); return *this;
}

template <format_t local, typename alloc>
template <size_t                       N> constexpr auto str<local, alloc>::operator=(__16STR__(rhs))& noexcept -> str& /* encoding of char16_t is trivial */
{
	this->__assign__<"UTF-16">(&rhs[N - N], &rhs[N - 1]); return *this;
}

template <format_t local, typename alloc>
template <size_t                       N> constexpr auto str<local, alloc>::operator=(__32STR__(rhs))& noexcept -> str& /* encoding of char32_t is trivial */
{
	this->__assign__<"UTF-32">(&rhs[N - N], &rhs[N - 1]); return *this;
}

template <format_t local, typename alloc>
template <format_t alien, typename arena> constexpr auto str<local, alloc>::operator=(__OWNED__(rhs))&& noexcept -> str&&
{
	return std::move(this->operator=(rhs));
}

template <format_t local, typename alloc>
template <format_t alien /* can't own */> constexpr auto str<local, alloc>::operator=(__SLICE__(rhs))&& noexcept -> str&&
{
	return std::move(this->operator=(rhs));
}

template <format_t local, typename alloc>
template <size_t                       N> constexpr auto str<local, alloc>::operator=(__EQSTR__(rhs))&& noexcept -> str&& requires (std::is_same_v<unit_t, char>)
{
	return std::move(this->operator=(rhs));
}

template <format_t local, typename alloc>
template <size_t                       N> constexpr auto str<local, alloc>::operator=(__08STR__(rhs))&& noexcept -> str&& /* encoding of char8_t is trivial */
{
	return std::move(this->operator=(rhs));
}

template <format_t local, typename alloc>
template <size_t                       N> constexpr auto str<local, alloc>::operator=(__16STR__(rhs))&& noexcept -> str&& /* encoding of char16_t is trivial */
{
	return std::move(this->operator=(rhs));
}

template <format_t local, typename alloc>
template <size_t                       N> constexpr auto str<local, alloc>::operator=(__32STR__(rhs))&& noexcept -> str&& /* encoding of char32_t is trivial */
{
	return std::move(this->operator=(rhs));
}

template <format_t local, typename alloc>
template <format_t alien, typename arena> constexpr auto str<local, alloc>::operator+=(__OWNED__(rhs))& noexcept -> str&
{
	this->__concat__<alien>(rhs.__head__(), rhs.__tail__()); return *this;
}

template <format_t local, typename alloc>
template <format_t alien /* can't own */> constexpr auto str<local, alloc>::operator+=(__SLICE__(rhs))& noexcept -> str&
{
	this->__concat__<alien>(rhs.__head__  , rhs.__tail__  ); return *this;
}

template <format_t local, typename alloc>
template <size_t                       N> constexpr auto str<local, alloc>::operator+=(__EQSTR__(rhs))& noexcept -> str& requires (std::is_same_v<unit_t, char>)
{
	this->__concat__<local>(&rhs[N - N], &rhs[N - 1]); return *this;
}

template <format_t local, typename alloc>
template <size_t                       N> constexpr auto str<local, alloc>::operator+=(__08STR__(rhs))& noexcept -> str& /* encoding of char8_t is trivial */
{
	this->__concat__<"UTF-8">(&rhs[N - N], &rhs[N - 1]); return *this;
}

template <format_t local, typename alloc>
template <size_t                       N> constexpr auto str<local, alloc>::operator+=(__16STR__(rhs))& noexcept -> str& /* encoding of char16_t is trivial */
{
	this->__concat__<"UTF-16">(&rhs[N - N], &rhs[N - 1]); return *this;
}

template <format_t local, typename alloc>
template <size_t                       N> constexpr auto str<local, alloc>::operator+=(__32STR__(rhs))& noexcept -> str& /* encoding of char32_t is trivial */
{
	this->__concat__<"UTF-32">(&rhs[N - N], &rhs[N - 1]); return *this;
}

template <format_t local, typename alloc>
template <format_t alien, typename arena> constexpr auto str<local, alloc>::operator+=(__OWNED__(rhs))&& noexcept -> str&&
{
	return std::move(this->operator+=(rhs));
}

template <format_t local, typename alloc>
template <format_t alien /* can't own */> constexpr auto str<local, alloc>::operator+=(__SLICE__(rhs))&& noexcept -> str&&
{
	return std::move(this->operator+=(rhs));
}

template <format_t local, typename alloc>
template <size_t                       N> constexpr auto str<local, alloc>::operator+=(__EQSTR__(rhs))&& noexcept -> str&& requires (std::is_same_v<unit_t, char>)
{
	return std::move(this->operator+=(rhs));
}

template <format_t local, typename alloc>
template <size_t                       N> constexpr auto str<local, alloc>::operator+=(__08STR__(rhs))&& noexcept -> str&& /* encoding of char8_t is trivial */
{
	return std::move(this->operator+=(rhs));
}

template <format_t local, typename alloc>
template <size_t                       N> constexpr auto str<local, alloc>::operator+=(__16STR__(rhs))&& noexcept -> str&& /* encoding of char16_t is trivial */
{
	return std::move(this->operator+=(rhs));
}

template <format_t local, typename alloc>
template <size_t                       N> constexpr auto str<local, alloc>::operator+=(__32STR__(rhs))&& noexcept -> str&& /* encoding of char32_t is trivial */
{
	return std::move(this->operator+=(rhs));
}

template <format_t local, typename alloc> constexpr auto str<local, alloc>::__insert__(unit_t* dest, char32_t code, int8_t step) noexcept -> __insert__t
{
	__insert__t out
	{
		.dest {dest},
		.does_shift {false},
		.does_alloc {false},
	};

	const auto a {0 < step ? +step : -step};
	const auto b {codec<local>::size(code)};

	if (a == b)
	{
		// no need to shift buffer :D
		codec<local>::encode(code, dest, step);
	}
	else if (a < b)
	{
		   out.does_shift = true;
		// out.does_alloc = true;
		
		const auto old_l {this->size()};
		const auto new_l {old_l - a+b };

		unit_t* tail {this->__tail__()};

		// 2x capacity growth
		if (this->capacity() < new_l)
		{
			// out.does_shift = true;
			   out.does_alloc = true;

			/**/ if (0 < step)
			{
				const auto off {dest - this->__head__()};

				this->capacity(old_l * 2);

				dest = this->__head__() + off;
				tail = this->__tail__()      ;
			}
			else if (step < 0)
			{
				const auto off {this->__tail__() - dest};

				this->capacity(old_l * 2);

				dest = this->__tail__() - off;
				tail = this->__tail__()      ;
			}
		}

		/**/ if (0 < step)
		{
			out.dest = dest;

			//                       dest
			//                       ↓
			//┌────────┬─────────────┬───┬──────────────┬────────┐
			//│ <head> │ left buffer │ A │ right buffer │ <tail> │
			//├────────┼─────────────┼───┴───┬──────────┴───┬────┴───┐
			//│ <head> │ left buffer │ [[B]] │ right buffer │ <tail> │
			//└────────┴─────────────┴───────┴──────────────┴────────┘
			//                       ↑
			//                       out.dest

			detail::__rcopy__<local, local>
			(
				dest + a, // const unit_t*
				tail + 0, // const unit_t*
				dest + b  //       unit_t*
			);
			codec<local>::encode(code, out.dest, +b);
		}
		else if (step < 0)
		{
			out.dest = dest - a + b;

			//                           dest
			//                           ↓
			//┌────────┬─────────────┬───┬──────────────┬────────┐
			//│ <head> │ left buffer │ A │ right buffer │ <tail> │
			//├────────┼─────────────┼───┴───┬──────────┴───┬────┴───┐
			//│ <head> │ left buffer │ [[B]] │ right buffer │ <tail> │
			//└────────┴─────────────┴───────┴──────────────┴────────┘
			//                               ↑
			//                               out.dest

			detail::__rcopy__<local, local>
			(
				dest - 0 + 0, // const unit_t*
				tail - 0 + 0, // const unit_t*
				dest - a + b  //       unit_t*
			);
			codec<local>::encode(code, out.dest, -b);
		}
		this->__size__(new_l);
	}
	else if (b < a)
	{
		   out.does_shift = true;
		// out.does_alloc = true;
		
		const auto old_l {this->size()};
		const auto new_l {old_l - a+b };

		unit_t* tail {this->__tail__()};

		/**/ if (0 < step)
		{
			out.dest = dest;

			//                       dest
			//                       ↓
			//┌────────┬─────────────┬───────┬──────────────┬────────┐
			//│ <head> │ left buffer │ [[A]] │ right buffer │ <tail> │
			//├────────┼─────────────┼───┬───┴──────────┬───┴────┬───┘
			//│ <head> │ left buffer │ B │ right buffer │ <tail> │
			//└────────┴─────────────┴───┴──────────────┴────────┘
			//                       ↑
			//                       out.dest

			detail::__fcopy__<local, local>
			(
				dest + a, // const unit_t*
				tail + 0, // const unit_t*
				dest + b  //       unit_t*
			);
			codec<local>::encode(code, out.dest, +b);
		}
		else if (step < 0)
		{
			out.dest = dest - a + b;

			//                               dest
			//                               ↓
			//┌────────┬─────────────┬───────┬──────────────┬────────┐
			//│ <head> │ left buffer │ [[A]] │ right buffer │ <tail> │
			//├────────┼─────────────┼───┬───┴──────────┬───┴────┬───┘
			//│ <head> │ left buffer │ B │ right buffer │ <tail> │
			//└────────┴─────────────┴───┴──────────────┴────────┘
			//                           ↑
			//                           out.dest

			detail::__fcopy__<local, local>
			(
				dest - 0 + 0, // const unit_t*
				tail - 0 + 0, // const unit_t*
				dest - a + b  //       unit_t*
			);
			codec<local>::encode(code, out.dest, -b);
		}
		this->__size__(new_l);
	}
	return out;
}

template <format_t local, typename alloc>
template <format_t alien /* can't own */> constexpr auto str<local, alloc>::__assign__(const typename codec<alien>::unit_t* rhs_0,
                                                                                       const typename codec<alien>::unit_t* rhs_N) noexcept -> __assign__t
{
	using T = typename codec<local>::unit_t;
	using U = typename codec<alien>::unit_t;

	if constexpr (local == alien)
	{
		//────────────────────────────────────────────────────────────────────────────────────────────────────────────────────//┐
		size_t size {0}; /* iteration is unnecessary; just + operation */ { size += detail::__units__<alien>(rhs_0, rhs_N); } //│
		//────────────────────────────────────────────────────────────────────────────────────────────────────────────────────//┘

		this->capacity(size);

		detail::__fcopy__<alien, local>
		(
			rhs_0,           // const unit_t*
			rhs_N,           // const unit_t*
			this->__head__() //       unit_t*
		);

		this->__size__(size);

		return {};
	}

	if constexpr (local != alien)
	{
		//────────────────────────────────────────────────────────────────────────────────────────────────────────────────────//┐
		size_t size {0}; for (const auto code : txt<alien>{rhs_0, rhs_N}) { size += codec<local>::size(code); /* trivial */ } //│
		//────────────────────────────────────────────────────────────────────────────────────────────────────────────────────//┘

		this->capacity(size);

		detail::__fcopy__<alien, local>
		(
			rhs_0,           // const unit_t*
			rhs_N,           // const unit_t*
			this->__head__() //       unit_t*
		);

		this->__size__(size);

		return {};
	}
}

template <format_t local, typename alloc>
template <format_t alien /* can't own */> constexpr auto str<local, alloc>::__concat__(const typename codec<alien>::unit_t* rhs_0,
                                                                                       const typename codec<alien>::unit_t* rhs_N) noexcept -> __concat__t
{
	using T = typename codec<local>::unit_t;
	using U = typename codec<alien>::unit_t;

	if constexpr (local == alien)
	{
		//───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────//┐
		size_t size {this->size()}; /* iteration is unnecessary; just + operation */ { size += detail::__units__<alien>(rhs_0, rhs_N); } //│
		//───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────//┘

		this->capacity(size);

		detail::__fcopy__<alien, local>
		(
			rhs_0,           // const unit_t*
			rhs_N,           // const unit_t*
			this->__tail__() //       unit_t*
		);

		this->__size__(size);

		return {};
	}

	if constexpr (local != alien)
	{
		//───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────//┐
		size_t size {this->size()}; for (const auto code : txt<alien>{rhs_0, rhs_N}) { size += codec<local>::size(code); /* trivial */ } //│
		//───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────//┘

		this->capacity(size);

		detail::__fcopy__<alien, local>
		(
			rhs_0,           // const unit_t*
			rhs_N,           // const unit_t*
			this->__tail__() //       unit_t*
		);

		this->__size__(size);

		return {};
	}
}

#pragma endregion str
#pragma region str::cursor

template <format_t local, typename alloc> template <typename iterator> constexpr str<local, alloc>::cursor<iterator>::operator const typename codec<local>::unit_t*() const noexcept
{
	return this->__needle__();
}

template <format_t local, typename alloc> template <typename iterator> constexpr str<local, alloc>::cursor<iterator>::operator const typename codec<local>::unit_t*() /*&*/ noexcept
{
	return this->__needle__();
}

template <format_t local, typename alloc> template <typename iterator> constexpr auto str<local, alloc>::cursor<iterator>::__needle__() const noexcept -> unit_t*
{
	switch (this->offset_tag)
	{
		case it_offset_relative_tag::HEAD: { return this->common->ptr + this->offset /* origin is head; L → L */; }
		case it_offset_relative_tag::TAIL: { return this->common->ptr - this->offset /* origin is tail; R → L */; }
	}
}

template <format_t local, typename alloc> template <typename iterator> constexpr auto str<local, alloc>::cursor<iterator>::operator*() const noexcept -> value_type
{
	switch (this->offset_tag)
	{
		case it_offset_relative_tag::HEAD: { return {this->common, this->__needle__(), this->offset_tag, this->cursor_tag}; }
		case it_offset_relative_tag::TAIL: { return {this->common, this->__needle__(), this->offset_tag, this->cursor_tag}; }
	}
}

template <format_t local, typename alloc> template <typename iterator> constexpr auto str<local, alloc>::cursor<iterator>::operator++(   ) noexcept -> iterator&
{
	/*&*/ ++this->weight;

	switch (this->cursor_tag)
	{
		case it_cursor_category_tag::LTOR: { this->offset += codec<local>::next(this->__needle__()); break; }
		case it_cursor_category_tag::RTOL: { this->offset -= codec<local>::back(this->__needle__()); break; }
	}

	return static_cast<iterator&>(*this);
}

template <format_t local, typename alloc> template <typename iterator> constexpr auto str<local, alloc>::cursor<iterator>::operator++(int) noexcept -> iterator
{
	const auto clone {*this};
	               ++(*this);

	return static_cast<iterator>(clone);
}

template <format_t local, typename alloc> template <typename iterator> constexpr auto str<local, alloc>::cursor<iterator>::operator--(   ) noexcept -> iterator&
{
	/*&*/ --this->weight;

	switch (this->cursor_tag)
	{
		case it_cursor_category_tag::LTOR: { this->offset -= codec<local>::back(this->__needle__()); break; }
		case it_cursor_category_tag::RTOL: { this->offset += codec<local>::next(this->__needle__()); break; }
	}

	return static_cast<iterator&>(*this);
}

template <format_t local, typename alloc> template <typename iterator> constexpr auto str<local, alloc>::cursor<iterator>::operator--(int) noexcept -> iterator
{
	const auto clone {*this};
	               --(*this);

	return static_cast<iterator>(clone);
}

template <format_t local, typename alloc> template <typename iterator> constexpr auto str<local, alloc>::cursor<iterator>::operator+(size_t value) noexcept -> iterator
{
	auto clone {*this};

	for (size_t i {0}; i < value; ++i) { ++clone; }

	return static_cast<iterator>(clone);
}

template <format_t local, typename alloc> template <typename iterator> constexpr auto str<local, alloc>::cursor<iterator>::operator-(size_t value) noexcept -> iterator
{
	auto clone {*this};

	for (size_t i {0}; i < value; ++i) { --clone; }

	return static_cast<iterator>(clone);
}

template <format_t local, typename alloc> template <typename iterator> constexpr auto str<local, alloc>::cursor<iterator>::operator+=(size_t value) noexcept -> iterator&
{
	for (size_t i {0}; i < value; ++i) { ++(*this); }

	return static_cast<iterator&>(*this);
}

template <format_t local, typename alloc> template <typename iterator> constexpr auto str<local, alloc>::cursor<iterator>::operator-=(size_t value) noexcept -> iterator&
{
	for (size_t i {0}; i < value; ++i) { --(*this); }

	return static_cast<iterator&>(*this);
}

template <format_t local, typename alloc> template <typename iterator> constexpr auto str<local, alloc>::cursor<iterator>::operator==(const iterator& rhs) const noexcept -> bool
{
	// short-circuit; evaluate 1 ptr chasing OP then evaluate 2 ptr chasing OP
	return this->weight == rhs.weight && this->common->src == rhs.common->src;
}

template <format_t local, typename alloc> template <typename iterator> constexpr auto str<local, alloc>::cursor<iterator>::operator!=(const iterator& rhs) const noexcept -> bool
{
	// short-circuit; evaluate 1 ptr chasing OP then evaluate 2 ptr chasing OP
	return this->weight != rhs.weight || this->common->src != rhs.common->src;
}

template <format_t local, typename alloc> template <typename iterator> [[nodiscard]] constexpr str<local, alloc>::cursor<iterator>::state::proxy::operator char32_t() const noexcept
{
	char32_t code;

	/* decode a code point */ codec<local>::decode(this->needle, code, [&]() constexpr noexcept
	{
		switch (this->cursor_tag)
		{
			case it_cursor_category_tag::LTOR: { return codec<local>::next(this->needle); }
			case it_cursor_category_tag::RTOL: { return codec<local>::back(this->needle); }
		}
	}
	());

	return code;
}

template <format_t local, typename alloc> template <typename iterator> constexpr auto str<local, alloc>::cursor<iterator>::state::proxy::operator=(char32_t code) noexcept -> proxy&
{
	const auto info {this->common->src->__insert__(this->needle, code, [&]() constexpr noexcept
	{
		switch (this->cursor_tag)
		{
			case it_cursor_category_tag::LTOR: { return codec<local>::next(this->needle); }
			case it_cursor_category_tag::RTOL: { return codec<local>::back(this->needle); }
		}
	}
	())};

	switch (this->offset_tag)
	{
		case it_offset_relative_tag::HEAD:
		{
			if (info.does_alloc)
			{
				this->common->ptr // ← stale
				=
				this->common->src->__head__();
			}
			this->needle = info.dest;
			break;
		}
		case it_offset_relative_tag::TAIL:
		{
			if (info.does_alloc
			    ||
			    info.does_shift)
			{
				this->common->ptr // ← stale
				=
				this->common->src->__tail__();
			}
			this->needle = info.dest;
			break;
		}
	}

	return *this;
}

template <format_t local, typename alloc> template <typename iterator> constexpr auto str<local, alloc>::cursor<iterator>::state::proxy::operator==(char32_t code) const noexcept -> bool
{
	return this->operator char32_t() == code;
}

template <format_t local, typename alloc> template <typename iterator> constexpr auto str<local, alloc>::cursor<iterator>::state::proxy::operator!=(char32_t code) const noexcept -> bool
{
	return this->operator char32_t() != code;
}

#pragma endregion str::cursor
#pragma region str::reader

template <format_t local, typename alloc> [[nodiscard]] constexpr str<local, alloc>::reader::operator char32_t() const noexcept
{
	const unit_t* head {this->src->__head__()};
	const unit_t* tail {this->src->__tail__()};

	size_t i {0};

	if constexpr (!codec<local>::is_variable
	              &&
	              !codec<local>::is_stateful)
	{
		if (this->arg < this->src->size())
		{
			head += this->arg;
			goto __SHORTCUT__;
		}
		return '\0';
	}

	for (; head < tail; head += codec<local>::next(head))
	{
		if (this->arg == i++)
		{
			__SHORTCUT__:

			char32_t code;

			const auto step {codec<local>::next(head)};
			codec<local>::decode(head, code, step);

			return code;
		}
	}
	return U'\0';
}

template <format_t local, typename alloc> constexpr auto str<local, alloc>::reader::operator==(char32_t code) const noexcept -> bool
{
	return this->operator char32_t() == code;
}

template <format_t local, typename alloc> constexpr auto str<local, alloc>::reader::operator!=(char32_t code) const noexcept -> bool
{
	return this->operator char32_t() != code;
}

#pragma endregion str::reader
#pragma region str::writer

template <format_t local, typename alloc> constexpr auto str<local, alloc>::writer::operator=(char32_t code) noexcept -> writer&
{
	const unit_t* head {this->src->__head__()};
	const unit_t* tail {this->src->__tail__()};

	size_t i {0};

	if constexpr (!codec<local>::is_variable
	              &&
	              !codec<local>::is_stateful)
	{
		if (this->arg < this->src->size())
		{
			head += this->arg;
			goto __SHORTCUT__;
		}
		return *this;
	}

	for (; head < tail; head += codec<local>::next(head))
	{
		if (this->arg == i++)
		{
			__SHORTCUT__:

			const auto step {codec<local>::next(head)};
			this->src->__insert__(head, code, step);

			return *this;
		}
	}
	return *this;
}

template <format_t local, typename alloc> [[nodiscard]] constexpr str<local, alloc>::writer::operator char32_t() const noexcept
{
	return reader {this->src, this->arg}.operator char32_t();
}

template <format_t local, typename alloc> constexpr auto str<local, alloc>::writer::operator==(char32_t code) const noexcept -> bool
{
	return reader {this->src, this->arg}.operator==(code);
}

template <format_t local, typename alloc> constexpr auto str<local, alloc>::writer::operator!=(char32_t code) const noexcept -> bool
{
	return reader {this->src, this->arg}.operator!=(code);
}

#pragma endregion str::writer
#pragma region txt::reader

template <format_t local /* can't own */> [[nodiscard]] constexpr txt<local>::reader::operator char32_t() const noexcept
{
	const unit_t* head {this->src->__head__};
	const unit_t* tail {this->src->__tail__};

	size_t i {0};

	if constexpr (!codec<local>::is_variable
	              &&
	              !codec<local>::is_stateful)
	{
		if (this->arg < this->src->size())
		{
			head += this->arg;
			goto __SHORTCUT__;
		}
		return '\0';
	}

	for (; head < tail; head += codec<local>::next(head))
	{
		if (this->arg == i++)
		{
			__SHORTCUT__:

			char32_t code;

			const auto step {codec<local>::next(head)};
			codec<local>::decode(head, code, step);

			return code;
		}
	}
	return U'\0';
}

template <format_t local /* can't own */> constexpr auto txt<local>::reader::operator==(char32_t code) const noexcept -> bool
{
	return this->operator char32_t() == code;
}

template <format_t local /* can't own */> constexpr auto txt<local>::reader::operator!=(char32_t code) const noexcept -> bool
{
	return this->operator char32_t() != code;
}

#pragma endregion txt::reader
#pragma region txt::writer

template <format_t local /* can't own */> [[nodiscard]] constexpr txt<local>::writer::operator char32_t() const noexcept
{
	return reader {this->src, this->arg}.operator char32_t();
}

template <format_t local /* can't own */> constexpr auto txt<local>::writer::operator==(char32_t code) const noexcept -> bool
{
	return reader {this->src, this->arg}.operator==(code);
}

template <format_t local /* can't own */> constexpr auto txt<local>::writer::operator!=(char32_t code) const noexcept -> bool
{
	return reader {this->src, this->arg}.operator!=(code);
}

#pragma endregion txt::writer
#pragma region filesystem

template <typename STRING>
// fs I/O at your service
auto fileof(const STRING& path) noexcept -> std::optional
                                            <
                                            	std::variant
                                            	<
                                            		str<"UTF-8">
                                            		,
                                            		str<"UTF-16">
                                            		,
                                            		str<"UTF-32">
                                            	>
                                            >
{
	enum encoding : uint8_t
	{
		UTF8_STD = (0 << 4) | 0,
		UTF8_BOM = (1 << 4) | 3,
		UTF16_BE = (2 << 4) | 2,
		UTF16_LE = (3 << 4) | 2,
		UTF32_BE = (4 << 4) | 4,
		UTF32_LE = (5 << 4) | 4,
	};

	static const auto byte_order_mask
	{
		[](std::ifstream& ifs) noexcept -> encoding
		{
			char buffer[4];

			ifs.seekg(0, std::ios::beg); // move to the beginning of the file #A
			ifs.read(&buffer[0], 4); const auto bytes {ifs.gcount()}; ifs.clear();
			ifs.seekg(0, std::ios::beg); // move to the beginning of the file #B

			// 00 00 FE FF
			if (4 <= bytes
			    &&
			    buffer[0] == '\x00'
			    &&
			    buffer[1] == '\x00'
			    &&
			    buffer[2] == '\xFE'
			    &&
			    buffer[3] == '\xFF') [[unlikely]] return UTF32_BE;

			// FF FE 00 00
			if (4 <= bytes
			    &&
			    buffer[0] == '\xFF'
			    &&
			    buffer[1] == '\xFE'
			    &&
			    buffer[2] == '\x00'
			    &&
			    buffer[3] == '\x00') [[unlikely]] return UTF32_LE;

			// FE FF
			if (2 <= bytes
			    &&
			    buffer[0] == '\xFE'
			    &&
			    buffer[1] == '\xFF') [[unlikely]] return UTF16_BE;

			// FF FE
			if (2 <= bytes
			    &&
			    buffer[0] == '\xFF'
			    &&
			    buffer[1] == '\xFE') [[unlikely]] return UTF16_LE;

			// EF BB BF
			if (3 <= bytes
			    &&
			    buffer[0] == '\xEF'
			    &&
			    buffer[1] == '\xBB'
			    &&
			    buffer[2] == '\xBF') [[unlikely]] return UTF8_BOM;

			return UTF8_STD;
		}
	};

	static const auto write_as_native {[]<format_t local, typename alloc>(std::ifstream& ifs, str<local, alloc>& str) noexcept -> void
	{
		   using T = typename codec<local>::unit_t;
		// using U = typename codec<alien>::unit_t;

		T buffer;

		T* dest {str.__head__()};
		T* head {str.__head__()};

		while (ifs.read(reinterpret_cast<char*>(&buffer), sizeof(T)))
		{
			if (buffer == '\r'
			    &&
			    ifs.read(reinterpret_cast<char*>(&buffer), sizeof(T))
			    &&
			    buffer != '\n')
			{
				*(dest++) = '\n';
			}
			*(dest++) = buffer;
		}
		str.__size__(dest - head);
	}};

	static const auto write_as_exotic {[]<format_t local, typename alloc>(std::ifstream& ifs, str<local, alloc>& str) noexcept -> void
	{
		   using T = typename codec<local>::unit_t;
		// using U = typename codec<alien>::unit_t;

		T buffer;

		T* dest {str.__head__()};
		T* head {str.__head__()};

		while (ifs.read(reinterpret_cast<char*>(&buffer), sizeof(T)))
		{
			if ((buffer = std::byteswap(buffer)) == '\r'
			    &&
			    ifs.read(reinterpret_cast<char*>(&buffer), sizeof(T))
			    &&
			    (buffer = std::byteswap(buffer)) != '\n')
			{
				*(dest++) = '\n';
			}
			*(dest++) = buffer;
		}
		str.__size__(dest - head);
	}};

	const std::filesystem::path fs
	{
		[&]() -> std::filesystem::path
		{
			using file_t = decltype(fs);
			using string = std::string;

			// constructible on the fly
			if constexpr (std::is_constructible_v<file_t, STRING>)
			{
				return path;
			}
			// at least convertible to file_t!
			else if constexpr (std::is_convertible_v<STRING, file_t>)
			{
				return static_cast<file_t>(path);
			}
			// at least convertible to string!
			else if constexpr (std::is_convertible_v<STRING, string>)
			{
				return static_cast<string>(path);
			}
			// ...constexpr failuare! DEAD-END!
			else static_assert(!"ERROR! call to 'fileof' is ambigious");
		}()
	};

	if (std::ifstream ifs {fs, std::ios::binary})
	{
		const auto BOM {byte_order_mask(ifs)};
		const auto off {BOM & 0xF /* ..?? */};

		size_t max;

		// to the BOM
		ifs.seekg(off, std::ios::beg); max = ifs.tellg() - static_cast<std::streamoff>(0x0);
		// to the END
		ifs.seekg(0x0, std::ios::end); max = ifs.tellg() - static_cast<std::streamoff>(max);
		// to the BOM
		ifs.seekg(off, std::ios::beg); // BOM offset calculation is done above, now rest...

		switch (BOM)
		{
			case UTF8_STD:
			{
				str<"UTF-8"> str;

				str.capacity(max / sizeof(typename decltype(str)::unit_t));

				if constexpr (std::endian::native == std::endian::little) write_as_native(ifs, str);
				                                                     else write_as_native(ifs, str);

				return str;
			}
			case UTF8_BOM:
			{
				str<"UTF-8"> str;

				str.capacity(max / sizeof(typename decltype(str)::unit_t));

				if constexpr (std::endian::native == std::endian::big) write_as_native(ifs, str);
				                                                  else write_as_native(ifs, str);

				return str;
			}
			case UTF16_LE:
			{
				str<"UTF-16"> str;

				str.capacity(max / sizeof(typename decltype(str)::unit_t));

				if constexpr (std::endian::native == std::endian::little) write_as_native(ifs, str);
				                                                     else write_as_exotic(ifs, str);

				return str;
			}
			case UTF16_BE:
			{
				str<"UTF-16"> str;

				str.capacity(max / sizeof(typename decltype(str)::unit_t));

				if constexpr (std::endian::native == std::endian::big) write_as_native(ifs, str);
				                                                  else write_as_exotic(ifs, str);

				return str;
			}
			case UTF32_LE:
			{
				str<"UTF-32"> str;

				str.capacity(max / sizeof(typename decltype(str)::unit_t));

				if constexpr (std::endian::native == std::endian::little) write_as_native(ifs, str);
				                                                     else write_as_exotic(ifs, str);

				return str;
			}
			case UTF32_BE:
			{
				str<"UTF-32"> str;

				str.capacity(max / sizeof(typename decltype(str)::unit_t));

				if constexpr (std::endian::native == std::endian::big) write_as_native(ifs, str);
				                                                  else write_as_exotic(ifs, str);

				return str;
			}
		}
	}
	return std::nullopt;
}

#pragma endregion filesystem

#undef __OWNED__
#undef __SLICE__
#undef __EQSTR__
#undef __08STR__
#undef __16STR__
#undef __32STR__

using utf8 = str<"UTF-8">;
using utf16 = str<"UTF-16">;
using utf32 = str<"UTF-32">;

using txt8 = txt<"UTF-8">;
using txt16 = txt<"UTF-16">;
using txt32 = txt<"UTF-32">;

#undef COPY_ASSIGNMENT
#undef MOVE_ASSIGNMENT

#undef COPY_CONSTRUCTOR
#undef MOVE_CONSTRUCTOR

template <size_t N> str(const char8_t (&_)[N]) -> str<"UTF-8">;
template <size_t N> str(const char16_t (&_)[N]) -> str<"UTF-16">;
template <size_t N> str(const char32_t (&_)[N]) -> str<"UTF-32">;

template <size_t N> txt(const char8_t (&_)[N]) -> txt<"UTF-8">;
template <size_t N> txt(const char16_t (&_)[N]) -> txt<"UTF-16">;
template <size_t N> txt(const char32_t (&_)[N]) -> txt<"UTF-32">;
}

template <utf::format_t local, typename alloc> struct std::hash<utf::str<local, alloc>>
{
	constexpr auto operator()(const utf::str<local, alloc>& str) const noexcept -> size_t
	{
		uint32_t seed {0};

		for (const auto code : str)
		{
			seed = 31 * seed + code;
		}
		return static_cast<size_t>(seed);
	}
};

template <utf::format_t local /* can't own */> struct std::hash<utf::txt<local /*##*/>>
{
	constexpr auto operator()(const utf::txt<local /*##*/>& str) const noexcept -> size_t
	{
		uint32_t seed {0};

		for (const auto code : str)
		{
			seed = 31 * seed + code;
		}
		return static_cast<size_t>(seed);
	}
};

template <utf::format_t local, typename alloc>
inline constexpr bool std::ranges::disable_sized_range<utf::str<local, alloc>> = true;

template <utf::format_t local /* can't own */>
inline constexpr bool std::ranges::disable_sized_range<utf::txt<local /*##*/>> = true;
