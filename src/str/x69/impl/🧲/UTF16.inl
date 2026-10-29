#pragma once

//───────────────────"
#include "x69/str.hpp"
//───────────────────"

constexpr auto x69::UTF_16::size([[maybe_unused]] const codeᵖᵗ& code) noexcept -> stride
{
	return 1 + (code > 0xFFFF); // T = 1
}

constexpr auto x69::UTF_16::next([[maybe_unused]] const codeᵘᵗ* data) noexcept -> stride
{
	return +1 + ((data[+0] & 0xFC00) == 0xD800);
}

constexpr auto x69::UTF_16::back([[maybe_unused]] const codeᵘᵗ* data) noexcept -> stride
{
	return -1 - ((data[-1] & 0xFC00) == 0xDC00);
}

constexpr auto x69::UTF_16::encode(const codeᵖᵗ& in, codeᵘᵗ* out, stride dir) noexcept -> void
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
			const auto code {in - 0x10000};

			out[+0] = 0xD800 | (code / 0x400);
			out[+1] = 0xDC00 | (code & 0x3FF);
			break;
		}
		case -2:
		{
			const auto code {in - 0x10000};

			out[-2] = 0xD800 | (code / 0x400);
			out[-1] = 0xDC00 | (code & 0x3FF);
			break;
		}
		// cannot recover from ts T^T
		default: { assert(!"corrupt");
		           std::unreachable(); }
	}
}

constexpr auto x69::UTF_16::decode(const codeᵘᵗ* in, codeᵖᵗ& out, stride dir) noexcept -> void
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
			out = 0x10000 // supplymentary
			      +
			      ((in[+0] - 0xD800) << 10)
			      +
			      ((in[+1] - 0xDC00) << 00);
			break;
		}
		case -2:
		{
			out = 0x10000 // supplymentary
			      +
			      ((in[-2] - 0xD800) << 10)
			      +
			      ((in[-1] - 0xDC00) << 00);
			break;
		}
		// cannot recover from ts T^T
		default: { assert(!"corrupt");
		           std::unreachable(); }
	}
}
