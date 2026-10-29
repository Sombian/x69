#pragma once

//───────────────────"
#include "x69/str.hpp"
//───────────────────"

template <typename F, typeᵐᵉᵐˀ A> constexpr x69::str<F, A>::str(auto&& init) noexcept requires requires { txt {init}; } : str()
{
	*this = init; // allows txt<Fˣ>, and anything thereof; for more, see txt<?>
}

template <typename F, typeᵐᵉᵐˀ A> constexpr x69::str<F, A>::str::str() noexcept
{
	if constexpr (IS_BE)
	{
		small[0b0] = (   '\0'   );
		bytes[RMB] = (MAX << 0b1);
	}
	if constexpr (IS_LE)
	{
		small[0b0] = (   '\0'   );
		bytes[RMB] = (MAX << 0b0);
	}
}

template <typename F, typeᵐᵉᵐˀ A> constexpr x69::str<F, A>::str::~str() noexcept
{
	if (_mode() != SSO)
	{
		const auto size {large.last - large.head};

		std::allocator_traits<decltype(useᵐᵉᵐˀ)>
		::deallocate(useᵐᵉᵐˀ, large.head, size);
	}
}

template <typename F, typeᵐᵉᵐˀ A> constexpr x69::str<F, A>::str(const str& lval) noexcept : str()
{
	if (this == &lval) [[unlikely]] return;

	spare(lval.size());
	_SSOᴺ(lval.size());

	detail::copy<F, F>(_head(), lval._head(),
	                            lval._tail());
}

template <typename F, typeᵐᵉᵐˀ A> constexpr x69::str<F, A>::str(/*&*/ str&& rval) noexcept : str()
{
	if (this == &rval) [[unlikely]] return;

	std::swap(     bytes,
	          rval.bytes);
}

template <typename F, typeᵐᵉᵐˀ A> constexpr auto x69::str<F, A>::operator=(const str& lval) noexcept -> str&
{
	if (this == &lval) [[unlikely]] return *this;

	spare(lval.size());
	_SSOᴺ(lval.size());

	detail::copy<F, F>(_head(), lval._head(),
	                            lval._tail());

	return *this;
}

template <typename F, typeᵐᵉᵐˀ A> constexpr auto x69::str<F, A>::operator=(/*&*/ str&& rval) noexcept -> str&
{
	if (this == &rval) [[unlikely]] return *this;

	std::swap(     bytes,
	          rval.bytes);

	return *this;
}

template <typename F, typeᵐᵉᵐˀ A> constexpr auto x69::str<F, A>::operator[](usize rhs) const noexcept -> handleᶠⁱⁿ<'L'>
{
	return super::operator[](rhs);
}

template <typename F, typeᵐᵉᵐˀ A> constexpr auto x69::str<F, A>::operator[](usize rhs) /*&*/ noexcept -> handleᵐᵘᵗ<'L'>
{
	return {super::operator[](rhs), this};
}

template <typename F, typeᵐᵉᵐˀ A> constexpr auto x69::str<F, A>::operator[](magic rhs) const noexcept -> handleᶠⁱⁿ<'R'>
{
	return super::operator[](rhs);
}

template <typename F, typeᵐᵉᵐˀ A> constexpr auto x69::str<F, A>::operator[](magic rhs) /*&*/ noexcept -> handleᵐᵘᵗ<'R'>
{
	return {super::operator[](rhs), this};
}

template <typename F, typeᵐᵉᵐˀ A> constexpr auto x69::str<F, A>::operator[](clamp rhs) const noexcept -> handleᶠⁱⁿ<'R'>
{
	return super::operator[](rhs);
}

template <typename F, typeᵐᵉᵐˀ A> constexpr auto x69::str<F, A>::operator[](clamp rhs) /*&*/ noexcept -> handleᵐᵘᵗ<'R'>
{
	return {super::operator[](rhs), this};
}

