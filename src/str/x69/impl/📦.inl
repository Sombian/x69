#pragma once

//───────────────────"
#include "x69/str.hpp"
//───────────────────"

template <typename _, typename F> constexpr auto x69::API::cstr<_, F>::_head() const noexcept -> const unit*
{
	/**/ if constexpr (!requires { decltype(&cstr::_head) /* fix; infi. recursion */ {&_::_head}; })
	{
		return static_cast<const _&>(*this)._head();
	}
	else if constexpr (requires (const _& obj) { { obj.head } -> std::convertible_to<const unit*>; })
	{
		return static_cast<const _&>(*this).head;
	}
	else static_assert(false, "cannot get head ptr; is it up-to-date?");
}

template <typename _, typename F> constexpr auto x69::API::cstr<_, F>::_tail() const noexcept -> const unit*
{
	/**/ if constexpr (!requires { decltype(&cstr::_tail) /* fix; infi. recursion */ {&_::_tail}; })
	{
		return static_cast<const _&>(*this)._tail();
	}
	else if constexpr (requires (const _& obj) { { obj.tail } -> std::convertible_to<const unit*>; })
	{
		return static_cast<const _&>(*this).tail;
	}
	else static_assert(false, "cannot get tail ptr; is it up-to-date?");
}

template <typename _, typename F> constexpr auto x69::API::cstr<_, F>::size() const noexcept -> usize
{
	return detail::ΔCU<F>(_head(), _tail());
}

template <typename _, typename F> constexpr auto x69::API::cstr<_, F>::length() const noexcept -> usize
{
	return detail::ΔCP<F>(_head(), _tail());
}

template <typename _, typename F> constexpr auto x69::API::cstr<_, F>::starts_w(auto&& inˀ) const noexcept -> bool requires requires { txt {inˀ}; }
{
	return [this]<typename Fˀ>(txt<Fˀ> inˀ)
	{
		return detail::swith<F, Fˀ>(    _head(),     _tail(),
		                            inˀ._head(), inˀ._tail());
	}
	(txt {inˀ});
}

template <typename _, typename F> constexpr auto x69::API::cstr<_, F>::contains(auto&& inˀ) const noexcept -> bool requires requires { txt {inˀ}; }
{
	return [this]<typename Fˀ>(txt<Fˀ> inˀ)
	{
		return detail::holds<F, Fˀ>(    _head(),     _tail(),
		                            inˀ._head(), inˀ._tail());
	}
	(txt {inˀ});
}

template <typename _, typename F> constexpr auto x69::API::cstr<_, F>::ends_w(auto&& inˀ) const noexcept -> bool requires requires { txt {inˀ}; }
{
	return [this]<typename Fˀ>(txt<Fˀ> inˀ)
	{
		return detail::ewith<F, Fˀ>(    _head(),     _tail(),
		                            inˀ._head(), inˀ._tail());
	}
	(txt {inˀ});
}

template <typename _, typename F> constexpr auto x69::API::cstr<_, F>::substr(clamp start, clamp until) const noexcept -> txt<F>
{
	return detail::substr<F>(_head(), _tail(), start, until);
}

template <typename _, typename F> constexpr auto x69::API::cstr<_, F>::substr(clamp start, magic until) const noexcept -> txt<F>
{
	return detail::substr<F>(_head(), _tail(), start, until);
}

template <typename _, typename F> constexpr auto x69::API::cstr<_, F>::substr(usize start, clamp until) const noexcept -> txt<F>
{
	return detail::substr<F>(_head(), _tail(), start, until);
}

template <typename _, typename F> constexpr auto x69::API::cstr<_, F>::substr(usize start, magic until) const noexcept -> txt<F>
{
	return detail::substr<F>(_head(), _tail(), start, until);
}

template <typename _, typename F> constexpr auto x69::API::cstr<_, F>::substr(usize start, usize until) const noexcept -> txt<F>
{
	return detail::substr<F>(_head(), _tail(), start, until);
}

template <typename _, typename F> constexpr auto x69::API::cstr<_, F>::begin() const noexcept -> cursor<'L'>
{
	return {_head()};
}

template <typename _, typename F> constexpr auto x69::API::cstr<_, F>::end() const noexcept -> cursor<'L'>
{
	return {_tail()};
}

template <typename _, typename F> constexpr auto x69::API::cstr<_, F>::rbegin() const noexcept -> cursor<'R'>
{
	return {_tail()};
}

template <typename _, typename F> constexpr auto x69::API::cstr<_, F>::rend() const noexcept -> cursor<'R'>
{
	return {_head()};
}

template <typename _, typename F> template <x69::usize N> constexpr auto x69::API::cstr<_, F>::it() const noexcept -> std::tuple<window<N>, usize>
{
	return {{_head(), _tail()}, 0};
}

template <typename _, typename F> template <x69::usize N> constexpr auto x69::API::cstr<_, F>::it() /*&*/ noexcept -> std::tuple<window<N>, usize>
{
	return {{_head(), _tail()}, 0};
}

template <typename _, typename F> constexpr auto x69::API::cstr<_, F>::operator[](usize rhs) const noexcept -> handle<'L'>
{
	return {detail::substr<F>(_head(), _tail(), rhs)};
}

template <typename _, typename F> constexpr auto x69::API::cstr<_, F>::operator[](magic rhs) const noexcept -> handle<'R'>
{
	return {detail::substr<F>(_head(), _tail(), rhs)};
}

template <typename _, typename F> constexpr auto x69::API::cstr<_, F>::operator[](clamp rhs) const noexcept -> handle<'R'>
{
	return {detail::substr<F>(_head(), _tail(), rhs)};
}

template <typename _, typename F> constexpr auto x69::API::cstr<_, F>::split(auto&& inˀ) const noexcept -> std::vector<txt<F>> requires requires { txt {inˀ}; }
{
	return [this]<typename Fˀ>(txt<Fˀ> inˀ)
	{
		return detail::split<F, Fˀ>(    _head(),     _tail(),
		                            inˀ._head(), inˀ._tail());
	}
	(txt {inˀ});
}

template <typename _, typename F> constexpr auto x69::API::cstr<_, F>::match(auto&& inˀ) const noexcept -> std::vector<txt<F>> requires requires { txt {inˀ}; }
{
	return [this]<typename Fˀ>(txt<Fˀ> inˀ)
	{
		return detail::match<F, Fˀ>(    _head(),     _tail(),
		                            inˀ._head(), inˀ._tail());
	}
	(txt {inˀ});
}

template <typename _, typename F> constexpr auto x69::API::cstr<_, F>::operator==(auto&& rhs) const noexcept -> bool requires requires { txt {rhs}; }
{
	return [this]<typename Fˀ>(txt<Fˀ> inˀ)
	{
		return detail::equal<F, Fˀ>(    _head(),     _tail(),
		                            inˀ._head(), inˀ._tail());
	}
	(txt {rhs});
}

template <typename _, typename F> constexpr auto x69::API::cstr<_, F>::operator!=(auto&& rhs) const noexcept -> bool requires requires { txt {rhs}; }
{
	return [this]<typename Fˀ>(txt<Fˀ> inˀ)
	{
		return !detail::equal<F, Fˀ>(    _head(),     _tail(),
		                             inˀ._head(), inˀ._tail());
	}
	(txt {rhs});
}

#pragma    region ::handle

template <typename _, typename F> template <char S> constexpr x69::API::cstr<_, F>::handle<S>::operator x69::codeᵖᵗ() const noexcept // getter..?
{
	if constexpr (S == 'L')
	{
		if (ptr == nullptr)
		{
			return '\0';
		}

		codeᵖᵗ out;

		auto dir {F::next(ptr)};
		F::decode(ptr, out, dir);

		return out;
	}
	if constexpr (S == 'R')
	{
		if (ptr == nullptr)
		{
			return '\0';
		}

		codeᵖᵗ out;

		auto dir {F::back(ptr)};
		F::decode(ptr, out, dir);

		return out;
	}
}

template <typename _, typename F> template <char S> constexpr auto x69::API::cstr<_, F>::handle<S>::operator==(codeᵖᵗ rhs) const noexcept -> bool
{
	if constexpr (S == 'L')
	{
		if (ptr == nullptr)
		{
			return false;
		}

		codeᵖᵗ out;

		auto dir {F::next(ptr)};
		F::decode(ptr, out, dir);

		return out == rhs;
	}
	if constexpr (S == 'R')
	{
		if (ptr == nullptr)
		{
			return false;
		}

		codeᵖᵗ out;

		auto dir {F::back(ptr)};
		F::decode(ptr, out, dir);

		return out == rhs;
	}
}

