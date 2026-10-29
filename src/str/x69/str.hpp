#pragma once

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
#include <iostream>
#include <algorithm>
#include <type_traits>

#include "../alias.hpp"

inline auto operator>>(std::istream& os, char32_t code) noexcept -> std::istream&;
inline auto operator<<(std::ostream& os, char32_t code) noexcept -> std::ostream&;

#define typeᵐᵉᵐˀ template <typename...> typename

#define COPY_CONSTRUCTOR(T) constexpr T(const T& lval) noexcept
#define MOVE_CONSTRUCTOR(T) constexpr T(/*&*/ T&& rval) noexcept

#define COPY_ASSIGNMENT(T) constexpr auto operator=(const T& lval) noexcept -> T&
#define MOVE_ASSIGNMENT(T) constexpr auto operator=(/*&*/ T&& rval) noexcept -> T&

//┌──────────────────────────────────────────────────────────────┐
//│ special thanks to facebook's folly::FBString.                │
//│                                                              │
//│ SSO mode uses every bytes of heap string struct using union. │
//│ this was achievable thanks to the very clever memory layout. │
//│                                                              │
//│ for more, watch https://www.youtube.com/watch?v=kPR8h4-qZdk. │
//└──────────────────────────────────────────────────────────────┘

namespace x69
{
	template <typename Fˀ,  /*/───<↓>───/*/
	          typeᵐᵉᵐˀ Aˀ = std::allocator> class str;
	template <typename Fˀ                 > class txt;

	/*──────────────────────────────────────────────────────────────────────────/#┐*/
	NEW_TYPE(codeᵖᵗ, char32_t, constexpr auto lower() const noexcept -> codeᵖᵗ; /*│*/
	                           constexpr auto title() const noexcept -> codeᵖᵗ; /*│*/
	                           constexpr auto upper() const noexcept -> codeᵖᵗ; /*│*/
	                                                                            /*│*/
	                           using c_str_ref = const char32_t(&)[1];          /*│*/
	                           using c_str_mut = /*&*/ char32_t(&)[1];          /*│*/
	                                                                            /*│*/
	                           constexpr operator c_str_ref() const noexcept;   /*│*/
	                           constexpr operator c_str_mut() /*&*/ noexcept;   /*│*/);
	/*──────────────────────────────────────────────────────────────────────────/#┘*/

	#define CODER_AND_DECODER(_unit, _is_variable, _is_stateful)            \
	{                                                                       \
	    static inline constexpr bool is_variable {_is_variable};            \
	    static inline constexpr bool is_stateful {_is_stateful};            \
	                                                                        \
	    NEW_TYPE(stride, i8);                                               \
	                                                                        \
	    typedef _unit codeᵘᵗ;                                               \
	                                                                        \
	    static constexpr auto /* 𝑓 code point → abs(len) */                 \
	    size([[maybe_unused]] const codeᵖᵗ& code) noexcept -> stride;       \
	    static constexpr auto /* 𝑓 code unit(s) → pos(len) */               \
	    next([[maybe_unused]] const codeᵘᵗ* data) noexcept -> stride;       \
	    static constexpr auto /* 𝑓 code unit(s) → neg(len) */               \
	    back([[maybe_unused]] const codeᵘᵗ* data) noexcept -> stride;       \
	                                                                        \
	    static constexpr auto /* 𝑓 code point → code unit(s). */            \
	    encode(const codeᵖᵗ& in, codeᵘᵗ* out, stride dir) noexcept -> void; \
	                                                                        \
	    static constexpr auto /* 𝑓 code unit(s) → code point. */            \
	    decode(const codeᵘᵗ* in, codeᵖᵗ& out, stride dir) noexcept -> void; \
	}                                                                       \

	// https://en.wikipedia.org/wiki/ASCII
	struct ASCII final
	CODER_AND_DECODER(char, false, false);

	// https://en.wikipedia.org/wiki/UTF-8
	struct UTF_8 final
	CODER_AND_DECODER(char8_t, true, false);

	// https://en.wikipedia.org/wiki/UTF-16
	struct UTF_16 final
	CODER_AND_DECODER(char16_t, true, false);

	// https://en.wikipedia.org/wiki/UTF-32
	struct UTF_32 final
	CODER_AND_DECODER(char32_t, false, false);

