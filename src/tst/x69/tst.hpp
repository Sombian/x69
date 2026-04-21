#pragma once

#include <cassert>
#include <cstddef>

#include <bit>
#include <memory>
#include <ranges>
#include <utility>
#include <optional>
#include <functional>
#include <type_traits>

#include "x69/str.hpp"

#ifndef x69_MALLOC //=============//
#define x69_MALLOC std::allocator //
#endif             //=============//

#define COPY_CONSTRUCTOR(T) constexpr T(const T&  other) noexcept
#define MOVE_CONSTRUCTOR(T) constexpr T(/*&*/ T&& other) noexcept

#define COPY_ASSIGNMENT(T) constexpr auto operator=(const T&  other) noexcept -> T&
#define MOVE_ASSIGNMENT(T) constexpr auto operator=(/*&*/ T&& other) noexcept -> T&

namespace x69
{
	template <typename /**/ T,
	          template <class>
	          typename malloc = x69_MALLOC> class tst final
	{
		// whether to store T directly within node_t
		static constexpr bool INLINE {std::invoke([]
		{
			if constexpr (std::is_void_v<T>)
			{
				return (static_cast<bool>(0));
			}
			if constexpr (!std::is_void_v<T>)
			{
				return sizeof(T) <= sizeof(T*);
			}
		})};

		//==========================//=============================//
		typedef std::conditional_t  // choose optimal <T> to store //
		<       //==================//                             //
		        !std::is_void_v<T>, //                             //
		        //==================//=============================//
		        std::conditional_t<INLINE, std::remove_cvref_t<T>, //
		                                   std::remove_cvref_t<T>* //
		>,
		std::monostate> data_t;

		                    class cursor;
		template <typename> class reader;
		template <typename> class writer;

		struct meta_t
		{
			// code-point has threshold of '0x10FFFF' per spec
			char32_t code : std::bit_width(size_t {0x10FFFF});
			// range: 0/1
			char32_t hold : 1 {0};
			// range: 0/1/2/3
			char32_t dirt : 2 {0};
			// range: max 1023
			char32_t drop : 8 {0};

			constexpr  meta_t() noexcept = default;
			constexpr ~meta_t() noexcept = default;
		};

		struct node_t : public meta_t
		{
			[[no_unique_address]] data_t data;

			node_t* to_l {nullptr};
			node_t* to_m {nullptr};
			node_t* to_r {nullptr};

			constexpr  node_t() noexcept = default;
			constexpr ~node_t() noexcept = default;
		};

		[[no_unique_address]] struct
		{
			typedef T data_t;

			[[no_unique_address]] std::conditional_t<INLINE, malloc<node_t>,
			                                                 malloc<node_t>>
			Γnode;

			[[no_unique_address]] std::conditional_t<INLINE, std::monostate,
			                                                 malloc<data_t>>
			Γdata;
		}
		tag;

		node_t* root {nullptr};

		// sanity check; meta_t should fit in char32_t
		static_assert(sizeof(meta_t) == sizeof(char32_t));

	public:

		template <typename K> requires requires (std::ranges::range_value_t<K> _)
		                                        { static_cast<char32_t>(  _  ); }
		constexpr tst(std::initializer_list<std::conditional_t<!std::is_void_v<T>, std::pair<K, T>, K>> args = {}) noexcept;

		constexpr tst(std::initializer_list<std::conditional_t<!std::is_void_v<T>, std::pair<x69::txt8, T>, x69::txt8>> args) noexcept;
		constexpr tst(std::initializer_list<std::conditional_t<!std::is_void_v<T>, std::pair<x69::txt16, T>, x69::txt16>> args) noexcept;
		constexpr tst(std::initializer_list<std::conditional_t<!std::is_void_v<T>, std::pair<x69::txt32, T>, x69::txt32>> args) noexcept;

		// constexpr  tst() noexcept;
		   constexpr ~tst() noexcept;

		COPY_CONSTRUCTOR(tst);
		MOVE_CONSTRUCTOR(tst);

		COPY_ASSIGNMENT(tst);
		MOVE_ASSIGNMENT(tst);

		constexpr auto view() const noexcept -> cursor;

		// constexpr auto begin() const noexcept -> const_iterator;
		// constexpr auto end() const noexcept -> const_iterator;

		// constexpr auto begin() /*&*/ noexcept -> /*&*/ iterator;
		// constexpr auto end() /*&*/ noexcept -> /*&*/ iterator;

		constexpr auto operator&(const tst& rhs) const noexcept -> tst;
		constexpr auto operator|(const tst& rhs) const noexcept -> tst;
		constexpr auto operator+(const tst& rhs) const noexcept -> tst;
		constexpr auto operator-(const tst& rhs) const noexcept -> tst;

		constexpr auto operator&=(const tst& rhs)&  noexcept -> tst& ;
		constexpr auto operator&=(const tst& rhs)&& noexcept -> tst&&;

		constexpr auto operator|=(const tst& rhs)&  noexcept -> tst& ;
		constexpr auto operator|=(const tst& rhs)&& noexcept -> tst&&;

		constexpr auto operator+=(const tst& rhs)&  noexcept -> tst& ;
		constexpr auto operator+=(const tst& rhs)&& noexcept -> tst&&;

		constexpr auto operator-=(const tst& rhs)&  noexcept -> tst& ;
		constexpr auto operator-=(const tst& rhs)&& noexcept -> tst&&;

		template <typename K> requires requires (std::ranges::range_value_t<K> _)
		                                        { static_cast<char32_t>(  _  ); }
		                                        // priority: txt8 > txt16 > txt32
		constexpr auto operator[](K&& rhs) const noexcept -> reader<std::conditional_t<std::is_constructible_v<K, x69::txt8> || std::is_convertible_v<K, x69::txt8>, x69::txt8,
		                                                            std::conditional_t<std::is_constructible_v<K, x69::txt16> || std::is_convertible_v<K, x69::txt16>, x69::txt16,
		                                                            std::conditional_t<std::is_constructible_v<K, x69::txt32> || std::is_convertible_v<K, x69::txt32>, x69::txt32,
		                                                            // fallback to original T
		                                                            std::remove_cvref_t<K>>>>>;

		template <typename K> requires requires (std::ranges::range_value_t<K> _)
		                                        { static_cast<char32_t>(  _  ); }
		                                        // priority: txt8 > txt16 > txt32
		constexpr auto operator[](K&& rhs) /*&*/ noexcept -> writer<std::conditional_t<std::is_constructible_v<x69::txt8, K> || std::is_convertible_v<K, x69::txt8>, x69::txt8,
		                                                            std::conditional_t<std::is_constructible_v<x69::txt16, K> || std::is_convertible_v<K, x69::txt16>, x69::txt16,
		                                                            std::conditional_t<std::is_constructible_v<x69::txt32, K> || std::is_convertible_v<K, x69::txt32>, x69::txt32,
		                                                            // fallback to original T
		                                                            std::remove_cvref_t<K>>>>>;

	private:

		static constexpr auto __patch__(node_t* a) noexcept -> node_t*;

		static constexpr auto __castΔ__(node_t* a) noexcept -> node_t*;
		static constexpr auto __rot2l__(node_t* a) noexcept -> node_t*;
		static constexpr auto __rot2r__(node_t* a) noexcept -> node_t*;

		static constexpr auto __height__(node_t* a) noexcept -> ptrdiff_t;
		static constexpr auto __factor__(node_t* a) noexcept -> ptrdiff_t;

		class cursor
		{
			typedef tst self_t;

			const self_t* src;
			/*&*/ node_t* ptr;

		public:

			constexpr cursor
			(
				decltype(src) src,
				decltype(ptr) ptr
			)
			noexcept : src {src},
			           ptr {ptr}
			{}

			constexpr auto get() const noexcept -> std::optional<data_t>;

			constexpr auto is_root() const noexcept -> bool;
			constexpr auto is_leaf() const noexcept -> bool;

			constexpr auto operator[](char32_t rhs) const noexcept -> bool;
			constexpr auto operator[](char32_t rhs) /*&*/ noexcept -> bool;
		};

		template <typename arg> class reader
		{
			const tst* src;
			/*&*/ arg  key;

		public:

			constexpr reader
			(
				decltype(src) src,
				decltype(key) key
			)
			noexcept : src {src},
			           key {key}
			{}

			   constexpr operator std::optional<data_t>() const noexcept;
			// constexpr operator std::optional<data_t>() /*&*/ noexcept;

			   constexpr operator bool() const noexcept;
			// constexpr operator bool() /*&*/ noexcept;

			// constexpr auto operator=(const T& rhs) noexcept -> writer& requires (!std::is_void_v<T>);

			constexpr auto operator==(const T& rhs) const noexcept -> bool requires (!std::is_void_v<T>);
			constexpr auto operator!=(const T& rhs) const noexcept -> bool requires (!std::is_void_v<T>);
		};

		template <typename arg> class writer
		{
			/*&*/ tst* src;
			/*&*/ arg  key;

		public:

			constexpr writer
			(
				decltype(src) src,
				decltype(key) key
			)
			noexcept : src {src},
			           key {key}
			{}

			   constexpr operator std::optional<data_t>() const noexcept;
			// constexpr operator std::optional<data_t>() /*&*/ noexcept;

			   constexpr operator bool() const noexcept;
			// constexpr operator bool() /*&*/ noexcept;

			constexpr auto operator=(const T& rhs) noexcept -> writer& requires (!std::is_void_v<T>);

			constexpr auto operator==(const T& rhs) const noexcept -> bool requires (!std::is_void_v<T>);
			constexpr auto operator!=(const T& rhs) const noexcept -> bool requires (!std::is_void_v<T>);
		};
	};
}

// NOLINTBEGIN(unused-includes)

#include "./private/tst.inl"

// NOLINTEND(unused-includes)

#undef COPY_ASSIGNMENT
#undef MOVE_ASSIGNMENT

#undef COPY_CONSTRUCTOR
#undef MOVE_CONSTRUCTOR
