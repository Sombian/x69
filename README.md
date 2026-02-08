# Sombian/string

[![wiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/Sombian/string)
[![hits](https://hits.sh/github.com/Sombian/string.svg)](https://github.com/Sombian/string)
[![stats](https://badgen.net/github/stars/Sombian/string)](https://github.com/Sombian/string)
[![stats](https://badgen.net/github/forks/Sombian/string)](https://github.com/Sombian/string)

a header only string impl. requires **C++23** or later.  
🎉 *fully compatible with: `Clang`, `GCC`, and `MSVC`*   

```c++
#include "string.hpp"

int main() noexcept
{
	utf::str str {u8"メスガキ"};

	std::cout << str << '\n';
}
```

## overview

| class | owns? | null-term? | use-after-free? |
|:-----:|:-----:|:----------:|:---------------:|
| `str` |   T   |   always   |       safe      |
| `txt` |   F   |   maybe?   |      **UB**     |

this impl encourages `error-as-value`, and explicitly forbids:  

- **SILENT FAILURE**
- **INVARIANT VIOLATION**
- **UNDEFINED BEHAVIOUR**

if a strict **O(1)** contract is necessary, please opt for the UTF-32 impl.  

### `str`

`str` is a struct that manages string content ownership.  
it supports seamless conversion between any available encodings.  
all its APIs are designed to accept strings of any supported encoding.  

### `txt`

`txt` is a lightweight struct that holds a pointer (view) to a string.  
similar to that of `str`, its APIs performs transcoding, automatically.  

## tips & tricks

code point random accessing is **O(N)** for variable-width encodings.  
therefore, using an iterator is highly recommended for linear traversal.  

### ✔️ O(N)

```c++
utf::str str {u8"hello world"};

// time complexity: O(N)
for (const auto code : str)
{
	// do something with it
}
```

### ❌ O(N^2)

```c++
utf::str str {u8"hello world"};

// time complexity: O(N)
const auto len {str.length()};

// time complexity: O(N)
for (int i {0}; i < len; ++i)
{
	// time complexity: O(N)
	const auto code {str[i]};
	// do something with it
}
```

---

likewise each `.length()` call is **O(N)** for variable-width encodings.  
therefore, it is important to cache the result if frequent access is needed.  

### ✔️ O(N) * 1

```c++
utf::utf8 str {u8"마법소녀 마도카☆마기카"};

const auto strlen {str.length()};

if (0 < strlen) { /*...*/ }
if (0 < strlen) { /*...*/ }
```

### ❌ O(N) * 2

```c++
utf::utf8 str {u8"마법소녀 마도카☆마기카"};

if (0 < str.length()) { /*...*/ }
if (0 < str.length()) { /*...*/ }
```

---

whilst the API works seamlessly with different encodings out of the box,  
its best to match the operand's encoding to that of lhs, for greater efficiency.  

### ✔️ std::memcpy

```c++
// utf8 to utf8
utf::utf8 str {u8"마법소녀 마도카☆마기카"};
```

### ❌ transcoding

```c++
// utf16 to utf8
utf::utf8 str {u"마법소녀 마도카☆마기카"};
```

---

if you need a substring that involves the end of the string, consider using `range::N`.  
it acts as a sentinel value, which enables backward traversal, for that extra performance.  

### ✔️ walks backward

```c++
utf::utf8 str {u8"마법소녀 마도카☆마기카"};

using range::N;

const auto txt {str.substr(0, N - 69)};
```

### ❌ walks forward

```c++
utf::utf8 str {u8"마법소녀 마도카☆마기카"};

const auto N {str.length()};

const auto txt {str.substr(0, N - 69)};
```

---

`str` supports self-healing iterators, which allows in-place code point mutation, during traversal.  
**note**: self-healing has its limits. any mutation that isnt done by *current* proxy will result in **UB**.  

### ✔️ safe mutation

```c++
utf::utf8 str {u8"마법소녀 마도카☆마기카"};

for (auto code : str)
{
	code = U'♥'; // ❌ buffer shift
	code = U'?'; // ✔️ buffer shift
}
```

### ❌ unsafe mutation

```c++
utf::utf8 str {u8"마법소녀 마도카☆마기카"};

for (auto code : str)
{
	// technically safe
	str = u8"마법소녀 마도카☆마기카";
	// canonically unsafe
	str = u8"기적도, 마법도, 있어..!";

	if (...)
	{
		// technically safe
		*this->begin() = U'♥'; // ❌ buffer shift
		// canonically unsafe
		*this->begin() = U'?'; // ✔️ buffer shift
	}
}
```