template <typename _, typename F> template <char S> constexpr auto x69::API::cstr<_, F>::handle<S>::operator!=(codeᵖᵗ rhs) const noexcept -> bool
{
	if constexpr (S == 'L')
	{
		if (ptr == nullptr)
		{
			return false;
		}

		codeᵖᵗ out;

		auto dir {F::next(ptr)};
		F::decode(ptr, out, dir);

		return out != rhs;
	}
	if constexpr (S == 'R')
	{
		if (ptr == nullptr)
		{
			return false;
		}

		codeᵖᵗ out;

		auto dir {F::back(ptr)};
		F::decode(ptr, out, dir);

		return out != rhs;
	}
}

#pragma endregion ::handle
#pragma    region ::cursor

template <typename _, typename F> template <char S> constexpr auto x69::API::cstr<_, F>::cursor<S>::operator*() const noexcept -> value_type
{
	if constexpr (S == 'L')
	{
		if (ptr == nullptr)
		{
			return '\0';
		}

		codeᵖᵗ out;

		auto dir {F::next(ptr)};
		F::decode(ptr, out, dir);

		return out;
	}
	if constexpr (S == 'R')
	{
		if (ptr == nullptr)
		{
			return '\0';
		}

		codeᵖᵗ out;

		auto dir {F::back(ptr)};
		F::decode(ptr, out, dir);

		return out;
	}
}

template <typename _, typename F> template <char S> constexpr auto x69::API::cstr<_, F>::cursor<S>::operator++(   ) noexcept -> cursor&
{
	if constexpr (S == 'L')
	{
		//    ┌────────────┐
		//    │ 0 < stride │
		//    └────[↓↓]────┘
		ptr += F::next(ptr);

		return *this;
	}
	if constexpr (S == 'R')
	{
		//    ┌────────────┐
		//    │ stride < 0 │
		//    └────[↓↓]────┘
		ptr += F::back(ptr);

		return *this;
	}
}

template <typename _, typename F> template <char S> constexpr auto x69::API::cstr<_, F>::cursor<S>::operator--(   ) noexcept -> cursor&
{
	if constexpr (S == 'L')
	{
		//    ┌────────────┐
		//    │ stride < 0 │
		//    └────[↓↓]────┘
		ptr += F::back(ptr);

		return *this;
	}
	if constexpr (S == 'R')
	{
		//    ┌────────────┐
		//    │ 0 < stride │
		//    └────[↓↓]────┘
		ptr += F::next(ptr);

		return *this;
	}
}

template <typename _, typename F> template <char S> constexpr auto x69::API::cstr<_, F>::cursor<S>::operator++(int) noexcept -> cursor
{
	const auto copy {*this};
	              ++(*this);

	return copy;
}

template <typename _, typename F> template <char S> constexpr auto x69::API::cstr<_, F>::cursor<S>::operator--(int) noexcept -> cursor
{
	const auto copy {*this};
	              --(*this);

	return copy;
}

template <typename _, typename F> template <char S> constexpr auto x69::API::cstr<_, F>::cursor<S>::operator==(const cursor& rhs) const noexcept -> bool
{
	return ptr == rhs.ptr;
}

template <typename _, typename F> template <char S> constexpr auto x69::API::cstr<_, F>::cursor<S>::operator!=(const cursor& rhs) const noexcept -> bool
{
	return ptr != rhs.ptr;
}

#pragma endregion ::cursor
#pragma    region ::window

template <typename _, typename F> template <x69::usize N> constexpr auto x69::API::cstr<_, F>::window<N>::operator[]([[maybe_unused]] usize rhs) noexcept -> handle<'L'>
{
	if (rhs == 0) [[unlikely]]
	{
		return {beg}; // O(1)
	}

	const auto fmt {_ranks()};

	const unit* ptr       ;
	usize       top {~0UZ};
	usize       NTH {~0UZ};

	const auto MRU {fmt[N - N]};
	const auto LRU {fmt[N - 1]};

	if (logs[MRU].key)
	{
		for (const auto i : fmt)
		{
			const auto& key {logs[i].key};
			const auto& off {logs[i].off};

			if (0 < off && (key < rhs ? static_cast<usize>(rhs - key)
			                          : static_cast<usize>(key - rhs))
			                          <
			               (top < rhs ? static_cast<usize>(rhs - top)
			                          : static_cast<usize>(top - rhs)))
			{
				ptr = beg + off;
				top = key      ;
				NTH = (i)      ;

				if (top == rhs)
				{
					// write := {ROW}
					map |= ROWᴺ[NTH];
					// clear := {COL}
					map &= COLᴺ[NTH];

					return {ptr};
				}
			}
		}

		assert(top != ~0UZ);
		assert(NTH != ~0UZ);

		/**/ if (rhs < top)
		{
			// ┌───────┬───────┬───────┐
			// │ <...> │ <...> │ <...> │
			// ↑───────↑───────↑───────↑
			// beg  →  rhs  ←  top     end

			const auto b2r {      rhs};
			const auto r2t {top - rhs};

			/**/ if (b2r == r2t)
			{
				ptr = beg; // reset

				for (usize i {0}; i < b2r; ++i)
				{
					ptr += F::next(ptr);
				}
			}
			else if (b2r < r2t)
			{
				ptr = beg; // reset

				for (usize i {0}; i < b2r; ++i)
				{
					ptr += F::next(ptr);
				}
			}
			else if (b2r > r2t)
			{
				// ptr = beg; // reset

				for (usize i {0}; i < r2t; ++i)
				{
					ptr += F::back(ptr);
				}
			}
		}
		else if (top < rhs)
		{
			// ┌───────┬───────┬───────┐
			// │ <...> │ <...> │ <...> │
			// ↑───────↑───────↑───────↑
			// beg     top  →  rhs     end

			// const auto b2t {      top};
			   const auto t2r {rhs - top};

			for (usize i {0}; i < t2r; ++i)
			{
				if (ptr == end) [[unlikely]]
				{
					logs[LRU] = log
					{
						.key {rhs},
						.off {(0)},
					};
					// write := {ROW}
					map |= ROWᴺ[LRU];
					// clear := {COL}
					map &= COLᴺ[LRU];

					return {nullptr};
				}
				ptr += F::next(ptr);
			}
		}
	}
	else [[unlikely]]
	{
		// ┌───────┬───────┐
		// │ <...> │ <...> │
		// ↑───────↑───────↑
		// beg  →  rhs     end

		   const auto b2r {      rhs};
		// const auto ??? {top - rhs};

		ptr = beg; // reset

		for (usize i {0}; i < b2r; ++i)
		{
			if (ptr == end) [[unlikely]]
			{
				logs[LRU] = log
				{
					.key {rhs},
					.off {(0)},
				};
				// write := {ROW}
				map |= ROWᴺ[LRU];
				// clear := {COL}
				map &= COLᴺ[LRU];

				return {nullptr};
			}
			ptr += F::next(ptr);
		}
	}

	logs[LRU] = log
	{
		.key {      rhs},
		.off {ptr - beg},
	};
	// write := {ROW}
	map |= ROWᴺ[LRU];
	// clear := {COL}
	map &= COLᴺ[LRU];

	return {ptr};
}

template <typename _, typename F> template <x69::usize N> constexpr auto x69::API::cstr<_, F>::window<N>::operator[]([[maybe_unused]] magic rhs) noexcept -> handle<'R'>
{
	return {end};
}

