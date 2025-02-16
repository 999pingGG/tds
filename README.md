# Tiny Data Structures
A WIP header-only collection of simple, type-safe data structures and containers for C. So far compiles absolutely
warning-free.

## Requirements
A C89 compiler. C11 is only required for the µnit library and `static_assert` in the tests, but you can disable it by
defining `TESTS_NO_STATIC_ASSERT`. Standard library not required as long as you provide your own memory management
functions. The tests are trivial to compile, but I'm using CMake here.

## Installation
Make the `include/tds` directory available inside any include directory you want. You can omit any files corresponding
to data structures you don't need, but the `private` directory is required for all of them. Optionally define the
`TDS_REALLOC` and `TDS_FREE` macros to expand to functions whose signature match standard `realloc` and `free` before
using any of the data structures. There's also `TDS_ASSERT` which you can define.

## Usage
`#include`'ing any of the data structure's header will insert its declarations and implementation of functions.  Every
time a header is included a new data structure is implemented. You can control the types contained in them by defining
macros before inclusion.
- It's a must to zero-initialize the structs!
- Use the `fini` function to free the memory!
- All the data is shallow-copied, no pointers are followed.
- You shouldn't use the struct members directly, always use the interface functions instead, they provide safety and are
future-proof.
  
The following data structures are available:

| Data structure | Description      | Default name | Available functions                              |
|----------------|------------------|--------------|--------------------------------------------------|
| Vector         | A dynamic array. | `vec`        | `push`, `get`, `count`, `first`, `clear`, `fini` |

The following macros need to be defined just once:

| Macro                  | Description                                                 | Default                                        |
|------------------------|-------------------------------------------------------------|------------------------------------------------|
| `TDS_PREFIX`           | The data structure types are prefixed with this by default. | Empty.                                         |
| `TDS_REALLOC`          | The `realloc` standard function.                            | `realloc`                                      |
| `TDS_FREE`             | The `free` standard function.                               | `free`                                         |
| `TDS_ASSERT`           | The `assert` standard function.                             | `assert` in debug mode, `((void)0)` otherwise. |
| `TDS_INITIAL_CAPACITY` | The initial capacity assigned to data structures.           | `4`                                            |

The following macros need to be defined before implementing a data structure:

| Macro                             | Description                                                                                                      | Default                                                                                                                                                                                               |
|-----------------------------------|------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `TDS_DECLARE` and `TDS_IMPLEMENT` | Insert only the declarations and implementations, respectively.                                                  | Both are automatically defined if both are left out.                                                                                                                                                  |
| `TDS_TYPE`                        | The name of the container's type.                                                                                | `{TDS_PREFIX}{data structure name}_{TDS_KEY_T}` for containers that don't use key-value pairs, `{TDS_PREFIX}{data structure name}_{TDS_KEY_T}_{TDS_VALUE_T}` for containers that use key-value pairs. | 
| `TDS_KEY_T`                       | The type that will be used for keys in containers, or for elements in containers that don't use key-value pairs. | `int`                                                                                                                                                                                                 |
| `TDS_VALUE_T`                     | The type that will be used for values in containers.                                                             | `int`                                                                                                                                                                                                 |
| `TDS_SIZE_T`                      | The type that will be used for sizes, for example, `count`, `capacity`, etc.                                     | `uint32_t`                                                                                                                                                                                            |

## Examples
```c
#include <string.h>

#include <tds/vector.h>

// Now the definitions and declarations for the type `vec_int` are available.

void do_something_with_memory(void* memory, const size_t size) {
  memset(memory, 66, size);
}

int main(int argc, char** argv) {
  vec_int v = { 0 };

  vec_int_push(&v, 117);
  assert(vec_int_get(&v, 0) == 117);
  assert(vec_int_count(&v) == 1);

  vec_int_push(&v, 343);
  assert(vec_int_get(&v, 1) == 343);
  assert(vec_int_count(&v) == 2);

  vec_int_push(&v, 2401);
  assert(vec_int_get(&v, 2) == 2401);
  assert(vec_int_count(&v) == 3);

  do_something_with_memory(vec_int_first(&v), vec_int_count(&v));

  vec_int_fini(&v);
  return 0;
}

#define TDS_KEY_T short
#include <tds/vector.h>

// Now the type `vec_short` is available along with its methods.

#define TDS_TYPE my_vector
#define TDS_KEY_T char
#include <tds/vector.h>

// Now there's a `my_vector` type holding an array of chars.
```

## Testing
Do a standard CMake build to build the tests.

## Motivation
For learning purposes, sharing the data structures across my projects and having full control over them.

## Roadmap
- Hashmap
- Set
- Bitset
- Pool
- Octree
