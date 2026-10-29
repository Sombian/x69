#pragma once

//───────────────────"
#include "x69/str.hpp"
//───────────────────"

constexpr auto x69::UTF_8::size([[maybe_unused]] const codeᵖᵗ& code) noexcept -> stride
{
	return 1 + (code > 0x007F)  // T = 1
	         + (code > 0x07FF)  // T = 1
	         + (code > 0xFFFF); // T = 1
}

constexpr auto x69::UTF_8::next([[maybe_unused]] const codeᵘᵗ* data) noexcept -> stride
{
	return 1 + (data[+0] >= 0xC0)  // T = 1
	         + (data[+0] >= 0xE0)  // T = 1
	         + (data[+0] >= 0xF0); // T = 1
}

constexpr auto x69::UTF_8::back([[maybe_unused]] const codeᵘᵗ* data) noexcept -> stride
{
	return (data[-1] & 0xC0) != 0x80 ? -1
	     : (data[-2] & 0xC0) != 0x80 ? -2
	     : (data[-3] & 0xC0) != 0x80 ? -3
	     :                             -4;
}

constexpr auto x69::UTF_8::encode(const codeᵖᵗ& in, codeᵘᵗ* out, stride dir) noexcept -> void
{
	switch (dir)
	{
		case +1:
		{
			out[+0] = static_cast<codeᵘᵗ>(in);
			break;
		}
		case -1:
		{
			out[-1] = static_cast<codeᵘᵗ>(in);
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
		// cannot recover from ts T^T
		default: { assert(!"corrupt");
		           std::unreachable(); }
	}
}

constexpr auto x69::UTF_8::decode(const codeᵘᵗ* in, codeᵖᵗ& out, stride dir) noexcept -> void
{
	switch (dir)
	{
		case +1:
		{
			out = static_cast<codeᵖᵗ>(in[+0]);
			break;
		}
		case -1:
		{
			out = static_cast<codeᵖᵗ>(in[-1]);
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
		// cannot recover from ts T^T
		default: { assert(!"corrupt");
		           std::unreachable(); }
	}
}