template <typename _, typename F> template <x69::usize N> constexpr auto x69::API::cstr<_, F>::window<N>::operator[]([[maybe_unused]] clamp rhs) noexcept -> handle<'R'>
{
	if (rhs == 0) [[unlikely]]
	{
		return {end}; // O(1)
	}

	const auto fmt {_ranks()};

	const unit* ptr       ;
	usize       top {~0UZ};
	usize       NTH {~0UZ};

	const auto MRU {fmt[N - N]};
	const auto LRU {fmt[N - 1]};

	if (logs[MRU].key)
	{
		for (const auto i : fmt)
		{
			const auto& key {logs[i].key};
			const auto& off {logs[i].off};

			if (off < 0 && (key < rhs ? static_cast<usize>(rhs - key)
			                          : static_cast<usize>(key - rhs))
			                          <
			               (top < rhs ? static_cast<usize>(rhs - top)
			                          : static_cast<usize>(top - rhs)))
			{
				ptr = end + off;
				top = key      ;
				NTH = (i)      ;

				if (top == rhs)
				{
					// write := {ROW}
					map |= ROWᴺ[NTH];
					// clear := {COL}
					map &= COLᴺ[NTH];

					return {ptr};
				}
			}
		}

		assert(top != ~0UZ);
		assert(NTH != ~0UZ);

		/**/ if (rhs < top)
		{
			// ┌───────┬───────┬───────┐
			// │ <...> │ <...> │ <...> │
			// ↑───────↑───────↑───────↑
			// beg     top  →  rhs  ←  end

			const auto t2r {top - rhs};
			const auto r2e {      rhs};

			/**/ if (t2r == r2e)
			{
				// ptr = end; // reset

				for (usize i {0}; i < t2r; ++i)
				{
					ptr += F::next(ptr);
				}
			}
			else if (t2r < r2e)
			{
				// ptr = end; // reset

				for (usize i {0}; i < t2r; ++i)
				{
					ptr += F::next(ptr);
				}
			}
			else if (t2r > r2e)
			{
				ptr = end; // reset

				for (usize i {0}; i < r2e; ++i)
				{
					ptr += F::back(ptr);
				}
			}
		}
		else if (top < rhs)
		{
			// ┌───────┬───────┬───────┐
			// │ <...> │ <...> │ <...> │
			// ↑───────↑───────↑───────↑
			// beg     rhs  ←  top     end

			   const auto r2t {rhs - top};
			// const auto t2e {      top};

			for (usize i {0}; i < r2t; ++i)
			{
				if (ptr == beg) [[unlikely]]
				{
					logs[LRU] = log
					{
						.key {rhs},
						.off {(0)},
					};
					// write := {ROW}
					map |= ROWᴺ[LRU];
					// clear := {COL}
					map &= COLᴺ[LRU];

					return {nullptr};
				}
				ptr += F::next(ptr);
			}
		}
	}
	else [[unlikely]]
	{
		// ┌───────┬───────┐
		// │ <...> │ <...> │
		// ↑───────↑───────↑
		// beg     rhs  ←  end

		   const auto r2e {top - rhs};
		// const auto ??? {      rhs};

		ptr = end; // reset

		for (usize i {0}; i < r2e; ++i)
		{
			if (ptr == end) [[unlikely]]
			{
				logs[LRU] = log
				{
					.key {rhs},
					.off {(0)},
				};
				// write := {ROW}
				map |= ROWᴺ[LRU];
				// clear := {COL}
				map &= COLᴺ[LRU];

				return {nullptr};
			}
			ptr += F::next(ptr);
		}
	}

	logs[LRU] = log
	{
		.key {      rhs},
		.off {end - ptr},
	};
	// write := {ROW}
	map |= ROWᴺ[LRU];
	// clear := {COL}
	map &= COLᴺ[LRU];

	return {ptr};
}

template <typename _, typename F> template <x69::usize N> constexpr auto x69::API::cstr<_, F>::window<N>::_ranks() const noexcept -> std::array<u8, N>
{
	constexpr usize MSK {(WORD {1} << N) - 1};

	[[maybe_unused]] std::array<u8, N> out {};

	for (usize i {0}; i < N; ++i)
	{
		const auto row {map >> (N * i) & MSK};
		// mapping := hamming weight → rank
		out[N - std::popcount(row) - 1] = i;
	}

	return out;
}

#pragma endregion ::window
#pragma    region ::concat

template <typename _, typename F> template <typename... Tᴺ> template <typename Fˀ,
                                                                      typeᵐᵉᵐˀ Aˀ> constexpr x69::API::cstr<_, F>::concat<Tᴺ...>::operator x69::str<Fˀ, Aˀ>() const noexcept
{
	str<Fˀ, Aˀ> out;

	usize size {0};

	std::apply([&](auto&&... args)
	{
		([&]<typename Fˣ>(txt<Fˣ> arg)
		{
			const auto* head {arg._head()};
			const auto* tail {arg._tail()};

			size += detail::ΔCU<Fˀ, Fˣ>(head,
			                            tail);
		}
		(txt {args}), ...);
	},
	args);

	// just enough fit
	out.spare(size);
	out._SSOᴺ(size);

	typename Fˀ::codeᵘᵗ* dest {out._head()};

	std::apply([&](auto&&... args)
	{
		([&]<typename Fˣ>(txt<Fˣ> arg)
		{
			const auto* head {arg._head()};
			const auto* tail {arg._tail()};

			dest += detail::copy<Fˀ, Fˣ>(dest, head,
			                                   tail);
		}
		(txt {args}), ...);
	},
	args);

	return out;
}

template <typename _, typename F> template <typename... Tᴺ> constexpr auto x69::API::cstr<_, F>::concat<Tᴺ...>::begin() const noexcept -> cursor<'L'>
{
	const txt str {std::get<sizeof...(Tᴺ) * 0>(args)};

	return {{this, 0, static_cast<const void*>(str._head() - 0),
	                  static_cast<const void*>(str._tail() - 1),
	                  static_cast<const void*>(str._head() - 0)}};
}

template <typename _, typename F> template <typename... Tᴺ> constexpr auto x69::API::cstr<_, F>::concat<Tᴺ...>::end() const noexcept -> cursor<'L'>
{
	const txt str {std::get<sizeof...(Tᴺ) - 1>(args)};

	return {{this, sizeof...(Tᴺ), static_cast<const void*>(str._head() - 0),
	                              static_cast<const void*>(str._tail() - 1),
	                              static_cast<const void*>(str._tail() - 1)}};
}

template <typename _, typename F> template <typename... Tᴺ> constexpr auto x69::API::cstr<_, F>::concat<Tᴺ...>::rbegin() const noexcept -> cursor<'R'>
{
	const txt str {std::get<sizeof...(Tᴺ) - 1>(args)};

	return {{this, 0, static_cast<const void*>(str._head() - 0),
	                  static_cast<const void*>(str._tail() - 1),
	                  static_cast<const void*>(str._tail() - 1)}};
}

template <typename _, typename F> template <typename... Tᴺ> constexpr auto x69::API::cstr<_, F>::concat<Tᴺ...>::rend() const noexcept -> cursor<'R'>
{
	const txt str {std::get<sizeof...(Tᴺ) * 0>(args)};

	return {{this, sizeof...(Tᴺ), static_cast<const void*>(str._head() - 0),
	                              static_cast<const void*>(str._tail() - 1),
	                              static_cast<const void*>(str._head() - 0)}};
}

template <typename _, typename F> template <typename... Tᴺ> constexpr auto x69::API::cstr<_, F>::concat<Tᴺ...>::operator[](usize rhs) const noexcept -> handle<'L'>
{
	const txt str {std::get<sizeof...(Tᴺ) * 0>(args)};

	needle<'L'> ptr {this, 0, static_cast<const void*>(str._head() - 0),
	                          static_cast<const void*>(str._tail() - 1),
	                          static_cast<const void*>(str._head() - 0)};

	for (usize i {0}; i < rhs; ++i) ++ptr;

	return {ptr};
}

template <typename _, typename F> template <typename... Tᴺ> constexpr auto x69::API::cstr<_, F>::concat<Tᴺ...>::operator[](magic rhs) const noexcept -> handle<'R'>
{
	const txt str {std::get<sizeof...(Tᴺ) - 1>(args)};

	needle<'R'> ptr {this, sizeof...(Tᴺ), static_cast<const void*>(str._head() - 0),
	                                      static_cast<const void*>(str._tail() - 1),
	                                      static_cast<const void*>(str._tail() - 1)};

	// for (usize i {0}; i < rhs; ++i) ++ptr;

	return {ptr};
}

template <typename _, typename F> template <typename... Tᴺ> constexpr auto x69::API::cstr<_, F>::concat<Tᴺ...>::operator[](clamp rhs) const noexcept -> handle<'R'>
{
	const txt str {std::get<sizeof...(Tᴺ) - 1>(args)};

	needle<'R'> ptr {this, sizeof...(Tᴺ), static_cast<const void*>(str._head() - 0),
	                                      static_cast<const void*>(str._tail() - 1),
	                                      static_cast<const void*>(str._tail() - 1)};

	for (usize i {0}; i < rhs; ++i) ++ptr;

	return {ptr};
}

