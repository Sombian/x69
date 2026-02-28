#pragma once

//=====================//
#include "x69/string.hpp"
//=====================//

namespace x69
{
	constexpr auto codec<"UTF-32">::size([[maybe_unused]] /*&*/ code_t code) noexcept -> int8_t
	{
		return 1;
	}

	constexpr auto codec<"UTF-32">::next([[maybe_unused]] const unit_t* data) noexcept -> int8_t
	{
		return +1;
	}

	constexpr auto codec<"UTF-32">::back([[maybe_unused]] const unit_t* data) noexcept -> int8_t
	{
		return -1;
	}

	constexpr auto codec<"UTF-32">::encode(const code_t in, unit_t* out, int8_t step) noexcept -> void
	{
		switch (step)
		{
			case +1:
			{
				out[+0] = static_cast<unit_t>(in);
				break;
			}
			case -1:
			{
				out[-1] = static_cast<unit_t>(in);
				break;
			}
			// 💢 ugh..? cannot recover
			default: { assert(!"corrupt");
			           std::unreachable(); }
		}
	}

	constexpr auto codec<"UTF-32">::decode(const unit_t* in, code_t& out, int8_t step) noexcept -> void
	{
		switch (step)
		{
			case +1:
			{
				out = static_cast<char32_t>(in[+0]);
				break;
			}
			case -1:
			{
				out = static_cast<char32_t>(in[-1]);
				break;
			}
			// 💢 ugh..? cannot recover
			default: { assert(!"corrupt");
			           std::unreachable(); }
		}
	}
}
