#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>

#include <bit>
#include <ios>
#include <ranges>
#include <memory>
#include <vector>
#include <variant>
#include <utility>
#include <istream>
#include <ostream>
#include <fstream>
#include <iterator>
#include <optional>
#include <algorithm>
#include <functional>
#include <filesystem>
#include <type_traits>

inline auto operator>>(std::istream& os, char32_t code) noexcept -> std::istream&;
inline auto operator<<(std::ostream& os, char32_t code) noexcept -> std::ostream&;

#ifndef x69_MALLOC // sets allocator 🎉
#define x69_MALLOC(T) std::allocator<T>
#endif             // sets allocator 🎉

#define COPY_CONSTRUCTOR(T) constexpr T(const T&  other) noexcept
#define MOVE_CONSTRUCTOR(T) constexpr T(/*&*/ T&& other) noexcept

#define COPY_ASSIGNMENT(T) constexpr auto operator=(const T&  rhs) noexcept -> T&
#define MOVE_ASSIGNMENT(T) constexpr auto operator=(/*&*/ T&& rhs) noexcept -> T&

namespace x69
{
	//┌─────────────────────────────────────────────────────────────┐
	//│ special thanks to facebook's folly::FBString                │
	//│                                                             │
	//│ SSO mode uses every bytes of heap string struct using union │
	//│ this was achievable thanks to the very clever memory layout │
	//│                                                             │
	//│ for more, watch https://www.youtube.com/watch?v=kPR8h4-qZdk │
	//└─────────────────────────────────────────────────────────────┘

	class code_t final
	{
		char32_t arg;

	public:

		constexpr code_t
		(
			decltype(arg) arg
		)
		noexcept : arg {arg}
		{}

		constexpr operator char32_t() const noexcept;
		constexpr operator char32_t() /*&*/ noexcept;

		constexpr  code_t() noexcept = default;
		constexpr ~code_t() noexcept = default;

		// constexpr auto to_lowercase() const noexcept;
		// constexpr auto to_uppercase() const noexcept;
		// constexpr auto to_titlecase() const noexcept;
	};

	template <size_t N> struct format_t { char _[N];
	[[nodiscard("🥝")]] constexpr auto operator==(const
	format_t& rhs) const noexcept -> bool = default;
	[[nodiscard("🍓")]] constexpr auto operator!=(const
	format_t& rhs) const noexcept -> bool = default;
	template <size_t U> constexpr auto operator==(const
	format_t<U>&) const noexcept -> bool { return 0; };
	template <size_t U> constexpr auto operator!=(const
	format_t<U>&) const noexcept -> bool { return 1; };
	constexpr format_t(const char (&str)[N]) noexcept
	{ std::ranges::copy(/**/ str /**/, this->_); } };

	enum class range : uint8_t {N};
	struct clamp { const size_t _;
	inline constexpr /**/ operator
	size_t() const { return _; } };
	inline constexpr auto operator-
	(range, size_t offset) noexcept
	-> clamp { return { offset }; }

	#define CODER_AND_DECODER(UNIT, IS_VARIABLE, IS_STATEFUL)                \
	{                                                                        \
	    static constexpr const bool is_variable {IS_VARIABLE};               \
	    static constexpr const bool is_stateful {IS_STATEFUL};               \
	                                                                         \
	    using unit_t = UNIT;                                                 \
	                                                                         \
	    constexpr  codec() noexcept = delete;                                \
	    constexpr ~codec() noexcept = delete;                                \
	                                                                         \
	    static constexpr auto size(/*&*/ code_t  code) noexcept -> int8_t;   \
	    static constexpr auto next(const unit_t* data) noexcept -> int8_t;   \
	    static constexpr auto back(const unit_t* data) noexcept -> int8_t;   \
	                                                                         \
	    static constexpr auto /* transform a code point into code units */   \
	    encode(const code_t in, unit_t* out, int8_t step) noexcept -> void;  \
	    static constexpr auto /* transform code units into a code point. */  \
	    decode(const unit_t* in, code_t& out, int8_t step) noexcept -> void; \
	}                                                                        \

	template <format_t name> struct codec
	CODER_AND_DECODER(void, false, false);

	// https://en.wikipedia.org/wiki/ASCII
	template <> struct codec<"ASCII"> final
	CODER_AND_DECODER(char, false, false);