template <typename _, typename F> template <typename... Tᴺ> template <char S> constexpr auto x69::API::cstr<_, F>::concat<Tᴺ...>::needle<S>::operator*() const noexcept -> codeᵖᵗ
{
	codeᵖᵗ out;

	[&]<usize... I>(std::index_sequence<I...>)
	{
		(
			(ctx == I && [&]<typename Fˀ>(txt<Fˀ> elmt)
			{
				typedef typename Fˀ::codeᵘᵗ unitˀ;

				if constexpr (S == 'L')
				{
					auto dir {Fˀ::next(static_cast<const unitˀ*>(ptr))};
					Fˀ::decode(static_cast<const unitˀ*>(ptr), out, dir);
				}
				if constexpr (S == 'R')
				{
					auto dir {Fˀ::back(static_cast<const unitˀ*>(ptr))};
					Fˀ::decode(static_cast<const unitˀ*>(ptr), out, dir);
				}
				return true; // T && T
			}
			(txt {std::get<I>(src->args)})
		)
		|| ...); // short-circuit chain reaction
	}
	(std::make_index_sequence<sizeof...(Tᴺ)>{});

	return out;
}

template <typename _, typename F> template <typename... Tᴺ> template <char S> constexpr auto x69::API::cstr<_, F>::concat<Tᴺ...>::needle<S>::operator++(   ) noexcept -> needle&
{
	if ([this] -> bool { if constexpr (S == 'L') { return ptr < end; }
	                     if constexpr (S == 'R') { return beg < ptr; } }
	())
	{
		[&]<usize... I>(std::index_sequence<I...>)
		{
			(
				(ctx == I && [&]<typename Fˀ>(txt<Fˀ> elmt)
				{
					typedef typename Fˀ::codeᵘᵗ unitˀ;

					if constexpr (S == 'L')
					{
						const auto dir {Fˀ::next(static_cast<const unitˀ*>(ptr))};
						ptr = static_cast<const char*>(ptr) + sizeof(unitˀ) * dir;
					}
					if constexpr (S == 'R')
					{
						const auto dir {Fˀ::back(static_cast<const unitˀ*>(ptr))};
						ptr = static_cast<const char*>(ptr) + sizeof(unitˀ) * dir;
					}
					return true; // T && T
				}
				(txt {std::get<I>(src->args)})
			)
			|| ...); // short-circuit chain reaction
		}
		(std::make_index_sequence<sizeof...(Tᴺ)>{});
	}
	else [[unlikely]] // wrap..!
	{
		++ctx;

		[&]<usize... I>(std::index_sequence<I...>)
		{
			(
				(ctx == I && [&]<typename Fˀ>(txt<Fˀ> elmt)
				{
					typedef typename Fˀ::codeᵘᵗ unitˀ;

					if constexpr (S == 'L')
					{
						ptr = beg = elmt._head() - 0;
						      end = elmt._tail() - 1;
					}
					if constexpr (S == 'R')
					{
						      beg = elmt._head() - 0;
						ptr = end = elmt._tail() - 1;
					}
					return true; // T && T
				}
				(txt {std::get<I>(src->args)})
			)
			|| ...); // short-circuit chain reaction
		}
		(std::make_index_sequence<sizeof...(Tᴺ)>{});
	}

	return *this;
}

template <typename _, typename F> template <typename... Tᴺ> template <char S> constexpr auto x69::API::cstr<_, F>::concat<Tᴺ...>::needle<S>::operator--(   ) noexcept -> needle&
{
	if ([this] -> bool { if constexpr (S == 'L') { return beg < ptr; }
	                     if constexpr (S == 'R') { return ptr < end; } }
	())
	{
		[&]<usize... I>(std::index_sequence<I...>)
		{
			(
				(ctx == I && [&]<typename Fˀ>(txt<Fˀ> elmt)
				{
					typedef typename Fˀ::codeᵘᵗ unitˀ;

					if constexpr (S == 'L')
					{
						const auto dir {Fˀ::back(static_cast<const unitˀ*>(ptr))};
						ptr = static_cast<const char*>(ptr) + sizeof(unitˀ) * dir;
					}
					if constexpr (S == 'R')
					{
						const auto dir {Fˀ::next(static_cast<const unitˀ*>(ptr))};
						ptr = static_cast<const char*>(ptr) + sizeof(unitˀ) * dir;
					}
					return true; // T && T
				}
				(txt {std::get<I>(src->args)})
			)
			|| ...); // short-circuit chain reaction
		}
		(std::make_index_sequence<sizeof...(Tᴺ)>{});
	}
	else [[unlikely]] // wrap..!
	{
		--ctx;

		[&]<usize... I>(std::index_sequence<I...>)
		{
			(
				(ctx == I && [&]<typename Fˀ>(txt<Fˀ> elmt)
				{
					typedef typename Fˀ::codeᵘᵗ unitˀ;

					if constexpr (S == 'L')
					{
						      beg = elmt._head() - 0;
						ptr = end = elmt._tail() - 1;
					}
					if constexpr (S == 'R')
					{
						ptr = beg = elmt._head() - 0;
						      end = elmt._tail() - 1;
					}
					return true; // T && T
				}
				(txt {std::get<I>(src->args)})
			)
			|| ...); // short-circuit chain reaction
		}
		(std::make_index_sequence<sizeof...(Tᴺ)>{});
	}

	return *this;
}

template <typename _, typename F> template <typename... Tᴺ> template <char S> constexpr auto x69::API::cstr<_, F>::concat<Tᴺ...>::needle<S>::operator++(int) noexcept -> needle
{
	const auto copy {*this};
	              ++(*this);

	return copy;
}

template <typename _, typename F> template <typename... Tᴺ> template <char S> constexpr auto x69::API::cstr<_, F>::concat<Tᴺ...>::needle<S>::operator--(int) noexcept -> needle
{
	const auto copy {*this};
	              ++(*this);

	return copy;
}

template <typename _, typename F> template <typename... Tᴺ> template <char S> constexpr auto x69::API::cstr<_, F>::concat<Tᴺ...>::needle<S>::operator==(const needle& rhs) const noexcept -> bool
{
	return src == rhs.src
	       &&
	       ctx == rhs.ctx
	       &&
	       ptr == rhs.ptr;
}

template <typename _, typename F> template <typename... Tᴺ> template <char S> constexpr auto x69::API::cstr<_, F>::concat<Tᴺ...>::needle<S>::operator!=(const needle& rhs) const noexcept -> bool
{
	return src != rhs.src
	       ||
	       ctx != rhs.ctx
	       ||
	       ptr != rhs.ptr;
}

template <typename _, typename F> template <typename... Tᴺ> template <char S> constexpr x69::API::cstr<_, F>::concat<Tᴺ...>::handle<S>::operator x69::codeᵖᵗ() const noexcept // getter..?
{
	return *ptr;
}

template <typename _, typename F> template <typename... Tᴺ> template <char S> constexpr auto x69::API::cstr<_, F>::concat<Tᴺ...>::handle<S>::operator==(codeᵖᵗ rhs) const noexcept -> bool
{
	return *ptr == rhs;
}

template <typename _, typename F> template <typename... Tᴺ> template <char S> constexpr auto x69::API::cstr<_, F>::concat<Tᴺ...>::handle<S>::operator!=(codeᵖᵗ rhs) const noexcept -> bool
{
	return *ptr != rhs;
}

template <typename _, typename F> template <typename... Tᴺ> template <char S> constexpr auto x69::API::cstr<_, F>::concat<Tᴺ...>::cursor<S>::operator*() const noexcept -> value_type
{
	return *ptr;
}

template <typename _, typename F> template <typename... Tᴺ> template <char S> constexpr auto x69::API::cstr<_, F>::concat<Tᴺ...>::cursor<S>::operator++(   ) noexcept -> cursor&
{
	       ++ptr;
	return *this;
}

template <typename _, typename F> template <typename... Tᴺ> template <char S> constexpr auto x69::API::cstr<_, F>::concat<Tᴺ...>::cursor<S>::operator--(   ) noexcept -> cursor&
{
	       --ptr;
	return *this;
}

template <typename _, typename F> template <typename... Tᴺ> template <char S> constexpr auto x69::API::cstr<_, F>::concat<Tᴺ...>::cursor<S>::operator++(int) noexcept -> cursor
{
	const auto copy {*this};
	              ++(*this);

	return copy;
}