	namespace API
	{
		template <typename _, typename F> class cstr
		{
			// shorthand; CODEC's code unit
			typedef typename F::codeᵘᵗ unit;

			template <char S>        class handle;
			template <char S> friend class handle;

			template <char S>        class cursor;
			template <char S> friend class cursor;

			template <usize N>        class window;
			template <usize N> friend class window;

			template <typename... Tᴺ>        class concat;
			template <typename... Tᴺ> friend class concat;

			// befriend to every instantiation thereof cstr
			template <typename, typename> friend class cstr;

			constexpr auto _head() const noexcept -> const unit*;
			constexpr auto _tail() const noexcept -> const unit*;

		public:

			// returns → number of __code unit__(s), excluding {null-term}.
			constexpr auto size() const noexcept -> usize;
			// returns → number of __code point__(s), excluding {null-term}.
			constexpr auto length() const noexcept -> usize;

			// returns → (whether or not := bool) it starts w/ {parameter}.
			constexpr auto starts_w(auto&& inˀ) const noexcept -> bool requires requires { txt {inˀ}; };

			// returns → (whether or not := bool) it contains {parameter}.
			constexpr auto contains(auto&& inˀ) const noexcept -> bool requires requires { txt {inˀ}; };

			// returns → (whether or not := bool) it ends w/ {parameter}.
			constexpr auto ends_w(auto&& inˀ) const noexcept -> bool requires requires { txt {inˀ}; };

			// returns → __slice__, of which is a product of substring.
			constexpr auto substr(clamp start, clamp until) const noexcept -> txt<F>;
			constexpr auto substr(clamp start, magic until) const noexcept -> txt<F>;
			constexpr auto substr(usize start, clamp until) const noexcept -> txt<F>;
			constexpr auto substr(usize start, magic until) const noexcept -> txt<F>;
			constexpr auto substr(usize start, usize until) const noexcept -> txt<F>;

			constexpr auto begin() const noexcept -> cursor<'L'>;
			constexpr auto end() const noexcept -> cursor<'L'>;

			constexpr auto rbegin() const noexcept -> cursor<'R'>;
			constexpr auto rend() const noexcept -> cursor<'R'>;

			// returns → memo, suitable for both sequential & random access.
			template <usize N = 3>
			constexpr auto it() const noexcept -> std::tuple<window<N>, usize>;
			template <usize N = 3>
			constexpr auto it() /*&*/ noexcept -> std::tuple<window<N>, usize>;

			constexpr auto operator[](usize rhs) const noexcept -> handle<'L'>;
			constexpr auto operator[](magic rhs) const noexcept -> handle<'R'>;
			constexpr auto operator[](clamp rhs) const noexcept -> handle<'R'>;

			// returns → list of __slice__, of which is a product of split(s).
			constexpr auto split(auto&& inˀ) const noexcept -> std::vector<txt<F>> requires requires { txt {inˀ}; };
			// returns → list of __slice__, of which is a product of match(s).
			constexpr auto match(auto&& inˀ) const noexcept -> std::vector<txt<F>> requires requires { txt {inˀ}; };

			// check following(s) in order: __address__, __length__, __content__.
			constexpr auto operator==(auto&& rhs) const noexcept -> bool requires requires { txt {rhs}; };
			constexpr auto operator!=(auto&& rhs) const noexcept -> bool requires requires { txt {rhs}; };

			friend constexpr auto operator==(auto&& lhs, const _& rhs) noexcept -> bool requires requires { txt {lhs}; } && (!std::is_base_of_v<cstr, std::remove_cvref_t<decltype(lhs)>>)
			{
				return rhs.operator==(lhs);
			}
			friend constexpr auto operator!=(auto&& lhs, const _& rhs) noexcept -> bool requires requires { txt {lhs}; } && (!std::is_base_of_v<cstr, std::remove_cvref_t<decltype(lhs)>>)
			{
				return rhs.operator!=(lhs);
			}

			friend constexpr auto operator+(const _& lhs, auto&& rhs) noexcept -> concat<decltype(lhs),
			                                                                             decltype(rhs)> requires requires { txt {rhs}; } // (!std::is_base_of_v<cstr, std::remove_cvref_t<decltype(rhs)>>)
			{
				return {lhs, rhs};
			}
			friend constexpr auto operator+(auto&& lhs, const _& rhs) noexcept -> concat<decltype(lhs),
			                                                                             decltype(rhs)> requires requires { txt {lhs}; } && (!std::is_base_of_v<cstr, std::remove_cvref_t<decltype(lhs)>>)
			{
				return {lhs, rhs};
			}

		private:

			template <char S> class handle
			{
				const unit* ptr;

			public:

				friend _; // {🔓}
				constexpr handle
				(
					decltype(ptr) _ptr
				)
				noexcept : ptr {_ptr} {}

				constexpr operator codeᵖᵗ() const noexcept; // getter..?

				constexpr auto operator==(codeᵖᵗ rhs) const noexcept -> bool;
				constexpr auto operator!=(codeᵖᵗ rhs) const noexcept -> bool;

				// constexpr auto operator=(codeᵖᵗ rhs) noexcept -> handle&;
			};

			template <char S> class cursor
			{
				const unit* ptr;

			public:

				using iterator_category = std::bidirectional_iterator_tag;
				using iterator_concept = std::bidirectional_iterator_tag;
				using difference_type = std::ptrdiff_t;
				using value_type = codeᵖᵗ;
				using reference = codeᵖᵗ;

				// stl compat {default_initializable}
				constexpr  cursor() noexcept = default;
				constexpr ~cursor() noexcept = default;

				friend _; // {🔓}
				constexpr cursor
				(
					const unit* _ptr
				)
				noexcept : ptr {_ptr} {}

				constexpr auto operator*() const noexcept -> value_type;

				constexpr auto operator++(   ) noexcept -> cursor&;
				constexpr auto operator--(   ) noexcept -> cursor&;
				constexpr auto operator++(int) noexcept -> cursor;
				constexpr auto operator--(int) noexcept -> cursor;

				constexpr auto operator==(const cursor& rhs) const noexcept -> bool;
				constexpr auto operator!=(const cursor& rhs) const noexcept -> bool;
			};

			template <usize N> class window
			{
				const unit* beg;
				const unit* end;

				WORD map;

				struct log { usize key {0};
				             isize off {0}; } logs[N];

				// ```
				// where N(3) = | 0 1 1 | where N(4) = | 0 1 1 1 |
				//              | 0 0 1 |              | 0 0 1 1 |
				//              | 0 0 0 |              | 0 0 0 1 |
				//                                     | 0 0 0 0 |
				// ```
				static constexpr auto NxN {[]
				{
					WORD out {0};

					for (usize y {0}; y < N; ++y)
					{
						for (usize x {0}; x < N; ++x)
						{
							if (x < y)
							{
								const auto off {y * N + x};

								out /**/ |= WORD {1} << off;
							}
						}
					}
					return out;
				}
				()};

				// given __N__ = __3__
				// ```
				// mask[0] = | 1 1 1 | mask[1] = | 0 0 0 | mask[2] = | 0 0 0 |
				//           | 0 0 0 |           | 1 1 1 |           | 0 0 0 |
				//           | 0 0 0 |           | 0 0 0 |           | 1 1 1 |
				// ```
				static constexpr auto ROWⁿ {[]
				{
					std::array<WORD, N> out {0};

					for (usize i {0}; i < N; ++i)
					{
						for (usize x {0}; x < N; ++x)
						{
							const auto off {i * N + x};

							out[i] |= WORD {1} << off;
						}
					}
					return out;
				}
				()};

				// given __N__ = __3__
				// ```
				// mask[0] = | 0 1 1 | mask[1] = | 1 0 1 | mask[2] = | 1 1 0 |
				//           | 0 1 1 |           | 1 0 1 |           | 1 1 0 |
				//           | 0 1 1 |           | 1 0 1 |           | 1 1 0 |
				// ```
				static constexpr auto COLⁿ {[]
				{
					std::array<WORD, N> out {0};

					for (usize i {0}; i < N; ++i)
					{
						for (usize x {0}; x < N; ++x)
						{
							for (usize y {0}; y < N; ++y)
							{
								if (x == i) { continue; }

								const auto off {y * N + x};

								out[i] |= WORD {1} << off;
							}
						}
					}
					return out;
				}
				()};

				static_assert(0 < N && N * N <= sizeof(WORD) * 8, "[1, 32]");

				// returns → [MRU...LRU] in std::array
				constexpr auto _ranks() const noexcept -> std::array<u8, N>;

			public:

				friend _; // {🔓}
				constexpr window
				(
					decltype(beg) _beg,
					decltype(end) _end
				)
				noexcept : beg {_beg},
				           end {_end},
				           map { NxN} {}

				constexpr auto operator[](usize rhs) noexcept -> handle<'L'>;
				constexpr auto operator[](magic rhs) noexcept -> handle<'R'>;
				constexpr auto operator[](clamp rhs) noexcept -> handle<'R'>;
			};

			template <typename... Tᴺ> class concat
			{
				template <typename T> using upcast = std::conditional_t // lval := ref
				<                                                       // rval := val
					std::is_lvalue_reference_v<T>, const std::remove_cvref_t<T>&,
					                               /*&*/ std::remove_cvref_t<T>>;

				std::tuple<upcast<Tᴺ>...> args;

				template <char S>        class handle;
				template <char S> friend class handle;

				template <char S>        class cursor;
				template <char S> friend class cursor;

			public:

				friend _; // {🔓}
				constexpr concat
				(
					upcast<Tᴺ>... _args
				)
				noexcept : args {_args...} {};

				template <typename Fˀ,
				          typeᵐᵉᵐˀ Aˀ>
				// flush; materialize?
				constexpr operator str<Fˀ, Aˀ>() const noexcept;

				constexpr auto begin() const noexcept -> cursor<'L'>;
				constexpr auto end() const noexcept -> cursor<'L'>;

				constexpr auto rbegin() const noexcept -> cursor<'R'>;
				constexpr auto rend() const noexcept -> cursor<'R'>;

				constexpr auto operator[](usize rhs) const noexcept -> handle<'L'>;
				constexpr auto operator[](magic rhs) const noexcept -> handle<'R'>;
				constexpr auto operator[](clamp rhs) const noexcept -> handle<'R'>;

				friend constexpr auto operator==(const concat& lhs, auto&& rhs) noexcept -> bool requires requires { txt {rhs}; } // (!std::is_base_of_v<concat, std::remove_cvref_t<decltype(rhs)>>)
				{
					return std::ranges::equal(lhs, txt {rhs});
				}
				friend constexpr auto operator!=(auto&& lhs, const concat& rhs) noexcept -> bool requires requires { txt {lhs}; } && (!std::is_base_of_v<concat, std::remove_cvref_t<decltype(lhs)>>)
				{
					return !std::ranges::equal(txt {lhs}, rhs);
				}

				friend constexpr auto operator+(const concat& lhs, auto&& rhs) noexcept -> concat<Tᴺ..., decltype(rhs)> requires requires { txt {rhs}; } // (!std::is_base_of_v<concat, std::remove_cvref_t<decltype(rhs)>>)
				{
					return std::apply([&](auto&&... lhs) -> concat<Tᴺ..., decltype(rhs)> { return {std::forward<decltype(lhs)>(lhs)..., std::forward<decltype(rhs)>(rhs)}; }, lhs.args);
				}
				friend constexpr auto operator+(auto&& lhs, const concat& rhs) noexcept -> concat<decltype(lhs), Tᴺ...> requires requires { txt {lhs}; } && (!std::is_base_of_v<concat, std::remove_cvref_t<decltype(lhs)>>)
				{
					return std::apply([&](auto&&... rhs) -> concat<decltype(lhs), Tᴺ...> { return {std::forward<decltype(lhs)>(lhs), std::forward<decltype(rhs)>(rhs)...}; }, rhs.args);
				}

				   template <typename... Tᴹ> friend constexpr auto operator+(const concat& lhs, concat<Tᴹ...>&& rhs) noexcept -> concat<Tᴺ..., Tᴹ...> // (!std::is_base_of_v<concat, std::remove_cvref_t<decltype(rhs)>>)
				   {
				   	return std::apply([&](auto&&... args) -> concat<decltype(lhs), Tᴺ...> { return {std::forward<decltype(args)>(args)...}; }, std::tuple_cat(lhs.args, rhs.args));
				   }
				// template <typename... Tᴹ> friend constexpr auto operator+(concat<Tᴹ...>&& lhs, const concat& rhs) noexcept -> concat<Tᴺ..., Tᴹ...> && (!std::is_base_of_v<concat, std::remove_cvref_t<decltype(lhs)>>)
				// {
				//  return std::apply([&](auto&&... args) -> concat<decltype(lhs), Tᴺ...> { return {std::forward<decltype(args)>(args)...}; }, std::tuple_cat(lhs.args, rhs.args));
				// }

			private:

				template <char S> class needle
				{
					const concat* src;
					// buffer ?meta?
					usize       ctx;
					const void* beg;
					const void* end;
					const void* ptr;

				public:

					constexpr needle
					(
						decltype(src) _src,
						decltype(ctx) _ctx,
						decltype(beg) _beg,
						decltype(end) _end,
						decltype(ptr) _ptr
					)
					noexcept : src {_src},
					           ctx {_ctx},
					           beg {_beg},
					           end {_end},
					           ptr {_ptr} {}

					constexpr auto operator*() const noexcept -> codeᵖᵗ;

					constexpr auto operator++(   ) noexcept -> needle&;
					constexpr auto operator--(   ) noexcept -> needle&;
					constexpr auto operator++(int) noexcept -> needle;
					constexpr auto operator--(int) noexcept -> needle;

					constexpr auto operator==(const needle& rhs) const noexcept -> bool;
					constexpr auto operator!=(const needle& rhs) const noexcept -> bool;
				};

				template <char S> class handle
				{
					needle<S> ptr {0,0,0,0,0};

				public:

					constexpr handle
					(
						decltype(ptr) _ptr
					)
					noexcept : ptr {_ptr} {}

					constexpr operator codeᵖᵗ() const noexcept; // getter..?
	
					constexpr auto operator==(codeᵖᵗ rhs) const noexcept -> bool;
					constexpr auto operator!=(codeᵖᵗ rhs) const noexcept -> bool;
	
					// constexpr auto operator=(codeᵖᵗ rhs) noexcept -> handle&;
				};

				template <char S> class cursor
				{
					needle<S> ptr {0,0,0,0,0};

				public:

					using iterator_category = std::bidirectional_iterator_tag;
					using iterator_concept = std::bidirectional_iterator_tag;
					using difference_type = std::ptrdiff_t;
					using value_type = codeᵖᵗ;
					using reference = codeᵖᵗ;

					// stl compat {default_initializable}
					constexpr  cursor() noexcept = default;
					constexpr ~cursor() noexcept = default;

					constexpr cursor
					(
						decltype(ptr) _ptr
					)
					noexcept : ptr {_ptr} {}

					constexpr auto operator*() const noexcept -> value_type;

					constexpr auto operator++(   ) noexcept -> cursor&;
					constexpr auto operator--(   ) noexcept -> cursor&;
					constexpr auto operator++(int) noexcept -> cursor;
					constexpr auto operator--(int) noexcept -> cursor;

					constexpr auto operator==(const cursor& rhs) const noexcept -> bool;
					constexpr auto operator!=(const cursor& rhs) const noexcept -> bool;
				};
			};

		protected:

			template <char  S> using cursor_t = cursor<S>;
			template <char  S> using handle_t = handle<S>;
			template <usize N> using window_t = window<N>;

			template <typename Tᵒᵇʲ,
			          typename Tᵒᵘᵗ> static constexpr auto use(const auto* obj, Tᵒᵘᵗ Tᵒᵇʲ::*ptr) noexcept -> const Tᵒᵘᵗ&
			requires
			(
				std::is_base_of_v<Tᵒᵇʲ, std::remove_cvref_t<decltype(*obj)>> && std::is_member_object_pointer_v<Tᵒᵘᵗ Tᵒᵇʲ::*>
			)
			{
				return static_cast<const Tᵒᵇʲ*>(obj)->*ptr;
			};
			template <typename Tᵒᵇʲ,
			          typename Tᵒᵘᵗ> static constexpr auto use(/*&*/ auto* obj, Tᵒᵘᵗ Tᵒᵇʲ::*ptr) noexcept -> /*&*/ Tᵒᵘᵗ&
			requires
			(
				std::is_base_of_v<Tᵒᵇʲ, std::remove_cvref_t<decltype(*obj)>> && std::is_member_object_pointer_v<Tᵒᵘᵗ Tᵒᵇʲ::*>
			)
			{
				return static_cast</*&*/ Tᵒᵇʲ*>(obj)->*ptr;
			};
		};
	}

