extern "C" int system(const char* inˀ);
static int WindowsConsoleSubSystem {[]
{
	#ifdef _WIN32//__{UTF8}__);
	system("chcp 65001 > nul");
	#endif//WIN32//__{UTF8}__);

	return 0; // dummy? return
}()};

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#ifndef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

const char dir[] {"./test/str/utf8std.txt"};

using namespace x69;

int main() noexcept
{
	if (auto _ {fileof(dir)})
	{
		std::visit([](auto _)
		{
			std::cout << _;
		},
		std::move(_.value()));
	}
}

#endif //CLOSE_CONFIG_IMPLEMENT_WITH_MAIN
#ifdef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

TEST_CASE("ASCII")
{
	x69::txt str {"마마마에는감동이있다"};

	CHECK(str.size() == str.length());
}

TEST_CASE("SSO23")
{
	x69::str small {u8"✦ ✦ ✦ ✦ ✦ ✦"};
	x69::str large {u8"✦Ø✦Ø✦Ø✦Ø✦Ø✦"};

	CHECK(small.size() == small.space());
	CHECK(large.size() == large.space());
}

TEST_CASE("A + B")
{
	x69::str foo {u8"foo"};
	x69::txt bar {u8"bar"};

	CHECK((foo + U"bar") == U"foobar");
	CHECK(U"foobar" == (U"foo" + bar));
}

TEST_CASE("flush")
{
	x69::str8 foo {u"foo"};
	x69::str8 bar {U"bar"};

	CHECK(x69::str8 {foo + bar} == U"foo"
	                               U"bar");
}

TEST_CASE("range")
{
	x69::txt str {u8"사랑해"};

	using x69::magic::N;

	CHECK(str[0] == U'사');
	CHECK(str[1] == U'랑');
	CHECK(str[2] == U'해');

	CHECK(str.substr(0, N) == str);
	CHECK(str.substr(0, 3) == str);
}

TEST_CASE("split")
{
	x69::txt str {u8"티라미수"
	              u8"☆"
	              u8"치즈케잌"
	              u8"☆"
	              u8"말차라떼"};

	auto split {str.split(U'☆')};

	CHECK(split[0] == u8"티라미수");
	CHECK(split[0] ==  u"티라미수");

	CHECK(split[1] == u8"치즈케잌");
	CHECK(split[1] ==  u"치즈케잌");

	CHECK(split[2] == u8"말차라떼");
	CHECK(split[2] ==  u"말차라떼");
}

TEST_CASE("dangl")
{
	CHECK(x69::str {u8"lorem"}
	               +
	      x69::str {u8"ipsum"} == u"lorem"
	                              u"ipsum");
}

TEST_CASE("alias")
{
	x69::str str {u8"우울계_𝕏친소 우울계_트친소"};

	str += str.substr(0, 15);

	CHECK(str == u"우울계_𝕏친소 우울계_트친소"
	             u"우울계_𝕏친소 우울계_트친소");
}

TEST_CASE("shift")
{
	x69::str str {u8"우울계_𝕏친소 우울계_트친소"
	              u8"정병계_𝕏친소 정병계_트친소"};

	for (auto [Σ, i] {str.it()}; Σ[i] != '\0'; ++i)
	{
		     if (Σ[i] == U'트') { Σ[i] = U'𝕏'; }
		else if (Σ[i] == U'𝕏') { Σ[i] = U'트'; }
	}

	CHECK(str == u"우울계_트친소 우울계_𝕏친소"
	             u"정병계_트친소 정병계_𝕏친소");
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

#endif //CLOSE_CONFIG_IMPLEMENT_WITH_MAIN