template <typename F, typeᵐᵉᵐˀ A> template <x69::usize N> constexpr auto x69::str<F, A>::it() const noexcept -> std::tuple<windowᶠⁱⁿ<N>, usize>
{
	return super::template it<N>();
}

template <typename F, typeᵐᵉᵐˀ A> template <x69::usize N> constexpr auto x69::str<F, A>::it() /*&*/ noexcept -> std::tuple<windowᵐᵘᵗ<N>, usize>
{
	return {{{_head(), _tail()}, this}, 0};
}

template <typename F, typeᵐᵉᵐˀ A> constexpr auto x69::str<F, A>::operator=(auto&& rhs)& noexcept -> str& requires requires { txt {rhs}; }
{
	[this]<typename Fˀ>(txt<Fˀ> inˀ)
	{
		const auto commit = [this](const typename Fˀ::codeᵘᵗ* rhs0,
		                           const typename Fˀ::codeᵘᵗ* rhsN)
		{
			const auto rhsL {detail::ΔCU<F, Fˀ>(rhs0,
			                                    rhsN)};

			spare(rhsL);

			detail::copy<F, Fˀ>(_head(), rhs0,
			                             rhsN);

			_SSOᴺ(rhsL);
		};

		if constexpr (std::is_same_v<F, Fˀ>)
		{
			if (_head() <= inˀ.head && inˀ.head <= _tail())
			{
				const str tmp {inˀ};

				commit(tmp._head(),
				       tmp._tail());
			}
			else [[likely]] // RHS is not a substr of LHS
			{
				commit(inˀ.head,
				       inˀ.tail);
			}
		}
		if constexpr (!std::is_same_v<F, Fˀ>)
		{
			commit(inˀ.head,
			       inˀ.tail);
		}
	}
	(txt {rhs});

	return *this;
}

template <typename F, typeᵐᵉᵐˀ A> constexpr auto x69::str<F, A>::operator=(auto&& rhs)&& noexcept -> str&& requires requires { txt {rhs}; }
{
	// delegate to lvalue ver. oper
	static_cast<str&>(*this) = rhs;

	return *this;
}

template <typename F, typeᵐᵉᵐˀ A> constexpr auto x69::str<F, A>::operator+=(auto&& rhs)& noexcept -> str& requires requires { txt {rhs}; }
{
	[this]<typename Fˀ>(txt<Fˀ> inˀ)
	{
		const auto commit = [this](const typename Fˀ::codeᵘᵗ* rhs0,
		                           const typename Fˀ::codeᵘᵗ* rhsN)
		{
			const auto sumL {super::size() + detail::ΔCU<F, Fˀ>(rhs0,
			                                                    rhsN)};

			spare(sumL);

			detail::copy<F, Fˀ>(_tail(), rhs0,
			                             rhsN);

			_SSOᴺ(sumL);
		};

		if constexpr (std::is_same_v<F, Fˀ>)
		{
			if (_head() <= inˀ.head && inˀ.head <= _tail())
			{
				const str tmp {inˀ};

				commit(tmp._head(),
				       tmp._tail());
			}
			else [[likely]] // RHS is not a substr of LHS
			{
				commit(inˀ.head,
				       inˀ.tail);
			}
		}
		if constexpr (!std::is_same_v<F, Fˀ>)
		{
			commit(inˀ.head,
			       inˀ.tail);
		}
	}
	(txt {rhs});

	return *this;
}

template <typename F, typeᵐᵉᵐˀ A> constexpr auto x69::str<F, A>::operator+=(auto&& rhs)&& noexcept -> str&& requires requires { txt {rhs}; }
{
	// delegate to lvalue ver. oper
	static_cast<str&>(*this) += rhs;

	return *this;
}

#pragma    region ${SSO23}