	template <typename F> class txt final : public API::cstr<txt<F>, F>
	{
		typedef API::cstr<txt, F> super;

		// shorthand; CODEC's code unit
		typedef typename F::codeᵘᵗ unit;

		const unit* head;
		const unit* tail;

		// befriend to every instantiation thereof of IMPL(s)
		template <typename, typeᵐᵉᵐˀ> friend class str;
		template <typename          > friend class txt;
		// befriend to every instantiation thereof of CRTP(s)
		template <typename, typename> friend class API::cstr;

	public:

		constexpr txt
		(
			decltype(head) _head,
			decltype(tail) _tail
		)
		noexcept : head {_head},
		           tail {_tail} {}

		template <usize N>
		// from {literal}
		constexpr txt
		(
			const unit (&init)[N]
		)
		noexcept : head {&init[N - N]},
		           tail {&init[N - 1]} {}

		template <typeᵐᵉᵐˀ A>
		// from {x69::str}
		constexpr txt
		(
			const str<F, A>& init
		)
		noexcept : head {init._head()},
		           tail {init._tail()} {}

		constexpr txt // UTF-8
		(
			const char8_t& _
		)
		noexcept requires (std::is_same_v<F, UTF_8>) : head {&_ + 0},
		                                               tail {&_ + 1} {}

		constexpr txt // UTF-16
		(
			const char16_t& _
		)
		noexcept requires (std::is_same_v<F, UTF_16>) : head {&_ + 0},
		                                                tail {&_ + 1} {}

		constexpr txt // UTF-32
		(
			const char32_t& _
		)
		noexcept requires (std::is_same_v<F, UTF_32>) : head {&_ + 0},
		                                                tail {&_ + 1} {}
	};