template <typename _, typename F> template <typename... Tᴺ> template <char S> constexpr auto x69::API::cstr<_, F>::concat<Tᴺ...>::cursor<S>::operator--(int) noexcept -> cursor
{
	const auto copy {*this};
	              --(*this);

	return copy;
}

template <typename _, typename F> template <typename... Tᴺ> template <char S> constexpr auto x69::API::cstr<_, F>::concat<Tᴺ...>::cursor<S>::operator==(const cursor& rhs) const noexcept -> bool
{
	return ptr == rhs.ptr;
}

template <typename _, typename F> template <typename... Tᴺ> template <char S> constexpr auto x69::API::cstr<_, F>::concat<Tᴺ...>::cursor<S>::operator!=(const cursor& rhs) const noexcept -> bool
{
	return ptr != rhs.ptr;
}

#pragma endregion ::concat
#pragma    region ::detail

template <typename Fᵒᵘᵗ> constexpr auto x69::detail::ΔCU(const typename Fᵒᵘᵗ::codeᵘᵗ* head,
                                                         const typename Fᵒᵘᵗ::codeᵘᵗ* tail) noexcept -> usize
{
	   typedef typename Fᵒᵘᵗ::codeᵘᵗ unit;
	// typedef typename Fˢʳᶜ::codeᵘᵗ unit;

	return tail - head;
}

template <typename Fᵒᵘᵗ> constexpr auto x69::detail::ΔCP(const typename Fᵒᵘᵗ::codeᵘᵗ* head,
                                                         const typename Fᵒᵘᵗ::codeᵘᵗ* tail) noexcept -> usize
{
	   typedef typename Fᵒᵘᵗ::codeᵘᵗ unit;
	// typedef typename Fˢʳᶜ::codeᵘᵗ unit;

	if constexpr (!Fᵒᵘᵗ::is_variable
	              &&
	              !Fᵒᵘᵗ::is_stateful)
	{
		return tail - head;
	}

	// typename Fˢʳᶜ::stride dir;
	   typename Fᵒᵘᵗ::stride dir;

	usize out {0};

	for (const unit* ptr {head}; ptr < tail; ptr += dir,
	                                         out += (1))
	{
		dir = Fᵒᵘᵗ::next(ptr);
	}
	return out;
}

template <typename Fᵒᵘᵗ,
          typename Fˢʳᶜ> constexpr auto x69::detail::ΔCU(const typename Fˢʳᶜ::codeᵘᵗ* head,
                                                         const typename Fˢʳᶜ::codeᵘᵗ* tail) noexcept -> usize
{
	typedef typename Fᵒᵘᵗ::codeᵘᵗ unitᵒ;
	typedef typename Fˢʳᶜ::codeᵘᵗ unitˢ;

	if constexpr (!Fˢʳᶜ::is_variable
	              &&
	              !Fˢʳᶜ::is_stateful
	              &&
	              !Fᵒᵘᵗ::is_variable
	              &&
	              !Fᵒᵘᵗ::is_stateful)
	{
		return tail - head;
	}

	typename Fˢʳᶜ::stride dirˢ;
	typename Fᵒᵘᵗ::stride dirᵒ;

	usize out {0};

	for (const unitˢ* ptr {head}; ptr < tail; ptr += dirˢ,
	                                          out += dirᵒ)
	{
		codeᵖᵗ tmp;

		/*&*/ dirˢ = Fˢʳᶜ::next(ptr);
		Fˢʳᶜ::decode(ptr, tmp, dirˢ);
		/*&*/ dirᵒ = Fᵒᵘᵗ::size(tmp);
	}
	return out;
}

template <typename Fᵒᵘᵗ,
          typename Fˢʳᶜ> constexpr auto x69::detail::copy(typename Fᵒᵘᵗ::codeᵘᵗ* dest, const typename Fˢʳᶜ::codeᵘᵗ* head,
                                                                                       const typename Fˢʳᶜ::codeᵘᵗ* tail) noexcept -> usize
{
	if constexpr (std::is_same_v<Fᵒᵘᵗ, Fˢʳᶜ>)
	{
		   typedef typename Fᵒᵘᵗ::codeᵘᵗ unit;
		// typedef typename Fˢʳᶜ::codeᵘᵗ unit;

		unit* last {dest + (tail - head)};

		/**/ if (dest == head)
		{
			const auto N {tail - head};

			return N; // delta {0...N}
		}
		else if (dest < head)
		{
			//          head         tail
			//          ↓            ↓
			//┌───┬─────┬────────────┬───┐
			//│ ? │ gap │ [[buffer]] │ ? │
			//├───┼─────┴──────┬─────┼───┤
			//│ ? │ [[buffer]] │ gap │ ? │
			//└───┴────────────┴─────┴───┘
			//    ↑            ↑
			//    dest         last

			const auto N {tail - head};

			for (usize i {0}; i < N; ++i)
			{
				*(dest+i+0) = *(head+i+0);
			}
			return N; // delta {0...N}
		}
		else if (dest > head)
		{
			//    head         tail
			//    ↓            ↓
			//┌───┬────────────┬─────┬───┐
			//│ ? │ [[buffer]] │ gap │ ? │
			//├───┼─────┬──────┴─────┼───┤
			//│ ? │ gap │ [[buffer]] │ ? │
			//└───┴─────┴────────────┴───┘
			//          ↑            ↑
			//          dest         last

			const auto N {tail - head};

			for (usize i {0}; i < N; ++i)
			{
				*(last-i-1) = *(tail-i-1);
			}
			return N; // delta {0...N}
		}
	}
	if constexpr (!std::is_same_v<Fᵒᵘᵗ, Fˢʳᶜ>)
	{
		typedef typename Fᵒᵘᵗ::codeᵘᵗ unitᵒ;
		typedef typename Fˢʳᶜ::codeᵘᵗ unitˢ;

		const unitˢ* src {head};
		/*&*/ unitᵒ* out {dest};

		typename Fˢʳᶜ::stride dirˢ;
		typename Fᵒᵘᵗ::stride dirᵒ;

		for (codeᵖᵗ tmp; src < tail; src += dirˢ,
		                             out += dirᵒ)
		{
			/*&*/ dirˢ = Fˢʳᶜ::next(src);
			Fˢʳᶜ::decode(src, tmp, dirˢ);

			/*&*/ dirᵒ = Fᵒᵘᵗ::size(tmp);
			Fᵒᵘᵗ::encode(tmp, out, dirᵒ);
		}
		return detail::ΔCU<Fᵒᵘᵗ>(dest, out);
	}
}

template <typename Fˡʰˢ,
          typename Fʳʰˢ> constexpr auto x69::detail::equal(const typename Fˡʰˢ::codeᵘᵗ* lhs0, const typename Fˡʰˢ::codeᵘᵗ* lhsN,
                                                           const typename Fʳʰˢ::codeᵘᵗ* rhs0, const typename Fʳʰˢ::codeᵘᵗ* rhsN) noexcept -> bool
{
	if constexpr (std::is_same_v<Fˡʰˢ, Fʳʰˢ>)
	{
		   typedef typename Fˡʰˢ::codeᵘᵗ unit;
		// typedef typename Fʳʰˢ::codeᵘᵗ unit;

		if (lhs0 == rhs0 && lhsN == rhsN)
		{
			return true;
		}

		if (detail::ΔCU<Fˡʰˢ>(lhs0, lhsN)
		    !=
		    detail::ΔCU<Fʳʰˢ>(rhs0, rhsN))
		{
			return false;
		}

		const unit* lhs {lhs0};
		const unit* rhs {rhs0};

		for (; lhs < lhsN && rhs < rhsN; lhs += 1,
		                                 rhs += 1)
		{
			if (*lhs != *rhs)
			{
				return false;
			}
		}
		return lhs == lhsN && rhs == rhsN;
	}
	if constexpr (!std::is_same_v<Fˡʰˢ, Fʳʰˢ>)
	{
		typedef typename Fˡʰˢ::codeᵘᵗ unitˡ;
		typedef typename Fʳʰˢ::codeᵘᵗ unitʳ;

		const unitˡ* lhs {lhs0};
		const unitʳ* rhs {rhs0};

		codeᵖᵗ lhsV;
		codeᵖᵗ rhsV;

		typename Fˡʰˢ::stride dirˡ;
		typename Fʳʰˢ::stride dirʳ;

		for (; lhs < lhsN && rhs < rhsN; lhs += dirˡ,
		                                 rhs += dirʳ)
		{
			/*&*/ dirˡ = Fˡʰˢ::next(lhs);
			Fˡʰˢ::decode(lhs, lhsV, dirˡ);

			/*&*/ dirʳ = Fʳʰˢ::next(rhs);
			Fʳʰˢ::decode(rhs, rhsV, dirʳ);

			if (lhsV != rhsV)
			{
				return false;
			}
		}
		return lhs == lhsN && rhs == rhsN;
	}
}

