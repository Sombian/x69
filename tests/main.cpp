#include <ranges>
#include <vector>
#include <variant>
#include <utility>
#include <iostream>

#include "x69/string.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#ifndef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

int main() noexcept
{
	#ifdef _MSC_VER//############//;
	std::system("chcp 65001 > NUL");
	#endif//MSC_VER//############//;

	x69::str8 str {u8"ABCDEFGHIJKLMNOPQRSTUVW"};

	for (auto code : str | std::views::reverse)
	{
		code = U'♥'; std::cout << str << '\n';
		code = U'?'; std::cout << str << '\n';
	}
}

#endif//DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#ifdef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

TEST_CASE("string")
{
	SUBCASE("SSO23")
	{
		x69::str8 small {u8"#마도카☆마기카!"};
		x69::str8 large {u8"#마도카☆마기카ƒ"};

		CHECK(small.size() == small.capacity());
		CHECK(large.size() == large.capacity());
	}

	SUBCASE("utils")
	{
		x69::txt8 _67 {u8"67"};
		x69::txt8 _FF {u8"FF"};

		CHECK(_67.stoi(10) == 6'7);
		CHECK(_FF.stoi(16) == 255);
	}

	SUBCASE("index")
	{
		x69::txt8 str {u8"티라미수"};

		using x69::range::N;

		CHECK(str[0] == U'티');
		CHECK(str[1] == U'라');
		CHECK(str[2] == U'미');
		CHECK(str[3] == U'수');

		CHECK(str.substr(0, N) == str);
		CHECK(str.substr(0, 4) == str);
	}

	SUBCASE("A + B")
	{
		x69::txt8 티라 {u8"티라"};
		x69::txt8 미수 {u8"미수"};

		CHECK(미수 == u"미수");
		CHECK(미수.length() == 2);

		CHECK(티라 == u"티라");
		CHECK(티라.length() == 2);

		x69::str8 X미수 {'?' + 미수};
		x69::str8 티라X {티라 + '?'};

		CHECK(X미수 == u"?미수");
		CHECK(X미수.length() == 3);

		CHECK(티라X == u"티라?");
		CHECK(티라X.length() == 3);
	}

	SUBCASE("split")
	{
		x69::txt8 str {u8"티라미수"
		               u8"☆"
		               u8"치즈케잌"
		               u8"☆"
		               u8"말차라떼"};

		auto split {str.split(U"☆")};

		CHECK(split[0] == u"티라미수");
		CHECK(split[1] == u"치즈케잌");
		CHECK(split[2] == u"말차라떼");

		CHECK(split[0] == u8"티라미수");
		CHECK(split[1] == u8"치즈케잌");
		CHECK(split[2] == u8"말차라떼");

		CHECK(str.split(str).size() == 0);
		CHECK(str.match(str).size() == 1);
	}

	SUBCASE("range")
	{
		x69::txt8 src {u8"티라미수"
		               u8"☆"
		               u8"치즈케잌"
		               u8"☆"
		               u8"말차라떼"};

		std::vector<x69::code_t> foo;
		std::vector<x69::code_t> bar;

		if (/*&*/ x69::str8 str {src}; 0 < str.size())
		{
			for (auto code : str) foo.push_back(code);
		}
		if (const x69::str8 str {src}; 0 < str.size())
		{
			for (auto code : str) bar.push_back(code);
		}

		CHECK(std::ranges::equal(foo.begin(), foo.end(),
		                         bar.begin(), bar.end()));
	}
}

TEST_CASE("fileof")
{
	SUBCASE("UTF-8")
	{
		const auto file {x69::fileof("./tests/data/utf8.txt")};

		REQUIRE(file.has_value());

		CHECK(std::holds_alternative<x69::str8>(file.value()));
	}

	SUBCASE("UTF-16-LE")
	{
		const auto file {x69::fileof("./tests/data/utf16le.txt")};

		REQUIRE(file.has_value());

		CHECK(std::holds_alternative<x69::str16>(file.value()));
	}

	SUBCASE("UTF-16-BE")
	{
		const auto file {x69::fileof("./tests/data/utf16be.txt")};

		REQUIRE(file.has_value());

		CHECK(std::holds_alternative<x69::str16>(file.value()));
	}
}

#endif//DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