	template <typename F,
	          typeᵐᵉᵐˀ A> class str final : public API::cstr<str<F,
	                                                             A>, F>
	{
		typedef API::cstr<str, F> super;

		// shorthand; CODEC's code unit
		typedef typename F::codeᵘᵗ unit;

		template <char S>        class handleᵐᵘᵗ;
		template <char S> friend class handleᵐᵘᵗ;

		template <usize N>        class windowᵐᵘᵗ;
		template <usize N> friend class windowᵐᵘᵗ;

		// befriend to every instantiation thereof of IMPL(s)
		template <typename, typeᵐᵉᵐˀ> friend class str;
		template <typename          > friend class txt;
		// befriend to every instantiation thereof of CRTP(s)
		template <typename, typename> friend class API::cstr;

		enum mode : BYTE
		{
			SSO = std::endian::native == std::endian::little ? 0b00000000
			                                                 : 0b00000000,
			STD = std::endian::native == std::endian::little ? 0b10000000
			                                                 : 0b00000001,
		};

		struct blob final
		{
			unit* head;
			unit* last;
			WORD  meta;
		};

		static constexpr u8 MAX {(sizeof(blob) - 1) / (sizeof(unit))};
		static constexpr u8 RMB {(sizeof(blob) - 1) * (      1     )};

		//┌────────────────────┐
		//│       small        │
		//├──────┬──────┬──────┤
		//│ head │ last │ meta │
		//├──────┴──────┴──────┤
		//│       bytes        │
		//└────────────────────┘

		[[no_unique_address, msvc::no_unique_address]] A<unit> useᵐᵉᵐˀ;

		union
		{
			unit small [sizeof(blob) / sizeof(unit)];
			blob large                              ;
			BYTE bytes [sizeof(blob) / sizeof(char)];
		};

		constexpr auto _mode() const noexcept -> mode;

		constexpr auto _head() const noexcept -> const unit*;
		constexpr auto _head() /*&*/ noexcept -> /*&*/ unit*;
		constexpr auto _tail() const noexcept -> const unit*;
		constexpr auto _tail() /*&*/ noexcept -> /*&*/ unit*;

		constexpr auto _SSOᴺ(usize inˀ) noexcept -> void;

		template <char  S> using handleᶠⁱⁿ = typename API::cstr<str, F>::template handle_t<S>;
		template <char  S> using cursorᶠⁱⁿ = typename API::cstr<str, F>::template cursor_t<S>;
		template <usize N> using windowᶠⁱⁿ = typename API::cstr<str, F>::template window_t<N>;

	public:

		// CRLF/CR to LF normalization.
		template <template <typename> typename Aˀ> friend auto fileof(auto&& pathˀ) noexcept -> std::optional
		                                                                                        <
		                                                                                        	std::variant
		                                                                                        	<
		                                                                                        		str<UTF_8, Aˀ>
		                                                                                        		,
		                                                                                        		str<UTF_16, Aˀ>
		                                                                                        		,
		                                                                                        		str<UTF_32, Aˀ>
		                                                                                        	>
		                                                                                        >;

		constexpr str(auto&& init) noexcept requires requires { txt {init}; };

		constexpr  str() noexcept;
		constexpr ~str() noexcept;

		COPY_CONSTRUCTOR(str);
		MOVE_CONSTRUCTOR(str);

		COPY_ASSIGNMENT(str);
		MOVE_ASSIGNMENT(str);

		// gets the number of code units it can hold, excluding {null-term}.
		constexpr auto space(/* get */) const noexcept -> usize;
		// sets the number of code units it can hold, excluding {null-term}.
		constexpr auto spare(usize inˀ) /*&*/ noexcept -> void;

		constexpr auto operator[](usize rhs) const noexcept -> handleᶠⁱⁿ<'L'>;
		constexpr auto operator[](usize rhs) /*&*/ noexcept -> handleᵐᵘᵗ<'L'>;

		constexpr auto operator[](magic rhs) const noexcept -> handleᶠⁱⁿ<'R'>;
		constexpr auto operator[](magic rhs) /*&*/ noexcept -> handleᵐᵘᵗ<'R'>;

		constexpr auto operator[](clamp rhs) const noexcept -> handleᶠⁱⁿ<'R'>;
		constexpr auto operator[](clamp rhs) /*&*/ noexcept -> handleᵐᵘᵗ<'R'>;

		template <usize N = 3>
		constexpr auto it() const noexcept -> std::tuple<windowᶠⁱⁿ<N>, usize>;
		template <usize N = 3>
		constexpr auto it() /*&*/ noexcept -> std::tuple<windowᵐᵘᵗ<N>, usize>;

		constexpr auto operator=(auto&& rhs)& noexcept -> str& requires requires { txt {rhs}; };
		constexpr auto operator=(auto&& rhs)&& noexcept -> str&& requires requires { txt {rhs}; };

		constexpr auto operator+=(auto&& rhs)& noexcept -> str& requires requires { txt {rhs}; };
		constexpr auto operator+=(auto&& rhs)&& noexcept -> str&& requires requires { txt {rhs}; };

	private:

		template <char S> class handleᵐᵘᵗ : public handleᶠⁱⁿ<S>
		{
			str* src;

		public:

			constexpr handleᵐᵘᵗ
			(
				handleᶠⁱⁿ<S> _ctx,
				decltype(src) _src
			)
			noexcept : handleᶠⁱⁿ<S>(_ctx), src {_src} {}

			constexpr auto operator=(codeᵖᵗ rhs) noexcept -> handleᵐᵘᵗ&;
		};

		template <char S> class cursorᵐᵘᵗ : public cursorᶠⁱⁿ<S>
		{
			str* src;

		public:

			constexpr cursorᵐᵘᵗ
			(
				cursorᶠⁱⁿ<S> _ctx,
				decltype(src) _src
			)
			noexcept : cursorᶠⁱⁿ<S>(_ctx), src {_src} {}

			// constexpr auto operator=(codeᵖᵗ rhs) noexcept -> handleᵐᵘᵗ&;
		};

		template <usize N> class windowᵐᵘᵗ : public windowᶠⁱⁿ<N>
		{
			str* src;

			template <char S>        class handleᵃˡᵗ;
			template <char S> friend class handleᵃˡᵗ;

		public:

			constexpr windowᵐᵘᵗ
			(
				windowᶠⁱⁿ<N> _ctx,
				decltype(src) _src
			)
			noexcept : windowᶠⁱⁿ<N>(_ctx), src {_src} {}

			constexpr auto operator[](usize rhs) noexcept -> handleᵃˡᵗ<'L'>;
			constexpr auto operator[](magic rhs) noexcept -> handleᵃˡᵗ<'R'>;
			constexpr auto operator[](clamp rhs) noexcept -> handleᵃˡᵗ<'R'>;

		private:

			template <char S> class handleᵃˡᵗ : public handleᵐᵘᵗ<S>
			{
				windowᵐᵘᵗ* src;

			public:

				constexpr handleᵃˡᵗ
				(
					handleᵐᵘᵗ<S> _ctx,
					decltype(src) _src
				)
				noexcept : handleᵐᵘᵗ<S>(_ctx), src {_src} {}

				constexpr auto operator=(codeᵖᵗ rhs) noexcept -> handleᵃˡᵗ&;
			};
		};
	};

