#pragma once

//====================//
#include "../str.hpp" //
//====================//

namespace x69
{
	template <format_t native
	          /* no malloc */>
	constexpr auto txt<native>::begin() const noexcept -> forward_iterator // requires nothing; always active
	{
		return {this->head};
	}

	template <format_t native
	          /* no malloc */>
	constexpr auto txt<native>::end() const noexcept -> forward_iterator // requires nothing; always active
	{
		return {this->tail};
	}

	template <format_t native
	          /* no malloc */>
	constexpr auto txt<native>::rbegin() const noexcept -> reverse_iterator requires (!codec<native>::is_stateful)
	{
		return {this->tail};
	}

	template <format_t native
	          /* no malloc */>
	constexpr auto txt<native>::rend() const noexcept -> reverse_iterator requires (!codec<native>::is_stateful)
	{
		return {this->head};
	}

#pragma region reader

	template <format_t native
	          /* no malloc */>
	constexpr txt<native>::reader::operator code_t() const noexcept
	{
		const unit_t* head {this->src->head};
		const unit_t* tail {this->src->tail};

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

	template <format_t native
	          /* no malloc */>
	constexpr auto txt<native>::reader::operator==(code_t code) const noexcept -> bool
	{
		return this->operator code_t() == code;
	}

	template <format_t native
	          /* no malloc */>
	constexpr auto txt<native>::reader::operator!=(code_t code) const noexcept -> bool
	{
		return this->operator code_t() != code;
	}

#pragma endregion reader
#pragma region writer

	template <format_t native
	          /* no malloc */>
	constexpr txt<native>::writer::operator code_t() const noexcept
	{
		return reader {this->src, this->key};
	}

	template <format_t native
	          /* no malloc */>
	constexpr auto txt<native>::writer::operator==(code_t code) const noexcept -> bool
	{
		return reader {this->src, this->key} == code;
	}

	template <format_t native
	          /* no malloc */>
	constexpr auto txt<native>::writer::operator!=(code_t code) const noexcept -> bool
	{
		return reader {this->src, this->key} != code;
	}

#pragma endregion writer
#pragma region cursor

	template <format_t native
	          /* no malloc */>
	template <typename alias,
	          format_t trait>
	constexpr txt<native>::cursor<alias, trait>::operator const typename codec<native>::unit_t*() const noexcept
	{
		return this->ptr;
	}

	template <format_t native
	          /* no malloc */>
	template <typename alias,
	          format_t trait>
	constexpr txt<native>::cursor<alias, trait>::operator const typename codec<native>::unit_t*() /*&*/ noexcept
	{
		return this->ptr;
	}

	template <format_t native
	          /* no malloc */>
	template <typename alias,
	          format_t trait>
	constexpr auto txt<native>::cursor<alias, trait>::operator*() const noexcept -> value_type
	{
		int8_t step;

		/**/ if constexpr (trait == "LTR")
		{
			step = codec<native>::next(this->ptr);
		}
		else if constexpr (trait == "RTL")
		{
			step = codec<native>::back(this->ptr);
		}

		code_t code;

		codec<native>::decode(this->ptr, code, step);

		return code;
	}

	template <format_t native
	          /* no malloc */>
	template <typename alias,
	          format_t trait>
	constexpr auto txt<native>::cursor<alias, trait>::operator++(   ) noexcept -> alias&
	{
		/**/ if constexpr (trait == "LTR")
		{
			this->ptr += codec<native>::next(this->ptr);
		}
		else if constexpr (trait == "RTL")
		{
			this->ptr += codec<native>::back(this->ptr);
		}

		return static_cast<alias&>(*this);
	}

	template <format_t native
	          /* no malloc */>
	template <typename alias,
	          format_t trait>
	constexpr auto txt<native>::cursor<alias, trait>::operator++(int) noexcept -> alias
	{
		const auto clone {*this};
		               ++(*this);

		return static_cast<alias&>(clone);
	}

	template <format_t native
	          /* no malloc */>
	template <typename alias,
	          format_t trait>
	constexpr auto txt<native>::cursor<alias, trait>::operator--(   ) noexcept -> alias&
	{
		/**/ if constexpr (trait == "LTR")
		{
			this->ptr += codec<native>::back(this->ptr);
		}
		else if constexpr (trait == "RTL")
		{
			this->ptr += codec<native>::next(this->ptr);
		}

		return static_cast<alias&>(*this);
	}

	template <format_t native
	          /* no malloc */>
	template <typename alias,
	          format_t trait>
	constexpr auto txt<native>::cursor<alias, trait>::operator--(int) noexcept -> alias
	{
		const auto clone {*this};
		               --(*this);

		return static_cast<alias&>(clone);
	}

	template <format_t native
	          /* no malloc */>
	template <typename alias,
	          format_t trait>
	constexpr auto txt<native>::cursor<alias, trait>::operator+(size_t value) noexcept -> alias
	{
		/*&*/ auto clone {*this};

		for (size_t i {0}; i < value; ++i)
		{
			++clone;
		}
		return static_cast<alias&>(clone);
	}

	template <format_t native
	          /* no malloc */>
	template <typename alias,
	          format_t trait>
	constexpr auto txt<native>::cursor<alias, trait>::operator-(size_t value) noexcept -> alias
	{
		/*&*/ auto clone {*this};

		for (size_t i {0}; i < value; ++i)
		{
			--clone;
		}
		return static_cast<alias&>(clone);
	}

	template <format_t native
	          /* no malloc */>
	template <typename alias,
	          format_t trait>
	constexpr auto txt<native>::cursor<alias, trait>::operator+=(size_t value) noexcept -> alias&
	{
		// /*&*/ auto clone {*this};

		for (size_t i {0}; i < value; ++i)
		{
			++(*this);
		}
		return static_cast<alias&>(*this);
	}

	template <format_t native
	          /* no malloc */>
	template <typename alias,
	          format_t trait>
	constexpr auto txt<native>::cursor<alias, trait>::operator-=(size_t value) noexcept -> alias&
	{
		// /*&*/ auto clone {*this};

		for (size_t i {0}; i < value; ++i)
		{
			--(*this);
		}
		return static_cast<alias&>(*this);
	}

	template <format_t native
	          /* no malloc */>
	template <typename alias,
	          format_t trait>
	constexpr auto txt<native>::cursor<alias, trait>::__incr__() const noexcept -> int8_t
	{
		/**/ if constexpr (trait == "LTR")
		{
			return codec<native>::next(this->ptr);
		}
		else if constexpr (trait == "RTL")
		{
			return codec<native>::back(this->ptr);
		}
	}

	template <format_t native
	          /* no malloc */>
	template <typename alias,
	          format_t trait>
	constexpr auto txt<native>::cursor<alias, trait>::__decr__() const noexcept -> int8_t
	{
		/**/ if constexpr (trait == "LTR")
		{
			return codec<native>::back(this->ptr);
		}
		else if constexpr (trait == "RTL")
		{
			return codec<native>::next(this->ptr);
		}
	}

#pragma endregion cursor

}
