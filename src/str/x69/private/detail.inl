#pragma once

//====================//
#include "../str.hpp" //
//====================//

namespace x69
{
	template <format_t source> constexpr auto detail::__units__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail) noexcept -> size_t
	{
		   typedef typename codec<source>::unit_t T;
		// typedef typename codec<target>::unit_t U;

		return tail - head;
	}

	template <format_t source> constexpr auto detail::__codes__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail) noexcept -> size_t
	{
		   typedef typename codec<source>::unit_t T;
		// typedef typename codec<target>::unit_t U;

		if constexpr (!codec<source>::is_variable)
		{
			return __units__<source>(head, tail);
		}

		if constexpr (codec<source>::is_stateful)
		{
			// TODO: abstracts pattern matching
		}

		size_t out {0};

		for (const T* ptr {head}; ptr < tail;
		     ptr += codec<source>::next(ptr))
		{
			++out;
		}
		return out;
	}

	template <format_t source,
	          format_t target> constexpr auto detail::__fcopy__(/*&*/ typename codec<target>::unit_t* dest,
	                                                            const typename codec<source>::unit_t* head,
	                                                            const typename codec<source>::unit_t* tail) noexcept -> size_t
	{
		typedef typename codec<source>::unit_t T;
		typedef typename codec<target>::unit_t U;

		if constexpr (source == target)
		{
			const U* ptr {head};
			/*&*/ T* out {dest};

			for (; ptr <= tail; ++ptr, ++out) *out = *ptr;

			return detail::__units__<target>(head, tail);
		}

		if constexpr (source != target)
		{
			U* out {dest};

			for (const T* ptr {head}; ptr < tail; )
			{
				code_t code;

				const auto T_step {codec<source>::next(ptr)};
				codec<source>::decode(ptr, code, T_step);
				const auto U_step {codec<target>::size(code)};
				codec<target>::encode(code, out, U_step);

				ptr += T_step;
				out += U_step;
			}

			return detail::__units__<target>(dest, out);
		}
	}

	template <format_t target> constexpr auto detail::__fcopy__(/*&*/ typename codec<target>::unit_t* dest,
	                                                            code_t                                code) noexcept -> size_t
	{
		// typedef typename codec<source>::unit_t T;
		   typedef typename codec<target>::unit_t U;

		const auto step {codec<target>::size(code)};
		codec<target>::encode(code, dest, step);

		return step;
	}

	template <format_t source,
	          format_t target> constexpr auto detail::__rcopy__(/*&*/ typename codec<target>::unit_t* dest,
	                                                            const typename codec<source>::unit_t* head,
	                                                            const typename codec<source>::unit_t* tail) noexcept -> size_t
	{
		typedef typename codec<source>::unit_t T;
		typedef typename codec<target>::unit_t U;

		if constexpr (source == target)
		{
			const U* ptr {head + (tail - head)};
			/*&*/ T* out {dest + (tail - head)};

			for (; head <= ptr; --ptr, --out) *out = *ptr;

			return detail::__units__<target>(head, tail);
		}

		if constexpr (source != target)
		{
			U* out {dest};

			for (const T* ptr {head}; ptr < tail; )
			{
				code_t code;

				const auto T_step {codec<source>::next(ptr)};
				codec<source>::decode(ptr, code, T_step);
				const auto U_step {codec<target>::size(code)};

				ptr += T_step;
				out += U_step;
			}

			for (const T* ptr {tail}; head < ptr; )
			{
				code_t code;

				const auto T_step {codec<source>::back(ptr)};
				codec<source>::decode(ptr, code, T_step);
				const auto U_step {codec<target>::size(code)};
				codec<target>::encode(code, out, U_step);

				ptr += T_step;
				out += U_step;
			}

			return detail::__units__<target>(dest, out);
		}
	}

	template <format_t target> constexpr auto detail::__rcopy__(/*&*/ typename codec<target>::unit_t* dest,
	                                                            code_t                                code) noexcept -> size_t
	{
		// typedef typename codec<source>::unit_t T;
		   typedef typename codec<target>::unit_t U;

		const auto step {codec<target>::size(code)};
		codec<target>::encode(code, dest, step);

		return step;
	}

	template <format_t source,
	          format_t target> constexpr auto detail::__equal__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
	                                                            const typename codec<target>::unit_t* rhs0, const typename codec<target>::unit_t* rhsN) noexcept -> bool
	{
		typedef typename codec<source>::unit_t T;
		typedef typename codec<target>::unit_t U;

		if constexpr (source == target)
		{
			if (lhs0 == rhs0
			    &&
			    lhsN == rhsN)
			{
				return true;
			}

			return detail::__units__<source>(lhs0, lhsN)
			       ==
			       detail::__units__<target>(rhs0, rhsN)
			       &&
			       std::ranges::equal(lhs0, lhsN, rhs0, rhsN);
		}

		if constexpr (source != target)
		{
			const T* lhs_ptr {lhs0};
			const U* rhs_ptr {rhs0};

			for (; lhs_ptr < lhsN && rhs_ptr < rhsN; )
			{
				code_t T_code;
				code_t U_code;

				const auto T_step {codec<source>::next(lhs_ptr)};
				const auto U_step {codec<target>::next(rhs_ptr)};

				codec<source>::decode(lhs_ptr, T_code, T_step);
				codec<target>::decode(rhs_ptr, U_code, U_step);

				if (T_code != U_code)
				{
					return false;
				}

				lhs_ptr += T_step;
				rhs_ptr += U_step;
			}

			return lhs_ptr == lhsN && rhs_ptr == rhsN;
		}
	}

	template <format_t source> constexpr auto detail::__equal__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
	                                                            code_t                                                                            code) noexcept -> bool
	{
		   typedef typename codec<source>::unit_t T;
		// typedef typename codec<target>::unit_t U;

		if (detail::__units__<source>(lhs0, lhsN) != 1) return false;

		code_t data;

		codec<source>::decode(lhs0, data, codec<source>::next(lhs0));

		return data == code;
	}

	template <format_t source,
	          format_t target> constexpr auto detail::__nqual__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
	                                                            const typename codec<target>::unit_t* rhs0, const typename codec<target>::unit_t* rhsN) noexcept -> bool
	{
		typedef typename codec<source>::unit_t T;
		typedef typename codec<target>::unit_t U;

		if constexpr (source == target)
		{
			if (lhs0 == rhs0
			    &&
			    lhsN == rhsN)
			{
				return false;
			}

			return detail::__units__<source>(lhs0, lhsN)
			       !=
			       detail::__units__<target>(rhs0, rhsN)
			       ||
			       !std::ranges::equal(lhs0, lhsN, rhs0, rhsN);
		}

		if constexpr (source != target)
		{
			const T* lhs_ptr {lhs0};
			const U* rhs_ptr {rhs0};

			for (; lhs_ptr < lhsN && rhs_ptr < rhsN; )
			{
				code_t T_code;
				code_t U_code;

				const auto T_step {codec<source>::next(lhs_ptr)};
				const auto U_step {codec<target>::next(rhs_ptr)};

				codec<source>::decode(lhs_ptr, T_code, T_step);
				codec<target>::decode(rhs_ptr, U_code, U_step);

				if (T_code != U_code)
				{
					return true;
				}

				lhs_ptr += T_step;
				rhs_ptr += U_step;
			}

			return lhs_ptr != lhsN || rhs_ptr != rhsN;
		}
	}

	template <format_t source> constexpr auto detail::__nqual__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
	                                                            code_t                                                                            code) noexcept -> bool
	{
		   typedef typename codec<source>::unit_t T;
		// typedef typename codec<target>::unit_t U;

		if (detail::__units__<source>(lhs0, lhsN) != 1) return true;

		code_t data;

		codec<source>::decode(lhs0, data, codec<source>::next(lhs0));

		return data != code;
	}

	template <format_t source,
	          format_t target> constexpr auto detail::__split__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
	                                                            const typename codec<target>::unit_t* rhs0, const typename codec<target>::unit_t* rhsN) noexcept -> std::vector<txt<source>>
	{
		typedef typename codec<source>::unit_t T;
		typedef typename codec<target>::unit_t U;

		std::vector<txt<source>> out;

		const T* last {lhs0};

		detail::__search__<source, target>(lhs0, lhsN,
		                                   rhs0, rhsN,
			// on every distinct L → R match
			[&](const T* head, const T* tail)
			{
				if (head != last)
				{
					out.emplace_back(last, head);
					last = tail; // update anchor
				}
			}
		);

		if (last != lhs0 && last < lhsN)
		{
			out.emplace_back(last, lhsN);
		}

		return out;
	}

	template <format_t source> constexpr auto detail::__split__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
	                                                            code_t                                                                            code) noexcept -> std::vector<txt<source>>
	{
		   typedef typename codec<source>::unit_t T;
		// typedef typename codec<target>::unit_t U;

		std::vector<txt<source>> out;

		const T* head {lhs0};
		const T* dest {lhs0};

		code_t data;
		int8_t step;

		for (; dest < lhsN; dest += step)
		{
			codec<source>::decode(dest, code,
			step = codec<source>::next(dest));

			if (data == code)
			{
				out.emplace_back(head, dest);
				head = dest + step; // update
			}
		}

		return out;
	}

	template <format_t source,
	          format_t target> constexpr auto detail::__match__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
	                                                            const typename codec<target>::unit_t* rhs0, const typename codec<target>::unit_t* rhsN) noexcept -> std::vector<txt<source>>
	{
		typedef typename codec<source>::unit_t T;
		typedef typename codec<target>::unit_t U;

		std::vector<txt<source>> out;

		// const T* last {lhs0};

		detail::__search__<source, target>(lhs0, lhsN,
		                                   rhs0, rhsN,
			// on every distinct L → R match
			[&](const T* head, const T* tail)
			{
				out.emplace_back(head, tail);
			}
		);

		// if (last != lhs0 && last < lhsN)
		// {
		// 	out.emplace_back(last, lhsN);
		// }

		return out;
	}

	template <format_t source> constexpr auto detail::__match__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
	                                                            code_t                                                                            code) noexcept -> std::vector<txt<source>>
	{
		   typedef typename codec<source>::unit_t T;
		// typedef typename codec<target>::unit_t U;

		std::vector<txt<source>> out;

		const T* dest {lhs0};
		const T* tail {lhs0};

		code_t data;
		int8_t step;

		for (; dest < lhsN; dest += step)
		{
			codec<source>::decode(dest, code,
			step = codec<source>::next(dest));

			if (data == code)
			{
				tail = dest + step; // update
				out.emplace_back(dest, tail);
			}
		}

		return out;
	}

	template <format_t source,
	          format_t target> constexpr auto detail::__holds__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
	                                                            const typename codec<target>::unit_t* rhs0, const typename codec<target>::unit_t* rhsN) noexcept -> bool
	{
		typedef typename codec<source>::unit_t T;
		typedef typename codec<target>::unit_t U;

		try
		{
			detail::__search__<source, target>(lhs0, lhsN,
			                                   rhs0, rhsN,
				// on every distinct L → R match
				[](const T* head, const T* tail) { throw static_cast<bool>(1); }
			);
			                                     { throw static_cast<bool>(0); }
		}
		catch (bool out)
		{
			return out;
		}
	}

	template <format_t source> constexpr auto detail::__holds__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
	                                                            code_t                                                                            code) noexcept -> bool
	{
		   typedef typename codec<source>::unit_t T;
		// typedef typename codec<target>::unit_t U;

		const T* dest {lhs0};
		const T* tail {lhs0};

		code_t data;
		int8_t step;

		for (; dest < lhsN; dest += step)
		{
			codec<source>::decode(dest, code,
			step = codec<source>::next(dest));

			if (data == code) { return true; }
		}

		return false;
	}

	template <format_t source,
	          format_t target> constexpr auto detail::__swith__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
	                                                            const typename codec<target>::unit_t* rhs0, const typename codec<target>::unit_t* rhsN) noexcept -> bool
	{
		typedef typename codec<source>::unit_t T;
		typedef typename codec<target>::unit_t U;

		if constexpr (source == target)
		{
			if (lhs0 == rhs0
			    &&
			    lhsN == rhsN)
			{
				return true;
			}

			const auto lhs_len {detail::__units__<source>(lhs0, lhsN)};
			const auto rhs_len {detail::__units__<target>(rhs0, rhsN)};

			return rhs_len == 0 // if rhs(delimeter) is an empty string (e.g. .starts_with(""))
			       ||
			       (lhs_len >= rhs_len && std::ranges::equal(lhs0, lhs0 + rhs_len, rhs0, rhsN));
		}

		if constexpr (source != target)
		{
			const T* lhs_ptr {lhs0};
			const U* rhs_ptr {rhs0};

			for (; lhs_ptr < lhsN && rhs_ptr < rhsN; )
			{
				code_t T_code;
				code_t U_code;

				const auto T_step {codec<source>::next(lhs_ptr)};
				const auto U_step {codec<target>::next(rhs_ptr)};

				codec<source>::decode(lhs_ptr, T_code, T_step);
				codec<target>::decode(rhs_ptr, U_code, U_step);

				if (T_code != U_code)
				{
					return false;
				}

				lhs_ptr += T_step;
				rhs_ptr += U_step;
			}

			return rhs_ptr == rhsN;
		}
	}

	template <format_t source> constexpr auto detail::__swith__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
	                                                            code_t                                                                            code) noexcept -> bool
	{
		   typedef typename codec<source>::unit_t T;
		// typedef typename codec<target>::unit_t U;

		if (detail::__units__<source>(lhs0, lhsN) < 1) return false;

		code_t data;

		codec<source>::decode(lhs0, data, codec<source>::next(lhs0));

		return data == code;
	}

	template <format_t source,
	          format_t target> constexpr auto detail::__ewith__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
	                                                            const typename codec<target>::unit_t* rhs0, const typename codec<target>::unit_t* rhsN) noexcept -> bool
	{
		typedef typename codec<source>::unit_t T;
		typedef typename codec<target>::unit_t U;

		if constexpr (source == target)
		{
			if (lhs0 == rhs0
			    &&
			    lhsN == rhsN)
			{
				return true;
			}

			const auto lhs_len {detail::__units__<source>(lhs0, lhsN)};
			const auto rhs_len {detail::__units__<target>(rhs0, rhsN)};

			return rhs_len == 0 // if rhs(delimeter) is an empty string (e.g. .ends_with(""))
			       ||
			       (lhs_len >= rhs_len && std::ranges::equal(lhsN - rhs_len, lhsN, rhs0, rhsN));
		}

		if constexpr (source != target)
		{
			const T* lhs_ptr {lhsN};
			const U* rhs_ptr {rhsN};

			for (; lhs0 < lhs_ptr && rhs0 < rhs_ptr; )
			{
				code_t T_code;
				code_t U_code;

				const auto T_step {codec<source>::back(lhs_ptr)};
				const auto U_step {codec<target>::back(rhs_ptr)};

				codec<source>::decode(lhs_ptr, T_code, T_step);
				codec<target>::decode(rhs_ptr, U_code, U_step);

				if (T_code != U_code)
				{
					return false;
				}

				lhs_ptr += T_step;
				rhs_ptr += U_step;
			}

			return rhs_ptr == rhs0;
		}
	}

	template <format_t source> constexpr auto detail::__ewith__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
	                                                            code_t                                                                            code) noexcept -> bool
	{
		   typedef typename codec<source>::unit_t T;
		// typedef typename codec<target>::unit_t U;

		if (detail::__units__<source>(lhs0, lhsN) < 1) return false;

		code_t data;

		codec<source>::decode(lhsN, data, codec<source>::back(lhsN));

		return data == code;
	}

	template <format_t source> constexpr auto detail::__substr__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail,
	                                                             clamp                                 from, clamp                                 dest) noexcept -> txt<source>
	{
		   typedef typename codec<source>::unit_t T;
		// typedef typename codec<target>::unit_t U;

		// e.g. str.substr(N - 1, N - 0);

		assert(dest < from);

		const T* foo {tail};
		for (size_t i {  0  }; i < dest && head < foo; ++i, foo += codec<source>::back(foo)) {}

		const T* bar {foo};

		for (size_t i {dest}; i < from && head < bar; ++i, bar += codec<source>::back(bar)) {}

		assert(head <= foo && foo <= tail);
		assert(head <= bar && bar <= tail);

		return {bar, foo};
	}

	template <format_t source> constexpr auto detail::__substr__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail,
	                                                             clamp                                 from, range                                 dest) noexcept -> txt<source>
	{
		   typedef typename codec<source>::unit_t T;
		// typedef typename codec<target>::unit_t U;

		// e.g. str.substr(N - 1, N);

		const T* foo {tail};

		for (size_t i {  0  }; i < from && head < foo; ++i, foo += codec<source>::back(foo)) {}

		const T* bar {tail};

		assert(head <= foo && foo <= tail);
		assert(head <= bar && bar <= tail);

		return {foo, bar};
	}

	template <format_t source> constexpr auto detail::__substr__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail,
	                                                             size_t                                from, clamp                                 dest) noexcept -> txt<source>
	{
		   typedef typename codec<source>::unit_t T;
		// typedef typename codec<target>::unit_t U;

		// e.g. str.substr(0, N - 1);

		const T* foo {head};

		for (size_t i {  0  }; i < from && foo < tail; ++i, foo += codec<source>::next(foo)) {}

		const T* bar {tail};

		for (size_t i {  0  }; i < dest && head < bar; ++i, bar += codec<source>::back(bar)) {}

		assert(head <= foo && foo <= tail);
		assert(head <= bar && bar <= tail);

		return {foo, bar};
	}

	template <format_t source> constexpr auto detail::__substr__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail,
	                                                             size_t                                from, range                                 dest) noexcept -> txt<source>
	{
		   typedef typename codec<source>::unit_t T;
		// typedef typename codec<target>::unit_t U;

		// e.g. str.substr(0, N);

		const T* foo {head};

		for (size_t i {  0  }; i < from && foo < tail; ++i, foo += codec<source>::next(foo)) {}

		const T* bar {tail};

		assert(head <= foo && foo <= tail);
		assert(head <= bar && bar <= tail);

		return {foo, bar};
	}

	template <format_t source> constexpr auto detail::__substr__(const typename codec<source>::unit_t* head, const typename codec<source>::unit_t* tail,
	                                                             size_t                                from, size_t                                dest) noexcept -> txt<source>
	{
		   typedef typename codec<source>::unit_t T;
		// typedef typename codec<target>::unit_t U;

		// e.g. str.substr(6, 9);

		assert(from < dest);

		const T* foo {head};

		for (size_t i {  0  }; i < from && foo < tail; ++i, foo += codec<source>::next(foo)) {}

		const T* bar {foo};

		for (size_t i {from}; i < dest && bar < tail; ++i, bar += codec<source>::next(bar)) {}

		assert(head <= foo && foo <= tail);
		assert(head <= bar && bar <= tail);

		return {foo, bar};
	}

	template <format_t source,
	          format_t target> constexpr auto detail::__search__(const typename codec<source>::unit_t* lhs0, const typename codec<source>::unit_t* lhsN,
	                                                             const typename codec<target>::unit_t* rhs0, const typename codec<target>::unit_t* rhsN,
	                                                             const auto& /* (const source::unit_t* head, const source::unit_t* tail) -> VOID */ fun) noexcept -> void
	{
		typedef typename codec<source>::unit_t T;
		typedef typename codec<target>::unit_t U;

		if constexpr (source == target)
		{
			const auto lhs_dif {detail::__units__<source>(lhs0, lhsN)};
			const auto rhs_dif {detail::__units__<target>(rhs0, rhsN)};

			if (0 < lhs_dif && 0 < rhs_dif)
			{
				if (lhs_dif == rhs_dif)
				{
					if (lhs0 == rhs0
					    &&
					    lhsN == rhsN)
					{
						fun(lhs0, lhsN);
					}
					else if (detail::__equal__<source, target>(lhs0, lhsN,
					                                           rhs0, rhsN))
					{
						fun(lhs0, lhsN);
					}
				}
				else if (lhs_dif < rhs_dif)
				{
					// nothing to do...
				}
				else if (lhs_dif > rhs_dif)
				{
					const T* out;

					// int8_t step;
					// code_t code;

					size_t i;
					size_t j;

					   std::vector<size_t> tbl (rhs_dif, 0);
					// std::vector<code_t> rhs (rhs_len, 0);

					i = 0;
					j = 0;

					for (const U* ptr {rhs0}; ptr < rhsN; ++ptr, ++i)
					{
						// codec<target>::decode(ptr, rhs[i],
						// step = codec<target>::next(ptr));

						while (0 < j && rhs0[i] != rhs0[j])
						{
							j = tbl[j - 1];
						}

						if /* match */ (rhs0[i] == rhs0[j])
						{
							tbl[i] = ++j;
						}
					}

					i = 0;
					j = 0;

					for (const T* ptr {lhs0}; ptr < lhsN; ++ptr, ++i)
					{
						// codec<source>::decode(ptr, code,
						// step = codec<source>::next(ptr));

						while (0 < j && *ptr != rhs0[j])
						{
							j = tbl[j - 1];
						}

						if /* match */ (*ptr == rhs0[j])
						{
							if (j == (  0  )) { out = ptr; /* 1st pos */ }

							++j;

							if (j == rhs_dif) { fun(out, ptr + 1); j = 0; }
						}
					}
				}
			}
		}
		if constexpr (source != target)
		{
			const auto lhs_len {detail::__codes__<source>(lhs0, lhsN)};
			const auto rhs_len {detail::__codes__<target>(rhs0, rhsN)};

			if (0 < lhs_len && 0 < rhs_len)
			{
				if (lhs_len == rhs_len)
				{
					// if (lhs0 == rhs0
					//     &&
					//     lhsN == rhsN)
					// {
					// 	fun(lhs0, lhsN);
					// }
					if (detail::__equal__<source, target>(lhs0, lhsN,
					                                      rhs0, rhsN))
					{
						fun(lhs0, lhsN);
					}
				}
				else if (lhs_len < rhs_len)
				{
					// nothing to do...
				}
				else if (lhs_len > rhs_len)
				{
					const T* out;

					code_t code;
					int8_t step;

					size_t i;
					size_t j;

					std::vector<size_t> tbl (rhs_len, 0);
					std::vector<code_t> rhs (rhs_len, 0);

					i = 0;
					j = 0;

					for (const U* ptr {rhs0}; ptr < rhsN; ptr += step, ++i)
					{
						codec<target>::decode(ptr, rhs[i],
						step = codec<target>::next(ptr));

						while (0 < j && rhs[i] != rhs[j])
						{
							j = tbl[j - 1];
						}

						if /* match */ (rhs[i] == rhs[j])
						{
							tbl[i] = ++j;
						}
					}

					i = 0;
					j = 0;

					for (const T* ptr {lhs0}; ptr < lhsN; ptr += step, ++i)
					{
						codec<source>::decode(ptr, code,
						step = codec<source>::next(ptr));

						while (0 < j && code != rhs[j])
						{
							j = tbl[j - 1];
						}

						if /* match */ (code == rhs[j])
						{
							if (j == (  0  )) { out = ptr; /* get 1st pos */ }

							++j;

							if (j == rhs_len) { j = 0; fun(out, ptr + step); }
						}
					}
				}
			}
		}
	}
}