	// https://en.wikipedia.org/wiki/UTF-8
	template <> struct codec<"UTF-8"> final
	CODER_AND_DECODER(char8_t, true, false);

	// https://en.wikipedia.org/wiki/UTF-16
	template <> struct codec<"UTF-16"> final
	CODER_AND_DECODER(char16_t, true, false);

	// https://en.wikipedia.org/wiki/UTF-32
	template <> struct codec<"UTF-32"> final
	CODER_AND_DECODER(char32_t, true, false);

	#undef CODER_AND_DECODER

	template <format_t local, typename alloc = x69_MALLOC(typename codec<local>::unit_t)> class str;
	template <format_t local                                                            > class txt;

	//┌───────┬───────┬────────────┬─────────────────┐
	//│ class │ owns? │ null-term? │ use-after-free? │
	//├───────┼───────┼────────────┼─────────────────┤
	//│ [str] │   T   │   always   │      safe       │
	//├───────┼───────┼────────────┼─────────────────┤
	//│ [txt] │   F   │   maybe?   │      [UB]       │
	//└───────┴───────┴────────────┴─────────────────┘

	template <format_t native,
	          typename derive> class cable
	{
		template <format_t, typename> friend class cable;

		//┌───────────────────────────────────────────────┐
		//│ note: helper funs are not encapsulated within │
		//│       and it was deliberate; to cut bin bloat │
		//└───────────────────────────────────────────────┘

		using unit_t = typename codec<native>::unit_t;

		constexpr auto head() const noexcept -> const unit_t*;
		constexpr auto tail() const noexcept -> const unit_t*;

		template <typename lhs_t, typename rhs_t> class concat;

		class const_forward_iterator; friend const_forward_iterator;
		class const_reverse_iterator; friend const_reverse_iterator;

	public:
		// returns the number of code units, excluding NULL-TERMINATOR.
		constexpr auto size() const noexcept -> size_t;
		// returns the number of code points, excluding NULL-TERMINATOR.
		constexpr auto length() const noexcept -> size_t;

		// returns a list of string slice, of which is a product of split aka division.
		template <typename string> requires requires (string&& _)
		                                             { txt {_}; }
		constexpr auto split(string&& value) const noexcept -> std::vector<txt<native>>;
		constexpr auto split(code_t   value) const noexcept -> std::vector<txt<native>>;

		// returns a list of string slice, of which is a product of search occurrence.
		template <typename string> requires requires (string&& _)
		                                             { txt {_}; }
		constexpr auto match(string&& value) const noexcept -> std::vector<txt<native>>;
		constexpr auto match(code_t   value) const noexcept -> std::vector<txt<native>>;

		// *self explanatory* returns whether or not it contains *parameter*.
		template <typename string> requires requires (string&& _)
		                                             { txt {_}; }
		constexpr auto contains(string&& value) const noexcept -> bool;
		constexpr auto contains(code_t   value) const noexcept -> bool;

		// *self explanatory* returns whether or not it starts with *parameter*.
		template <typename string> requires requires (string&& _)
		                                             { txt {_}; }
		constexpr auto starts_with(string&& value) const noexcept -> bool;
		constexpr auto starts_with(code_t   value) const noexcept -> bool;

		// *self explanatory* returns whether or not it ends with *parameter*.
		template <typename string> requires requires (string&& _)
		                                             { txt {_}; }
		constexpr auto ends_with(string&& value) const noexcept -> bool;
		constexpr auto ends_with(code_t   value) const noexcept -> bool;

		// returns a slice, of which is a product of substring. N is a sentinel value.
		constexpr auto substr(clamp  start, clamp  until) const noexcept -> txt<native>;
		constexpr auto substr(clamp  start, range  until) const noexcept -> txt<native>;
		constexpr auto substr(size_t start, clamp  until) const noexcept -> txt<native>;
		constexpr auto substr(size_t start, range  until) const noexcept -> txt<native>;
		constexpr auto substr(size_t start, size_t until) const noexcept -> txt<native>;

		// iterators

		constexpr auto begin() const noexcept -> const_forward_iterator; // requires nothing; always active
		constexpr auto end() const noexcept -> const_forward_iterator; // requires nothing; always active

		constexpr auto rbegin() const noexcept -> const_reverse_iterator requires (!codec<native>::is_stateful);
		constexpr auto rend() const noexcept -> const_reverse_iterator requires (!codec<native>::is_stateful);

		// operators

		constexpr auto operator[](size_t rhs) const noexcept -> decltype(auto);
		constexpr auto operator[](size_t rhs) /*&*/ noexcept -> decltype(auto);

		template <typename string> requires requires (string&& _)
		                                             { txt {_}; }
		constexpr auto operator==(string&& rhs) const noexcept -> bool;
		constexpr auto operator==(code_t   rhs) const noexcept -> bool;

		template <typename string> requires requires (string&& _)
		                                             { txt {_}; }
		constexpr auto operator!=(string&& rhs) const noexcept -> bool;
		constexpr auto operator!=(code_t   rhs) const noexcept -> bool;

		// this + ???

		template <typename string> requires requires (string&& _)
		                                             { txt {_}; }
		constexpr auto operator+(string&& rhs) const noexcept -> concat<txt<native>, decltype(txt {rhs})>
		{
			return {{static_cast<const derive&>(*this)}, {txt {rhs}}};
		}
		constexpr auto operator+(code_t   rhs) const noexcept -> concat<txt<native>, decltype(   rhs   )>
		{
			return {{static_cast<const derive&>(*this)}, {   rhs   }};
		}

		// ??? + this

		template <typename string> requires requires (string&& _)
		                                             { txt {_}; } && (!std::is_base_of_v<std::remove_cvref_t<string>, derive>)
		                                                             // fix; ambiguious overloads by causes by derived class
		friend constexpr auto operator+(string&& lhs, const derive& rhs) noexcept -> concat<decltype(txt {lhs}), txt<native>>
		{
			return {/* no need for static_cast */ {txt {lhs}}, {rhs}};
		}
		friend constexpr auto operator+(code_t   lhs, const derive& rhs) noexcept -> concat<decltype(   lhs   ), txt<native>>
		{
			return {/* no need for static_cast */ {   lhs   }, {rhs}};
		}

	private:

		template <typename lhs_t,
		          typename rhs_t> class concat
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

			   template <format_t alien, typename arena> constexpr operator str<alien, arena>() const noexcept;
			// template <format_t alien, typename arena> constexpr operator str<alien, arena>() /*&*/ noexcept;

			// this + ???

			template <typename string> requires requires (string&& _)
			                                             { txt {_}; }
			constexpr auto operator+(string&& rhs) const noexcept -> concat<concat, decltype(txt {rhs})>
			{
				return {{*this}, {txt {rhs}}};
			}
			constexpr auto operator+(code_t   rhs) const noexcept -> concat<concat, decltype(   rhs   )>
			{
				return {{*this}, {   rhs   }};
			}

			// ??? + this

			template <typename string> requires requires (string&& _)
			                                             { txt {_}; }
			friend constexpr auto operator+(string&& lhs, concat& rhs) noexcept -> concat<decltype(txt {lhs}), concat>
			{
				return {{txt {lhs}}, {rhs}};
			}
			friend constexpr auto operator+(code_t   lhs, concat& rhs) noexcept -> concat<decltype(   rhs   ), concat>
			{
				return {{   lhs   }, {rhs}};
			}

		private:

			constexpr auto __for_each__(const auto&& fun) const noexcept -> void;
		};

