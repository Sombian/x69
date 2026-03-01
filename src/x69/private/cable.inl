#pragma once

//=====================//
#include "x69/string.hpp"
//=====================//

namespace x69
{
	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::head() const noexcept -> const unit_t*
	{
		if constexpr (requires { static_cast<const derive*>(this)->head(); })
		     return static_cast<const derive*>(this)->head();
		else return static_cast<const derive*>(this)->head  ;
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::tail() const noexcept -> const unit_t*
	{
		if constexpr (requires { static_cast<const derive*>(this)->tail(); })
		     return static_cast<const derive*>(this)->tail();
		else return static_cast<const derive*>(this)->tail  ;
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::size() const noexcept -> size_t
	{
		return detail::__units__<native>(this->head(), this->tail());
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::length() const noexcept -> size_t
	{
		return detail::__codes__<native>(this->head(), this->tail());
	}

	template <format_t native,
	          typename derive>
	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	constexpr auto cable<native, derive>::split(string&& value) const noexcept -> std::vector<txt<native>>
	{
		return [&self = *this]<format_t exotic>(txt<exotic> span)
		{
			return detail::__split__<native, exotic>(self.head(), self.tail(),
			                                         span.head  , span.tail  );
		}
		(txt {value});
	}

	template <format_t native,
	          typename exotic>
	constexpr auto cable<native, exotic>::split(code_t   value) const noexcept -> std::vector<txt<native>>
	{
		return detail::__split__<native>(this->head(), this->tail(), value);
	}

	template <format_t native,
	          typename derive>
	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	constexpr auto cable<native, derive>::match(string&& value) const noexcept -> std::vector<txt<native>>
	{
		return [&self = *this]<format_t exotic>(txt<exotic> span)
		{
			return detail::__match__<native, exotic>(self.head(), self.tail(),
			                                         span.head  , span.tail  );
		}
		(txt {value});
	}

	template <format_t native,
	          typename exotic>
	constexpr auto cable<native, exotic>::match(code_t   value) const noexcept -> std::vector<txt<native>>
	{
		return detail::__match__<native>(this->head(), this->tail(), value);
	}

	template <format_t native,
	          typename derive>
	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	constexpr auto cable<native, derive>::contains(string&& value) const noexcept -> bool
	{
		return [&self = *this]<format_t exotic>(txt<exotic> span)
		{
			return detail::__holds__<native, exotic>(self.head(), self.tail(),
			                                         span.head  , span.tail  );
		}
		(txt {value});
	}

	template <format_t native,
	          typename exotic>
	constexpr auto cable<native, exotic>::contains(code_t   value) const noexcept -> bool
	{
		return detail::__holds__<native>(this->head(), this->tail(), value);
	}

	template <format_t native,
	          typename derive>
	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	constexpr auto cable<native, derive>::starts_with(string&& value) const noexcept -> bool
	{
		return [&self = *this]<format_t exotic>(txt<exotic> span)
		{
			return detail::__swith__<native, exotic>(self.head(), self.tail(),
			                                         span.head  , span.tail  );
		}
		(txt {value});
	}

	template <format_t native,
	          typename exotic>
	constexpr auto cable<native, exotic>::starts_with(code_t   value) const noexcept -> bool
	{
		return detail::__swith__<native>(this->head(), this->tail(), value);
	}

	template <format_t native,
	          typename derive>
	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	constexpr auto cable<native, derive>::ends_with(string&& value) const noexcept -> bool
	{
		return [&self = *this]<format_t exotic>(txt<exotic> span)
		{
			return detail::__ewith__<native, exotic>(self.head(), self.tail(),
			                                         span.head  , span.tail  );
		}
		(txt {value});
	}

	template <format_t native,
	          typename exotic>
	constexpr auto cable<native, exotic>::ends_with(code_t   value) const noexcept -> bool
	{
		return detail::__ewith__<native>(this->head(), this->tail(), value);
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::substr(clamp  start, clamp  until) const noexcept -> txt<native>
	{
		return detail::__substr__<native>(this->head(), this->tail(), start, until);
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::substr(clamp  start, range  until) const noexcept -> txt<native>
	{
		return detail::__substr__<native>(this->head(), this->tail(), start, until);
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::substr(size_t start, clamp  until) const noexcept -> txt<native>
	{
		return detail::__substr__<native>(this->head(), this->tail(), start, until);
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::substr(size_t start, range  until) const noexcept -> txt<native>
	{
		return detail::__substr__<native>(this->head(), this->tail(), start, until);
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::substr(size_t start, size_t until) const noexcept -> txt<native>
	{
		return detail::__substr__<native>(this->head(), this->tail(), start, until);
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::stoi(uint8_t radix) const noexcept -> size_t
	{
		assert(2 <= radix && radix <= 36);

		size_t value {0};

		constexpr static const auto table {[]
		{
			std::array<uint8_t, 256> impl {255};

			for (uint8_t i {'0'}; i <= '9'; ++i)
			{
				impl[i] = i - '0' + 0x0;
			}
			for (uint8_t i {'a'}; i <= 'z'; ++i)
			{
				impl[i] = i - 'a' + 0xA;
			}
			for (uint8_t i {'A'}; i <= 'Z'; ++i)
			{
				impl[i] = i - 'A' + 0xA;
			}
			return impl; // 1 to 1 digit mapping
		}
		()};

		for (code_t code : *this)
		{
			value = value * radix
			        +
			        (table[code]);
		}

		return value;
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::stof(uint8_t radix) const noexcept -> double
	{
		assert(2 <= radix && radix <= 36);

		double value {0};
		size_t scale {0};

		bool _ {false};

		constexpr static const auto table {[]
		{
			std::array<uint8_t, 256> impl {255};

			for (uint8_t i {'0'}; i <= '9'; ++i)
			{
				impl[i] = i - '0' + 0x0;
			}
			for (uint8_t i {'a'}; i <= 'z'; ++i)
			{
				impl[i] = i - 'a' + 0xA;
			}
			for (uint8_t i {'A'}; i <= 'Z'; ++i)
			{
				impl[i] = i - 'A' + 0xA;
			}
			return impl; // 1 to 1 digit mapping
		}
		()};

		for (code_t code : *this)
		{
			if (code == '.')
			{
				_ = true;
				continue;
			}

			value = value * radix
			        +
			        (table[code]);

			if (_ /* ??? */)
			{
				scale *= radix;
			}
		}

		return value / scale;
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::begin() const noexcept -> const_forward_iterator // requires nothing; always active
	{
		return {this->head()};
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::end() const noexcept -> const_forward_iterator // requires nothing; always active
	{
		return {this->tail()};
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::rbegin() const noexcept -> const_reverse_iterator requires (!codec<native>::is_stateful)
	{
		return {this->tail()};
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::rend() const noexcept -> const_reverse_iterator requires (!codec<native>::is_stateful)
	{
		return {this->head()};
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::operator[](size_t value) const noexcept -> decltype(auto)
	{
		return typename derive::reader {static_cast<derive*>(this), value};
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::operator[](size_t value) /*&*/ noexcept -> decltype(auto)
	{
		return typename derive::writer {static_cast<derive*>(this), value};
	}

	template <format_t native,
	          typename derive>
	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	constexpr auto cable<native, derive>::operator==(string&& rhs) const noexcept -> bool
	{
		return [&self = *this]<format_t exotic>(txt<exotic> span)
		{
			return detail::__equal__<native, exotic>(self.head(), self.tail(),
			                                         span.head  , span.tail  );
		}
		(txt {rhs});
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::operator==(code_t   rhs) const noexcept -> bool
	{
		return detail::__equal__<native>(this->head(), this->tail(), rhs);
	}

	template <format_t native,
	          typename derive>
	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	constexpr auto cable<native, derive>::operator!=(string&& rhs) const noexcept -> bool
	{
		return [&self = *this]<format_t exotic>(txt<exotic> span)
		{
			return detail::__nqual__<native, exotic>(self.head(), self.tail(),
			                                         span.head  , span.tail  );
		}
		(txt {rhs});
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::operator!=(code_t   rhs) const noexcept -> bool
	{
		return detail::__nqual__<native>(this->head(), this->tail(), rhs);
	}

#pragma region concat

	template <format_t native,
	          typename derive>
	template <typename lhs_t,
	          typename rhs_t>
	template <format_t exotic,
	          typename malloc>
	constexpr cable<native, derive>::concat<lhs_t, rhs_t>::operator str<exotic, malloc>() const noexcept
	{
		using T = typename codec<native>::unit_t;
		using U = typename codec<exotic>::unit_t;

		str<exotic, malloc> out;

		// step 1. calc size
		std::size_t size {0};

		this->__for_each__([&](auto data)
		{
			typedef decltype(data) X;

			if constexpr (std::is_same_v<X, code_t>)
			{
				size += codec<exotic>::size(data);
			}

			if constexpr (!std::is_same_v<X, code_t>)
			{
				[&]<format_t rarity>(txt<rarity> span)
				{
					if constexpr (exotic == rarity)
					{
						size += detail::__units__<exotic>(span.head, span.tail);
					}

					if constexpr (exotic != rarity)
					{
						for (auto code : span) size += codec<exotic>::size(code);
					}
				}
				(data);
			}
		});

		// step 2. sets size
		out.capacity(size);
		out.__size__(size);

		// step 3. sets data
		U* dest {out.head()};

		this->__for_each__([&](auto data)
		{
			typedef decltype(data) X;

			if constexpr (std::is_same_v<X, code_t>)
			{
				dest += detail::__fcopy__<exotic>(dest,
				                                  data);
			}

			if constexpr (!std::is_same_v<X, code_t>)
			{
				[&]<format_t rarity>(txt<rarity> span)
				{
					dest += detail::__fcopy__<rarity, exotic>(     dest,
					                                          span.head,
					                                          span.tail);
				}
				(data);
			}
		});

		return out;
	}

	template <format_t native,
	          typename derive>
	template <typename lhs_t,
	          typename rhs_t>
	constexpr auto cable<native, derive>::concat<lhs_t, rhs_t>::__for_each__(const auto&& fun) const noexcept -> void
	{
		if constexpr (requires(lhs_t l) { l.__for_each__(fun); })
		{ this->lhs.__for_each__(fun); } else { fun(this->lhs); }

		if constexpr (requires(rhs_t r) { r.__for_each__(fun); })
		{ this->rhs.__for_each__(fun); } else { fun(this->rhs); }
	}

#pragma endregion concat
#pragma region cursor

	template <format_t native,
	          typename derive>
	template <typename alias,
	          format_t trait>
	constexpr cable<native, derive>::cursor<alias, trait>::operator const typename codec<native>::unit_t*() const noexcept
	{
		return this->ptr;
	}

	template <format_t native,
	          typename derive>
	template <typename alias,
	          format_t trait>
	constexpr cable<native, derive>::cursor<alias, trait>::operator const typename codec<native>::unit_t*() /*&*/ noexcept
	{
		return this->ptr;
	}

	template <format_t native,
	          typename derive>
	template <typename alias,
	          format_t trait>
	constexpr auto cable<native, derive>::cursor<alias, trait>::operator*() const noexcept -> value_type
	{
		code_t code;

		codec<native>::decode(this->ptr, code, [&]
		{
			/**/ if constexpr (trait == "LTR") return codec<native>::next(this->ptr);
			else if constexpr (trait == "RTL") return codec<native>::back(this->ptr);
		}
		());

		return code;
	}

	template <format_t native,
	          typename derive>
	template <typename alias,
	          format_t trait>
	constexpr auto cable<native, derive>::cursor<alias, trait>::operator++(   ) noexcept -> alias&
	{
		/**/ if constexpr (trait == "LTR") this->ptr += codec<native>::next(this->ptr);
		else if constexpr (trait == "RTL") this->ptr += codec<native>::back(this->ptr);

		return static_cast<alias&>(*this);
	}

	template <format_t native,
	          typename derive>
	template <typename alias,
	          format_t trait>
	constexpr auto cable<native, derive>::cursor<alias, trait>::operator++(int) noexcept -> alias
	{
		const auto clone {*this};
		               ++(*this);

		return static_cast<alias&>(clone);
	}

	template <format_t native,
	          typename derive>
	template <typename alias,
	          format_t trait>
	constexpr auto cable<native, derive>::cursor<alias, trait>::operator--(   ) noexcept -> alias&
	{
		/**/ if constexpr (trait == "LTR") this->ptr += codec<native>::back(this->ptr);
		else if constexpr (trait == "RTL") this->ptr += codec<native>::next(this->ptr);

		return static_cast<alias&>(*this);
	}

	template <format_t native,
	          typename derive>
	template <typename alias,
	          format_t trait>
	constexpr auto cable<native, derive>::cursor<alias, trait>::operator--(int) noexcept -> alias
	{
		const auto clone {*this};
		               --(*this);

		return static_cast<alias&>(clone);
	}

	template <format_t native,
	          typename derive>
	template <typename alias,
	          format_t trait>
	constexpr auto cable<native, derive>::cursor<alias, trait>::operator+(size_t value) noexcept -> alias
	{
		/*&*/ auto clone {*this};

		for (size_t i {0}; i < value; ++i) { ++clone; }

		return static_cast<alias&>(clone);
	}

	template <format_t native,
	          typename derive>
	template <typename alias,
	          format_t trait>
	constexpr auto cable<native, derive>::cursor<alias, trait>::operator-(size_t value) noexcept -> alias
	{
		/*&*/ auto clone {*this};

		for (size_t i {0}; i < value; ++i) { --clone; }

		return static_cast<alias&>(clone);
	}

	template <format_t native,
	          typename derive>
	template <typename alias,
	          format_t trait>
	constexpr auto cable<native, derive>::cursor<alias, trait>::operator+=(size_t value) noexcept -> alias&
	{
		// auto clone {*this};

		for (size_t i {0}; i < value; ++i) { ++(*this); }

		return static_cast<alias&>(*this);
	}

	template <format_t native,
	          typename derive>
	template <typename alias,
	          format_t trait>
	constexpr auto cable<native, derive>::cursor<alias, trait>::operator-=(size_t value) noexcept -> alias&
	{
		// auto clone {*this};

		for (size_t i {0}; i < value; ++i) { --(*this); }

		return static_cast<alias&>(*this);
	}

#pragma endregion cursor

}