template <typename F, typeᵐᵉᵐˀ A> constexpr auto x69::str<F, A>::_mode() const noexcept -> mode
{
	if constexpr (IS_BE)
	{
		// given (v := 0b10000000'11000000'11110000)
		// ┌───[byte]────┬───[byte]────┬───[byte]────┐
		// │ 10'00'00'00 │ 11'00'00'00 │ 11'11'00'00 │
		// └─↑─────────↑─┴─────────────┴─────────────┘
		//   MSB       LSB
		return static_cast<mode>(bytes[RMB] & 0b00000001);
	}
	if constexpr (IS_LE)
	{
		// given (v := 0b10000000'11000000'11110000)
		// ┌───[byte]────┬───[byte]────┬───[byte]────┐
		// │ 11'11'00'00 │ 11'00'00'00 │ 10'00'00'00 │
		// └─↑─────────↑─┴─────────────┴─────────────┘
		//   MSB       LSB
		return static_cast<mode>(bytes[RMB] & 0b10000000);
	}
}

template <typename F, typeᵐᵉᵐˀ A> constexpr auto x69::str<F, A>::_head() const noexcept -> const unit*
{
	return const_cast<str&>(*this)._head();
}

template <typename F, typeᵐᵉᵐˀ A> constexpr auto x69::str<F, A>::_head() /*&*/ noexcept -> /*&*/ unit*
{
	typedef std::decay_t<decltype(bytes[0])> small_t;
	typedef std::decay_t<decltype(usize{0})> large_t;

	if constexpr (IS_BE)
	{
		return _mode() == SSO ? &small     [0]
		                      : &large.head[0];
	}
	if constexpr (IS_LE)
	{
		return _mode() == SSO ? &small     [0]
		                      : &large.head[0];
	}
}

template <typename F, typeᵐᵉᵐˀ A> constexpr auto x69::str<F, A>::_tail() const noexcept -> const unit*
{
	return const_cast<str&>(*this)._tail();
}

template <typename F, typeᵐᵉᵐˀ A> constexpr auto x69::str<F, A>::_tail() /*&*/ noexcept -> /*&*/ unit*
{
	typedef std::decay_t<decltype(bytes[0])> small_t;
	typedef std::decay_t<decltype(usize{0})> large_t;

	if constexpr (IS_BE)
	{
		return _mode() == SSO ? &small     [MAX - (bytes[RMB] >> 1)]
		                      : &large.head[      (large.meta >> 1)];
	}
	if constexpr (IS_LE)
	{
		constexpr small_t S {small_t {1} << (sizeof(small_t) * 8 - 1)};
		constexpr large_t L {large_t {1} << (sizeof(large_t) * 8 - 1)};

		return _mode() == SSO ? &small     [MAX - (bytes[RMB] & ~S)]
		                      : &large.head[      (large.meta & ~L)];
	}
}

template <typename F, typeᵐᵉᵐˀ A> constexpr auto x69::str<F, A>::_SSOᴺ(usize inˀ) noexcept -> void
{
	typedef std::decay_t<decltype(bytes[0])> small_t;
	typedef std::decay_t<decltype(usize{0})> large_t;

	if constexpr (IS_BE)
	{
		const auto tag {static_cast<small_t>(MAX - inˀ)};

		/**/ if (_mode() == SSO)
		{
			small     [inˀ] = 0; bytes[RMB] = (static_cast<small_t>(tag) << 1) | SSO;
		}
		else if (_mode() == STD)
		{
			large.head[inˀ] = 0; large.meta = (static_cast<large_t>(inˀ) << 1) | STD;
		}
	}
	if constexpr (IS_LE)
	{
		const auto tag {static_cast<small_t>(MAX - inˀ)};

		constexpr usize S {(sizeof(small_t) - 0) * 8};
		constexpr usize L {(sizeof(large_t) - 1) * 8};

		/**/ if (_mode() == SSO)
		{
			small     [inˀ] = 0; bytes[RMB] = (static_cast<small_t>(SSO) << S) | tag;
		}
		else if (_mode() == STD)
		{
			large.head[inˀ] = 0; large.meta = (static_cast<large_t>(STD) << L) | inˀ;
		}
	}
}

