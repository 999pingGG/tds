#include <limits.h>
#include <stdint.h>

#include <munit.h>

#define TDS_KEY_T uint8_t
#include <tds/vector.h>

#define TDS_SIZE_T int8_t
#define TDS_KEY_T uint16_t
#include <tds/vector.h>

#include <tds/vector.h>

#define TDS_TYPE vector_u64
#define TDS_KEY_T uint64_t
#include <tds/vector.h>

#define TDS_SIZE_T uint8_t
#define TDS_KEY_T uint8_t
#define TDS_VALUE_T uint64_t
#include <tds/hashmap.h>

#define TDS_TYPE hashmap_u16_to_u8
#define TDS_KEY_T uint16_t
#define TDS_VALUE_T uint8_t
#include <tds/hashmap.h>

#include <tds/hashmap.h>

#include <tds/set.h>

#ifndef TESTS_NO_STATIC_ASSERT
#include <assert.h>

static_assert(TDS_MAX_VALUE(int8_t) == INT8_MAX, "Macro is wrong.");
static_assert(TDS_MAX_VALUE(uint8_t) == UINT8_MAX, "Macro is wrong.");
static_assert(TDS_MAX_VALUE(int16_t) == INT16_MAX, "Macro is wrong.");
static_assert(TDS_MAX_VALUE(uint16_t) == UINT16_MAX, "Macro is wrong.");
static_assert(TDS_MAX_VALUE(int32_t) == INT32_MAX, "Macro is wrong.");
static_assert(TDS_MAX_VALUE(uint32_t) == UINT32_MAX, "Macro is wrong.");
static_assert(TDS_MAX_VALUE(int64_t) == INT64_MAX, "Macro is wrong.");
static_assert(TDS_MAX_VALUE(uint64_t) == UINT64_MAX, "Macro is wrong.");
#endif

typedef struct test_data_structures_t {
  vec_uint8_t uint8_vec;
  vec_uint16_t uint16_vec;
  vec_int int_vec;
  vector_u64 uint64_vec;
  hashmap_uint8_t_uint64_t uint64_hashmap;
  hashmap_u16_to_u8 u8_hashmap;
  hashmap_int_int int_hashmap;
  set_int int_set;
} test_data_structures_t;

static void* setup(const MunitParameter params[], void* user_data) {
  (void)params;
  (void)user_data;
  return calloc(1, sizeof(test_data_structures_t));
}

static void tear_down(void* fixture) {
  test_data_structures_t* data_structures = fixture;

  vec_uint8_t_fini(&data_structures->uint8_vec);
  vec_uint16_t_fini(&data_structures->uint16_vec);
  vec_int_fini(&data_structures->int_vec);
  vector_u64_fini(&data_structures->uint64_vec);
  hashmap_uint8_t_uint64_t_fini(&data_structures->uint64_hashmap);
  hashmap_u16_to_u8_fini(&data_structures->u8_hashmap);
  hashmap_int_int_fini(&data_structures->int_hashmap);
  set_int_fini(&data_structures->int_set);
  free(fixture);
}

static MunitResult append(const MunitParameter* params, void* fixture) {
  (void)params;
  test_data_structures_t* data_structures = fixture;

  // Use INT8_MAX due to using a TDS_SIZE_T of type int8_t
  for (int8_t i = 0; i < INT8_MAX; i++) {
    vec_uint8_t_append(&data_structures->uint8_vec, (uint8_t)munit_rand_int_range(0, UINT8_MAX));
    vec_uint16_t_append(&data_structures->uint16_vec, (uint16_t)munit_rand_int_range(0, UINT16_MAX));
    vec_int_append(&data_structures->int_vec, munit_rand_int_range(0, INT_MAX));
    vector_u64_append(&data_structures->uint64_vec, munit_rand_int_range(0, INT_MAX));
    hashmap_uint8_t_uint64_t_set(
      &data_structures->uint64_hashmap,
      (uint8_t)munit_rand_int_range(0, UINT8_MAX),
      munit_rand_int_range(0, INT_MAX)
    );
    hashmap_u16_to_u8_set(
      &data_structures->u8_hashmap,
      (uint16_t)munit_rand_int_range(0, UINT16_MAX),
      (uint8_t)munit_rand_int_range(0, UINT8_MAX)
    );
    hashmap_int_int_set(
      &data_structures->int_hashmap,
      munit_rand_int_range(0, INT_MAX),
      munit_rand_int_range(0, INT_MAX)
    );
    set_int_add(&data_structures->int_set, munit_rand_int_range(0, INT_MAX));

    munit_assert_int(vec_uint8_t_count(&data_structures->uint8_vec), ==, i + 1);
    munit_assert_int8(vec_uint16_t_count(&data_structures->uint16_vec), ==, i + 1);
    munit_assert_int(vec_int_count(&data_structures->int_vec), ==, i + 1);
    munit_assert_int(vector_u64_count(&data_structures->uint64_vec), ==, i + 1);
    munit_assert_int(set_int_count(&data_structures->int_set), == , i + 1);
  }

  return MUNIT_OK;
}

