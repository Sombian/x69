#pragma once

//=====================//
#include "x69/string.hpp"
//=====================//

namespace x69
{
#pragma region reader

	template <format_t native
	          /* allocator */>
	[[nodiscard]] constexpr txt<native>::reader::operator code_t() const noexcept
	{
		const unit_t* head {this->src->__head__};
		const unit_t* tail {this->src->__tail__};

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
		return U'\0';
	}

	template <format_t native
	          /* allocator */>
	constexpr auto txt<native>::reader::operator==(code_t code) const noexcept -> bool
	{
		return this->operator code_t() == code;
	}

	template <format_t native
	          /* allocator */>
	constexpr auto txt<native>::reader::operator!=(code_t code) const noexcept -> bool
	{
		return this->operator code_t() != code;
	}

#pragma endregion reader
#pragma region writer

	template <format_t native
	          /* allocator */>
	[[nodiscard]] constexpr txt<native>::writer::operator code_t() const noexcept
	{
		return reader {this->src, this->arg}.operator code_t();
	}

	template <format_t native
	          /* allocator */>
	constexpr auto txt<native>::writer::operator==(code_t code) const noexcept -> bool
	{
		return reader {this->src, this->arg}.operator==(code);
	}

	template <format_t native
	          /* allocator */>
	constexpr auto txt<native>::writer::operator!=(code_t code) const noexcept -> bool
	{
		return reader {this->src, this->arg}.operator!=(code);
	}

#pragma endregion writer

}
