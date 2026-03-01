#include <cstdint>

#include <array>
#include <tuple>
#include <variant>
#include <utility>

#include "x69/string.hpp"

#define UnicodeDataTXT "./src/tools/data/UnicodeData.txt"
#define CaseFoldingTXT "./src/tools/data/CaseFolding.txt"

struct props
{
	struct mapping
	{
		uint16_t upper; // <────────────┐
		uint16_t lower; // <────────────┤
		uint16_t title; // <────────────┤
	}                   //              │
	case_mapping;       //              │
	                    //              │
	struct folding      //              │
	{                   //              │
		uint16_t index; // ───[equal?]──┘
		uint16_t width;
	}
	case_folding;
};

static constexpr auto case_mapping() noexcept -> std::array<props, 0x10FFFF + 1>;
static constexpr auto case_folding() noexcept -> std::array<props, 0x10FFFF + 1>;

int main() noexcept
{
	// TODO
}

static constexpr auto case_mapping() noexcept -> std::array<props, 0x10FFFF + 1>
{
	decltype(case_mapping()) table;

	std::visit([&table](const auto&& file)
	{
		for (auto line : file.split('\n'))
		{
			//┌───────────────────────┐
			//│ 00: code              │
			//│ 01: name              │
			//│ 02: type              │
			//│ 03: COMB class        │
			//│ 04: BIDI class        │
			//│ 05: COMP class        │
			//│ 06: <...>             │
			//│ 07: <...>             │
			//│ 08: <...>             │
			//│ 09: BIDI mirror       │
			//│ 10: <...>             │
			//│ 11: <...>             │
			//│ 12: uppercase mapping │
			//│ 13: lowercase mapping │
			//│ 14: titlecase mapping │
			//└───────────────────────┘

			auto info {line.split(';')};

			if (info[1].ends_with(u8"First>"))
			{

			}
			if (info[1].ends_with(u8"Last>"))
			{

			}
		}
	},
	x69::fileof(UnicodeDataTXT).value());

	return table;
}

static constexpr auto case_folding() noexcept -> std::array<props, 0x10FFFF + 1>
{
	decltype(case_folding()) table;

	std::visit([&table](const auto&& file)
	{
		for (auto line : file.split('\n'))
		{
			// TODO
		}
	},
	x69::fileof(CaseFoldingTXT).value());

	return table;
}