template <typename Fˡʰˢ,
          typename Fʳʰˢ> constexpr auto x69::detail::swith(const typename Fˡʰˢ::codeᵘᵗ* lhs0, const typename Fˡʰˢ::codeᵘᵗ* lhsN,
                                                           const typename Fʳʰˢ::codeᵘᵗ* rhs0, const typename Fʳʰˢ::codeᵘᵗ* rhsN) noexcept -> bool
{

	if constexpr (std::is_same_v<Fˡʰˢ, Fʳʰˢ>)
	{
		   typedef typename Fˡʰˢ::codeᵘᵗ unit;
		// typedef typename Fʳʰˢ::codeᵘᵗ unit;

		if (rhs0 == rhsN) // RHS is empty
		{
			return true;
		}

		if (lhs0 == rhs0 && lhsN == rhsN)
		{
			return true;
		}

		if (detail::ΔCU<Fˡʰˢ>(lhs0, lhsN)
		    <
		    detail::ΔCU<Fʳʰˢ>(rhs0, rhsN))
		{
			return false;
		}

		const unit* lhs {lhs0};
		const unit* rhs {rhs0};

		for (; lhs < lhsN && rhs < rhsN; lhs += 1,
		                                 rhs += 1)
		{
			if (*lhs != *rhs)
			{
				return false;
			}
		}
		return true;
	}
	if constexpr (!std::is_same_v<Fˡʰˢ, Fʳʰˢ>)
	{
		typedef typename Fˡʰˢ::codeᵘᵗ unitˡ;
		typedef typename Fʳʰˢ::codeᵘᵗ unitʳ;

		if (rhs0 == rhsN) // RHS is empty
		{
			return true;
		}

		const unitˡ* lhs {lhs0};
		const unitʳ* rhs {rhs0};

		codeᵖᵗ lhsV;
		codeᵖᵗ rhsV;

		typename Fˡʰˢ::stride dirˡ;
		typename Fʳʰˢ::stride dirʳ;

		for (; lhs < lhsN && rhs < rhsN; lhs += dirˡ,
		                                 rhs += dirʳ)
		{
			/*&*/ dirˡ = Fˡʰˢ::next(lhs);
			Fˡʰˢ::decode(lhs, lhsV, dirˡ);

			/*&*/ dirʳ = Fʳʰˢ::next(rhs);
			Fʳʰˢ::decode(rhs, rhsV, dirʳ);

			if (lhsV != rhsV)
			{
				return false;
			}
		}
		return rhs == rhsN;
	}
}

template <typename Fˡʰˢ,
          typename Fʳʰˢ> constexpr auto x69::detail::ewith(const typename Fˡʰˢ::codeᵘᵗ* lhs0, const typename Fˡʰˢ::codeᵘᵗ* lhsN,
                                                           const typename Fʳʰˢ::codeᵘᵗ* rhs0, const typename Fʳʰˢ::codeᵘᵗ* rhsN) noexcept -> bool
{
	if constexpr (std::is_same_v<Fˡʰˢ, Fʳʰˢ>)
	{
		   typedef typename Fˡʰˢ::codeᵘᵗ unit;
		// typedef typename Fʳʰˢ::codeᵘᵗ unit;

		if (rhs0 == rhsN) // RHS is empty
		{
			return true;
		}

		if (lhs0 == rhs0 && lhsN == rhsN)
		{
			return true;
		}

		if (detail::ΔCU<Fˡʰˢ>(lhs0, lhsN)
		    <
		    detail::ΔCU<Fʳʰˢ>(rhs0, rhsN))
		{
			return false;
		}

		const unit* lhs {lhsN};
		const unit* rhs {rhsN};

		for (; lhs != lhs0 && rhs != rhs0; lhs -= 1,
		                                   rhs -= 1)
		{
			if (*lhs != *rhs)
			{
				return false;
			}
		}
		return rhs == rhs0;
	}
	if constexpr (!std::is_same_v<Fˡʰˢ, Fʳʰˢ>)
	{
		typedef typename Fˡʰˢ::codeᵘᵗ unitˡ;
		typedef typename Fʳʰˢ::codeᵘᵗ unitʳ;

		if (rhs0 == rhsN) // RHS is empty
		{
			return true;
		}

		const unitˡ* lhs {lhsN};
		const unitʳ* rhs {rhsN};

		codeᵖᵗ lhsV;
		codeᵖᵗ rhsV;

		typename Fˡʰˢ::stride dirˡ;
		typename Fʳʰˢ::stride dirʳ;

		for (; lhs != lhs0 && rhs != rhs0; lhs += dirˡ,
		                                   rhs += dirʳ)
		{
			/*&*/ dirˡ = Fˡʰˢ::back(lhs);
			Fˡʰˢ::decode(lhs, lhsV, dirˡ);

			/*&*/ dirʳ = Fʳʰˢ::back(rhs);
			Fʳʰˢ::decode(rhs, rhsV, dirʳ);

			if (lhsV != rhsV)
			{
				return false;
			}
		}
		return rhs == rhs0;
	}
}

template <typename Fˡʰˢ,
          typename Fʳʰˢ> constexpr auto x69::detail::holds(const typename Fˡʰˢ::codeᵘᵗ* lhs0, const typename Fˡʰˢ::codeᵘᵗ* lhsN,
                                                           const typename Fʳʰˢ::codeᵘᵗ* rhs0, const typename Fʳʰˢ::codeᵘᵗ* rhsN) noexcept -> bool
{
	if constexpr (std::is_same_v<Fˡʰˢ, Fʳʰˢ>)
	{
		   typedef typename Fˡʰˢ::codeᵘᵗ unit;
		// typedef typename Fʳʰˢ::codeᵘᵗ unit;

		if (rhs0 == rhsN) // RHS is empty
		{
			return true;
		}

		bool out {false};

		detail::search<Fˡʰˢ, Fʳʰˢ>(lhs0, lhsN,
		                           rhs0, rhsN,
		[&](const auto* head,
		    const auto* tail) -> bool
		{
			return !(out = true);
		});
		return out; // @see (↑↑↑)
	}
	if constexpr (!std::is_same_v<Fˡʰˢ, Fʳʰˢ>)
	{
		typedef typename Fˡʰˢ::codeᵘᵗ unitˡ;
		typedef typename Fʳʰˢ::codeᵘᵗ unitʳ;

		if (rhs0 == rhsN) // RHS is empty
		{
			return true;
		}

		bool out {false};

		detail::search<Fˡʰˢ, Fʳʰˢ>(lhs0, lhsN,
		                           rhs0, rhsN,
		[&](const auto* head,
		    const auto* tail) -> bool
		{
			return !(out = true);
		});
		return out; // @see (↑↑↑)
	}
}

template <typename Fˡʰˢ,
          typename Fʳʰˢ> constexpr auto x69::detail::split(const typename Fˡʰˢ::codeᵘᵗ* lhs0, const typename Fˡʰˢ::codeᵘᵗ* lhsN,
                                                           const typename Fʳʰˢ::codeᵘᵗ* rhs0, const typename Fʳʰˢ::codeᵘᵗ* rhsN) noexcept -> std::vector<txt<Fˡʰˢ>>
{
	if constexpr (std::is_same_v<Fˡʰˢ, Fʳʰˢ>)
	{
		   typedef typename Fˡʰˢ::codeᵘᵗ unit;
		// typedef typename Fʳʰˢ::codeᵘᵗ unit;

		if (rhs0 == rhsN) // RHS is empty
		{
			return {};
		}

		std::vector<txt<Fˡʰˢ>> out;

		const unit* last {lhs0};

		detail::search<Fˡʰˢ, Fʳʰˢ>(lhs0, lhsN,
		                           rhs0, rhsN,
		[&](const auto* head,
		    const auto* tail) -> bool
		{
			out.emplace_back(last, head);

			last = tail;

			return true; // exhaust..!
		});

		if (last != lhs0 && last < lhsN)
		{
			out.emplace_back(last, lhsN);
		}

		return out;
	}
	if constexpr (!std::is_same_v<Fˡʰˢ, Fʳʰˢ>)
	{
		typedef typename Fˡʰˢ::codeᵘᵗ unitˡ;
		typedef typename Fʳʰˢ::codeᵘᵗ unitʳ;

		if (rhs0 == rhsN) // RHS is empty
		{
			return {};
		}

		std::vector<txt<Fˡʰˢ>> out;

		const unitˡ* last {lhs0};

		detail::search<Fˡʰˢ, Fʳʰˢ>(lhs0, lhsN,
		                           rhs0, rhsN,
		[&](const auto* head,
		    const auto* tail) -> bool
		{
			out.emplace_back(last, head);

			last = tail;

			return true; // exhaust..!
		});

		if (last != lhs0 && last < lhsN)
		{
			out.emplace_back(last, lhsN);
		}

		return out;
	}
}

