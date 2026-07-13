# Tiny Data Structures (WIP)

A header-only collection of small, type-safe data structures for C. Compiles absolutely warning-free so far.

It's meant to allow me to share my own data structures across projects; therefore, I made some design choices which
might give other people the ick. Like assuming memory allocation always succeeds and prefering asserting to runtime
error detection.

The library currently provides:

- Vectors
- Hash maps
- Sets
- Dense pools

## Requirements

A C99 compiler. C11 is only required for the µnit library and `static_assert` in the tests, but you can disable it by
defining `TESTS_NO_STATIC_ASSERT`. Standard library not required as long as you provide your own memory management
functions. The tests are trivial to compile, but I'm using CMake here.

## Installation

Make `include/tds` available through your compiler's include path. You can omit headers for data structures you do not use, but `include/tds/private` is required by all of them.

Also make [rapidhash.h](https://github.com/Nicoshev/rapidhash) available for inclusion. It is used by the default hash map hashing macro and by the current set implementation.

## How It Works

Including one of the public headers generates declarations and implementations for one concrete container type. You customize the generated type by defining macros before the `#include`.

Keep these rules in mind:

- Zero-initialize every container before first use.
- Call `*_fini` when you are done with a container.
- All values are shallow-copied.
- Treat struct fields as internal implementation details and use the public functions instead.
- If you include the same header multiple times, each inclusion should configure a different generated type.

By default, if neither `TDS_DECLARE` nor `TDS_IMPLEMENT` is defined, the header emits both declarations and implementations.

## Available Data Structures

| Data structure | Default generated type | Description |
|---|---|---|
| Vector | `vec_<value-type>` | A dynamic contiguous array. |
| Hash map | `hashmap_<key-type>_<value-type>` | An unordered key-value container using Robin Hood hashing. |
| Set | `set_<value-type>` | An unordered container of unique values using Robin Hood hashing. |
| Dense pool | `dense_pool_<value-type>` | A dense array with stable sparse IDs and O(1) add/remove by ID. |

## Public API

### Vector

Header: `#include <tds/vector.h>`

| Function | Description |
|---|---|
| `append` | Appends one value to the end of the vector. |
| `reserve` | Ensures capacity for at least `capacity` elements. |
| `grow` | Extends the vector by `count` elements and returns a pointer to the first newly reserved slot. |
| `remove` | Removes the element at `index` and shifts later elements left. |
| `get` | Returns the value at `index`. |
| `count` | Returns the current number of elements. |
| `first` | Returns a pointer to the contiguous backing array, or `NULL` when empty. |
| `clear` | Removes all elements but keeps the allocated buffer. |
| `reclaim` | Shrinks the allocated buffer to exactly `count` elements, or frees it if empty. |
| `fini` | Finalizes the vector and frees all storage. |

### Hash map

Header: `#include <tds/hashmap.h>`

| Function | Description |
|---|---|
| `get` | Returns a pointer to the stored value for `key`, or `NULL` if the key is absent. |
| `reserve` | Ensures enough backing capacity for at least `capacity` buckets before Robin Hood rehashing rules are applied. |
| `set` | Inserts or replaces the value for `key`. Returns nonzero if a new key was inserted, or zero if an existing key's value was replaced. |
| `iter` | Creates an iterator for traversing occupied entries. |
| `next` | Advances an iterator. Returns nonzero while an entry is available. |
| `remove` | Removes `key` if present. Returns nonzero if an entry was removed, or zero if the key was absent. |
| `count` | Returns the number of stored entries. |
| `clear` | Removes all entries but keeps the bucket array allocated. |
| `reclaim` | Shrinks the bucket array to the smallest prime capacity that satisfies the current load. |
| `fini` | Finalizes the map and frees all storage. |

The generated iterator type is named `<generated_type>_iter_t` and exposes:

- `key`: the current key
- `value`: a pointer to the current value

### Set

Header: `#include <tds/set.h>`

| Function | Description |
|---|---|
| `contains` | Returns nonzero if the value is present. |
| `reserve` | Ensures enough backing capacity for at least `capacity` buckets before rehashing is necessary. |
| `add` | Inserts the value if it is not already present. Returns nonzero if the value was inserted, or zero if it was already present. |
| `remove` | Removes the value if present. Returns nonzero if a value was removed, or zero if it was absent. |
| `count` | Returns the number of stored values. |
| `clear` | Removes all values but keeps the bucket array allocated. |
| `reclaim` | Tries to shrink the backing storage as much as possible without loading the hash map over the limit. |
| `fini` | Finalizes the set and frees all storage. |

### Dense pool

Header: `#include <tds/dense-pool.h>`

| Function | Description |
|---|---|
| `append` | Appends a value and returns its stable sparse ID. |
| `reserve` | Ensures capacity for at least `capacity` stored IDs. |
| `remove` | Removes the element identified by `id` and returns the dense index that was vacated. |
| `get` | Returns the value associated with `id`. |
| `count` | Returns the current number of live elements. |
| `first` | Returns a pointer to the dense value array. |
| `clear` | Removes all elements but keeps allocated storage. |
| `reclaim` | Shrinks storage to the highest live sparse ID plus one and rebuilds the free-ID list. |
| `fini` | Finalizes the pool and frees all storage. |

## Configuration Macros

### Global macros

Define these before including any TDS header if you want to override the defaults:

| Macro | Description | Default |
|---|---|---|
| `TDS_PREFIX` | Prefix added to generated default type names. | Empty |
| `TDS_CALLOC` | Allocation function compatible with `calloc`. | `calloc` |
| `TDS_REALLOC` | Reallocation function compatible with `realloc`. | `realloc` |
| `TDS_FREE` | Deallocation function compatible with `free`. | `free` |
| `TDS_MEMSET` | Memory set function compatible with `memset`. | `memset` |
| `TDS_MEMMOVE` | Memory move function compatible with `memmove`. | `memmove` |
| `TDS_ASSERT` | Assertion macro used for internal checks. | `assert` in debug builds, `((void)0)` with `NDEBUG` |
| `TDS_INITIAL_CAPACITY` | Initial requested capacity for growing containers. | `4` |

### Per-container macros

Define these immediately before a specific header inclusion:

| Macro | Description | Default |
|---|---|---|
| `TDS_DECLARE` | Emit declarations only. | Auto-defined with `TDS_IMPLEMENT` if both are omitted |
| `TDS_IMPLEMENT` | Emit implementations only. | Auto-defined with `TDS_DECLARE` if both are omitted |
| `TDS_TYPE` | Explicit name of the generated container type. | Derived from the container name and element types |
| `TDS_KEY_T` | Key type for key-value containers. | `int` |
| `TDS_VALUE_T` | Stored value type. | `int` |
| `TDS_SIZE_T` | Integer type used for counts, indices, and capacities. | `uint32_t` |
| `TDS_HASH_KEY(key)` | Hash expression for hash map keys. | `rapidhash(&key, sizeof(key))` |
| `TDS_KEY_EQUALS(a, b)` | Equality test for hash map keys. | `a == b` |
| `TDS_KEY_FINI(x)` | Cleanup hook run when a hash map key is removed or finalized. | Empty |
| `TDS_VALUE_FINI(x)` | Cleanup hook run when a stored value is removed or finalized. | Empty |

Notes:

- `TDS_HASH_KEY` and `TDS_KEY_EQUALS` apply to `hashmap.h`.
- The current `set.h` implementation hashes and compares values directly and does not expose equivalent customization hooks yet.
- `TDS_VALUE_FINI` applies to every container.

## Examples

### Basic vector usage

```c
#include <assert.h>
#include <tds/vector.h>

int main(void) {
    vec_int numbers = { 0 };

    vec_int_append(&numbers, 10);
    vec_int_append(&numbers, 20);
    vec_int_append(&numbers, 30);

    assert(vec_int_count(&numbers) == 3);
    assert(vec_int_get(&numbers, 1) == 20);

    int* slot = vec_int_grow(&numbers, 2);
    slot[0] = 40;
    slot[1] = 50;

    assert(vec_int_count(&numbers) == 5);
    assert(vec_int_first(&numbers)[4] == 50);

    vec_int_reclaim(&numbers);
    vec_int_fini(&numbers);
    return 0;
}
```

### Custom vector type

```c
#include <tds/vector.h>

#define TDS_VALUE_T short
#include <tds/vector.h>

#define TDS_TYPE byte_buffer
#define TDS_VALUE_T unsigned char
#include <tds/vector.h>

int main(void) {
    vec_short samples = { 0 };
    byte_buffer bytes = { 0 };

    vec_short_append(&samples, 7);
    byte_buffer_append(&bytes, 0xff);

    vec_short_fini(&samples);
    byte_buffer_fini(&bytes);
    return 0;
}
```

### Hash map with iteration

```c
#include <assert.h>
#include <stdio.h>

#define TDS_TYPE int_labels
#define TDS_KEY_T int
#define TDS_VALUE_T const char*
#include <tds/hashmap.h>

int main(void) {
    int_labels labels = { 0 };

    int_labels_reserve(&labels, 16);
    assert(int_labels_set(&labels, 1, "one"));
    assert(int_labels_set(&labels, 2, "two"));
    assert(!int_labels_set(&labels, 2, "two updated"));

    const char** value = int_labels_get(&labels, 2);
    assert(value && *value);
    puts(*value);

    int_labels_iter_t it = int_labels_iter(&labels);
    while (int_labels_next(&it)) {
        printf("%d -> %s\n", it.key, *it.value);
    }

    int_labels_fini(&labels);
    return 0;
}
```

### Hash map with an explicit generated type

```c
#include <assert.h>
#include <string.h>

#include <rapidhash.h>

#define TDS_TYPE word_counts
#define TDS_KEY_T const char*
#define TDS_VALUE_T int
#define TDS_HASH_KEY(key) rapidhash(key, strlen(key))
#define TDS_KEY_EQUALS(a, b) (strcmp((a), (b)) == 0)
#include <tds/hashmap.h>

int main(void) {
    word_counts counts = { 0 };

    word_counts_set(&counts, "apple", 3);
    word_counts_set(&counts, "pear", 1);

    int* count = word_counts_get(&counts, "apple");
    assert(count && *count == 3);

    word_counts_fini(&counts);
    return 0;
}
```

### Set and dense pool

```c
#include <assert.h>

#include <tds/set.h>
#include <tds/dense-pool.h>

int main(void) {
    set_int ids = { 0 };
    dense_pool_int pool = { 0 };

    assert(set_int_add(&ids, 7));
    assert(!set_int_add(&ids, 7));
    assert(set_int_add(&ids, 42));
    assert(set_int_count(&ids) == 2);
    assert(set_int_contains(&ids, 42));
    assert(set_int_remove(&ids, 42));
    assert(!set_int_remove(&ids, 42));

    int a = dense_pool_int_append(&pool, 100);
    int b = dense_pool_int_append(&pool, 200);
    assert(dense_pool_int_get(&pool, a) == 100);

    dense_pool_int_remove(&pool, a);
    int reused = dense_pool_int_append(&pool, 300);
    assert(reused == a);
    assert(dense_pool_int_get(&pool, b) == 200);

    set_int_fini(&ids);
    dense_pool_int_fini(&pool);
    return 0;
}
```

## Testing

Build and run the tests with a standard CMake workflow:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

## Motivation

For learning purposes, sharing the data structures across my projects, having full control over them and practicing
profiling and optimization.

## Roadmap

- Bitset
- Dynamic AABB tree
- More examples and deeper API notes