static MunitResult remove_test(const MunitParameter* params, void* fixture) {
  (void)params;
  test_data_structures_t* data_structures = fixture;

  int values[INT8_MAX];
  for (int8_t i = 0; i < INT8_MAX; i++) {
    values[i] = munit_rand_int_range(0, UINT16_MAX);
    vec_uint8_t_append(&data_structures->uint8_vec, (uint8_t)values[i]);
    vec_uint16_t_append(&data_structures->uint16_vec, (uint16_t)values[i]);
    vec_int_append(&data_structures->int_vec, values[i]);
    vector_u64_append(&data_structures->uint64_vec, values[i]);
    hashmap_uint8_t_uint64_t_set(&data_structures->uint64_hashmap, i, values[i]);
    hashmap_u16_to_u8_set(&data_structures->u8_hashmap, i, (uint8_t)values[i]);
    hashmap_int_int_set(&data_structures->int_hashmap, i, values[i]);
    set_int_add(&data_structures->int_set, values[i]);
  }

  vec_uint8_t_remove(&data_structures->uint8_vec, 0);
  munit_assert_uint8(vec_uint8_t_get(&data_structures->uint8_vec, 0), ==, (uint8_t)values[1]);
  munit_assert_uint8(vec_uint8_t_get(&data_structures->uint8_vec, 100), ==, (uint8_t)values[101]);
  munit_assert_uint32(vec_uint8_t_count(&data_structures->uint8_vec), ==, 126);

  for (int i = 0; i < 80; i++) {
    vec_uint8_t_remove(&data_structures->uint8_vec, 1);
    munit_assert_uint8(vec_uint8_t_get(&data_structures->uint8_vec, 0), ==, (uint8_t)values[1]);
    munit_assert_uint8(vec_uint8_t_get(&data_structures->uint8_vec, 10), ==, (uint8_t)values[12 + i]);
    munit_assert_uint32(vec_uint8_t_count(&data_structures->uint8_vec), ==, 125 - i);
  }

  vec_uint16_t_remove(&data_structures->uint16_vec, 10);
  munit_assert_int(vec_uint16_t_get(&data_structures->uint16_vec, 20), ==, values[21]);
  munit_assert_int(vec_uint16_t_get(&data_structures->uint16_vec, 125), ==, values[126]);
  munit_assert_int8(vec_uint16_t_count(&data_structures->uint16_vec), ==, 126);

  vec_uint16_t_remove(&data_structures->uint16_vec, 125);
  munit_assert_int(vec_uint16_t_get(&data_structures->uint16_vec, 0), ==, values[0]);
  munit_assert_int(vec_uint16_t_get(&data_structures->uint16_vec, 124), ==, values[125]);
  munit_assert_int8(vec_uint16_t_count(&data_structures->uint16_vec), ==, 125);

  while (vec_uint16_t_count(&data_structures->uint16_vec)) {
    vec_uint16_t_remove(&data_structures->uint16_vec, 0);
  }
  munit_assert_int8(vec_uint16_t_count(&data_structures->uint16_vec), ==, 0);

  vector_u64_remove(&data_structures->uint64_vec, vector_u64_count(&data_structures->uint64_vec) - 1);
  munit_assert_uint64(
    vector_u64_get(&data_structures->uint64_vec, vector_u64_count(&data_structures->uint64_vec) - 1), ==, values[125]
  );

  int counts[] = {
    hashmap_uint8_t_uint64_t_count(&data_structures->uint64_hashmap),
    hashmap_u16_to_u8_count(&data_structures->u8_hashmap),
    hashmap_int_int_count(&data_structures->int_hashmap),
  };
  for (
    int i = 300, j = 300, k = 300;
    i >= 0 && j >= 0 && k >= 0;
    i -= munit_rand_int_range(0, 5), j -= munit_rand_int_range(0, 5), k -= munit_rand_int_range(0, 5)
  ) {
    if (hashmap_uint8_t_uint64_t_get(&data_structures->uint64_hashmap, (uint8_t)i)) {
      counts[0]--;
    }
    if (hashmap_u16_to_u8_get(&data_structures->u8_hashmap, (uint16_t)i)) {
      counts[1]--;
    }
    if (hashmap_int_int_get(&data_structures->int_hashmap, i)) {
      counts[2]--;
    }
    hashmap_uint8_t_uint64_t_remove(&data_structures->uint64_hashmap, (uint8_t)i);
    hashmap_uint8_t_uint64_t_remove(&data_structures->uint64_hashmap, (uint8_t)i);
    hashmap_uint8_t_uint64_t_remove(&data_structures->uint64_hashmap, (uint8_t)i);
    hashmap_u16_to_u8_remove(&data_structures->u8_hashmap, (uint16_t)i);
    hashmap_u16_to_u8_remove(&data_structures->u8_hashmap, (uint16_t)i);
    hashmap_int_int_remove(&data_structures->int_hashmap, i);
  }

  munit_assert_int(hashmap_uint8_t_uint64_t_count(&data_structures->uint64_hashmap), ==, counts[0]);
  munit_assert_int(hashmap_u16_to_u8_count(&data_structures->u8_hashmap), ==, counts[1]);
  munit_assert_int(hashmap_int_int_count(&data_structures->int_hashmap), ==, counts[2]);

  for (int i = 0; i < 100; i++) {
    int should_contain = 1;
    for (int j = 0; j < i; j++) {
      if (values[j] == values[i]) {
        // If the value appears more than once, then it's already removed.
        should_contain = 0;
        break;
      }
    }
    munit_assert_int(set_int_contains(&data_structures->int_set, values[i]), ==, should_contain);
    set_int_remove(&data_structures->int_set, values[i]);
  }
  munit_assert_uint32(set_int_count(&data_structures->int_set), ==, TDS_COUNTOF(values) - 100);

  return MUNIT_OK;
}