template <typename Fˡʰˢ,
          typename Fʳʰˢ> constexpr auto x69::detail::match(const typename Fˡʰˢ::codeᵘᵗ* lhs0, const typename Fˡʰˢ::codeᵘᵗ* lhsN,
                                                           const typename Fʳʰˢ::codeᵘᵗ* rhs0, const typename Fʳʰˢ::codeᵘᵗ* rhsN) noexcept -> std::vector<txt<Fˡʰˢ>>
{
	if constexpr (std::is_same_v<Fˡʰˢ, Fʳʰˢ>)
	{
		   typedef typename Fˡʰˢ::codeᵘᵗ unit;
		// typedef typename Fʳʰˢ::codeᵘᵗ unit;

		if (rhs0 == rhsN) // RHS is empty
		{
			return {};
		}

		std::vector<txt<Fˡʰˢ>> out;

		detail::search<Fˡʰˢ, Fʳʰˢ>(lhs0, lhsN,
		                           rhs0, rhsN,
		[&](const auto* head,
		    const auto* tail) -> bool
		{
			out.emplace_back(head, tail);

			return true; // exhaust..!
		});

		return out;
	}
	if constexpr (!std::is_same_v<Fˡʰˢ, Fʳʰˢ>)
	{
		typedef typename Fˡʰˢ::codeᵘᵗ unitˡ;
		typedef typename Fʳʰˢ::codeᵘᵗ unitʳ;

		if (rhs0 == rhsN) // RHS is empty
		{
			return {};
		}

		std::vector<txt<Fˡʰˢ>> out;

		detail::search<Fˡʰˢ, Fʳʰˢ>(lhs0, lhsN,
		                           rhs0, rhsN,
		[&](const auto* head,
		    const auto* tail) -> bool
		{
			out.emplace_back(head, tail);

			return true; // exhaust..!
		});

		return out;
	}
}

template <typename Fˡʰˢ,
          typename Fʳʰˢ> constexpr auto x69::detail::search(const typename Fˡʰˢ::codeᵘᵗ* lhs0, const typename Fˡʰˢ::codeᵘᵗ* lhsN,
                                                            const typename Fʳʰˢ::codeᵘᵗ* rhs0, const typename Fʳʰˢ::codeᵘᵗ* rhsN, auto&& fn)
// info {error-free} & {interface}
noexcept(std::is_nothrow_invocable_v<decltype(fn), const typename Fʳʰˢ::codeᵘᵗ*,
                                                   const typename Fʳʰˢ::codeᵘᵗ*>) -> void requires (std::is_invocable_r_v<bool, decltype(fn), const typename Fʳʰˢ::codeᵘᵗ*,
                                                                                                                                              const typename Fʳʰˢ::codeᵘᵗ*>)
{
	if constexpr (std::is_same_v<Fˡʰˢ, Fʳʰˢ>)
	{
		   typedef typename Fˡʰˢ::codeᵘᵗ unit;
		// typedef typename Fʳʰˢ::codeᵘᵗ unit;

		const auto lhsL {lhsN - lhs0};
		const auto rhsL {rhsN - rhs0};

		if (lhs0 == lhsN) // LHS is empty
		{
			return; // skip
		}

		if (rhs0 == rhsN) // RHS is empty
		{
			return; // skip
		}

		if (lhsL >= rhsL) // exccessive..?
		{
			return; // skip
		}

		if (lhs0 == rhs0) // intersects..?
		{
			fn(lhs0, lhsN);
			return; // skip
		}

		const unit* ptr {lhs0};
		const unit* lhs       ;
		const unit* rhs       ;

		for (const auto off {detail::ΔCU<Fˡʰˢ, Fʳʰˢ>(rhs0, rhsN)}; ptr < lhsN; )
		{
			for (lhs = ptr, rhs = rhs0; lhs < lhsN && rhs < rhsN; lhs += 1,
			                                                      rhs += 1)
			{
				if (*lhs != *rhs) { break; }
			}

			if (rhs != rhsN)
			{
				ptr += Fˡʰˢ::next(ptr);
			}
			else [[likely]]
			{
				if (!fn(ptr,ptr+off))
				{
					return;
				}
				ptr += (/**/off/**/);
			}
		}
	}
	if constexpr (!std::is_same_v<Fˡʰˢ, Fʳʰˢ>)
	{
		typedef typename Fˡʰˢ::codeᵘᵗ unitˡ;
		typedef typename Fʳʰˢ::codeᵘᵗ unitʳ;

		const auto lhsL {lhsN - lhs0};
		const auto rhsL {rhsN - rhs0};

		if (lhs0 == lhsN) // LHS is empty
		{
			return; // skip
		}

		if (rhs0 == rhsN) // RHS is empty
		{
			return; // skip
		}

		const unitˡ* ptr {lhs0};
		const unitˡ* lhs       ;
		const unitʳ* rhs       ;

		for (const auto off {detail::ΔCU<Fˡʰˢ, Fʳʰˢ>(rhs0, rhsN)}; ptr < lhsN; )
		{
			typename Fˡʰˢ::stride dirˡ;
			typename Fʳʰˢ::stride dirʳ;

			codeᵖᵗ lhsV;
			codeᵖᵗ rhsV;

			for (lhs = ptr, rhs = rhs0; lhs < lhsN && rhs < rhsN; lhs += dirˡ,
			                                                      rhs += dirʳ)
			{
				/*&*/ dirˡ = Fˡʰˢ::next(lhs);
				Fˡʰˢ::decode(lhs, lhsV, dirˡ);

				/*&*/ dirʳ = Fʳʰˢ::next(rhs);
				Fʳʰˢ::decode(rhs, rhsV, dirʳ);

				if (lhsV != rhsV) { break; }
			}

			if (rhs != rhsN)
			{
				ptr += Fˡʰˢ::next(ptr);
			}
			else [[likely]]
			{
				if (!fn(ptr,ptr+off))
				{
					return;
				}
				ptr += (/**/off/**/);
			}
		}
	}
}

template <typename Fᵒᵘᵗ> constexpr auto x69::detail::substr(const typename Fᵒᵘᵗ::codeᵘᵗ* head, const typename Fᵒᵘᵗ::codeᵘᵗ* tail, usize off) noexcept -> decltype(head)
{
	   typedef typename Fᵒᵘᵗ::codeᵘᵗ unit;
	// typedef typename Fˢʳᶜ::codeᵘᵗ unit;

	const unit* dest {head};

	   for (usize i {0x0}; i < off; ++i) { if (dest == tail) { return nullptr; } dest += Fᵒᵘᵗ::next(dest); }
	// for (usize i {0x0}; i < off; ++i) { if (dest == head) { return nullptr; } dest += Fᵒᵘᵗ::back(dest); }

	assert(head <= dest);
	assert(dest <= tail);

	return dest;
}

template <typename Fᵒᵘᵗ> constexpr auto x69::detail::substr(/*&*/ typename Fᵒᵘᵗ::codeᵘᵗ* head, /*&*/ typename Fᵒᵘᵗ::codeᵘᵗ* tail, usize off) noexcept -> decltype(head)
{
	   typedef typename Fᵒᵘᵗ::codeᵘᵗ unit;
	// typedef typename Fˢʳᶜ::codeᵘᵗ unit;

	/*&*/ unit* dest {head};

	   for (usize i {0x0}; i < off; ++i) { if (dest == tail) { return nullptr; } dest += Fᵒᵘᵗ::next(dest); }
	// for (usize i {0x0}; i < off; ++i) { if (dest == head) { return nullptr; } dest += Fᵒᵘᵗ::back(dest); }

	assert(head <= dest);
	assert(dest <= tail);

	return dest;
}

