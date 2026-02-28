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

static constexpr auto UnicodeData() noexcept -> std::array<props, 0x10FFFF + 1>;
static constexpr auto CaseFolding() noexcept -> std::array<props, 0x10FFFF + 1>;

int main() noexcept
{
	// TODO
}

static constexpr auto UnicodeData() noexcept -> std::array<props, 0x10FFFF + 1>
{
	decltype(UnicodeData()) table;

	std::visit([&](const auto&& file)
	{
		for (auto line : file.split('\n'))
		{
			// TODO
		}
	},
	x69::fileof(UnicodeDataTXT).value());

	return table;
}

static constexpr auto CaseFolding() noexcept -> std::array<props, 0x10FFFF + 1>
{
	decltype(CaseFolding()) table;

	std::visit([&](const auto&& file)
	{
		for (auto line : file.split('\n'))
		{
			// TODO
		}
	},
	x69::fileof(CaseFoldingTXT).value());

	return table;
}