	namespace detail
	{
		template <typename Fᵒᵘᵗ> static constexpr auto ΔCU(const typename Fᵒᵘᵗ::codeᵘᵗ* head,
		                                                   const typename Fᵒᵘᵗ::codeᵘᵗ* tail) noexcept -> usize;

		template <typename Fᵒᵘᵗ> static constexpr auto ΔCP(const typename Fᵒᵘᵗ::codeᵘᵗ* head,
		                                                   const typename Fᵒᵘᵗ::codeᵘᵗ* tail) noexcept -> usize;

		template <typename Fᵒᵘᵗ,
		          typename Fˢʳᶜ> static constexpr auto ΔCU(const typename Fˢʳᶜ::codeᵘᵗ* head,
		                                                   const typename Fˢʳᶜ::codeᵘᵗ* tail) noexcept -> usize;

		template <typename Fᵒᵘᵗ,
		          typename Fˢʳᶜ> static constexpr auto copy(typename Fᵒᵘᵗ::codeᵘᵗ* dest, const typename Fˢʳᶜ::codeᵘᵗ* head,
		                                                                                 const typename Fˢʳᶜ::codeᵘᵗ* tail) noexcept -> usize;

		template <typename Fˡʰˢ,
		          typename Fʳʰˢ> static constexpr auto equal(const typename Fˡʰˢ::codeᵘᵗ* lhs0, const typename Fˡʰˢ::codeᵘᵗ* lhsN,
		                                                     const typename Fʳʰˢ::codeᵘᵗ* rhs0, const typename Fʳʰˢ::codeᵘᵗ* rhsN) noexcept -> bool;