template <typename Fᵒᵘᵗ> constexpr auto x69::detail::substr(const typename Fᵒᵘᵗ::codeᵘᵗ* head, const typename Fᵒᵘᵗ::codeᵘᵗ* tail, magic off) noexcept -> decltype(head)
{
	   typedef typename Fᵒᵘᵗ::codeᵘᵗ unit;
	// typedef typename Fˢʳᶜ::codeᵘᵗ unit;

	const unit* dest {tail};

	// for (usize i {0x0}; i < off; ++i) { if (dest == tail) { return nullptr; } dest += Fᵒᵘᵗ::next(dest); }
	// for (usize i {0x0}; i < off; ++i) { if (dest == head) { return nullptr; } dest += Fᵒᵘᵗ::back(dest); }

	assert(head <= dest);
	assert(dest <= tail);

	return dest;
}

template <typename Fᵒᵘᵗ> constexpr auto x69::detail::substr(/*&*/ typename Fᵒᵘᵗ::codeᵘᵗ* head, /*&*/ typename Fᵒᵘᵗ::codeᵘᵗ* tail, magic off) noexcept -> decltype(head)
{
	   typedef typename Fᵒᵘᵗ::codeᵘᵗ unit;
	// typedef typename Fˢʳᶜ::codeᵘᵗ unit;

	/*&*/ unit* dest {tail};

	// for (usize i {0x0}; i < off; ++i) { if (dest == tail) { return nullptr; } dest += Fᵒᵘᵗ::next(dest); }
	// for (usize i {0x0}; i < off; ++i) { if (dest == head) { return nullptr; } dest += Fᵒᵘᵗ::back(dest); }

	assert(head <= dest);
	assert(dest <= tail);

	return dest;
}

template <typename Fᵒᵘᵗ> constexpr auto x69::detail::substr(const typename Fᵒᵘᵗ::codeᵘᵗ* head, const typename Fᵒᵘᵗ::codeᵘᵗ* tail, clamp off) noexcept -> decltype(head)
{
	   typedef typename Fᵒᵘᵗ::codeᵘᵗ unit;
	// typedef typename Fˢʳᶜ::codeᵘᵗ unit;

	const unit* dest {tail};

	// for (usize i {0x0}; i < off; ++i) { if (dest == tail) { return nullptr; } dest += Fᵒᵘᵗ::next(dest); }
	   for (usize i {0x0}; i < off; ++i) { if (dest == head) { return nullptr; } dest += Fᵒᵘᵗ::back(dest); }

	assert(head <= dest);
	assert(dest <= tail);

	return dest;
}

template <typename Fᵒᵘᵗ> constexpr auto x69::detail::substr(/*&*/ typename Fᵒᵘᵗ::codeᵘᵗ* head, /*&*/ typename Fᵒᵘᵗ::codeᵘᵗ* tail, clamp off) noexcept -> decltype(head)
{
	   typedef typename Fᵒᵘᵗ::codeᵘᵗ unit;
	// typedef typename Fˢʳᶜ::codeᵘᵗ unit;

	/*&*/ unit* dest {tail};

	// for (usize i {0x0}; i < off; ++i) { if (dest == tail) { return nullptr; } dest += Fᵒᵘᵗ::next(dest); }
	   for (usize i {0x0}; i < off; ++i) { if (dest == head) { return nullptr; } dest += Fᵒᵘᵗ::back(dest); }

	assert(head <= dest);
	assert(dest <= tail);

	return dest;
}

template <typename Fᵒᵘᵗ> constexpr auto x69::detail::substr(const typename Fᵒᵘᵗ::codeᵘᵗ* head, const typename Fᵒᵘᵗ::codeᵘᵗ* tail, clamp beg, clamp end) noexcept -> txt<Fᵒᵘᵗ>
{
	   typedef typename Fᵒᵘᵗ::codeᵘᵗ unit;
	// typedef typename Fˢʳᶜ::codeᵘᵗ unit;

	const unit* ptrN {tail};

	for (usize i {0x0}; i < end && head < ptrN; ++i) { ptrN += Fᵒᵘᵗ::back(ptrN); }

	const unit* ptr0 {ptrN};

	for (usize i {end}; i < beg && head < ptr0; ++i) { ptr0 += Fᵒᵘᵗ::back(ptr0); }

	assert(head <= ptr0);
	assert(ptr0 <= tail);
	assert(head <= ptrN);
	assert(ptrN <= tail);

	return {ptr0, ptrN};
}

template <typename Fᵒᵘᵗ> constexpr auto x69::detail::substr(const typename Fᵒᵘᵗ::codeᵘᵗ* head, const typename Fᵒᵘᵗ::codeᵘᵗ* tail, clamp beg, magic end) noexcept -> txt<Fᵒᵘᵗ>
{
	   typedef typename Fᵒᵘᵗ::codeᵘᵗ unit;
	// typedef typename Fˢʳᶜ::codeᵘᵗ unit;

	const unit* ptr0 {tail};

	for (usize i {0x0}; i < beg && head < ptr0; ++i) { ptr0 += Fᵒᵘᵗ::back(ptr0); }

	const unit* ptrN {tail};

	assert(head <= ptr0);
	assert(ptr0 <= tail);
	assert(head <= ptrN);
	assert(ptrN <= tail);

	return {ptr0, ptrN};
}

template <typename Fᵒᵘᵗ> constexpr auto x69::detail::substr(const typename Fᵒᵘᵗ::codeᵘᵗ* head, const typename Fᵒᵘᵗ::codeᵘᵗ* tail, usize beg, clamp end) noexcept -> txt<Fᵒᵘᵗ>
{
	   typedef typename Fᵒᵘᵗ::codeᵘᵗ unit;
	// typedef typename Fˢʳᶜ::codeᵘᵗ unit;

	const unit* ptr0 {head};

	for (usize i {0x0}; i < beg && ptr0 < tail; ++i) { ptr0 += Fᵒᵘᵗ::next(ptr0); }

	const unit* ptrN {tail};

	for (usize i {0x0}; i < end && head < ptrN; ++i) { ptrN += Fᵒᵘᵗ::back(ptrN); }

	assert(head <= ptr0);
	assert(ptr0 <= tail);
	assert(head <= ptrN);
	assert(ptrN <= tail);

	return {ptr0, ptrN};
}

template <typename Fᵒᵘᵗ> constexpr auto x69::detail::substr(const typename Fᵒᵘᵗ::codeᵘᵗ* head, const typename Fᵒᵘᵗ::codeᵘᵗ* tail, usize beg, magic end) noexcept -> txt<Fᵒᵘᵗ>
{
	   typedef typename Fᵒᵘᵗ::codeᵘᵗ unit;
	// typedef typename Fˢʳᶜ::codeᵘᵗ unit;

	const unit* ptr0 {head};

	for (usize i {0x0}; i < beg && ptr0 < tail; ++i) { ptr0 += Fᵒᵘᵗ::next(ptr0); }

	const unit* ptrN {tail};

	assert(head <= ptr0);
	assert(ptr0 <= tail);
	assert(head <= ptrN);
	assert(ptrN <= tail);

	return {ptr0, ptrN};
}

template <typename Fᵒᵘᵗ> constexpr auto x69::detail::substr(const typename Fᵒᵘᵗ::codeᵘᵗ* head, const typename Fᵒᵘᵗ::codeᵘᵗ* tail, usize beg, usize end) noexcept -> txt<Fᵒᵘᵗ>
{
	   typedef typename Fᵒᵘᵗ::codeᵘᵗ unit;
	// typedef typename Fˢʳᶜ::codeᵘᵗ unit;

	const unit* ptr0 {head};

	for (usize i {0x0}; i < beg && ptr0 < tail; ++i) { ptr0 += Fᵒᵘᵗ::next(ptr0); }

	const unit* ptrN {ptr0};

	for (usize i {beg}; i < end && ptrN < tail; ++i) { ptrN += Fᵒᵘᵗ::next(ptrN); }

	assert(head <= ptr0);
	assert(ptr0 <= tail);
	assert(head <= ptrN);
	assert(ptrN <= tail);

	return {ptr0, ptrN};
}

#pragma    endregion ::detail
