#pragma once

//=====================//
#include "x69/string.hpp"
//=====================//

namespace x69
{
	constexpr auto codec<"UTF-8">::size([[maybe_unused]] /*&*/ code_t code) noexcept -> int8_t
	{
		const size_t N (std::bit_width
		(static_cast<uint32_t>(code)));

		//┌───────────────────────┐
		//│ U+000000 ... U+00007F │ -> 1 code unit
		//│ U+000080 ... U+0007FF │ -> 2 code unit
		//│ U+000800 ... U+00FFFF │ -> 3 code unit
		//│ U+010000 ... U+10FFFF │ -> 4 code unit
		//└───────────────────────┘

		return 1 + (8 <= N) + (12 <= N) + (17 <= N);
	}

	constexpr auto codec<"UTF-8">::next([[maybe_unused]] const unit_t* data) noexcept -> int8_t
	{
		constexpr static const int8_t table[]
		{
			//┌─────────────────────────────┐
			//│ 0x0 ... 0xB -> single units │
			//└─────────────────────────────┘
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			1, 1,
			//┌─────────────────────────────┐
			//│ 0xC ... 0xF -> [[variable]] │
			//└─────────────────────────────┘
			2, 2,
			3, 4,
		};

		return table[(data[0] >> 0x4) & 0x0F];
	}

	constexpr auto codec<"UTF-8">::back([[maybe_unused]] const unit_t* data) noexcept -> int8_t
	{
		int8_t i {-1};
		// until start byte sequence is found...
		for (; (data[i] & 0xC0) == 0x80; --i) {}
		return i;
	}

	constexpr auto codec<"UTF-8">::encode(const code_t in, unit_t* out, int8_t step) noexcept -> void
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
				out[+0] = 0xC0 | ((in >> 06) & 0x1F);
				out[+1] = 0x80 | ((in >> 00) & 0x3F);
				break;
			}
			case -2:
			{
				out[-2] = 0xC0 | ((in >> 06) & 0x1F);
				out[-1] = 0x80 | ((in >> 00) & 0x3F);
				break;
			}
			case +3:
			{
				out[+0] = 0xE0 | ((in >> 12) & 0x0F);
				out[+1] = 0x80 | ((in >> 06) & 0x3F);
				out[+2] = 0x80 | ((in >> 00) & 0x3F);
				break;
			}
			case -3:
			{
				out[-3] = 0xE0 | ((in >> 12) & 0x0F);
				out[-2] = 0x80 | ((in >> 06) & 0x3F);
				out[-1] = 0x80 | ((in >> 00) & 0x3F);
				break;
			}
			case +4:
			{
				out[+0] = 0xF0 | ((in >> 18) & 0x07);
				out[+1] = 0x80 | ((in >> 12) & 0x3F);
				out[+2] = 0x80 | ((in >> 06) & 0x3F);
				out[+3] = 0x80 | ((in >> 00) & 0x3F);
				break;
			}
			case -4:
			{
				out[-4] = 0xF0 | ((in >> 18) & 0x07);
				out[-3] = 0x80 | ((in >> 12) & 0x3F);
				out[-2] = 0x80 | ((in >> 06) & 0x3F);
				out[-1] = 0x80 | ((in >> 00) & 0x3F);
				break;
			}
			// 💢 ugh..? cannot recover
			default: { assert(!"corrupt");
			           std::unreachable(); }
		}
	}

	constexpr auto codec<"UTF-8">::decode(const unit_t* in, code_t& out, int8_t step) noexcept -> void
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
				out = ((in[+0] & 0x1F) << 06)
				      |
				      ((in[+1] & 0x3F) << 00);
				break;
			}
			case -2:
			{
				out = ((in[-2] & 0x1F) << 06)
				      |
				      ((in[-1] & 0x3F) << 00);
				break;
			}
			case +3:
			{
				out = ((in[+0] & 0x0F) << 12)
				      |
				      ((in[+1] & 0x3F) << 06)
				      |
				      ((in[+2] & 0x3F) << 00);
				break;
			}
			case -3:
			{
				out = ((in[-3] & 0x0F) << 12)
				      |
				      ((in[-2] & 0x3F) << 06)
				      |
				      ((in[-1] & 0x3F) << 00);
				break;
			}
			case +4:
			{
				out = ((in[+0] & 0x07) << 18)
				      |
				      ((in[+1] & 0x3F) << 12)
				      |
				      ((in[+2] & 0x3F) << 06)
				      |
				      ((in[+3] & 0x3F) << 00);
				break;
			}
			case -4:
			{
				out = ((in[-4] & 0x07) << 18)
				      |
				      ((in[-3] & 0x3F) << 12)
				      |
				      ((in[-2] & 0x3F) << 06)
				      |
				      ((in[-1] & 0x3F) << 00);
				break;
			}
			// 💢 ugh..? cannot recover
			default: { assert(!"corrupt");
			           std::unreachable(); }
		}
	}
}
