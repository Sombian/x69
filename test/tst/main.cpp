#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h" /*─────────*/
/*───────────────────────────/*─────────*/

#include "x69/tst.hpp"

#include <ranges>
#include <vector>

TEST_CASE("index")
{
	x69::tst<int> tst
	{
		{u8"foo"_txt, 69},
		{u8"bar"_txt, 74},
	};

	CHECK(tst[u8"foo"] == 69);
	CHECK(tst[u8"bar"] == 74);

	tst[u8"foo"] = 74;
	tst[u8"bar"] = 69;

	CHECK(tst[u8"foo"] == 74);
	CHECK(tst[u8"bar"] == 69);
}

TEST_CASE("cursor")
{
	x69::tst<int> tst
	{
		{u8"foo"_txt, 69},
		{u8"bar"_txt, 74}
	};

	SUBCASE("foo")
	{
		auto cursor {tst.view()};

		REQUIRE(cursor['f']);
		REQUIRE(cursor['o']);
		REQUIRE(cursor['o']);

		CHECK(cursor.get() == 69);
	}
	SUBCASE("bar")
	{
		auto cursor {tst.view()};

		REQUIRE(cursor['b']);
		REQUIRE(cursor['a']);
		REQUIRE(cursor['r']);

		CHECK(cursor.get() == 74);
	}
}

TEST_CASE("balance")
{
	SUBCASE("LL")
	{
		x69::tst<int> tst
		{
			{u8"cat"_txt, 1},
			{u8"bat"_txt, 2},
			{u8"ant"_txt, 3},
		};

		CHECK(tst[u8"cat"] == 1);
		CHECK(tst[u8"bat"] == 2);
		CHECK(tst[u8"ant"] == 3);
	}

	SUBCASE("RR")
	{
		x69::tst<int> tst
		{
			{u8"ant"_txt, 1},
			{u8"bat"_txt, 2},
			{u8"cat"_txt, 3},
		};

		CHECK(tst[u8"ant"] == 1);
		CHECK(tst[u8"bat"] == 2);
		CHECK(tst[u8"cat"] == 3);
	}

	SUBCASE("LR")
	{
		x69::tst<int> tst
		{
			{u8"cat"_txt, 1},
			{u8"ant"_txt, 2},
			{u8"bat"_txt, 3},
		};

		CHECK(tst[u8"cat"] == 1);
		CHECK(tst[u8"ant"] == 2);
		CHECK(tst[u8"bat"] == 3);
	}

	SUBCASE("RL")
	{
		x69::tst<int> tst
		{
			{u8"ant"_txt, 1},
			{u8"cat"_txt, 2},
			{u8"bat"_txt, 3},
		};

		CHECK(tst[u8"ant"] == 1);
		CHECK(tst[u8"cat"] == 2);
		CHECK(tst[u8"bat"] == 3);
	}

	SUBCASE("zig-zag")
	{
		x69::tst<int> tst
		{
			{u8"mmm"_txt, 1},
			{u8"aaa"_txt, 2},
			{u8"zzz"_txt, 3},
			{u8"eee"_txt, 4},
			{u8"ccc"_txt, 5},
		};

		CHECK(tst[u8"mmm"] == 1);
		CHECK(tst[u8"aaa"] == 2);
		CHECK(tst[u8"zzz"] == 3);
		CHECK(tst[u8"eee"] == 4);
		CHECK(tst[u8"ccc"] == 5);
	}
}
