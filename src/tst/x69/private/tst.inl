#pragma once

//====================//
#include "../tst.hpp" //
//====================//

namespace x69
{
	template <typename /**/ T,
	          template <class>
	          typename malloc>
	template <typename /**/ K> requires requires (std::ranges::range_value_t<K> _)
	                                             { static_cast<char32_t>(  _  ); }
	constexpr tst<T, malloc>::tst(std::initializer_list<std::conditional_t<!std::is_void_v<T>, std::pair<K, T>, K>> args) noexcept
	{
		if constexpr (std::is_void_v<T>)
		{
			// TODO
		}
		if constexpr (!std::is_void_v<T>)
		{
			for (auto&& [key, val] : args)
			{
				(*this)[key] = val; // :D
			}
		}
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr tst<T, malloc>::tst(std::initializer_list<std::conditional_t<!std::is_void_v<T>, std::pair<x69::txt8, T>, x69::txt8>> args) noexcept
	{
		if constexpr (std::is_void_v<T>)
		{
			// TODO
		}
		if constexpr (!std::is_void_v<T>)
		{
			for (auto&& [key, val] : args)
			{
				(*this)[key] = val; // :D
			}
		}
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr tst<T, malloc>::tst(std::initializer_list<std::conditional_t<!std::is_void_v<T>, std::pair<x69::txt16, T>, x69::txt16>> args) noexcept
	{
		if constexpr (std::is_void_v<T>)
		{
			// TODO
		}
		if constexpr (!std::is_void_v<T>)
		{
			for (auto&& [key, val] : args)
			{
				(*this)[key] = val; // :D
			}
		}
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr tst<T, malloc>::tst(std::initializer_list<std::conditional_t<!std::is_void_v<T>, std::pair<x69::txt32, T>, x69::txt32>> args) noexcept
	{
		if constexpr (std::is_void_v<T>)
		{
			// TODO
		}
		if constexpr (!std::is_void_v<T>)
		{
			for (auto&& [key, val] : args)
			{
				(*this)[key] = val; // :D
			}
		}
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr tst<T, malloc>::~tst() noexcept
	{
		node_t* self {this->root   };
		node_t* back {      nullptr};

		enum dirt : uint8_t
		{
			init = 0b00,
			to_l = 0b01,
			to_m = 0b10,
			to_r = 0b11,
		};

		while (self != nullptr)
		{
			switch (self->dirt)
			{
				case to_l: { goto P2; }
				case to_m: { goto P3; }
				case to_r: { goto GC; }
			}

			P1:
			{
				self->dirt = to_l;

				if (node_t* next {self->to_l}; next != nullptr)
				{
					self->to_l = back;
					back       = self;
					self       = next;
					// goto ↙
					continue;
				}
			}

			P2:
			{
				self->dirt = to_m;

				if (node_t* next {self->to_m}; next != nullptr)
				{
					self->to_m = back;
					back       = self;
					self       = next;
					// goto ↓
					continue;
				}
			}

			P3:
			{
				self->dirt = to_r;

				if (node_t* next {self->to_r}; next != nullptr)
				{
					self->to_r = back;
					back       = self;
					self       = next;
					// goto ↘
					continue;
				}
			}

			GC:
			{
				if constexpr (!INLINE)
				{
					if (self->hold == 0b1)
					{
						std::allocator_traits<decltype(this->tag.Γdata)>::destroy(this->tag.Γdata, self->data);
						std::allocator_traits<decltype(this->tag.Γdata)>::deallocate(this->tag.Γdata, self->data, 1);
					}
				}
				{
					std::allocator_traits<decltype(this->tag.Γnode)>::destroy(this->tag.Γnode, self /*&*/);
					std::allocator_traits<decltype(this->tag.Γnode)>::deallocate(this->tag.Γnode, self /*&*/, 1);
				}

				// goto ↑
				self = back;

				if (back != nullptr)
				{
					switch (back->dirt)
					{
						// wipe lt branch
						case to_l: { node_t* ptr {back->to_l}; back->to_l = nullptr; back = ptr; break; }
						// wipe eq branch
						case to_m: { node_t* ptr {back->to_m}; back->to_m = nullptr; back = ptr; break; }
						// wipe gt branch
						case to_r: { node_t* ptr {back->to_r}; back->to_r = nullptr; back = ptr; break; }
					}
				}
			}
		}
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr tst<T, malloc>::tst(const tst& other) noexcept
	{
		// copy constructor
		if (this != &other)
		{
			// WIP
		}
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr tst<T, malloc>::tst(/*&*/ tst&& other) noexcept
	{
		// move constructor
		if (this != &other)
		{
			std::swap(this->tag,
			          other.tag);

			std::swap(this->root,
			          other.root);
		}
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::operator=(const tst& other) noexcept -> tst&
	{
		// copy assignment
		if (this != &other)
		{
			// WIP
		}
		return *this;
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::operator=(/*&*/ tst&& other) noexcept -> tst&
	{
		// move assignment
		if (this != &other)
		{
			std::swap(this->tag,
			          other.tag);

			std::swap(this->root,
			          other.root);
		}
		return *this;
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::view() const noexcept -> cursor
	{
		return {this, nullptr};
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::operator&(const tst& rhs) const noexcept -> tst
	{
		// WIP
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::operator|(const tst& rhs) const noexcept -> tst
	{
		// WIP
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::operator+(const tst& rhs) const noexcept -> tst
	{
		// WIP
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::operator-(const tst& rhs) const noexcept -> tst
	{
		// WIP
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::operator&=(const tst& rhs)& noexcept -> tst&
	{
		// WIP
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::operator&=(const tst& rhs)&& noexcept -> tst&&
	{
		// WIP
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::operator|=(const tst& rhs)& noexcept -> tst&
	{
		// WIP
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::operator|=(const tst& rhs)&& noexcept -> tst&&
	{
		// WIP
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::operator+=(const tst& rhs)& noexcept -> tst&
	{
		// WIP
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::operator+=(const tst& rhs)&& noexcept -> tst&&
	{
		// WIP
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::operator-=(const tst& rhs)& noexcept -> tst&
	{
		// WIP
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::operator-=(const tst& rhs)&& noexcept -> tst&&
	{
		// WIP
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	template <typename /**/ K> requires requires (std::ranges::range_value_t<K> _)
	                                             { static_cast<char32_t>(  _  ); }
	constexpr auto tst<T, malloc>::operator[](K&& rhs) const noexcept -> reader<std::conditional_t<std::is_constructible_v<K, x69::txt8> || std::is_convertible_v<K, x69::txt8>, x69::txt8,
	                                                                            std::conditional_t<std::is_constructible_v<K, x69::txt16> || std::is_convertible_v<K, x69::txt16>, x69::txt16,
	                                                                            std::conditional_t<std::is_constructible_v<K, x69::txt32> || std::is_convertible_v<K, x69::txt32>, x69::txt32,
	                                                                            // fallback to original T
	                                                                            std::remove_cvref_t<K>>>>>
	{
		return {this, {rhs}};
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	template <typename /**/ K> requires requires (std::ranges::range_value_t<K> _)
	                                             { static_cast<char32_t>(  _  ); }
	constexpr auto tst<T, malloc>::operator[](K&& rhs) /*&*/ noexcept -> writer<std::conditional_t<std::is_constructible_v<x69::txt8, K> || std::is_convertible_v<K, x69::txt8>, x69::txt8,
	                                                                            std::conditional_t<std::is_constructible_v<x69::txt16, K> || std::is_convertible_v<K, x69::txt16>, x69::txt16,
	                                                                            std::conditional_t<std::is_constructible_v<x69::txt32, K> || std::is_convertible_v<K, x69::txt32>, x69::txt32,
	                                                                            // fallback to original T
	                                                                            std::remove_cvref_t<K>>>>>
	{
		return {this, {rhs}};
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::__patch__(node_t* a) noexcept -> node_t*
	{
		const auto bf {__factor__(a)};

		// left heavy
		if (+1 < bf && a->to_l != nullptr)
		{
			// LL case
			if (__height__(a->to_l->to_l) >= __height__(a->to_l->to_r))
			{
				return __rot2r__(a);
			}

			// LR case
			a->to_l = __rot2l__(a->to_l);

			return __rot2r__(a);
		}

		// right heavy
		if (bf < -1 && a->to_r != nullptr)
		{
			// RR case
			if (__height__(a->to_r->to_l) <= __height__(a->to_r->to_r))
			{
				return __rot2l__(a);
			}

			// RL case
			a->to_r = __rot2r__(a->to_r);

			return __rot2l__(a);
		}

		return a;
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::__castΔ__(node_t* a) noexcept -> node_t*
	{
		a->drop = 1 + std::max(__height__(a->to_l),
		                       __height__(a->to_r));

		return a;
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::__rot2l__(node_t* a) noexcept -> node_t*
	{
		//┌────────┬───────┬───────┬─────────┐
		//│        │ A     │       │    B    │
		//│        │  ╲    │       │  ╱   ╲  │
		//│ BEFORE │   B   │ AFTER │ A     C │
		//│        │    ╲  │       │         │
		//│        │     C │       │         │
		//└────────┴───────┴───────┴─────────┘

		node_t* b {a->to_r};
		node_t* c {b->to_l};

		b->to_l = a;
		a->to_r = c;

		__castΔ__(a);
		__castΔ__(b);

		return b;
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::__rot2r__(node_t* a) noexcept -> node_t*
	{
		//┌────────┬───────┬───────┬─────────┐
		//│        │     A │       │    B    │
		//│        │    ╱  │       │  ╱   ╲  │
		//│ BEFORE │   B   │ AFTER │ C     A │
		//│        │  ╱    │       │         │
		//│        │ C     │       │         │
		//└────────┴───────┴───────┴─────────┘

		node_t* b {a->to_l};
		node_t* c {b->to_r};

		b->to_r = a;
		a->to_l = c;

		__castΔ__(a);
		__castΔ__(b);

		return b;
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::__height__(node_t* a) noexcept -> ptrdiff_t
	{
		return a == nullptr ? 0 : a->drop;
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::__factor__(node_t* a) noexcept -> ptrdiff_t
	{
		return a == nullptr ? 0 : __height__(a->to_l)
		                          -
		                          __height__(a->to_r);
	}

#pragma region cursor

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::cursor::get() const noexcept -> std::optional<data_t>
	{
		if (this->ptr == nullptr)
		{
			return std::nullopt;
		}

		if (this->ptr->hold == 0b0)
		{
			return std::nullopt;
		}

		return this->ptr->data;
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::cursor::is_root() const noexcept -> bool
	{
		return this->ptr == nullptr || this->src->root == this->ptr;
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::cursor::is_leaf() const noexcept -> bool
	{
		return this->ptr == nullptr || this->ptr->to_m == nullptr;
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	constexpr auto tst<T, malloc>::cursor::operator[](char32_t code) /*&*/ noexcept -> bool
	{
		enum dirt : uint8_t
		{
			init = 0b00,
			to_l = 0b01,
			to_m = 0b10,
			to_r = 0b11,
		};

		node_t* self {this->ptr == nullptr
		             ?
		             this->src->root /*①*/
		             :
		             this->ptr->to_m /*②*/};

		// in case of ①: skips root descent
		// in case of ②: descend by 1 level

		while (self != nullptr)
		{
			/**/ if (self->code == code)
			{
				// self = self->to_m;
				break;
			}
			else if (code < self->code)
			{
				self = self->to_l;
				// break;
			}
			else if (self->code < code)
			{
				self = self->to_r;
				// break;
			}
		}

		if (self != nullptr)
		{
			this->ptr = self;
		}

		return self != nullptr;
	}

#pragma endregion cursor
#pragma region reader

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	template <typename /**/ K>
	constexpr tst<T, malloc>::reader<K>::operator std::optional<typename tst<T, malloc>::data_t>() const noexcept
	{
		enum dirt : uint8_t
		{
			init = 0b00,
			to_l = 0b01,
			to_m = 0b10,
			to_r = 0b11,
		};

		node_t* self {this->src->root};
		node_t* back {    nullptr    };

		for (auto code : this->key)
		{
			while (self != nullptr)
			{
				/**/ if (self->code == code)
				{
					if (self->to_m == nullptr)
					{
						// return std::nullopt;
					}
					back = self      ;
					self = self->to_m;
					break;
				}
				else if (code < self->code)
				{
					if (self->to_l == nullptr)
					{
						return std::nullopt;
					}
					back = self      ;
					self = self->to_l;
					// break;
				}
				else if (self->code < code)
				{
					if (self->to_r == nullptr)
					{
						return std::nullopt;
					}
					back = self      ;
					self = self->to_r;
					// break;
				}
			}
		}

		if (back == nullptr)
		{
			return std::nullopt;
		}

		if (back->hold == 0b0)
		{
			return std::nullopt;
		}

		return back->data;
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	template <typename /**/ K>
	constexpr tst<T, malloc>::reader<K>::operator bool() const noexcept
	{
		return this->operator std::optional<data_t>();
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	template <typename /**/ K>
	constexpr auto tst<T, malloc>::reader<K>::operator==(const T& rhs) const noexcept -> bool requires (!std::is_void_v<T>)
	{
		return this->operator std::optional<data_t>() == rhs;
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	template <typename /**/ K>
	constexpr auto tst<T, malloc>::reader<K>::operator!=(const T& rhs) const noexcept -> bool requires (!std::is_void_v<T>)
	{
		return this->operator std::optional<data_t>() != rhs;
	}

#pragma endregion reader
#pragma region writer

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	template <typename /**/ K>
	constexpr tst<T, malloc>::writer<K>::operator std::optional<typename tst<T, malloc>::data_t>() const noexcept
	{
		return reader<K> {this->src, this->key};
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	template <typename /**/ K>
	constexpr tst<T, malloc>::writer<K>::operator bool() const noexcept
	{
		return reader<K> {this->src, this->key};
	}
	
	
	template <typename /**/ T,
	          template <class>
	          typename malloc>
	template <typename /**/ K>
	constexpr auto tst<T, malloc>::writer<K>::operator=(const T& rhs) noexcept -> writer& requires (!std::is_void_v<T>)
	{
		enum dirt : uint8_t
		{
			init = 0b00,
			to_l = 0b01,
			to_m = 0b10,
			to_r = 0b11,
		};

		node_t* self {this->src->root};
		node_t* back {    nullptr    };
		// double ptr for malloc
		auto* slot {&this->src->root};

		// schorr-waite descent
		for (auto code : this->key)
		{
			while (true)
			{
				if (self == nullptr)
				{
					typedef decltype(this->src->tag.Γnode) allocator_t;

					node_t* temp {(*slot)};

					(*slot) = std::allocator_traits<allocator_t>
					          ::
					          allocate(this->src->tag.Γnode, 1);

					::new (static_cast<void*>(*slot)) node_t ();

					// writes to node_t
					(*slot)->code = code;

					self = (*slot);
					       (*slot) = temp;
				}

				/**/ if (self->code == code)
				{
					node_t* temp {self->to_m};
					// sets node ptr
					slot = &self->to_m;
					// [[invariant]]
					self->to_m = back;
					self->dirt = to_m;
					back       = self;
					self       = temp;

					break;
				}
				else if (code < self->code)
				{
					node_t* temp {self->to_l};
					// sets node ptr
					slot = &self->to_l;
					// [[invariant]]
					self->to_l = back;
					self->dirt = to_l;
					back       = self;
					self       = temp;

					// break;
				}
				else if (self->code < code)
				{
					node_t* temp {self->to_r};
					// sets node ptr
					slot = &self->to_r;
					// [[invariant]]
					self->to_r = back;
					self->dirt = to_r;
					back       = self;
					self       = temp;

					// break;
				}
			}
		}

		// climb ↑ by 1 level
		{
			node_t* temp {back->to_m};

			back->to_m = self;
			back->dirt = init;
			self       = back;
			back       = temp;
		}

		if constexpr (!INLINE)
		{
			typedef decltype(this->src->tag.Γdata) allocator_t;

			self->data = std::allocator_traits<allocator_t>
			                ::
			                allocate(this->src->tag.Γdata, 1);

			 (self->hold) = 0b1;
			*(self->data) = rhs;
		}
		else
		{
			 (self)->hold = 0b1;
			 (self)->data = rhs;
		}

		// schorr-waite ascent
		while (back != nullptr)
		{
			switch (back->dirt)
			{
				case to_l:
				{
					node_t* temp {back->to_l};

					self = __castΔ__(self);
					self = __patch__(self);

					back->to_l = self;
					back->dirt = init;
					self       = back;
					back       = temp;

					break;
				}
				case to_m:
				{
					node_t* temp {back->to_m};

					   self = __castΔ__(self);
					// self = __patch__(self);

					back->to_m = self;
					back->dirt = init;
					self       = back;
					back       = temp;

					break;
				}
				case to_r:
				{
					node_t* temp {back->to_r};

					self = __castΔ__(self);
					self = __patch__(self);

					back->to_r = self;
					back->dirt = init;
					self       = back;
					back       = temp;

					break;
				}
				default:
				{
					assert(!"failure");
					std::unreachable();
				}
			}
		}

		assert(self != nullptr);
		assert(back == nullptr);

		self = __castΔ__(self);
		self = __patch__(self);
		// update the root ptr
		this->src->root = self;

		return *this;
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	template <typename /**/ K>
	constexpr auto tst<T, malloc>::writer<K>::operator==(const T& rhs) const noexcept -> bool requires (!std::is_void_v<T>)
	{
		return reader<K> {this->src, this->key} == rhs;
	}

	template <typename /**/ T,
	          template <class>
	          typename malloc>
	template <typename /**/ K>
	constexpr auto tst<T, malloc>::writer<K>::operator!=(const T& rhs) const noexcept -> bool requires (!std::is_void_v<T>)
	{
		return reader<K> {this->src, this->key} != rhs;
	}

#pragma endregion writer

}
