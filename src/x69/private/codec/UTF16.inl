#pragma once

//=====================//
#include "x69/string.hpp"
//=====================//

namespace x69
{
	constexpr auto codec<"UTF-16">::size([[maybe_unused]] /*&*/ code_t code) noexcept -> int8_t
	{
		//┌───────────────────────┐
		//│ U+000000 ... U+00D7FF │ -> 1 code unit
		//│ U+00E000 ... U+00FFFF │ -> 1 code unit
		//│ U+000000 ... U+10FFFF │ -> 2 code unit
		//└───────────────────────┘

		return 1 + (0xFFFF /* pair? */ < code);
	}

	constexpr auto codec<"UTF-16">::next([[maybe_unused]] const unit_t* data) noexcept -> int8_t
	{
		constexpr static const int8_t table[]
		{
			//┌──────────────────────────────────┐
			//│ 0x0000 ... 0xD7FF -> head of BMP │
			//└──────────────────────────────────┘
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			//┌──────────────────────────────────┐
			//│ 0xD800 ... 0xDBFF -> H surrogate │
			//└──────────────────────────────────┘
			2, 2,
			//┌──────────────────────────────────┐
			//│ 0xDC00 ... 0xDFFF -> L surrogate │
			//└──────────────────────────────────┘
			0, 0,
			//┌──────────────────────────────────┐
			//│ 0xE000 ... 0xFFFF -> rest of BMP │
			//└──────────────────────────────────┘
			1, 1,
			1, 1,
			1, 1,
			1, 1,
		};

		return table[(data[0] >> 10) & 0x3F];
	}

	constexpr auto codec<"UTF-16">::back([[maybe_unused]] const unit_t* data) noexcept -> int8_t
	{
		int8_t i {-1};
		// until start byte sequence is found...
		for (; (data[i] >> 0xA) == 0x37; --i) {}
		return i;
	}

	constexpr auto codec<"UTF-16">::encode(const code_t in, unit_t* out, int8_t step) noexcept -> void
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
			case +2:
			{
				const char32_t code {in - 0x10000};
				out[+0] = 0xD800 | (code / 0x400);
				out[+1] = 0xDC00 | (code & 0x3FF);
				break;
			}
			case -2:
			{
				const char32_t code {in - 0x10000};
				out[-2] = 0xD800 | (code / 0x400);
				out[-1] = 0xDC00 | (code & 0x3FF);
				break;
			}
			// 💢 ugh..? cannot recover
			default: { assert(!"corrupt");
			           std::unreachable(); }
		}
	}

	constexpr auto codec<"UTF-16">::decode(const unit_t* in, code_t& out, int8_t step) noexcept -> void
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
			case +2:
			{
				out = 0x10000 // supplymentary
				      |
				      ((in[+0] - 0xD800) << 10)
				      |
				      ((in[+1] - 0xDC00) << 00);
				break;
			}
			case -2:
			{
				out = 0x10000 // supplymentary
				      |
				      ((in[-2] - 0xD800) << 10)
				      |
				      ((in[-1] - 0xDC00) << 00);
				break;
			}
			// 💢 ugh..? cannot recover
			default: { assert(!"corrupt");
			           std::unreachable(); }
		}
	}
}