		template <typename Fˡʰˢ,
		          typename Fʳʰˢ> static constexpr auto swith(const typename Fˡʰˢ::codeᵘᵗ* lhs0, const typename Fˡʰˢ::codeᵘᵗ* lhsN,
		                                                     const typename Fʳʰˢ::codeᵘᵗ* rhs0, const typename Fʳʰˢ::codeᵘᵗ* rhsN) noexcept -> bool;

		template <typename Fˡʰˢ,
		          typename Fʳʰˢ> static constexpr auto ewith(const typename Fˡʰˢ::codeᵘᵗ* lhs0, const typename Fˡʰˢ::codeᵘᵗ* lhsN,
		                                                     const typename Fʳʰˢ::codeᵘᵗ* rhs0, const typename Fʳʰˢ::codeᵘᵗ* rhsN) noexcept -> bool;

		template <typename Fˡʰˢ,
		          typename Fʳʰˢ> static constexpr auto holds(const typename Fˡʰˢ::codeᵘᵗ* lhs0, const typename Fˡʰˢ::codeᵘᵗ* lhsN,
		                                                     const typename Fʳʰˢ::codeᵘᵗ* rhs0, const typename Fʳʰˢ::codeᵘᵗ* rhsN) noexcept -> bool;

		template <typename Fˡʰˢ,
		          typename Fʳʰˢ> static constexpr auto split(const typename Fˡʰˢ::codeᵘᵗ* lhs0, const typename Fˡʰˢ::codeᵘᵗ* lhsN,
		                                                     const typename Fʳʰˢ::codeᵘᵗ* rhs0, const typename Fʳʰˢ::codeᵘᵗ* rhsN) noexcept -> std::vector<txt<Fˡʰˢ>>;

		template <typename Fˡʰˢ,
		          typename Fʳʰˢ> static constexpr auto match(const typename Fˡʰˢ::codeᵘᵗ* lhs0, const typename Fˡʰˢ::codeᵘᵗ* lhsN,
		                                                     const typename Fʳʰˢ::codeᵘᵗ* rhs0, const typename Fʳʰˢ::codeᵘᵗ* rhsN) noexcept -> std::vector<txt<Fˡʰˢ>>;

		template <typename Fˡʰˢ,
		          typename Fʳʰˢ> static constexpr auto search(const typename Fˡʰˢ::codeᵘᵗ* lhs0, const typename Fˡʰˢ::codeᵘᵗ* lhsN,
		                                                      const typename Fʳʰˢ::codeᵘᵗ* rhs0, const typename Fʳʰˢ::codeᵘᵗ* rhsN, auto&& fn)
		// info {error-free} & {interface}
		noexcept(std::is_nothrow_invocable_v<decltype(fn), const typename Fʳʰˢ::codeᵘᵗ*,
		                                                   const typename Fʳʰˢ::codeᵘᵗ*>) -> void requires (std::is_invocable_r_v<bool, decltype(fn), const typename Fʳʰˢ::codeᵘᵗ*,
		                                                                                                                                              const typename Fʳʰˢ::codeᵘᵗ*>);

		template <typename Fᵒᵘᵗ> static constexpr auto substr(const typename Fᵒᵘᵗ::codeᵘᵗ* head, const typename Fᵒᵘᵗ::codeᵘᵗ* tail, usize off) noexcept -> decltype(head);
		template <typename Fᵒᵘᵗ> static constexpr auto substr(/*&*/ typename Fᵒᵘᵗ::codeᵘᵗ* head, /*&*/ typename Fᵒᵘᵗ::codeᵘᵗ* tail, usize off) noexcept -> decltype(head);

		template <typename Fᵒᵘᵗ> static constexpr auto substr(const typename Fᵒᵘᵗ::codeᵘᵗ* head, const typename Fᵒᵘᵗ::codeᵘᵗ* tail, magic off) noexcept -> decltype(head);
		template <typename Fᵒᵘᵗ> static constexpr auto substr(/*&*/ typename Fᵒᵘᵗ::codeᵘᵗ* head, /*&*/ typename Fᵒᵘᵗ::codeᵘᵗ* tail, magic off) noexcept -> decltype(head);

		template <typename Fᵒᵘᵗ> static constexpr auto substr(const typename Fᵒᵘᵗ::codeᵘᵗ* head, const typename Fᵒᵘᵗ::codeᵘᵗ* tail, clamp off) noexcept -> decltype(head);
		template <typename Fᵒᵘᵗ> static constexpr auto substr(/*&*/ typename Fᵒᵘᵗ::codeᵘᵗ* head, /*&*/ typename Fᵒᵘᵗ::codeᵘᵗ* tail, clamp off) noexcept -> decltype(head);

		   template <typename Fᵒᵘᵗ> static constexpr auto substr(const typename Fᵒᵘᵗ::codeᵘᵗ* head, const typename Fᵒᵘᵗ::codeᵘᵗ* tail, clamp beg, clamp end) noexcept -> txt<Fᵒᵘᵗ>;
		// template <typename Fᵒᵘᵗ> static constexpr auto substr(/*&*/ typename Fᵒᵘᵗ::codeᵘᵗ* head, /*&*/ typename Fᵒᵘᵗ::codeᵘᵗ* tail, clamp beg, clamp end) noexcept -> txt<Fᵒᵘᵗ>;

		   template <typename Fᵒᵘᵗ> static constexpr auto substr(const typename Fᵒᵘᵗ::codeᵘᵗ* head, const typename Fᵒᵘᵗ::codeᵘᵗ* tail, clamp beg, magic end) noexcept -> txt<Fᵒᵘᵗ>;
		// template <typename Fᵒᵘᵗ> static constexpr auto substr(/*&*/ typename Fᵒᵘᵗ::codeᵘᵗ* head, /*&*/ typename Fᵒᵘᵗ::codeᵘᵗ* tail, clamp beg, magic end) noexcept -> txt<Fᵒᵘᵗ>;