static MunitResult get_set(const MunitParameter* params, void* fixture) {
  (void)params;
  test_data_structures_t* data_structures = fixture;

  int values[INT8_MAX];
  for (int8_t i = 0; i < INT8_MAX; i++) {
    values[i] = munit_rand_int_range(0, INT_MAX);
  }

  for (int8_t i = 0; i < INT8_MAX; i++) {
    vec_uint8_t_append(&data_structures->uint8_vec, (uint8_t)values[i]);
    vec_uint16_t_append(&data_structures->uint16_vec, (uint16_t)values[i]);
    vec_int_append(&data_structures->int_vec, values[i]);
    vector_u64_append(&data_structures->uint64_vec, values[i]);

    hashmap_uint8_t_uint64_t_set(
      &data_structures->uint64_hashmap,
      (uint8_t)i,
      values[i]
    );
    int count = hashmap_uint8_t_uint64_t_count(&data_structures->uint64_hashmap);
    hashmap_uint8_t_uint64_t_set(
      &data_structures->uint64_hashmap,
      (uint8_t)i,
      values[i]
    );
    munit_assert_int(count, ==, hashmap_uint8_t_uint64_t_count(&data_structures->uint64_hashmap));

    hashmap_u16_to_u8_set(
      &data_structures->u8_hashmap,
      (uint16_t)i,
      (uint8_t)values[i]
    );
    count = hashmap_u16_to_u8_count(&data_structures->u8_hashmap);
    hashmap_u16_to_u8_set(
      &data_structures->u8_hashmap,
      (uint16_t)i,
      (uint8_t)values[i]
    );
    munit_assert_int(count, ==, hashmap_u16_to_u8_count(&data_structures->u8_hashmap));

    hashmap_int_int_set(
      &data_structures->int_hashmap,
      i,
      values[i]
    );
    count = hashmap_int_int_count(&data_structures->int_hashmap);
    hashmap_int_int_set(
      &data_structures->int_hashmap,
      i,
      values[i]
    );
    munit_assert_int(count, ==, hashmap_int_int_count(&data_structures->int_hashmap));
  }

  for (int8_t i = 0; i < INT8_MAX; i++) {
    munit_assert_uint8(vec_uint8_t_get(&data_structures->uint8_vec, i), == , (uint8_t)values[i]);
    munit_assert_uint16(vec_uint16_t_get(&data_structures->uint16_vec, i), == , (uint16_t)values[i]);
    munit_assert_int(vec_int_get(&data_structures->int_vec, i), == , values[i]);
    munit_assert_uint64(vector_u64_get(&data_structures->uint64_vec, i), == , values[i]);

    const uint64_t* result1 = hashmap_uint8_t_uint64_t_get(&data_structures->uint64_hashmap, (uint8_t)i);
    munit_assert_not_null(result1);
    munit_assert_uint64(*result1, == , values[i]);

    const uint8_t* result2 = hashmap_u16_to_u8_get(&data_structures->u8_hashmap, (uint16_t)i);
    munit_assert_not_null(result2);
    munit_assert_uint8(*result2, == , (uint8_t)values[i]);

    const int* result3 = hashmap_int_int_get(&data_structures->int_hashmap, i);
    munit_assert_not_null(result3);
    munit_assert_int(*result3, == , values[i]);
  }

  return MUNIT_OK;
}

