#pragma once

//=====================//
#include "x69/string.hpp"
//=====================//

namespace x69
{
	template <format_t native,
	          typename malloc>
	constexpr str<native, malloc>::operator const unit_t*() const noexcept
	{
		return this->head();
	}

	template <format_t native,
	          typename malloc>
	constexpr str<native, malloc>::operator /*&*/ unit_t*() /*&*/ noexcept
	{
		return this->head();
	}

	template <format_t native,
	          typename malloc>
	constexpr str<native, malloc>::str(const str& other) noexcept
	{
		// copy constructor
		if (this != &other)
		{
			this->capacity(other.size());

			detail::__fcopy__<native, native>(this->head(),
			                                  other.head(),
			                                  other.tail());

			this->__size__(other.size());
		}
	}

	template <format_t native,
	          typename malloc>
	constexpr str<native, malloc>::str(/*&*/ str&& other) noexcept
	{
		// move constructor
		if (this != &other)
		{
			std::swap(this->store.__union__.bytes,
			          other.store.__union__.bytes);
		}
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::operator=(const str& other) noexcept -> str&
	{
		// copy assignment
		if (this != &other)
		{
			this->capacity(other.size());

			detail::__fcopy__<native, native>(this->head(),
			                                  other.head(),
			                                  other.tail());

			this->__size__(other.size());
		}
		return *this;
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::operator=(/*&*/ str&& other) noexcept -> str&
	{
		// move assignment
		if (this != &other)
		{
			std::swap(this->store.__union__.bytes,
			          other.store.__union__.bytes);
		}
		return *this;
	}

	template <format_t native,
	          typename malloc>
	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	constexpr str<native, malloc>::str(string&& value) noexcept
	{
		this->operator=(value);
	}

	template <format_t native,
	          typename malloc>
	constexpr str<native, malloc>::str(code_t   value) noexcept
	{
		this->operator=(value);
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::begin() /*&*/ noexcept -> forward_iterator // requires nothing; always active
	{
		typedef typename forward_iterator::state common;

		return {std::make_shared<common>(this, this->head()),
		        (size_t {0x0}),
		        (size_t {0x0}),
		        forward_iterator::zero_t::HEAD /* (align) */};
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::end() /*&*/ noexcept -> forward_iterator // requires nothing; always active
	{
		typedef typename forward_iterator::state common;

		return {std::make_shared<common>(this, this->tail()),
		        (size_t {0x0}),
		        this->length(),
		        forward_iterator::zero_t::TAIL /* (align) */};
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::rbegin() /*&*/ noexcept -> reverse_iterator requires (!codec<native>::is_stateful)
	{
		typedef typename forward_iterator::state common;

		return {std::make_shared<common>(this, this->tail()),
		        (size_t {0x0}),
		        (size_t {0x0}),
		        reverse_iterator::zero_t::TAIL /* (align) */};
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::rend() /*&*/ noexcept -> reverse_iterator requires (!codec<native>::is_stateful)
	{
		typedef typename forward_iterator::state common;

		return {std::make_shared<common>(this, this->head()),
		        (size_t {0x0}),
		        this->length(),
		        reverse_iterator::zero_t::HEAD /* (align) */};
	}

	template <format_t native,
	          typename malloc>
	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	constexpr auto str<native, malloc>::operator=(string&& rhs)& noexcept -> str&
	{
		[this]<format_t exotic>(txt<exotic> slice)
		{
			using T = typename codec<native>::unit_t;
			using U = typename codec<exotic>::unit_t;

			if constexpr (native == exotic)
			{
				size_t size {slice.size()};

				this->capacity(    size    );
				T* const dest {this->head()};
				this->__size__(    size    );

				detail::__fcopy__<exotic, native>(      dest,
				                                  slice.head,
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

				this->capacity(    size    );
				T* const dest {this->head()};
				this->__size__(    size    );

				detail::__fcopy__<exotic, native>(      dest,
				                                  slice.head,
				                                  slice.tail);
			}
		}
		(txt {rhs});

		return *this;
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::operator=(code_t   rhs)& noexcept -> str&
	{
		   using T = typename codec<native>::unit_t;
		// using U = typename codec<exotic>::unit_t;

		size_t size {size_t { 1 }};

		this->capacity(    size    );
		T* const dest {this->head()};
		this->__size__(    size    );

		detail::__fcopy__<native>(dest, rhs);

		return *this;
	}

	template <format_t native,
	          typename malloc>
	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	constexpr auto str<native, malloc>::operator=(string&& rhs)&& noexcept -> str&&
	{
		return *this = rhs;
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::operator=(code_t   rhs)&& noexcept -> str&&
	{
		return *this = rhs;
	}

	template <format_t native,
	          typename malloc>
	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	constexpr auto str<native, malloc>::operator+=(string&& rhs)& noexcept -> str&
	{
		[this]<format_t exotic>(txt<exotic> slice)
		{
			using T = typename codec<native>::unit_t;
			using U = typename codec<exotic>::unit_t;

			if constexpr (native == exotic)
			{
				size_t size {this->size()
				             +
				             slice.size()};

				this->capacity(    size    );
				T* const dest {this->tail()};
				this->__size__(    size    );

				detail::__fcopy__<exotic, native>(      dest,
				                                  slice.head,
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

				this->capacity(    size    );
				T* const dest {this->tail()};
				this->__size__(    size    );

				detail::__fcopy__<exotic, native>(      dest,
				                                  slice.head,
				                                  slice.tail);
			}
		}
		(txt {rhs});

		return *this;
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::operator+=(code_t   rhs)& noexcept -> str&
	{
		   using T = typename codec<native>::unit_t;
		// using U = typename codec<exotic>::unit_t;

		size_t size {this->size()
		             +
		             size_t { 1 }};

		this->capacity(    size    );
		T* const dest {this->tail()};
		this->__size__(    size    );

		detail::__fcopy__<native>(dest, rhs);

		return *this;
	}

	template <format_t native,
	          typename malloc>
	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	constexpr auto str<native, malloc>::operator+=(string&& rhs)&& noexcept -> str&&
	{
		return *this + rhs;
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::operator+=(code_t   rhs)&& noexcept -> str&&
	{
		return *this += rhs;
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::__insert__(unit_t* dest, code_t code, int8_t step) noexcept -> __insert__t
	{
		__insert__t out
		{
			.dest {dest},
			.does_shift {false},
			.does_alloc {false},
		};

		const auto a {0 < step ? +step : -step};
		const auto b {codec<native>::size(code)};

		if (a == b)
		{
			// no need to shift buffer :D
			codec<native>::encode(code, dest, step);
		}
		else if (a < b)
		{
			   out.does_shift = true;
			// out.does_alloc = true;
			const auto old_l {this->size()};
			const auto new_l {old_l - a+b };

			unit_t* tail {this->tail()};

			// 2x capacity growth
			if (this->capacity() < new_l)
			{
				// out.does_shift = true;
				   out.does_alloc = true;

				/**/ if (0 < step)
				{
					const auto off {dest - this->head()};

					this->capacity(old_l * 2);

					dest = this->head() + off;
					tail = this->tail()      ;
				}
				else if (step < 0)
				{
					const auto off {this->tail() - dest};

					this->capacity(old_l * 2);

					dest = this->tail() - off;
					tail = this->tail()      ;
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

				detail::__rcopy__<native, native>(dest + b,
				                                  dest + a,
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

				detail::__rcopy__<native, native>(dest - a + b,
				                                  dest - 0 + 0,
				                                  tail - 0 + 0);

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

			unit_t* tail {this->tail()};

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

				detail::__rcopy__<native, native>(dest + b,
				                                  dest + a,
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

				detail::__rcopy__<native, native>(dest - a + b,
				                                  dest - 0 + 0,
				                                  tail - 0 + 0);

				codec<native>::encode(code, out.dest, -b);
			}
			this->__size__(new_l);
		}
		return out;
	}

#pragma region SSO23

	template <format_t native,
	          typename malloc>
	constexpr str<native, malloc>::buffer::operator const typename str<native, malloc>::unit_t*() const noexcept
	{
		return this->head;
	}

	template <format_t native,
	          typename malloc>
	constexpr str<native, malloc>::buffer::operator /*&*/ typename str<native, malloc>::unit_t*() /*&*/ noexcept
	{
		return this->head;
	}

	template <format_t native,
	          typename malloc>
	constexpr str<native, malloc>::storage::storage() noexcept
	{
		static constexpr const auto init {MAX << SFT};

		this->__union__.small[0x0] = '\0'; // c-str
		this->__union__.bytes[RMB] = init; // SSO23
	}

	template <format_t native,
	          typename malloc>
	constexpr str<native, malloc>::storage::~storage() noexcept
	{
		if (this->mode() == LARGE)
		{
			std::allocator_traits<malloc>::deallocate((*this),
			                                          (*this).__union__.large.head,
			                                          (*this).__union__.large.last
			                                          -
			                                          (*this).__union__.large.head);
		}
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::storage::mode() const noexcept -> mode_t
	{
		return static_cast<mode_t>(this->__union__.bytes[RMB] & MSK);
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::storage::mode() /*&*/ noexcept -> mode_t
	{
		return static_cast<mode_t>(this->__union__.bytes[RMB] & MSK);
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::head() const noexcept -> const unit_t*
	{
		return this->store.mode() == SMALL
		       ?
		       this->store.__union__.small // ✨ roeses are red, violets are blue
		       :
		       this->store.__union__.large; // ✨ roeses are red, violets are blue
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::head() /*&*/ noexcept -> /*&*/ unit_t*
	{
		return this->store.mode() == SMALL
		       ?
		       this->store.__union__.small // ✨ roeses are red, violets are blue
		       :
		       this->store.__union__.large; // ✨ roeses are red, violets are blue
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::tail() const noexcept -> const unit_t*
	{
		return this->store.mode() == SMALL
		       ?
		       &this->store.__union__.small[MAX - (this->store.__union__.bytes[RMB] >> SFT)]
		       :
		       &this->store.__union__.large[this->store.__union__.large.size /* get as-is */];
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::tail() /*&*/ noexcept -> /*&*/ unit_t*
	{
		return this->store.mode() == SMALL
		       ?
		       &this->store.__union__.small[MAX - (this->store.__union__.bytes[RMB] >> SFT)]
		       :
		       &this->store.__union__.large[this->store.__union__.large.size /* get as-is */];
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::last() const noexcept -> const unit_t*
	{
		return this->store.mode() == SMALL
		       ?
		       &this->store.__union__.small[MAX] // ✨ roeses are red, violets are blue
		       :
		       this->store.__union__.large.last; // ✨ roeses are red, violets are blue
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::last() /*&*/ noexcept -> /*&*/ unit_t*
	{
		return this->store.mode() == SMALL
		       ?
		       &this->store.__union__.small[MAX] // ✨ roeses are red, violets are blue
		       :
		       this->store.__union__.large.last; // ✨ roeses are red, violets are blue
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::capacity(/* getter */) const noexcept -> size_t
	{
		return this->store.mode() == SMALL
		       ?
		       MAX // or calculate the ptrdiff_t just as large mode as shown down below
		       :
		       this->store.__union__.large.last - this->store.__union__.large.head - 1;
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::capacity(size_t value) /*&*/ noexcept -> void
	{
		if (this->capacity() < value)
		{
			const auto size {detail::__units__<native>(this->head(), this->tail())};

			// step 1: request
			unit_t* head {std::allocator_traits<malloc>::allocate(this->store, value + 1)};
			unit_t* last {/* (half open ptrn) one-past-the-end */(head/*<&>*/+ value + 1)};

			// step 2: migrate
			detail::__fcopy__<native, native>(head,
			                                  this->head(),
			                                  this->tail());

			// step 3: release
			if (this->store.mode() == LARGE)
			{
				std::allocator_traits<malloc>::deallocate(this->store,
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

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::__size__(size_t value) noexcept -> void
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
#pragma region cursor

	template <format_t native,
	          typename malloc>
	template <typename alias, format_t trait> constexpr str<native, malloc>::cursor<alias, trait>::operator const typename codec<native>::unit_t*() const noexcept
	{
		return this->__needle__();
	}

	template <format_t native,
	          typename malloc>
	template <typename alias, format_t trait> constexpr str<native, malloc>::cursor<alias, trait>::operator const typename codec<native>::unit_t*() /*&*/ noexcept
	{
		return this->__needle__();
	}

	template <format_t native,
	          typename malloc>
	template <typename alias, format_t trait> constexpr auto str<native, malloc>::cursor<alias, trait>::operator*() const noexcept -> value_type
	{
		switch (this->policy)
		{
			case zero_t::HEAD: { return {this->common, this->__needle__(), this->policy}; }
			case zero_t::TAIL: { return {this->common, this->__needle__(), this->policy}; }
		}
	}

	template <format_t native,
	          typename malloc>
	template <typename alias, format_t trait> constexpr auto str<native, malloc>::cursor<alias, trait>::__needle__() const noexcept -> unit_t*
	{
		switch (this->policy)
		{
			case zero_t::HEAD: { return this->common->anchor + this->offset /* L → R */; }
			case zero_t::TAIL: { return this->common->anchor - this->offset /* R → L */; }
		}
	}

	template <format_t native,
	          typename malloc>
	template <typename alias, format_t trait> constexpr auto str<native, malloc>::cursor<alias, trait>::operator++(   ) noexcept -> alias&
	{
		/**/ if constexpr (trait == "LTR") this->offset += codec<native>::next(this->__needle__());
		else if constexpr (trait == "RTL") this->offset -= codec<native>::back(this->__needle__());

		++this->weight;

		return static_cast<alias&>(*this);
	}

	template <format_t native,
	          typename malloc>
	template <typename alias, format_t trait> constexpr auto str<native, malloc>::cursor<alias, trait>::operator++(int) noexcept -> alias
	{
		const auto clone {*this};
		               ++(*this);

		return static_cast<alias&>(clone);
	}

	template <format_t native,
	          typename malloc>
	template <typename alias, format_t trait> constexpr auto str<native, malloc>::cursor<alias, trait>::operator--(   ) noexcept -> alias&
	{
		/**/ if constexpr (trait == "LTR") this->offset -= codec<native>::back(this->__needle__());
		else if constexpr (trait == "RTL") this->offset += codec<native>::next(this->__needle__());

		--this->weight;

		return static_cast<alias&>(*this);
	}

	template <format_t native,
	          typename malloc>
	template <typename alias, format_t trait> constexpr auto str<native, malloc>::cursor<alias, trait>::operator--(int) noexcept -> alias
	{
		const auto clone {*this};
		               --(*this);

		return static_cast<alias&>(clone);
	}

	template <format_t native,
	          typename malloc>
	template <typename alias, format_t trait> constexpr auto str<native, malloc>::cursor<alias, trait>::operator+(size_t value) noexcept -> alias
	{
		/*&*/ auto clone {*this};

		for (size_t i {0}; i < value; ++i) { ++clone; }

		return static_cast<alias&>(clone);
	}

	template <format_t native,
	          typename malloc>
	template <typename alias, format_t trait> constexpr auto str<native, malloc>::cursor<alias, trait>::operator-(size_t value) noexcept -> alias
	{
		/*&*/ auto clone {*this};

		for (size_t i {0}; i < value; ++i) { --clone; }

		return static_cast<alias&>(clone);
	}

	template <format_t native,
	          typename malloc>
	template <typename alias, format_t trait> constexpr auto str<native, malloc>::cursor<alias, trait>::operator+=(size_t value) noexcept -> alias&
	{
		// auto clone {*this};

		for (size_t i {0}; i < value; ++i) { ++(*this); }

		return static_cast<alias&>(*this);
	}

	template <format_t native,
	          typename malloc>
	template <typename alias, format_t trait> constexpr auto str<native, malloc>::cursor<alias, trait>::operator-=(size_t value) noexcept -> alias&
	{
		// auto clone {*this};

		for (size_t i {0}; i < value; ++i) { --(*this); }

		return static_cast<alias&>(*this);
	}

	template <format_t native,
	          typename malloc>
	template <typename alias, format_t trait> constexpr auto str<native, malloc>::cursor<alias, trait>::operator==(const cursor& rhs) const noexcept -> bool
	{
		return this->weight == rhs.weight && this->common->target == rhs.common->target;
	}

	template <format_t native,
	          typename malloc>
	template <typename alias, format_t trait> constexpr auto str<native, malloc>::cursor<alias, trait>::operator!=(const cursor& rhs) const noexcept -> bool
	{
		return this->weight != rhs.weight || this->common->target != rhs.common->target;
	}

	template <format_t native,
	          typename malloc>
	template <typename alias, format_t trait> [[nodiscard]] constexpr str<native, malloc>::cursor<alias, trait>::proxy::operator code_t() const noexcept
	{
		code_t code;

		codec<native>::decode(this->needle, code, [&]
		{
			/**/ if constexpr (trait == "LTR") return codec<native>::next(this->needle);
			else if constexpr (trait == "RTL") return codec<native>::back(this->needle);
		}
		());

		return code;
	}

	template <format_t native,
	          typename malloc>
	template <typename alias, format_t trait> constexpr auto str<native, malloc>::cursor<alias, trait>::proxy::operator=(code_t code) noexcept -> proxy&
	{
		const auto mutate {this->common->target->__insert__(this->needle, code, [&]
		{
			/**/ if constexpr (trait == "LTR") return codec<native>::next(this->needle);
			else if constexpr (trait == "RTL") return codec<native>::back(this->needle);
		}
		())};

		switch (this->policy)
		{
			case zero_t::HEAD:
			{
				if (mutate.does_alloc)
				{
					this->common->anchor // stale
					=
					this->common->target->head();
				}
				this->needle = mutate.dest;
				break;
			}
			case zero_t::TAIL:
			{
				if (mutate.does_alloc
				    ||
				    mutate.does_shift)
				{
					this->common->anchor // stale
					=
					this->common->target->tail();
				}
				this->needle = mutate.dest;
				break;
			}
		}

		return *this;
	}

	template <format_t native,
	          typename malloc>
	template <typename alias, format_t trait> constexpr auto str<native, malloc>::cursor<alias, trait>::proxy::operator==(code_t code) const noexcept -> bool
	{
		return this->operator code_t() == code;
	}

	template <format_t native,
	          typename malloc>
	template <typename alias, format_t trait> constexpr auto str<native, malloc>::cursor<alias, trait>::proxy::operator!=(code_t code) const noexcept -> bool
	{
		return this->operator code_t() != code;
	}

#pragma endregion cursor
#pragma region reader

	template <format_t native,
	          typename malloc>
	[[nodiscard]] constexpr str<native, malloc>::reader::operator code_t() const noexcept
	{
		const unit_t* head {this->src->head()};
		const unit_t* tail {this->src->tail()};

		if constexpr (!codec<native>::is_variable
		              &&
		              !codec<native>::is_stateful)
		{
			// jump arithmetic
			head += this->arg;

			if (this->arg < this->src->size())
			{
				code_t code;

				codec<native>::decode(head, code, codec<native>::next(head));

				return code;
			}
			return '\0';
		}

		for (size_t i {0}; head < tail; ++i, head += codec<native>::next(head))
		{
			if (i == this->arg)
			{
				code_t code;

				codec<native>::decode(head, code, codec<native>::next(head));

				return code;
			}
		}
		return '\0';
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::reader::operator==(code_t code) const noexcept -> bool
	{
		return this->operator code_t() == code;
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::reader::operator!=(code_t code) const noexcept -> bool
	{
		return this->operator code_t() != code;
	}

#pragma endregion reader
#pragma region writer

	template <format_t native,
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
			head += this->arg;

			if (this->arg < this->src->size())
			{
				this->src->__insert__(head, code, codec<native>::next(head));

				return *this;
			}
			return *this;
		}

		for (size_t i {0}; head < tail; ++i, head += codec<native>::next(head))
		{
			if (i == this->arg)
			{
				this->src->__insert__(head, code, codec<native>::next(head));

				return *this;
			}
		}
		return *this;
	}

	template <format_t native,
	          typename malloc>
	[[nodiscard]] constexpr str<native, malloc>::writer::operator code_t() const noexcept
	{
		return reader {this->src, this->arg}.operator code_t();
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::writer::operator==(code_t code) const noexcept -> bool
	{
		return reader {this->src, this->arg}.operator==(code);
	}

	template <format_t native,
	          typename malloc>
	constexpr auto str<native, malloc>::writer::operator!=(code_t code) const noexcept -> bool
	{
		return reader {this->src, this->arg}.operator!=(code);
	}

#pragma endregion writer

}