		   template <typename Fᵒᵘᵗ> static constexpr auto substr(const typename Fᵒᵘᵗ::codeᵘᵗ* head, const typename Fᵒᵘᵗ::codeᵘᵗ* tail, usize beg, clamp end) noexcept -> txt<Fᵒᵘᵗ>;
		// template <typename Fᵒᵘᵗ> static constexpr auto substr(/*&*/ typename Fᵒᵘᵗ::codeᵘᵗ* head, /*&*/ typename Fᵒᵘᵗ::codeᵘᵗ* tail, usize beg, clamp end) noexcept -> txt<Fᵒᵘᵗ>;

		   template <typename Fᵒᵘᵗ> static constexpr auto substr(const typename Fᵒᵘᵗ::codeᵘᵗ* head, const typename Fᵒᵘᵗ::codeᵘᵗ* tail, usize beg, magic end) noexcept -> txt<Fᵒᵘᵗ>;
		// template <typename Fᵒᵘᵗ> static constexpr auto substr(/*&*/ typename Fᵒᵘᵗ::codeᵘᵗ* head, /*&*/ typename Fᵒᵘᵗ::codeᵘᵗ* tail, usize beg, magic end) noexcept -> txt<Fᵒᵘᵗ>;

		   template <typename Fᵒᵘᵗ> static constexpr auto substr(const typename Fᵒᵘᵗ::codeᵘᵗ* head, const typename Fᵒᵘᵗ::codeᵘᵗ* tail, usize beg, usize end) noexcept -> txt<Fᵒᵘᵗ>;
		// template <typename Fᵒᵘᵗ> static constexpr auto substr(/*&*/ typename Fᵒᵘᵗ::codeᵘᵗ* head, /*&*/ typename Fᵒᵘᵗ::codeᵘᵗ* tail, usize beg, usize end) noexcept -> txt<Fᵒᵘᵗ>;
	};

	inline auto operator<<(std::ostream& os, auto&& rval) noexcept -> std::ostream& requires requires { txt {rval}; }
	{
		typedef decltype(os) forward_t;
	
		for (const auto code : txt {rval})
		{
			::operator<<(os, code);
		}
		return std::forward<forward_t>(os);
	}
	
	inline auto operator<<(std::istream& is, auto&& rval) noexcept -> std::istream& requires requires { txt {rval}; }
	{
		typedef decltype(is) forward_t;
	
		for (const auto code : txt {rval})
		{
			::operator>>(is, code);
		}
		return std::forward<forward_t>(is);
	}

	template <template <typename> typename Aˀ = std::allocator> static auto fileof(auto&& path) noexcept -> std::optional
	                                                                                                        <
	                                                                                                        	std::variant
	                                                                                                        	<
	                                                                                                        		str<UTF_8, Aˀ>
	                                                                                                        		,
	                                                                                                        		str<UTF_16, Aˀ>
	                                                                                                        		,
	                                                                                                        		str<UTF_32, Aˀ>
	                                                                                                        	>
	                                                                                                        >
	{
		enum encode : u8
		{
			UTF8_STD = (0 << 4) | 0,
			UTF8_BOM = (1 << 4) | 3,
			UTF16_BE = (2 << 4) | 2,
			UTF16_LE = (3 << 4) | 2,
			UTF32_BE = (4 << 4) | 4,
			UTF32_LE = (5 << 4) | 4,
		};

		constexpr auto byte_order_mask = [](std::ifstream& ifs) -> std::tuple<encode, usize>
		{
			char buffer[4];

			ifs.seekg(0, std::ios::beg); // 1. move to the beginning of the file
			ifs.read(&buffer[0], 4); const auto bytes {ifs.gcount()}; ifs.clear();
			ifs.seekg(0, std::ios::beg); // 2. move to the beginning of the file

			switch (bytes)
			{
				case 4: [[unlikely]]
				{
					// 00 00 FE FF
					if (buffer[0] == '\x00'
					    &&
					    buffer[1] == '\x00'
					    &&
					    buffer[2] == '\xFE'
					    &&
					    buffer[3] == '\xFF') [[unlikely]] return {UTF32_BE, UTF32_BE & 0xF};

					// FF FE 00 00
					if (buffer[0] == '\xFF'
					    &&
					    buffer[1] == '\xFE'
					    &&
					    buffer[2] == '\x00'
					    &&
					    buffer[3] == '\x00') [[unlikely]] return {UTF32_LE, UTF32_LE & 0xF};

					[[fallthrough]];
				}
				case 2: [[unlikely]]
				{
					// FE FF
					if (buffer[0] == '\xFE'
					    &&
					    buffer[1] == '\xFF') [[unlikely]] return {UTF16_BE, UTF16_BE & 0xF};

					// FF FE
					if (buffer[0] == '\xFF'
					    &&
					    buffer[1] == '\xFE') [[unlikely]] return {UTF16_LE, UTF16_LE & 0xF};

					[[fallthrough]];
				}
				case 3: [[unlikely]]
				{
					// EF BB BF
					if (buffer[0] == '\xEF'
					    &&
					    buffer[1] == '\xBB'
					    &&
					    buffer[2] == '\xBF') [[unlikely]] return {UTF8_BOM, UTF8_BOM & 0xF};

					[[fallthrough]];
				}
			}
			//     (BOM-less UTF-8 encoding?)
			return {UTF8_STD, UTF8_STD & 0xF};
		};

		constexpr auto write_as_native = []<typename Fˀ>(std::ifstream& ifs, str<Fˀ, Aˀ>& out)
		{
			   typedef typename Fˀ::codeᵘᵗ unitˣ;
			// typedef typename Fᵞ::codeᵘᵗ unitᵞ;

			unitˣ buffer;

			unitˣ* /*&*/ dest {out._head()};
			unitˣ* const head {out._head()};

			while (ifs.read(reinterpret_cast<char*>(&buffer), sizeof(unitˣ)))
			{
				if (buffer == '\r'
				    &&
				    ifs.read(reinterpret_cast<char*>(&buffer), sizeof(unitˣ))
				    &&
				    buffer != '\n')
				{
					*(dest++) = '\n';
				}
				*(dest++) = buffer;
			}
			out._SSOᴺ(dest - head);
		};

		constexpr auto write_as_exotic = []<typename Fˀ>(std::ifstream& ifs, str<Fˀ, Aˀ>& out)
		{
			   typedef typename Fˀ::codeᵘᵗ unitˣ;
			// typedef typename Fᵞ::codeᵘᵗ unitᵞ;

			unitˣ buffer;

			unitˣ* /*&*/ dest {out._head()};
			unitˣ* const head {out._head()};

			while (ifs.read(reinterpret_cast<char*>(&buffer), sizeof(unitˣ)))
			{
				if ((buffer = std::byteswap(buffer)) == '\r'
				    &&
				    ifs.read(reinterpret_cast<char*>(&buffer), sizeof(unitˣ))
				    &&
				    (buffer = std::byteswap(buffer)) != '\n')
				{
					*(dest++) = '\n';
				}
				*(dest++) = buffer;
			}
			out._SSOᴺ(dest - head);
		};

		if (std::ifstream ifs {path, std::ios::binary})
		{
			const auto [BOM, off] {byte_order_mask(ifs)};

			// to the BOM
			ifs.seekg(off, std::ios::beg); const auto beg {ifs.tellg()};
			// to the END
			ifs.seekg(0x0, std::ios::end); const auto end {ifs.tellg()};
			// to the BOM
			ifs.seekg(off, std::ios::beg); const auto max {end - beg};

			switch (BOM)
			{
				case UTF8_STD:
				{
					str<UTF_8, Aˀ> out;

					out.spare(max / sizeof(typename decltype(out)::unit));

					/* 📚 endian insensitive */ write_as_native(ifs, out);

					return {out};
				}
				case UTF8_BOM:
				{
					str<UTF_8, Aˀ> out;

					out.spare(max / sizeof(typename decltype(out)::unit));

					/* 📚 endian insensitive */ write_as_native(ifs, out);

					return {out};
				}
				case UTF16_LE:
				{
					str<UTF_16, Aˀ> out;

					out.spare(max / sizeof(typename decltype(out)::unit));

					if constexpr (std::endian::native == std::endian::little) write_as_native(ifs, out);
					                                                     else write_as_exotic(ifs, out);

					return {out};
				}
				case UTF16_BE:
				{
					str<UTF_16, Aˀ> out;

					out.spare(max / sizeof(typename decltype(out)::unit));

					if constexpr (std::endian::native == std::endian::big) write_as_native(ifs, out);
					                                                  else write_as_exotic(ifs, out);

					return {out};
				}
				case UTF32_LE:
				{
					str<UTF_32, Aˀ> out;

					out.spare(max / sizeof(typename decltype(out)::unit));

					if constexpr (std::endian::native == std::endian::little) write_as_native(ifs, out);
					                                                     else write_as_exotic(ifs, out);

					return {out};
				}
				case UTF32_BE:
				{
					str<UTF_32, Aˀ> out;

					out.spare(max / sizeof(typename decltype(out)::unit));

					if constexpr (std::endian::native == std::endian::big) write_as_native(ifs, out);
					                                                  else write_as_exotic(ifs, out);

					return {out};
				}
			}
		}
		return std::nullopt;
	}