template <typename F, typeᵐᵉᵐˀ A> constexpr auto x69::str<F, A>::space(/* get */) const noexcept -> usize
{
	typedef std::decay_t<decltype(bytes[0])> small_t;
	typedef std::decay_t<decltype(usize{0})> large_t;

	if constexpr (IS_BE)
	{
		return _mode() == SSO ? MAX // or _tail() - _head()
		                      : large.last - large.head - 1;
	}
	if constexpr (IS_LE)
	{
		return _mode() == SSO ? MAX // or _tail() - _head()
		                      : large.last - large.head - 1;
	}
}

template <typename F, typeᵐᵉᵐˀ A> constexpr auto x69::str<F, A>::spare(usize inˀ) /*&*/ noexcept -> void
{
	typedef std::decay_t<decltype(bytes[0])> small_t;
	typedef std::decay_t<decltype(usize{0})> large_t;

	if constexpr (IS_BE)
	{
		if (inˀ <= MAX || inˀ <= space()) { return; }

		const auto null_terminator_v {inˀ + 1 + '\0'};

		unit* dest {std::allocator_traits<decltype(useᵐᵉᵐˀ)>
		            ::allocate(useᵐᵉᵐˀ, null_terminator_v)};

		const unit* old0 {_head()};
		const unit* oldN {_tail()};

		detail::copy<F, F>(dest, old0,
		                         oldN);

		const auto tag {oldN - old0};

		if (_mode() != SSO) [[likely]]
		{
			const auto size {large.last - large.head};

			std::allocator_traits<decltype(useᵐᵉᵐˀ)>
			::deallocate(useᵐᵉᵐˀ, large.head, size);
		}

		large.head = (dest                    ); // sets ptr0
		large.last = (dest + null_terminator_v); // sets ptrN
		large.meta = (static_cast<large_t>(tag) << 1) | STD;
	}
	if constexpr (IS_LE)
	{
		if (inˀ <= MAX || inˀ <= space()) { return; }

		const auto null_terminator_v {inˀ + 1 + '\0'};

		unit* dest {std::allocator_traits<decltype(useᵐᵉᵐˀ)>
		           ::allocate(useᵐᵉᵐˀ, null_terminator_v)};

		const unit* old0 {_head()};
		const unit* oldN {_tail()};

		detail::copy<F, F>(dest, old0,
		                         oldN);

		const auto tag {oldN - old0};

		if (_mode() != SSO) [[likely]]
		{
			const auto size {large.last - large.head};

			std::allocator_traits<decltype(useᵐᵉᵐˀ)>
			::deallocate(useᵐᵉᵐˀ, large.head, size);
		}

		// constexpr usize S {(sizeof(small_t) - 0) * 8};
		   constexpr usize L {(sizeof(large_t) - 1) * 8};

		large.head = (dest                    ); // sets ptr0
		large.last = (dest + null_terminator_v); // sets ptrN
		large.meta = (static_cast<large_t>(STD) << L) | tag;
	}
}

#pragma endregion ${SSO23}
#pragma    region ::handle