static MunitResult count(const MunitParameter* params, void* fixture) {
  (void)params;
  test_data_structures_t* data_structures = fixture;

  const int8_t max = (int8_t)munit_rand_int_range(0, INT8_MAX);
  for (int8_t i = 0; i < max; i++) {
    vec_uint8_t_append(&data_structures->uint8_vec, 0);
    vec_uint16_t_append(&data_structures->uint16_vec, 0);
    vec_int_append(&data_structures->int_vec, 0);
    vector_u64_append(&data_structures->uint64_vec, 0);
    hashmap_uint8_t_uint64_t_set(&data_structures->uint64_hashmap, 0, 0);
    hashmap_u16_to_u8_set(&data_structures->u8_hashmap, i, 0);
    hashmap_int_int_set(&data_structures->int_hashmap, i * 5, 0);

    munit_assert_int(vec_uint8_t_count(&data_structures->uint8_vec), ==, i + 1);
    munit_assert_int8(vec_uint16_t_count(&data_structures->uint16_vec), ==, i + 1);
    munit_assert_int(vec_int_count(&data_structures->int_vec), ==, i + 1);
    munit_assert_int(vector_u64_count(&data_structures->uint64_vec), ==, i + 1);
    munit_assert_uint32(hashmap_uint8_t_uint64_t_count(&data_structures->uint64_hashmap), ==, 1);
    munit_assert_uint32(hashmap_u16_to_u8_count(&data_structures->u8_hashmap), ==, i + 1);
    munit_assert_uint32(hashmap_int_int_count(&data_structures->int_hashmap), ==, i + 1);
  }

  return MUNIT_OK;
}

#define TDS_TEST(fun) {\
  .name = "/"#fun,\
  .test = fun,\
  .setup = setup,\
  .tear_down = tear_down,\
}

int main(const int argc, char* const* argv) {
  MunitTest containers[] = {
    TDS_TEST(append),
    TDS_TEST(remove_test),
    TDS_TEST(get_set),
    TDS_TEST(count),
    { 0 },
  };

  MunitSuite suites[] = {
    {
      .prefix = "/containers",
      .tests = containers,
    },
    { 0 },
  };

  const MunitSuite main_suite = {
    .prefix = "/tiny-data-structures",
    .suites = suites,
    .iterations = 10000,
  };

  return munit_suite_main(&main_suite, NULL, argc, argv);
}