	typedef str<ASCII> strˀ;
	typedef str<UTF_8> str8;
	typedef str<UTF_16> str16;
	typedef str<UTF_32> str32;

	str(char&) -> str<ASCII>;
	str(char8_t&) -> str<UTF_8>;
	str(char16_t&) -> str<UTF_16>;
	str(char32_t&) -> str<UTF_32>;

	typedef txt<ASCII> txtˀ;
	typedef txt<UTF_8> txt8;
	typedef txt<UTF_16> txt16;
	typedef txt<UTF_32> txt32;

	txt(char&) -> txt<ASCII>;
	txt(char8_t&) -> txt<UTF_8>;
	txt(char16_t&) -> txt<UTF_16>;
	txt(char32_t&) -> txt<UTF_32>;

	template <usize N> str(const char (&)[N]) -> str<ASCII>;
	template <usize N> str(const char8_t (&)[N]) -> str<UTF_8>;
	template <usize N> str(const char16_t (&)[N]) -> str<UTF_16>;
	template <usize N> str(const char32_t (&)[N]) -> str<UTF_32>;

	template <usize N> txt(const char (&)[N]) -> txt<ASCII>;
	template <usize N> txt(const char8_t (&)[N]) -> txt<UTF_8>;
	template <usize N> txt(const char16_t (&)[N]) -> txt<UTF_16>;
	template <usize N> txt(const char32_t (&)[N]) -> txt<UTF_32>;
}

#include "impl/📦.inl"

#include "impl/str.inl"
#include "impl/txt.inl"

#include "impl/🧲/UTF8.inl"
#include "impl/🧲/UTF16.inl"
#include "impl/🧲/UTF32.inl"
#include "impl/🧲/ASCII.inl"

inline auto operator>>(std::istream& is, char32_t code) noexcept -> std::istream&
{
	// TODO
}

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

inline auto operator>>(std::istream& is, x69::codeᵖᵗ code) noexcept -> std::istream&
{
	return is >> static_cast<char32_t>(code);
}
inline auto operator<<(std::ostream& os, x69::codeᵖᵗ code) noexcept -> std::ostream&
{
	return os << static_cast<char32_t>(code);
}

constexpr auto operator ""_str(const char* ptr, x69::usize len) noexcept -> x69::strˀ
{
	return {x69::txtˀ {ptr, ptr + len}};
}

constexpr auto operator ""_txt(const char* ptr, x69::usize len) noexcept -> x69::txtˀ
{
	return {ptr, ptr + len};
}

constexpr auto operator ""_str(const char8_t* ptr, x69::usize len) noexcept -> x69::str8
{
	return {x69::txt8 {ptr, ptr + len}};
}

constexpr auto operator ""_txt(const char8_t* ptr, x69::usize len) noexcept -> x69::txt8
{
	return {ptr, ptr + len};
}

constexpr auto operator ""_str(const char16_t* ptr, x69::usize len) noexcept -> x69::str16
{
	return {x69::txt16 {ptr, ptr + len}};
}

constexpr auto operator ""_txt(const char16_t* ptr, x69::usize len) noexcept -> x69::txt16
{
	return {ptr, ptr + len};
}

constexpr auto operator ""_str(const char32_t* ptr, x69::usize len) noexcept -> x69::str32
{
	return {x69::txt32 {ptr, ptr + len}};
}

constexpr auto operator ""_txt(const char32_t* ptr, x69::usize len) noexcept -> x69::txt32
{
	return {ptr, ptr + len};
}