template <typename F, typeᵐᵉᵐˀ A> template <char S> constexpr auto x69::str<F, A>::handleᵐᵘᵗ<S>::operator=(codeᵖᵗ rhs) noexcept -> handleᵐᵘᵗ&
{
	if constexpr (S == 'L')
	{
		auto& ptr {super::use(this, &handleᶠⁱⁿ<S>::ptr)};

		if (ptr == nullptr) [[unlikely]]
		{
			return *this;
		}

		const auto dirᵖᵗʳ {F::next(ptr)};
		const auto dirʳʰˢ {F::size(rhs)};

		const unit* beg {src->_head()};
		const unit* end {src->_tail()};

		/**/ if (dirᵖᵗʳ < dirʳʰˢ)
		{
			// beg <:--{off}--:> ptr
			// ↓                 ↓
			// ┌────────┬────────┬─────┬────────┬────────┐
			// │ <head> │ buffer │ ??? │ buffer │ <tail> │
			// ├────────┼────────┼─────┴────────┴───┬────┴───┐
			// │ <head> │ buffer │ ({RHS}) │ buffer │ <tail> │
			// └────────┴────────┴─────────┴────────┴────────┘
			// ↑                 ↑
			// beg <:--{off}--:> ptr

			if (src->space() < src->size() - dirᵖᵗʳ + dirʳʰˢ)
			{
				const auto off {ptr - beg};

				src->spare(src->size() * 2);

				beg = src->_head();
				end = src->_tail();
				// apply offset
				ptr = (beg + off);
			}
		}
		else if (dirᵖᵗʳ > dirʳʰˢ)
		{
			// beg <:--{off}--:> ptr
			// ↓                 ↓
			// ┌────────┬────────┬─────────┬────────┬────────┐
			// │ <head> │ buffer │ ({???}) │ buffer │ <tail> │
			// ├────────┼────────┼─────┬───┴────┬───┴────┬───┘
			// │ <head> │ buffer │ RHS │ buffer │ <tail> │
			// └────────┴────────┴─────┴────────┴────────┘
			// ↑                 ↑
			// beg <:--{off}--:> ptr
		}
		// const_cast is safe in this context
		unit*& out {const_cast<unit*&>(ptr)};

		if (dirᵖᵗʳ - dirʳʰˢ != 0) [[likely]]
		{
			detail::copy<F, F>(out + dirʳʰˢ, out + dirᵖᵗʳ, end);

			src->_SSOᴺ(src->size() - dirᵖᵗʳ
			                       + dirʳʰˢ);
		}
		F::encode(rhs, out, dirʳʰˢ);

		return *this;
	}
	if constexpr (S == 'R')
	{
		auto*& ptr {super::use(this, &handleᶠⁱⁿ<S>::ptr)};

		if (ptr == nullptr) [[unlikely]]
		{
			return *this;
		}

		const auto dirᵖᵗʳ {F::back(ptr)};
		const auto dirʳʰˢ {F::size(rhs)};

		const unit* beg {src->_head()};
		const unit* end {src->_tail()};

		/**/ if (0 < dirʳʰˢ - dirᵖᵗʳ)
		{
			//                        ptr <:--{off}--:> end
			//                        ↓                 ↓
			//┌────────┬────────┬─────┬────────┬────────┐
			//│ <head> │ buffer │ ??? │ buffer │ <tail> │
			//├────────┼────────┼─────┴───┬────┴───┬────┴───┐
			//│ <head> │ buffer │ ({RHS}) │ buffer │ <tail> │
			//└────────┴────────┴─────────┴────────┴────────┘
			//                            ↑                 ↑
			//                            ptr <:--{off}--:> end

			if (src->space() < src->size() - dirᵖᵗʳ + dirʳʰˢ)
			{
				const auto off {end - ptr};

				src->spare(src->size() * 2);

				beg = src->_head();
				end = src->_tail();
				// apply offset
				ptr = (end - off);
			}
		}
		else if (0 > dirʳʰˢ - dirᵖᵗʳ)
		{
			//                             ptr <:--{off}--:> end
			//                             ↓                 ↓
			// ┌────────┬────────┬─────────┬────────┬────────┐
			// │ <head> │ buffer │ ({???}) │ buffer │ <tail> │
			// ├────────┼────────┼─────┬───┴────┬───┴────┬───┘
			// │ <head> │ buffer │ RHS │ buffer │ <tail> │
			// └────────┴────────┴─────┴────────┴────────┘
			//                         ↑                 ↑
			//                         ptr <:--{off}--:> end
		}
		// const_cast is safe in this context
		unit*& out {const_cast<unit*&>(ptr)};

		if (dirᵖᵗʳ + dirʳʰˢ != 0) [[likely]]
		{
			detail::copy<F, F>(out + dirᵖᵗʳ - dirʳʰˢ, out, end);

			src->_SSOᴺ(src->size() + dirᵖᵗʳ
			                       - dirʳʰˢ);
		}
		F::encode(rhs, out, dirʳʰˢ);

		return *this;
	}
}