		template <typename alias,
		          format_t trait> class cursor
		{
			const unit_t* ptr;

		public:

			using iterator_category = std::conditional_t<codec<native>::is_stateful, std::forward_iterator_tag, std::bidirectional_iterator_tag>;
			using iterator_concept = std::conditional_t<codec<native>::is_stateful, std::forward_iterator_tag, std::bidirectional_iterator_tag>;
			using difference_type = std::ptrdiff_t;
			using value_type = code_t;
			using reference = code_t;

			constexpr cursor
			(
				decltype(ptr) ptr
			)
			noexcept : ptr {ptr}
			{}

			[[nodiscard]] constexpr operator const unit_t*() const noexcept;
			[[nodiscard]] constexpr operator const unit_t*() /*&*/ noexcept;

			constexpr auto operator*() const noexcept -> value_type;

			// stl compat; default constructible
			constexpr  cursor() noexcept = default;
			constexpr ~cursor() noexcept = default;

			constexpr auto operator++(   ) noexcept -> alias&;
			constexpr auto operator++(int) noexcept -> alias;
			constexpr auto operator--(   ) noexcept -> alias&;
			constexpr auto operator--(int) noexcept -> alias;

			constexpr auto operator+(size_t value) noexcept -> alias;
			constexpr auto operator-(size_t value) noexcept -> alias;

			constexpr auto operator+=(size_t value) noexcept -> alias&;
			constexpr auto operator-=(size_t value) noexcept -> alias&;

			constexpr auto operator==(const cursor& rhs) const noexcept -> bool = default;
			constexpr auto operator!=(const cursor& rhs) const noexcept -> bool = default;
		};

