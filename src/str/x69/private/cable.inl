#pragma once

//====================//
#include "../str.hpp" //
//====================//

namespace x69
{
	#define this_head [this] constexpr noexcept -> const unit_t*                  \
	{                                                                             \
	    if constexpr (requires { static_cast<const derive*>(this)->__head__(); }) \
	         return static_cast<const derive*>(this)->__head__();                 \
	    if constexpr (requires { static_cast<const derive*>(this)->  head    ; }) \
	         return static_cast<const derive*>(this)->  head    ;                 \
	    else static_assert("cannot get head ptr; is macro #define up-to-date?");  \
	}                                                                             \
	()                                                                            \

	#define this_tail [this] constexpr noexcept -> const unit_t*                  \
	{                                                                             \
	    if constexpr (requires { static_cast<const derive*>(this)->__tail__(); }) \
	         return static_cast<const derive*>(this)->__tail__();                 \
	    if constexpr (requires { static_cast<const derive*>(this)->  tail    ; }) \
	         return static_cast<const derive*>(this)->  tail    ;                 \
	    else static_assert("cannot get tail ptr; is macro #define up-to-date?");  \
	}                                                                             \
	()                                                                            \

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::size() const noexcept -> size_t
	{
		return detail::__units__<native>(this_head, this_tail);
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::length() const noexcept -> size_t
	{
		return detail::__codes__<native>(this_head, this_tail);
	}

	template <format_t native,
	          typename derive>
	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	constexpr auto cable<native, derive>::split(string&& value) const noexcept -> std::vector<txt<native>>
	{
		return [this]<format_t exotic>(txt<exotic> span)
		{
			return detail::__split__<native, exotic>(this_head, this_tail,
			                                         span.head, span.tail);
		}
		(txt {value});
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::split(code_t   value) const noexcept -> std::vector<txt<native>>
	{
		return detail::__split__<native>(this_head, this_tail, value);
	}

	template <format_t native,
	          typename derive>
	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	constexpr auto cable<native, derive>::match(string&& value) const noexcept -> std::vector<txt<native>>
	{
		return [this]<format_t exotic>(txt<exotic> span)
		{
			return detail::__match__<native, exotic>(this_head, this_tail,
			                                         span.head, span.tail);
		}
		(txt {value});
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::match(code_t   value) const noexcept -> std::vector<txt<native>>
	{
		return detail::__match__<native>(this_head, this_tail, value);
	}

	template <format_t native,
	          typename derive>
	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	constexpr auto cable<native, derive>::contains(string&& value) const noexcept -> bool
	{
		return [this]<format_t exotic>(txt<exotic> span)
		{
			return detail::__holds__<native, exotic>(this_head, this_tail,
			                                         span.head, span.tail);
		}
		(txt {value});
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::contains(code_t   value) const noexcept -> bool
	{
		return detail::__holds__<native>(this_head, this_tail, value);
	}

	template <format_t native,
	          typename derive>
	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	constexpr auto cable<native, derive>::starts_with(string&& value) const noexcept -> bool
	{
		return [this]<format_t exotic>(txt<exotic> span)
		{
			return detail::__swith__<native, exotic>(this_head, this_tail,
			                                         span.head, span.tail);
		}
		(txt {value});
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::starts_with(code_t   value) const noexcept -> bool
	{
		return detail::__swith__<native>(this_head, this_tail, value);
	}

	template <format_t native,
	          typename derive>
	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	constexpr auto cable<native, derive>::ends_with(string&& value) const noexcept -> bool
	{
		return [this]<format_t exotic>(txt<exotic> span)
		{
			return detail::__ewith__<native, exotic>(this_head, this_tail,
			                                         span.head, span.tail);
		}
		(txt {value});
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::ends_with(code_t   value) const noexcept -> bool
	{
		return detail::__ewith__<native>(this_head, this_tail, value);
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::substr(clamp  start, clamp  until) const noexcept -> txt<native>
	{
		return detail::__substr__<native>(this_head, this_tail, start, until);
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::substr(clamp  start, range  until) const noexcept -> txt<native>
	{
		return detail::__substr__<native>(this_head, this_tail, start, until);
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::substr(size_t start, clamp  until) const noexcept -> txt<native>
	{
		return detail::__substr__<native>(this_head, this_tail, start, until);
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::substr(size_t start, range  until) const noexcept -> txt<native>
	{
		return detail::__substr__<native>(this_head, this_tail, start, until);
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::substr(size_t start, size_t until) const noexcept -> txt<native>
	{
		return detail::__substr__<native>(this_head, this_tail, start, until);
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::stoi(uint8_t radix) const noexcept -> size_t
	{
		assert(2 <= radix && radix <= 36);

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

		size_t value {0};

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

		double value {0};
		size_t scale {0};

		bool _ {false};

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
	constexpr auto cable<native, derive>::operator[](size_t value) const noexcept -> decltype(auto)
	{
		return typename derive::reader {static_cast<const derive*>(this), value};
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::operator[](size_t value) /*&*/ noexcept -> decltype(auto)
	{
		return typename derive::writer {static_cast</*&*/ derive*>(this), value};
	}

	template <format_t native,
	          typename derive>
	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	constexpr auto cable<native, derive>::operator==(string&& rhs) const noexcept -> bool
	{
		return [this]<format_t exotic>(txt<exotic> span)
		{
			return detail::__equal__<native, exotic>(this_head, this_tail,
			                                         span.head, span.tail);
		}
		(txt {rhs});
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::operator==(code_t   rhs) const noexcept -> bool
	{
		return detail::__equal__<native>(this_head, this_tail, rhs);
	}

	template <format_t native,
	          typename derive>
	template <typename string> requires requires (string&& _)
	                                             { txt {_}; }
	constexpr auto cable<native, derive>::operator!=(string&& rhs) const noexcept -> bool
	{
		return [this]<format_t exotic>(txt<exotic> span)
		{
			return detail::__nqual__<native, exotic>(this_head, this_tail,
			                                         span.head, span.tail);
		}
		(txt {rhs});
	}

	template <format_t native,
	          typename derive>
	constexpr auto cable<native, derive>::operator!=(code_t   rhs) const noexcept -> bool
	{
		return detail::__nqual__<native>(this_head, this_tail, rhs);
	}

#pragma region concat

	template <format_t native,
	          typename derive>
	template <typename lhs_t,
	          typename rhs_t>
	template <format_t exotic,
	          template <class>
	          typename malloc>
	constexpr cable<native, derive>::concat<lhs_t, rhs_t>::operator str<exotic,
	                                                                    malloc>() const noexcept
	{
		// typedef typename codec<native>::unit_t T;
		   typedef typename codec<exotic>::unit_t U;

		str<exotic, malloc> copy;
		size_t size {0}; U* dest;

		this->__for_each__([&](auto data)
		{
			if constexpr (std::is_same_v<decltype(data), code_t>)
			{
				size += codec<exotic>::size(data);
			}

			if constexpr (!std::is_same_v<decltype(data), code_t>)
			{
				[&]<format_t _>(txt<_> span)
				{
					if constexpr (exotic == _)
					{
						size += detail::__units__<exotic>(span.head, span.tail);
					}

					if constexpr (exotic != _)
					{
						for (auto code : span) size += codec<exotic>::size(code);
					}
				}
				(data);
			}
		});

		       copy.capacity(size);
		dest = copy.__head__(    );
		       copy.__size__(size);

		// write
		this->__for_each__([&](auto data)
		{
			if constexpr (std::is_same_v<decltype(data), code_t>)
			{
				dest += detail::__fcopy__<exotic>(dest, data);
			}

			if constexpr (!std::is_same_v<decltype(data), code_t>)
			{
				[&]<format_t _>(txt<_> span)
				{
					dest += detail::__fcopy__<_, exotic>(dest, span.head,
					                                           span.tail);
				}
				(data);
			}
		});

		return copy;
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

	#undef this_head
	#undef this_tail
}
