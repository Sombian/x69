#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h" /*─────────*/
/*───────────────────────────/*─────────*/

#include "x69/str.hpp"

#include <ranges>
#include <vector>

TEST_CASE("SSO23")
{
	x69::str small {u8"ÃBCDEFGHIJKLMNOPQRSTUV"};
	x69::str large {u8"ÃBCDEFGHIJKLMNOPQRSTUṼ"};

	CHECK(small.size() == small.capacity());
	CHECK(large.size() == large.capacity());
}

TEST_CASE("A + B")
{
	const x69::str foo {u8"foo"}; // ← c_str
	const x69::txt bar {u8"bar"}; // ← slice

	CHECK(x69::str8 {foo + bar} == U"foobar");
	CHECK(x69::str8 {foo + foo} == U"foofoo");
}

TEST_CASE("range")
{
	const x69::txt str {u8"티라미수"};

	using x69::range::N;

	CHECK(str[0] == U'티');
	CHECK(str[1] == U'라');
	CHECK(str[2] == U'미');
	CHECK(str[3] == U'수');

	CHECK(str.substr(0, N) == str);
	CHECK(str.substr(0, 4) == str);
}

TEST_CASE("split")
{
	const x69::txt str {u8"티라미수"
	                    u8"☆"
	                    u8"치즈케잌"
	                    u8"☆"
	                    u8"말차라떼"};

	auto split {str.split(U"☆")};

	CHECK(split[0] ==  u"티라미수");
	CHECK(split[0] == u8"티라미수");

	CHECK(split[1] ==  u"치즈케잌");
	CHECK(split[1] == u8"치즈케잌");

	CHECK(split[2] ==  u"말차라떼");
	CHECK(split[2] == u8"말차라떼");
}

TEST_CASE("range")
{
	const x69::txt src {u8"티라미수"
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

TEST_CASE("UTF-8")
{
	const auto file {x69::fileof("./test/str/utf8std.txt")};

	REQUIRE(file.has_value());

	CHECK(std::holds_alternative<x69::str8>(file.value()));
}

TEST_CASE("UTF-16-LE")
{
	const auto file {x69::fileof("./test/str/utf16le.txt")};

	REQUIRE(file.has_value());

	CHECK(std::holds_alternative<x69::str16>(file.value()));
}

TEST_CASE("UTF-16-BE")
{
	const auto file {x69::fileof("./test/str/utf16be.txt")};

	REQUIRE(file.has_value());

	CHECK(std::holds_alternative<x69::str16>(file.value()));
}