		class const_forward_iterator : public cursor<const_forward_iterator, "LTR"> { public: using cursor<const_forward_iterator, "LTR">::cursor; };
		class const_reverse_iterator : public cursor<const_reverse_iterator, "RTL"> { public: using cursor<const_reverse_iterator, "RTL">::cursor; };
	};

	template <format_t native,
	          typename malloc> class str final : public cable<native, str<native,
	                                                                      malloc>>
	{
		template <format_t, typename> friend class cable;
		template <format_t, typename> friend class    str;
		template <format_t          > friend class    txt;

		//┌───────┬────────────┬─────────────────┐
		//│ owns? │ null-term? │ use-after-free? │
		//├───────┼────────────┼─────────────────┤
		//│   T   │   always   │      safe       │
		//└───────┴────────────┴─────────────────┘

		using unit_t = typename codec<native>::unit_t;

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

		struct storage : public malloc
		{
			union
			{
				buffer large;

				unit_t small
				[sizeof(buffer) / sizeof(unit_t)];

				uint8_t bytes
				[sizeof(buffer) / sizeof(uint8_t)];
			}
			__union__;

			constexpr  storage() noexcept;
			constexpr ~storage() noexcept;

			// single source of truth; category.
			constexpr auto mode() const noexcept -> mode_t;
			constexpr auto mode() /*&*/ noexcept -> mode_t;
		};
		#undef IS_LITTLE

		static_assert(sizeof(storage) == sizeof(buffer));
		static_assert(std::is_standard_layout_v<buffer>);
		static_assert(std::is_trivially_copyable_v<buffer>);
		static_assert(sizeof(buffer) == sizeof(size_t) * 3);
		static_assert(alignof(buffer) == alignof(size_t) * 1);
		static_assert(offsetof(buffer, head) == sizeof(size_t) * 0);
		static_assert(offsetof(buffer, last) == sizeof(size_t) * 1);

		// returns ptr to buffer's 1st element.
		constexpr auto head() const noexcept -> const unit_t*;
		constexpr auto head() /*&*/ noexcept -> /*&*/ unit_t*;

		// returns ptr to buffer's last element.
		constexpr auto tail() const noexcept -> const unit_t*;
		constexpr auto tail() /*&*/ noexcept -> /*&*/ unit_t*;

		// returns ptr to buffer's last = capacity.
		constexpr auto last() const noexcept -> const unit_t*;
		constexpr auto last() /*&*/ noexcept -> /*&*/ unit_t*;

		// fixes invariant; use it after internal manipulation.
		constexpr auto __size__(size_t value) noexcept -> void;

		storage store;

		class reader; friend reader;
		class writer; friend writer;

		class forward_iterator; friend forward_iterator;
		class reverse_iterator; friend reverse_iterator;

		typedef struct { unit_t* dest; bool does_shift;
		                               bool does_alloc; } __insert__t;

		// 2x capacity growth
		constexpr auto __insert__(unit_t* dest, code_t code, int8_t step) noexcept -> __insert__t;

	public:

		// optional; returns the content of a file with CRLF/CR to LF normalization.
		template <typename STRING> friend auto fileof(const STRING& path) noexcept -> std::optional
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

		explicit constexpr operator const unit_t*() const noexcept;
		explicit constexpr operator /*&*/ unit_t*() /*&*/ noexcept;

		// rule of 5

		COPY_CONSTRUCTOR(str);
		MOVE_CONSTRUCTOR(str);

		COPY_ASSIGNMENT(str);
		MOVE_ASSIGNMENT(str);

		// constructors

		constexpr  str() noexcept = default;
		constexpr ~str() noexcept = default;

		template <typename string> requires requires (string&& _)
		                                             { txt {_}; }
		constexpr str(string&& value) noexcept;
		constexpr str(code_t   value) noexcept;

		// returns the number of code units it can hold, excluding NULL-TERMINATOR.
		constexpr auto capacity(/* getter */) const noexcept -> size_t;
		// changes the number of code units it can hold, excluding NULL-TERMINATOR.
		constexpr auto capacity(size_t value) /*&*/ noexcept -> void;

		// iterator

		using cable<native, str>::begin; // fix; name hiding
		using cable<native, str>::end; // fix; name hiding

		constexpr auto begin() /*&*/ noexcept -> forward_iterator; // requires nothing; always active
		constexpr auto end() /*&*/ noexcept -> forward_iterator; // requires nothing; always active

		using cable<native, str>::rbegin; // fix; name hiding
		using cable<native, str>::rend; // fix; name hiding

		constexpr auto rbegin() /*&*/ noexcept -> reverse_iterator requires (!codec<native>::is_stateful);
		constexpr auto rend() /*&*/ noexcept -> reverse_iterator requires (!codec<native>::is_stateful);

		// operators

		template <typename string> requires requires (string&& _)
		                                             { txt {_}; }
		constexpr auto operator=(string&& rhs)& noexcept -> str&;
		constexpr auto operator=(code_t   rhs)& noexcept -> str&;

		template <typename string> requires requires (string&& _)
		                                             { txt {_}; }
		constexpr auto operator=(string&& rhs)&& noexcept -> str&&;
		constexpr auto operator=(code_t   rhs)&& noexcept -> str&&;

		template <typename string> requires requires (string&& _)
		                                             { txt {_}; }
		constexpr auto operator+=(string&& rhs)& noexcept -> str&;
		constexpr auto operator+=(code_t   rhs)& noexcept -> str&;

		template <typename string> requires requires (string&& _)
		                                             { txt {_}; }
		constexpr auto operator+=(string&& rhs)&& noexcept -> str&&;
		constexpr auto operator+=(code_t   rhs)&& noexcept -> str&&;

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

			[[nodiscard]] constexpr operator code_t() const noexcept;

			constexpr auto operator==(code_t code) const noexcept -> bool;
			constexpr auto operator!=(code_t code) const noexcept -> bool;
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

			constexpr auto operator=(code_t code) noexcept -> writer&;
			[[nodiscard]] constexpr operator code_t() const noexcept;

			constexpr auto operator==(code_t code) const noexcept -> bool;
			constexpr auto operator!=(code_t code) const noexcept -> bool;
		};

		template <typename alias,
		          format_t trait> class cursor
		{
			using self_t = str;

			// std::views::reverse impl in Clang/GCC/MSVC:
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

			enum zero_t
			{
				HEAD,
				TAIL,
			};

			class state
			{
				// nothing to do...

			public:

				/*&&&*/ self_t* target;
				mutable unit_t* anchor;

				constexpr state
				(
					decltype(target) target,
					decltype(anchor) anchor
				)
				noexcept : target {target},
				           anchor {anchor}
				{}
			};

			class proxy
			{
				std::shared_ptr<state> common;
				unit_t*                needle;
				zero_t                 policy;

			public:

				constexpr proxy
				(
					decltype(common) common,
					decltype(needle) needle,
					decltype(policy) policy
				)
				noexcept : common {common},
				           needle {needle},
				           policy {policy}
				{}

				[[nodiscard]] constexpr operator code_t() const noexcept;
				constexpr auto operator=(code_t code) noexcept -> proxy&;

				constexpr auto operator==(code_t code) const noexcept -> bool;
				constexpr auto operator!=(code_t code) const noexcept -> bool;
			};

			friend str;

			std::shared_ptr<state> common;
			size_t                 offset;
			size_t                 weight;
			zero_t                 policy;

		public:

			using iterator_category = std::conditional_t<codec<native>::is_stateful, std::forward_iterator_tag, std::bidirectional_iterator_tag>;
			using iterator_concept = std::conditional_t<codec<native>::is_stateful, std::forward_iterator_tag, std::bidirectional_iterator_tag>;
			using difference_type = std::ptrdiff_t;
			using value_type = proxy;
			using reference = proxy;

			constexpr cursor
			(
				decltype(common) common,
				decltype(offset) offset,
				decltype(weight) weight,
				decltype(policy) policy
			)
			noexcept : common {common},
			           offset {offset},
			           weight {weight},
			           policy {policy}
			{};

			[[nodiscard]] constexpr operator const unit_t*() const noexcept;
			[[nodiscard]] constexpr operator const unit_t*() /*&*/ noexcept;

			constexpr auto operator*() const noexcept -> value_type;

			// stl compat; default constructible
			constexpr  cursor() noexcept = default;
			constexpr ~cursor() noexcept = default;

			constexpr auto operator++(   ) noexcept -> alias&;
			constexpr auto operator++(int) noexcept -> alias;
			constexpr auto operator--(   ) noexcept -> alias&;
			constexpr auto operator--(int) noexcept -> alias;

			constexpr auto operator+(size_t value) noexcept -> alias;
			constexpr auto operator-(size_t value) noexcept -> alias;

			constexpr auto operator+=(size_t value) noexcept -> alias&;
			constexpr auto operator-=(size_t value) noexcept -> alias&;

			constexpr auto operator==(const cursor& rhs) const noexcept -> bool;
			constexpr auto operator!=(const cursor& rhs) const noexcept -> bool;

		private:

			constexpr auto __needle__() const noexcept -> unit_t*;
		};

		class forward_iterator : public cursor<forward_iterator, "LTR"> { using cursor<forward_iterator, "LTR">::cursor; };
		class reverse_iterator : public cursor<reverse_iterator, "RTL"> { using cursor<reverse_iterator, "RTL">::cursor; };
	};

	template <format_t native> class txt final : public cable<native, txt<native>>
	{
		template <format_t, typename> friend class cable;
		template <format_t, typename> friend class   str;
		template <format_t          > friend class   txt;

		//┌───────┬────────────┬─────────────────┐
		//│ owns? │ null-term? │ use-after-free? │
		//├───────┼────────────┼─────────────────┤
		//│   F   │   maybe?   │      [UB]       │
		//└───────┴────────────┴─────────────────┘

		using unit_t = typename codec<native>::unit_t;

		const unit_t* head;
		const unit_t* tail;

		friend class reader;
		friend class writer;

	public:

		constexpr txt
		(
			decltype(head) head,
			decltype(tail) tail
		)
		noexcept : head {head},
		           tail {tail}
		{}

		template <size_t N>
		constexpr txt
		(
			const unit_t (&str)[N]
		)
		noexcept : head {&str[N - N]},
		           tail {&str[N - 1]}
		{}

		template <size_t N>
		constexpr txt
		(
			/*&*/ unit_t (&str)[N]
		)
		noexcept : head {&str[N - N]},
		           tail {&str[N - 1]}
		{}

		template <typename arena>
		constexpr txt
		(
			const str<native, arena>& str
		)
		noexcept : head {str.head()},
		           tail {str.tail()}
		{}

		template <typename arena>
		constexpr txt
		(
			/*&*/ str<native, arena>& str
		)
		noexcept : head {str.head()},
		           tail {str.tail()}
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

			[[nodiscard]] constexpr operator code_t() const noexcept;

			constexpr auto operator==(code_t code) const noexcept -> bool;
			constexpr auto operator!=(code_t code) const noexcept -> bool;
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

			// constexpr auto operator=(code_t code) noexcept -> writer&;
			[[nodiscard]] constexpr operator code_t() const noexcept;

			constexpr auto operator==(code_t code) const noexcept -> bool;
			constexpr auto operator!=(code_t code) const noexcept -> bool;
		};
	};

	namespace detail
	{
		template <format_t source> static constexpr auto __units__(const typename codec<source>::unit_t* head,
		                                                           const typename codec<source>::unit_t* tail) noexcept -> size_t;

		template <format_t source> static constexpr auto __codes__(const typename codec<source>::unit_t* head,
		                                                           const typename codec<source>::unit_t* tail) noexcept -> size_t;

		template <format_t source,
		          format_t target> static constexpr auto __fcopy__(/*&*/ typename codec<target>::unit_t* dest,
		                                                           const typename codec<source>::unit_t* head,
		                                                           const typename codec<source>::unit_t* tail) noexcept -> size_t;

		template <format_t target> static constexpr auto __fcopy__(/*&*/ typename codec<target>::unit_t* dest,
		                                                           code_t                                code) noexcept -> size_t;

		template <format_t source,
		          format_t target> static constexpr auto __rcopy__(/*&*/ typename codec<target>::unit_t* dest,
		                                                           const typename codec<source>::unit_t* head,
		                                                           const typename codec<source>::unit_t* tail) noexcept -> size_t;

		template <format_t target> static constexpr auto __rcopy__(/*&*/ typename codec<target>::unit_t* dest,
		                                                           code_t                                code) noexcept -> size_t;

		template <format_t source,
		          format_t target> static constexpr auto __equal__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
		                                                           const typename codec<target>::unit_t* rhs0, const typename codec<target>::unit_t* rhsN) noexcept -> bool;

		template <format_t source> static constexpr auto __equal__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
		                                                           code_t                                                                            code) noexcept -> bool;

		template <format_t source,
		          format_t target> static constexpr auto __nqual__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
		                                                           const typename codec<target>::unit_t* rhs0, const typename codec<target>::unit_t* rhsN) noexcept -> bool;

		template <format_t source> static constexpr auto __nqual__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
		                                                           code_t                                                                            code) noexcept -> bool;

		template <format_t source,
		          format_t target> static constexpr auto __split__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
		                                                           const typename codec<target>::unit_t* rhs0, const typename codec<target>::unit_t* rhsN) noexcept -> std::vector<txt<source>>;

		template <format_t source> static constexpr auto __split__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
		                                                           code_t                                                                            code) noexcept -> std::vector<txt<source>>;

		template <format_t source,
		          format_t target> static constexpr auto __match__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
		                                                           const typename codec<target>::unit_t* rhs0, const typename codec<target>::unit_t* rhsN) noexcept -> std::vector<txt<source>>;

		template <format_t source> static constexpr auto __match__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
		                                                           code_t                                                                            code) noexcept -> std::vector<txt<source>>;

		template <format_t source,
		          format_t target> static constexpr auto __holds__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
		                                                           const typename codec<target>::unit_t* rhs0, const typename codec<target>::unit_t* rhsN) noexcept -> bool;

		template <format_t source> static constexpr auto __holds__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
		                                                           code_t                                                                            code) noexcept -> bool;

		template <format_t source,
		          format_t target> static constexpr auto __swith__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
		                                                           const typename codec<target>::unit_t* rhs0, const typename codec<target>::unit_t* rhsN) noexcept -> bool;

		template <format_t source> static constexpr auto __swith__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
		                                                           code_t                                                                            code) noexcept -> bool;

		template <format_t source,
		          format_t target> static constexpr auto __ewith__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
		                                                           const typename codec<target>::unit_t* rhs0, const typename codec<target>::unit_t* rhsN) noexcept -> bool;

		template <format_t source> static constexpr auto __ewith__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
		                                                           code_t                                                                            code) noexcept -> bool;

		template <format_t source> static constexpr auto __substr__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail,
		                                                            clamp                                 from, clamp                                 dest) noexcept -> txt<source>;

		template <format_t source> static constexpr auto __substr__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail,
		                                                            clamp                                 from, range                                 dest) noexcept -> txt<source>;

		template <format_t source> static constexpr auto __substr__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail,
		                                                            size_t                                from, clamp                                 dest) noexcept -> txt<source>;

		template <format_t source> static constexpr auto __substr__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail,
		                                                            size_t                                from, range                                 dest) noexcept -> txt<source>;

		template <format_t source> static constexpr auto __substr__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail,
		                                                            size_t                                from, size_t                                dest) noexcept -> txt<source>;

		template <format_t source,
		          format_t target> static constexpr auto __search__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
		                                                            const typename codec<target>::unit_t* rhs0, const typename codec<target>::unit_t* rhsN,
		                                                            const auto& /* (const source::unit_t* head, const source::unit_t* tail) -> VOID */ fun) noexcept -> void;
	};

