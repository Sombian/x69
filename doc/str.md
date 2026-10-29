### `str`

`str` is a struct that manages string content ownership.  
it supports seamless conversion between any available encodings,  
and its API accept strings in any supported encoding out of the box.  

### `txt`

`txt` is a lightweight struct that holds beg & end ptr to a string.  
similar to that of `str`, its API performs transcoding, automatically.  

## tl;dr

| class | owns? | null-term? | use-after-free? |
|:-----:|:-----:|:----------:|:---------------:|
| `str` | **T** |   always   |       safe      |
| `txt` | **F** |   mostly   |      **UB**     |

## TIPs

code point random accessing is **O(N)** for variable-width encodings.  
therefore, using an iterator is highly recommended for linear traversal.  

### ✔️ O(N)

```c++
x69::str8 str {u8"hello world"};

for (const auto code : str)
{
    // do something...
}
```

### ❌ O(N^2)

```c++
x69::str8 str {u8"hello world"};

const auto len {str.length()};

for (int i {0}; i < len; ++i)
{
    // do something....
}
```

---

likewise, each `.length()` call is **O(N)** for variable-width encodings.  
because of this, its advised to cache the result if you need frequent access.  

### ✔️ O(N) * 1

```c++
x69::str8 str {u8"마법소녀 마도카☆마기카"};

const auto strlen {str.length()};

if (0 < strlen) { /*...*/ }
if (0 < strlen) { /*...*/ }
```

### ❌ O(N) * 2

```c++
x69::str8 str {u8"마법소녀 마도카☆마기카"};

if (0 < str.length()) { /*...*/ }
if (0 < str.length()) { /*...*/ }
```

---

whilst the API works seamlessly with different encodings out of the box,  
its best to match the operand's encoding to that of lhs, for greater efficiency.  

### ✔️ std::memcpy

```c++
// utf8 to utf8
x69::str8 str {u8"마법소녀 마도카☆마기카"};
```

### ❌ transcoding

```c++
// utf16 to utf8
x69::str8 str {u"마법소녀 마도카☆마기카"};
```

---

if you need a substring anchored near the end of the string, consider using `magic::N`,  
which acts as a sentinel value that enables backward traversal for that extra performance.  

### ✔️ walks backward

```c++
x69::str8 str {u8"마법소녀 마도카☆마기카"};

using x69::magic::N;

const auto txt {str.substr(0, N - 67)};
```

### ❌ walks forward

```c++
x69::str8 str {u8"마법소녀 마도카☆마기카"};

const auto N {str.length()};

const auto txt {str.substr(0, N - 67)};
```

---

all string concatenation operations are deferred until materialization („• ֊ •„).  
*this includes, but not limited to* := `LHS + RHS`, `LHS == RHS`, `LHS != RHS`, etc.  

### ✔️ expr templ

```c++
x69::str8 foo {u"fooᶠᵒᵒ"};
x69::str8 bar {U"barᵇᵃʳ"};

if (/* omit */(foo + bar) == U"fooᶠᵒᵒ"
                             U"barᵇᵃʳ")
{
    // do something...
}
```

### ❌ materialize

```c++
x69::str8 foo {u"fooᶠᵒᵒ"};
x69::str8 bar {U"barᵇᵃʳ"};

if (x69::str8 {foo + bar} == U"fooᶠᵒᵒ"
                             U"barᵇᵃʳ")
{
    // do something...
}
```

---

in need of frequent look-before/ahead, consider using `.it<N>()`, where `N` specifies len of the `most recently used` cache.  
(,; ⩌ ;,) self-healing has its limits. **mutations** perfomed outside the active proxy will result in **UB**, albeit sometimes it can be safe.  

### ✔️ safe mutation

```c++
x69::str str {u8"마법소녀 마도카☆마기카 2026: Rising"};

for (auto [win, i] {str.it()}; win[i] != '\0'; ++i)
{
    if (win[i] == U'☆') win[i] = U'♥'; // (˶ˆᗜˆ˵)
}
```

### ❌ unsafe mutation

```c++
x69::str str {u8"마법소녀 마도카☆마기카 2026: Rising"};

for (auto [win, i] {str.it()}; win[i] != '\0'; ++i)
{
    if (win[i] == U'☆') str[i] = U'♥'; // (╥ ᴗ ╥)
}
```
