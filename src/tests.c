#include <munit.h>

#include <limits.h>
#include <stdint.h>

#define TDS_KEY_T uint8_t
#include <tds/vector.h>

#define TDS_SIZE_T int8_t
#define TDS_KEY_T uint16_t
#include <tds/vector.h>

#include <tds/vector.h>

#define TDS_TYPE vector_u64
#define TDS_KEY_T uint64_t
#include <tds/vector.h>

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
  free(fixture);
}

static MunitResult push(const MunitParameter* params, void* fixture) {
  (void)params;
  test_data_structures_t* data_structures = fixture;

  // Use INT8_MAX due to using a TDS_SIZE_T of type int8_t
  for (int8_t i = 0; i < INT8_MAX; i++) {
    vec_uint8_t_push(&data_structures->uint8_vec, (uint8_t)munit_rand_int_range(0, UINT8_MAX));
    vec_uint16_t_push(&data_structures->uint16_vec, (uint16_t)munit_rand_int_range(0, UINT16_MAX));
    vec_int_push(&data_structures->int_vec, munit_rand_int_range(0, INT_MAX));
    vector_u64_push(&data_structures->uint64_vec, munit_rand_int_range(0, INT_MAX));

    munit_assert_int(vec_uint8_t_count(&data_structures->uint8_vec), ==, i + 1);
    munit_assert_int8(vec_uint16_t_count(&data_structures->uint16_vec), ==, i + 1);
    munit_assert_int(vec_int_count(&data_structures->int_vec), ==, i + 1);
    munit_assert_int(vector_u64_count(&data_structures->uint64_vec), ==, i + 1);
  }

  return MUNIT_OK;
}

static MunitResult get(const MunitParameter* params, void* fixture) {
  (void)params;
  test_data_structures_t* data_structures = fixture;

  int values[INT8_MAX];
  for (int8_t i = 0; i < INT8_MAX; i++) {
    values[i] = munit_rand_int_range(0, INT_MAX);
  }

  for (int8_t i = 0; i < INT8_MAX; i++) {
    vec_uint8_t_push(&data_structures->uint8_vec, (uint8_t)values[i]);
    vec_uint16_t_push(&data_structures->uint16_vec, (uint16_t)values[i]);
    vec_int_push(&data_structures->int_vec, values[i]);
    vector_u64_push(&data_structures->uint64_vec, values[i]);

    munit_assert_uint8(vec_uint8_t_get(&data_structures->uint8_vec, i), ==, (uint8_t)values[i]);
    munit_assert_uint16(vec_uint16_t_get(&data_structures->uint16_vec, i), ==, (uint16_t)values[i]);
    munit_assert_int(vec_int_get(&data_structures->int_vec, i), ==, values[i]);
    munit_assert_uint64(vector_u64_get(&data_structures->uint64_vec, i), ==, values[i]);
  }

  return MUNIT_OK;
}

static MunitResult count(const MunitParameter* params, void* fixture) {
  (void)params;
  test_data_structures_t* data_structures = fixture;

  const int max = munit_rand_int_range(0, INT8_MAX);
  for (int8_t i = 0; i < max; i++) {
    vec_uint8_t_push(&data_structures->uint8_vec, 0);
    vec_uint16_t_push(&data_structures->uint16_vec, 0);
    vec_int_push(&data_structures->int_vec, 0);
    vector_u64_push(&data_structures->uint64_vec, 0);

    munit_assert_int(vec_uint8_t_count(&data_structures->uint8_vec), ==, i + 1);
    munit_assert_int8(vec_uint16_t_count(&data_structures->uint16_vec), ==, i + 1);
    munit_assert_int(vec_int_count(&data_structures->int_vec), ==, i + 1);
    munit_assert_int(vector_u64_count(&data_structures->uint64_vec), ==, i + 1);
  }

  return MUNIT_OK;
}

int main(const int argc, char* const* argv) {
  MunitTest containers[] = {
    {
      .name = "push",
      .test = push,
      .setup = setup,
      .tear_down = tear_down,
    },
    {
      .name = "get",
      .test = get,
      .setup = setup,
      .tear_down = tear_down,
    },
    {
      .name = "count",
      .test = count,
      .setup = setup,
      .tear_down = tear_down,
    },
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