#pragma region iostream

	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	inline auto operator<<(std::ostream& os, string&& value) noexcept -> decltype(auto)
	{
		for (const code_t code : txt {value})
		{
			/*global*/::operator<<(os, code);
		}
		return std::forward<decltype(os)>(os);
	}

	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	inline auto operator<<(std::istream& is, string&& value) noexcept -> decltype(auto)
	{
		for (const code_t code : txt {value})
		{
			/*global*/::operator>>(is, code);
		}
		return std::forward<decltype(is)>(is);
	}

#pragma endregion iostream
#pragma region character

	constexpr code_t::operator char32_t() const noexcept
	{
		return this->arg;
	}

	constexpr code_t::operator char32_t() /*&*/ noexcept
	{
		return this->arg;
	}

#pragma endregion character
#pragma region filesystem

	template <typename STRING> /*&*/ auto fileof(const STRING& path) noexcept -> std::optional
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

		static const auto byte_order_mask {[](std::ifstream& ifs) -> encoding
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
		}};

		static const auto write_as_native {[]<format_t local, typename alloc>(std::ifstream& ifs, str<local, alloc>& str) -> void
		{
			   using T = typename codec<local>::unit_t;
			// using U = typename codec<alien>::unit_t;

			T buffer;

			T* dest {str.head()};
			T* head {str.head()};

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

		static const auto write_as_exotic {[]<format_t local, typename alloc>(std::ifstream& ifs, str<local, alloc>& str) -> void
		{
			   using T = typename codec<local>::unit_t;
			// using U = typename codec<alien>::unit_t;

			T buffer;

			T* dest {str.head()};
			T* head {str.head()};

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

		const std::filesystem::path fs {[&]() noexcept -> decltype(fs)
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
		}
		()};

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

					/* UTF-8 is endian insensitive */ write_as_native(ifs, str);

					return str;
				}
				case UTF8_BOM:
				{
					str<"UTF-8"> str;

					str.capacity(max / sizeof(typename decltype(str)::unit_t));

					/* UTF-8 is endian insensitive */ write_as_native(ifs, str);

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

	using str8 = str<"UTF-8">;
	using str16 = str<"UTF-16">;
	using str32 = str<"UTF-32">;

	using txt8 = txt<"UTF-8">;
	using txt16 = txt<"UTF-16">;
	using txt32 = txt<"UTF-32">;

	template <size_t N> str(const char8_t (&_)[N]) -> str<"UTF-8">;
	template <size_t N> str(const char16_t (&_)[N]) -> str<"UTF-16">;
	template <size_t N> str(const char32_t (&_)[N]) -> str<"UTF-32">;

	template <size_t N> txt(const char8_t (&_)[N]) -> txt<"UTF-8">;
	template <size_t N> txt(const char16_t (&_)[N]) -> txt<"UTF-16">;
	template <size_t N> txt(const char32_t (&_)[N]) -> txt<"UTF-32">;
}

// NOLINTBEGIN(unused-includes)

#include "./private/cable.inl"
#include "./private/detail.inl"

#include "./private/model/str.inl"
#include "./private/model/txt.inl"

#include "./private/codec/UTF8.inl"
#include "./private/codec/UTF16.inl"
#include "./private/codec/UTF32.inl"
#include "./private/codec/ASCII.inl"

// NOLINTEND(unused-includes)

#undef COPY_ASSIGNMENT
#undef MOVE_ASSIGNMENT

#undef COPY_CONSTRUCTOR
#undef MOVE_CONSTRUCTOR

inline auto operator<<(std::ostream& os, char32_t code) noexcept -> std::ostream&
{
	char out[4]; short unit {0};

	/**/ if (code <= 0x00007F)
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

template <x69::format_t local, typename alloc> struct std::hash<x69::str<local, alloc>>
{
	constexpr auto operator()(const x69::str<local, alloc>& str) const noexcept -> size_t
	{
		uint32_t seed {0};

		for (const auto code : str)
		{
			seed = 31 * seed + code;
		}
		return static_cast<size_t>(seed);
	}
};

template <x69::format_t local /* can't own */> struct std::hash<x69::txt<local  /*&*/>>
{
	constexpr auto operator()(const x69::txt<local  /*&*/>& str) const noexcept -> size_t
	{
		uint32_t seed {0};

		for (const auto code : str)
		{
			seed = 31 * seed + code;
		}
		return static_cast<size_t>(seed);
	}
};

template <x69::format_t local, typename alloc>
inline constexpr bool std::ranges::disable_sized_range<x69::str<local, alloc>> = true;

template <x69::format_t local /* can't own */>
inline constexpr bool std::ranges::disable_sized_range<x69::txt<local  /*&*/>> = true;
