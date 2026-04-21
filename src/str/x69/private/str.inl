#pragma once

//====================//
#include "../str.hpp" //
//====================//

namespace x69
{
	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr str<native, malloc>::operator const unit_t*() const noexcept
	{
		return this->__head__();
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr str<native, malloc>::operator /*&*/ unit_t*() /*&*/ noexcept
	{
		return this->__head__();
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	constexpr str<native, malloc>::str(string&& value) noexcept
	{
		this->__init__();
		(*this) = value;
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr str<native, malloc>::str(code_t   value) noexcept
	{
		this->__init__();
		(*this) = value;
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr str<native, malloc>::str(/* default! */) noexcept
	{
		this->__init__();
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr str<native, malloc>::str(const str& other) noexcept
	{
		// copy constructor
		if (this != &other)
		{
			this->capacity(other.size());

			detail::__fcopy__<native, native>(this->__head__(), other.__head__(),
			                                                    other.__tail__());

			this->__size__(other.size());
		}
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr str<native, malloc>::str(/*&*/ str&& other) noexcept
	{
		// move constructor
		if (this != &other)
		{
			std::swap(this->bytes,
			          other.bytes);
		}
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::operator=(const str& other) noexcept -> str&
	{
		// copy assignment
		if (this != &other)
		{
			this->capacity(other.size());

			detail::__fcopy__<native, native>(this->__head__(), other.__head__(),
			                                                    other.__tail__());

			this->__size__(other.size());
		}
		return *this;
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::operator=(/*&*/ str&& other) noexcept -> str&
	{
		// move assignment
		if (this != &other)
		{
			std::swap(this->bytes,
			          other.bytes);
		}
		return *this;
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::begin() const noexcept -> decltype(auto) // requires nothing; always active
	{
		return txt<native> {this->__head__(), this->__tail__()}.begin();
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::end() const noexcept -> decltype(auto) // requires nothing; always active
	{
		return txt<native> {this->__head__(), this->__tail__()}.end();
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::begin() /*&*/ noexcept -> forward_iterator // requires nothing; always active
	{
		typedef typename forward_iterator::state common;

		return {std::make_shared<common>(this, this->__head__()),
		        (size_t {0x0}),
		        (size_t {0x0}),
		        forward_iterator::zero_t::HEAD /* ptr anchor */};
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::end() /*&*/ noexcept -> forward_iterator // requires nothing; always active
	{
		typedef typename forward_iterator::state common;

		return {std::make_shared<common>(this, this->__tail__()),
		        (size_t {0x0}),
		        this->length(),
		        forward_iterator::zero_t::TAIL /* ptr anchor */};
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::rbegin() const noexcept -> decltype(auto) requires (!codec<native>::is_stateful)
	{
		return txt<native> {this->__head__(), this->__tail__()}.rbegin();
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::rend() const noexcept -> decltype(auto) requires (!codec<native>::is_stateful)
	{
		return txt<native> {this->__head__(), this->__tail__()}.rend();
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::rbegin() /*&*/ noexcept -> reverse_iterator requires (!codec<native>::is_stateful)
	{
		typedef typename forward_iterator::state common;

		return {std::make_shared<common>(this, this->__tail__()),
		        (size_t {0x0}),
		        (size_t {0x0}),
		        reverse_iterator::zero_t::TAIL /* ptr anchor */};
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::rend() /*&*/ noexcept -> reverse_iterator requires (!codec<native>::is_stateful)
	{
		typedef typename forward_iterator::state common;

		return {std::make_shared<common>(this, this->__head__()),
		        (size_t {0x0}),
		        this->length(),
		        reverse_iterator::zero_t::HEAD /* ptr anchor */};
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	constexpr auto str<native, malloc>::operator=(string&& rhs)& noexcept -> str&
	{
		[this]<format_t exotic>(txt<exotic> slice)
		{
			   typedef typename codec<native>::unit_t T;
			// typedef typename codec<exotic>::unit_t U;

			if constexpr (native == exotic)
			{
				size_t size {slice.size()};

				this->capacity(      size      );
				T* const dest {this->__head__()};
				this->__size__(      size      );

				detail::__fcopy__<exotic, native>(dest, slice.head,
				                                        slice.tail);
			}

			if constexpr (native != exotic)
			{
				size_t size {size_t { 0 }};

				for (code_t code : slice)
				{
					size += codec<exotic>
					        ::size(code);
				}

				this->capacity(      size      );
				T* const dest {this->__head__()};
				this->__size__(      size      );

				detail::__fcopy__<exotic, native>(dest, slice.head,
				                                        slice.tail);
			}
		}
		(txt {rhs});

		return *this;
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::operator=(code_t   rhs)& noexcept -> str&
	{
		   typedef typename codec<native>::unit_t T;
		// typedef typename codec<exotic>::unit_t U;

		size_t size {size_t { 1 }};

		this->capacity(      size      );
		T* const dest {this->__head__()};
		this->__size__(      size      );

		detail::__fcopy__<native>(dest, rhs);

		return *this;
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	constexpr auto str<native, malloc>::operator=(string&& rhs)&& noexcept -> str&&
	{
		return *this = rhs;
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::operator=(code_t   rhs)&& noexcept -> str&&
	{
		return *this = rhs;
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	constexpr auto str<native, malloc>::operator+=(string&& rhs)& noexcept -> str&
	{
		[this]<format_t exotic>(txt<exotic> slice)
		{
			   typedef typename codec<native>::unit_t T;
			// typedef typename codec<exotic>::unit_t U;

			if constexpr (native == exotic)
			{
				size_t size {this->size()
				             +
				             slice.size()};

				this->capacity(      size      );
				T* const dest {this->__tail__()};
				this->__size__(      size      );

				detail::__fcopy__<exotic, native>(dest, slice.head,
				                                        slice.tail);
			}

			if constexpr (native != exotic)
			{
				size_t size {this->size()};

				for (code_t code : slice)
				{
					size += codec<exotic>
					        ::size(code);
				}

				this->capacity(      size      );
				T* const dest {this->__tail__()};
				this->__size__(      size      );

				detail::__fcopy__<exotic, native>(dest, slice.head,
				                                        slice.tail);
			}
		}
		(txt {rhs});

		return *this;
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::operator+=(code_t   rhs)& noexcept -> str&
	{
		   typedef typename codec<native>::unit_t T;
		// typedef typename codec<exotic>::unit_t U;

		size_t size {this->size()
		             +
		             size_t { 1 }};

		this->capacity(      size      );
		T* const dest {this->__tail__()};
		this->__size__(      size      );

		detail::__fcopy__<native>(dest, rhs);

		return *this;
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	constexpr auto str<native, malloc>::operator+=(string&& rhs)&& noexcept -> str&&
	{
		return *this + rhs;
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::operator+=(code_t   rhs)&& noexcept -> str&&
	{
		return *this += rhs;
	}

#pragma region SSO23

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr str<native, malloc>::buffer::operator const typename str<native, malloc>::unit_t*() const noexcept
	{
		return this->head;
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr str<native, malloc>::buffer::operator /*&*/ typename str<native, malloc>::unit_t*() /*&*/ noexcept
	{
		return this->head;
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr str<native, malloc>::~str() noexcept
	{
		typedef decltype(this->tag) allocator_t;

		if (this->__mode__() == mode_t::LARGE)
		{
			const auto location {this->large.head                   };
			const auto capacity {this->large.last - this->large.head};

			std::allocator_traits<allocator_t>::deallocate(this->tag, location,
			                                                          capacity);
		}
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::__init__() /*&*/ noexcept -> void
	{
		constexpr char init {MAX << SFT};

		this->small[0x0] = '\0'; // c-str
		this->bytes[RMB] = init; // SSO23
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::__mode__() const noexcept -> mode_t
	{
		assert((this->bytes[RMB] & MSK) == mode_t::SMALL
		       ||
		       (this->bytes[RMB] & MSK) == mode_t::LARGE);

		return static_cast<mode_t>(this->bytes[RMB] & MSK);
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::__head__() const noexcept -> const unit_t*
	{
		return this->__mode__() == mode_t::SMALL
		       ?
		       this->small // ✨ roeses are red, violets are blue
		       :
		       this->large; // ✨ roeses are red, violets are blue
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::__head__() /*&*/ noexcept -> /*&*/ unit_t*
	{
		return this->__mode__() == mode_t::SMALL
		       ?
		       this->small // ✨ roeses are red, violets are blue
		       :
		       this->large; // ✨ roeses are red, violets are blue
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::__tail__() const noexcept -> const unit_t*
	{
		return this->__mode__() == mode_t::SMALL
		       ?
		       &this->small[MAX - (this->bytes[RMB] >> SFT)]
		       :
		       &this->large[this->large.size /* ✨ as-is */];
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::__tail__() /*&*/ noexcept -> /*&*/ unit_t*
	{
		return this->__mode__() == mode_t::SMALL
		       ?
		       &this->small[MAX - (this->bytes[RMB] >> SFT)]
		       :
		       &this->large[this->large.size /* ✨ as-is */];
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::__last__() const noexcept -> const unit_t*
	{
		return this->__mode__() == mode_t::SMALL
		       ?
		       &this->small[MAX] // ✨ roeses are red, violets are blue
		       :
		       this->large.last; // ✨ roeses are red, violets are blue
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::__last__() /*&*/ noexcept -> /*&*/ unit_t*
	{
		return this->__mode__() == mode_t::SMALL
		       ?
		       &this->small[MAX] // ✨ roeses are red, violets are blue
		       :
		       this->large.last; // ✨ roeses are red, violets are blue
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::__size__(size_t value) noexcept -> void
	{
		switch (this->__mode__())
		{
			case mode_t::SMALL:
			{
				const auto slot {(MAX - value) << SFT};

				//┌────────────┐    ┌───────[LE]───────┐
				//│ 0b0XXXXXXX │ -> │ no need for skip │
				//└────────────┘    └──────────────────┘

				//┌────────────┐    ┌───────[BE]───────┐
				//│ 0bXXXXXXX0 │ -> │ skip right 1 bit │
				//└────────────┘    └──────────────────┘

				this->bytes[RMB] = slot;
				this->small[value] = '\0';
				break;
			}
			case mode_t::LARGE:
			{
				//               update
				//                 ↓↓
				//┌──────┬──────┬──────┬──────┐
				//│ head │ last │ size │ meta │
				//└──────┴──────┴──────┴──────┘

				this->large.size = value;
				this->large[value] = '\0';
				break;
			}
		}
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::capacity(/* getter */) const noexcept -> size_t
	{
		return this->__mode__() == mode_t::SMALL
		       ?
		       MAX // or calc std::ptrdiff_t like below
		       :
		       this->large.last - this->large.head - 1;
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::capacity(size_t value) /*&*/ noexcept -> void
	{
		if (this->capacity() < value)
		{
			typedef decltype(this->tag) allocator_t;

			unit_t* const head {std::allocator_traits<allocator_t>
			                       ::
			                       allocate(this->tag, value + 1)};
			// half open ptrn; one-past-the-end
			unit_t* const last {head + value + 1};

			unit_t* const old_head {this->__head__()};
			unit_t* const old_tail {this->__tail__()};

			detail::__fcopy__<native, native>(head, old_head,
			                                        old_tail);

			if (this->__mode__() == mode_t::LARGE)
			{
				const auto location {this->large.head                   };
				const auto capacity {this->large.last - this->large.head};

				std::allocator_traits<allocator_t>::deallocate(this->tag, location,
				                                                          capacity);
			}

			// cheaper to call __units__ than to call .size() as it skip ifs
			const auto size {detail::__units__<native>(old_head, old_tail)};

			this->large.head = head;
			this->large.last = last;
			this->large.size = size;
			this->large.meta = mode_t::LARGE;
		}
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::__insert__(unit_t* dest,
	                                               code_t  code,
	                                               int8_t  step) noexcept -> __insert__t
	{
		__insert__t out
		{
			.does_shift {false},
			.does_alloc {false},
		};

		const auto a {0 < step ? +step : -step};
		const auto b {codec<native>::size(code)};

		if (a == b)
		{
			out.dest = dest;

			//                       dest
			//                       ↓
			//┌────────┬─────────────┬───┬──────────────┬────────┐
			//│ <head> │ left buffer │ A │ right buffer │ <tail> │
			//├────────┼─────────────┼───┼──────────────┼────────┤
			//│ <head> │ left buffer │ B │ right buffer │ <tail> │
			//└────────┴─────────────┴───┴──────────────┴────────┘
			//                       ↑
			//                       out.dest

			codec<native>::encode(code, dest, step);
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

				detail::__rcopy__<native, native>(dest + b, dest + a,
				                                            tail + 0);

				codec<native>::encode(code, out.dest, +b);
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

				detail::__rcopy__<native, native>(dest - a + b, dest,
				                                                tail);

				codec<native>::encode(code, out.dest, -b);
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

				detail::__rcopy__<native, native>(dest + b, dest + a,
				                                            tail + 0);

				codec<native>::encode(code, out.dest, +b);
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

				detail::__rcopy__<native, native>(dest - a + b, dest,
				                                                tail);

				codec<native>::encode(code, out.dest, -b);
			}
			this->__size__(new_l);
		}
		return out;
	}

#pragma endregion SSO23
#pragma region reader

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr str<native, malloc>::reader::operator code_t() const noexcept
	{
		const unit_t* head {this->src->__head__()};
		const unit_t* tail {this->src->__tail__()};

		if constexpr (!codec<native>::is_variable
		              &&
		              !codec<native>::is_stateful)
		{
			// jump arithmetic
			head += this->key;

			if (this->key < this->src->size())
			{
				code_t code;

				codec<native>::decode(head, code, codec<native>::next(head));

				return code;
			}
			return '\0';
		}

		for (size_t i {0}; head < tail; ++i, head += codec<native>::next(head))
		{
			if (i == this->key)
			{
				code_t code;

				codec<native>::decode(head, code, codec<native>::next(head));

				return code;
			}
		}
		return '\0';
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::reader::operator==(code_t code) const noexcept -> bool
	{
		return this->operator code_t() == code;
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::reader::operator!=(code_t code) const noexcept -> bool
	{
		return this->operator code_t() != code;
	}

#pragma endregion reader
#pragma region writer

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr str<native, malloc>::writer::operator code_t() const noexcept
	{
		return reader {this->src, this->key};
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::writer::operator=(code_t code) noexcept -> writer&
	{
		const unit_t* head {this->src->__head__()};
		const unit_t* tail {this->src->__tail__()};

		if constexpr (!codec<native>::is_variable
		              &&
		              !codec<native>::is_stateful)
		{
			// jump arithmetic
			head += this->key;

			if (this->key < this->src->size())
			{
				this->src->__insert__(head, code, codec<native>::next(head));

				return *this;
			}
			return *this;
		}

		for (size_t i {0}; head < tail; ++i, head += codec<native>::next(head))
		{
			if (i == this->key)
			{
				this->src->__insert__(head, code, codec<native>::next(head));

				return *this;
			}
		}
		return *this;
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::writer::operator==(code_t code) const noexcept -> bool
	{
		return reader {this->src, this->key} == code;
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	constexpr auto str<native, malloc>::writer::operator!=(code_t code) const noexcept -> bool
	{
		return reader {this->src, this->key} != code;
	}

#pragma endregion writer
#pragma region cursor

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename alias,
	          format_t trait>
	constexpr str<native, malloc>::cursor<alias, trait>::operator const typename codec<native>::unit_t*() const noexcept
	{
		return this->__dest__();
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename alias,
	          format_t trait>
	constexpr str<native, malloc>::cursor<alias, trait>::operator const typename codec<native>::unit_t*() /*&*/ noexcept
	{
		return this->__dest__();
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename alias,
	          format_t trait>
	constexpr auto str<native, malloc>::cursor<alias, trait>::operator*() const noexcept -> value_type
	{
		return {this->common, this->__dest__(), this->policy};
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename alias,
	          format_t trait>
	constexpr auto str<native, malloc>::cursor<alias, trait>::operator++(   ) noexcept -> alias&
	{
		/**/ if constexpr (trait == "LTR")
		{
			this->offset += codec<native>::next(this->__dest__());
		}
		else if constexpr (trait == "RTL")
		{
			this->offset -= codec<native>::back(this->__dest__());
		}
		// Δ distance
		++this->weight;

		return static_cast<alias&>(*this);
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename alias,
	          format_t trait>
	constexpr auto str<native, malloc>::cursor<alias, trait>::operator++(int) noexcept -> alias
	{
		const auto clone {*this};
		               ++(*this);

		return static_cast<alias&>(clone);
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename alias,
	          format_t trait>
	constexpr auto str<native, malloc>::cursor<alias, trait>::operator--(   ) noexcept -> alias&
	{
		/**/ if constexpr (trait == "LTR")
		{
			this->offset -= codec<native>::back(this->__dest__());
		}
		else if constexpr (trait == "RTL")
		{
			this->offset += codec<native>::next(this->__dest__());
		}
		// Δ distance
		--this->weight;

		return static_cast<alias&>(*this);
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename alias,
	          format_t trait>
	constexpr auto str<native, malloc>::cursor<alias, trait>::operator--(int) noexcept -> alias
	{
		const auto clone {*this};
		               --(*this);

		return static_cast<alias&>(clone);
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename alias,
	          format_t trait>
	constexpr auto str<native, malloc>::cursor<alias, trait>::operator+(size_t value) noexcept -> alias
	{
		/*&*/ auto clone {*this};

		for (size_t i {0}; i < value; ++i)
		{
			++clone;
		}
		return static_cast<alias&>(clone);
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename alias,
	          format_t trait>
	constexpr auto str<native, malloc>::cursor<alias, trait>::operator-(size_t value) noexcept -> alias
	{
		/*&*/ auto clone {*this};

		for (size_t i {0}; i < value; ++i)
		{
			--clone;
		}
		return static_cast<alias&>(clone);
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename alias,
	          format_t trait>
	constexpr auto str<native, malloc>::cursor<alias, trait>::operator+=(size_t value) noexcept -> alias&
	{
		// /*&*/ auto clone {*this};

		for (size_t i {0}; i < value; ++i)
		{
			++(*this);
		}
		return static_cast<alias&>(*this);
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename alias,
	          format_t trait>
	constexpr auto str<native, malloc>::cursor<alias, trait>::operator-=(size_t value) noexcept -> alias&
	{
		// /*&*/ auto clone {*this};

		for (size_t i {0}; i < value; ++i)
		{
			--(*this);
		}
		return static_cast<alias&>(*this);
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename alias,
	          format_t trait>
	constexpr auto str<native, malloc>::cursor<alias, trait>::operator==(const cursor& rhs) const noexcept -> bool
	{
		       // ① short-circuit?
		return this->weight /* Δ */ == rhs.weight /* Δ */
		       &&
		       // ② delay ptr deref
		       this->common->target == rhs.common->target;
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename alias,
	          format_t trait>
	constexpr auto str<native, malloc>::cursor<alias, trait>::operator!=(const cursor& rhs) const noexcept -> bool
	{
		       // ① short-circuit?
		return this->weight /* Δ */ != rhs.weight /* Δ */
		       ||
		       // ② delay ptr deref
		       this->common->target != rhs.common->target;
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename alias,
	          format_t trait>
	constexpr auto str<native, malloc>::cursor<alias, trait>::__incr__() const noexcept -> int8_t
	{
		/**/ if constexpr (trait == "LTR")
		{
			return codec<native>::next(this->__dest__());
		}
		else if constexpr (trait == "RTL")
		{
			return codec<native>::back(this->__dest__());
		}
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename alias,
	          format_t trait>
	constexpr auto str<native, malloc>::cursor<alias, trait>::__decr__() const noexcept -> int8_t
	{
		/**/ if constexpr (trait == "LTR")
		{
			return codec<native>::back(this->__dest__());
		}
		else if constexpr (trait == "RTL")
		{
			return codec<native>::next(this->__dest__());
		}
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename alias,
	          format_t trait>
	constexpr auto str<native, malloc>::cursor<alias, trait>::__dest__() const noexcept -> unit_t*
	{
		switch (this->policy)
		{
			[[likely]] case zero_t::HEAD:
			{
				return this->common->anchor + this->offset;
			}
			/* ugh? */ case zero_t::TAIL:
			{
				return this->common->anchor - this->offset;
			}
		}
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename alias,
	          format_t trait>
	constexpr str<native, malloc>::cursor<alias, trait>::proxy::operator code_t() const noexcept
	{
		int8_t step;

		/**/ if constexpr (trait == "LTR")
		{
			step = codec<native>::next(this->needle);
		}
		else if constexpr (trait == "RTL")
		{
			step = codec<native>::back(this->needle);
		}

		code_t code;

		codec<native>::decode(this->needle, code, step);

		return code;
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename alias,
	          format_t trait>
	constexpr auto str<native, malloc>::cursor<alias, trait>::proxy::operator=(code_t code) noexcept -> proxy&
	{
		int8_t step;

		/**/ if constexpr (trait == "LTR")
		{
			step = codec<native>::next(this->needle);
		}
		else if constexpr (trait == "RTL")
		{
			step = codec<native>::back(this->needle);
		}

		// insert at needle
		const auto info {this->common->target
		->
		__insert__(this->needle, code, step)};

		// refresh stale ptr
		switch (this->policy)
		{
			[[likely]] case zero_t::HEAD:
			{
				if (info.does_shift)
				{
					this->common->anchor = this->common->target->__head__();
				}
				this->needle = info.dest;
				break;
			}
			/* ugh? */ case zero_t::TAIL:
			{
				if (info.does_alloc
				    ||
				    info.does_shift)
				{
					this->common->anchor = this->common->target->__tail__();
				}
				this->needle = info.dest;
				break;
			}
		}

		return *this;
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename alias,
	          format_t trait>
	constexpr auto str<native, malloc>::cursor<alias, trait>::proxy::operator==(code_t code) const noexcept -> bool
	{
		return this->operator code_t() == code;
	}

	template <format_t native,
	          template <class>
	          typename malloc>
	template <typename alias,
	          format_t trait>
	constexpr auto str<native, malloc>::cursor<alias, trait>::proxy::operator!=(code_t code) const noexcept -> bool
	{
		return this->operator code_t() != code;
	}

#pragma endregion cursor

}