#pragma endregion ::handle
#pragma    region ::window

template <typename F, typeᵐᵉᵐˀ A> template <x69::usize N> constexpr auto x69::str<F, A>::windowᵐᵘᵗ<N>::operator[](usize rhs) noexcept -> handleᵃˡᵗ<'L'>
{
	return {{windowᶠⁱⁿ<N>::operator[](rhs), src}, this};
}

template <typename F, typeᵐᵉᵐˀ A> template <x69::usize N> constexpr auto x69::str<F, A>::windowᵐᵘᵗ<N>::operator[](magic rhs) noexcept -> handleᵃˡᵗ<'R'>
{
	return {{windowᶠⁱⁿ<N>::operator[](rhs), src}, this};
}

template <typename F, typeᵐᵉᵐˀ A> template <x69::usize N> constexpr auto x69::str<F, A>::windowᵐᵘᵗ<N>::operator[](clamp rhs) noexcept -> handleᵃˡᵗ<'R'>
{
	return {{windowᶠⁱⁿ<N>::operator[](rhs), src}, this};
}

template <typename F, typeᵐᵉᵐˀ A> template <x69::usize N> template <char S> constexpr auto x69::str<F, A>::windowᵐᵘᵗ<N>::handleᵃˡᵗ<S>::operator=(codeᵖᵗ rhs) noexcept -> handleᵃˡᵗ&
{
	if constexpr (S == 'L')
	{
		auto& ptr {super::use(this, &handleᶠⁱⁿ<S>::ptr)};

		if (ptr == nullptr) [[unlikely]]
		{
			return *this;
		}

		auto& beg {super::use(src, &windowᶠⁱⁿ<N>::beg)};
		auto& end {super::use(src, &windowᶠⁱⁿ<N>::end)};

		auto dirᵖᵗʳ {F::next(ptr)};
		auto dirʳʰˢ {F::size(rhs)};

		handleᵐᵘᵗ<S>::operator=(rhs);

		// if shift occurs
		if (dirᵖᵗʳ - dirʳʰˢ != 0)
		{
			const auto dif {ptr - beg};
			// refresh stale ptr(s)
			beg = src->src->_head();
			end = src->src->_tail();

			for (auto& [key, off] : super::use(src, &windowᶠⁱⁿ<N>::logs))
			{
				if (0 < off && dif < off)
				{
					// where := [0 < ptr]
					//          [0 < rhs]
					//          [0 < rhs]

					off = off - dirᵖᵗʳ
					          + dirʳʰˢ;
				}
			}
		}

		return *this;
	}
	if constexpr (S == 'R')
	{
		auto& ptr {super::use(this, &handleᶠⁱⁿ<S>::ptr)};

		if (ptr == nullptr) [[unlikely]]
		{
			return *this;
		}

		auto& beg {super::use(src, &windowᶠⁱⁿ<N>::beg)};
		auto& end {super::use(src, &windowᶠⁱⁿ<N>::end)};

		auto dirᵖᵗʳ {F::back(ptr)};
		auto dirʳʰˢ {F::size(rhs)};

		handleᵐᵘᵗ<S>::operator=(rhs);

		// if shift occurs
		if (dirᵖᵗʳ + dirʳʰˢ != 0)
		{
			const auto dif {end - ptr};
			// refresh stale ptr(s)
			beg = src->src->_head();
			end = src->src->_tail();

			for (auto& [key, off] : super::use(src, &windowᶠⁱⁿ<N>::logs))
			{
				if (off < 0 && off < dif)
				{
					// where := [0 > ptr]
					//          [0 > rhs]
					//          [0 < rhs]

					off = off + dirᵖᵗʳ
					          - dirʳʰˢ;
				}
			}
		}

		return *this;
	}
}

#pragma endregion ::window
