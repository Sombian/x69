#pragma once

//───────────────────"
#include "x69/str.hpp"
//───────────────────"

constexpr auto x69::UTF_32::size([[maybe_unused]] const codeᵖᵗ& code) noexcept -> stride
{
	return +1;
}

constexpr auto x69::UTF_32::next([[maybe_unused]] const codeᵘᵗ* data) noexcept -> stride
{
	return +1;
}

constexpr auto x69::UTF_32::back([[maybe_unused]] const codeᵘᵗ* data) noexcept -> stride
{
	return -1;
}

constexpr auto x69::UTF_32::encode(const codeᵖᵗ& in, codeᵘᵗ* out, stride dir) noexcept -> void
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
		// cannot recover from ts T^T
		default: { assert(!"corrupt");
		           std::unreachable(); }
	}
}

constexpr auto x69::UTF_32::decode(const codeᵘᵗ* in, codeᵖᵗ& out, stride dir) noexcept -> void
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
		// cannot recover from ts T^T
		default: { assert(!"corrupt");
		           std::unreachable(); }
	}
}
