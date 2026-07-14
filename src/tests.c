#include <limits.h>
#include <stdint.h>

#include <munit.h>

#define TDS_BIT_COUNT 1
#include <tds/bitset.h>

#define TDS_BIT_COUNT 64
#include <tds/bitset.h>

#define TDS_BIT_COUNT 65
#include <tds/bitset.h>

#define TDS_TYPE small_bitset
#define TDS_BIT_COUNT 10
#define TDS_WORD_T uint8_t
#include <tds/bitset.h>

#define TDS_VALUE_T uint8_t
#include <tds/vector.h>

#define TDS_SIZE_T int8_t
#define TDS_VALUE_T uint16_t
#include <tds/vector.h>

#include <tds/vector.h>

#define TDS_TYPE vector_u64
#define TDS_VALUE_T uint64_t
#include <tds/vector.h>

#include <tds/queue.h>

#define TDS_TYPE small_queue
#define TDS_SIZE_T uint8_t
#define TDS_VALUE_T uint16_t
#include <tds/queue.h>

static unsigned queue_values_finalized;

static void finalize_queue_value(const int value) {
    (void)value;
    queue_values_finalized++;
}

#define TDS_TYPE cleanup_queue
#define TDS_VALUE_FINI(value) finalize_queue_value(value)
#include <tds/queue.h>

#define TDS_SIZE_T uint8_t
#define TDS_KEY_T uint8_t
#define TDS_VALUE_T uint64_t
#include <tds/hashmap.h>

#define TDS_TYPE hashmap_u16_to_u8
#define TDS_KEY_T uint16_t
#define TDS_VALUE_T uint8_t
#include <tds/hashmap.h>

#include <tds/hashmap.h>

#define TDS_SIZE_T uint8_t
#include <tds/dense-pool.h>

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
    queue_int int_queue;
    small_queue small_queue;
    cleanup_queue cleanup_queue;
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

static void* setup_dense_pool(const MunitParameter params[], void* user_data) {
    (void)params;
    (void)user_data;
    return calloc(1, sizeof(dense_pool_int));
}

static void tear_down(void* fixture) {
    test_data_structures_t* data_structures = fixture;

    vec_uint8_t_fini(&data_structures->uint8_vec);
    vec_uint16_t_fini(&data_structures->uint16_vec);
    vec_int_fini(&data_structures->int_vec);
    vector_u64_fini(&data_structures->uint64_vec);
    queue_int_fini(&data_structures->int_queue);
    small_queue_fini(&data_structures->small_queue);
    cleanup_queue_fini(&data_structures->cleanup_queue);
    hashmap_uint8_t_uint64_t_fini(&data_structures->uint64_hashmap);
    hashmap_u16_to_u8_fini(&data_structures->u8_hashmap);
    hashmap_int_int_fini(&data_structures->int_hashmap);
    set_int_fini(&data_structures->int_set);
    free(fixture);
}

static void tear_down_dense_pool(void* fixture) {
    dense_pool_int_fini(fixture);
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
            munit_rand_int_range(0, INT_MAX));
        hashmap_u16_to_u8_set(
            &data_structures->u8_hashmap,
            (uint16_t)munit_rand_int_range(0, UINT16_MAX),
            (uint8_t)munit_rand_int_range(0, UINT8_MAX));
        hashmap_int_int_set(
            &data_structures->int_hashmap,
            munit_rand_int_range(0, INT_MAX),
            munit_rand_int_range(0, INT_MAX));
        set_int_add(&data_structures->int_set, munit_rand_int_range(0, INT_MAX));

        munit_assert_int(vec_uint8_t_count(&data_structures->uint8_vec), ==, i + 1);
        munit_assert_int8(vec_uint16_t_count(&data_structures->uint16_vec), ==, i + 1);
        munit_assert_int(vec_int_count(&data_structures->int_vec), ==, i + 1);
        munit_assert_int(vector_u64_count(&data_structures->uint64_vec), ==, i + 1);
        munit_assert_int(set_int_count(&data_structures->int_set), == , i + 1);
    }

    return MUNIT_OK;
}

static MunitResult append_and_remove_dense_pool(const MunitParameter* params, void* fixture) {
    (void)params;
    dense_pool_int* pool = fixture;

    int values[UINT8_MAX];
    for (uint8_t i = 0; i < UINT8_MAX; i++) {
        values[i] = i * 100;
        const uint8_t id = dense_pool_int_append(pool, values[i]);
        munit_assert_uint8(id, ==, i);
        munit_assert_uint8(dense_pool_int_count(pool), ==, i + 1);
        munit_assert_int(dense_pool_int_get(pool, id), ==, values[i]);
    }

    munit_assert_uint8(dense_pool_int_remove(pool, 254), ==, 254);
    munit_assert_uint8(dense_pool_int_append(pool, 254), ==, 254);

    uint8_t removed[20];
    for (unsigned i = 0, next = 0; i < TDS_COUNTOF(removed); i++, next += munit_rand_int_range(1, 10)) {
        assert(i < UINT8_MAX && next < UINT8_MAX);
        removed[i] = (uint8_t)next;
        munit_assert_uint8(dense_pool_int_remove(pool, removed[i]), ==, removed[i]);
        munit_assert_uint8(dense_pool_int_count(pool), ==, (uint8_t)(UINT8_MAX - i - 1));
    }

    for (int i = TDS_COUNTOF(removed) - 1; i >= 0; i--) {
        munit_assert_uint8(dense_pool_int_append(pool, 0), ==, removed[i]);
    }

    const uint8_t expected_id = pool->count - 1;
    dense_pool_int_remove(pool, expected_id);
    munit_assert_uint8(dense_pool_int_append(pool, 0), ==, expected_id);

    return MUNIT_OK;
}

static MunitResult dense_pool_id_reuse(const MunitParameter* params, void* fixture) {
    (void)params;
    dense_pool_int* pool = fixture;

    dense_pool_int_append(pool, 44);
    dense_pool_int_append(pool, 55);

    munit_assert_uint8(pool->dense[pool->sparse[0]], ==, 0);
    munit_assert_uint8(pool->dense[pool->sparse[1]], ==, 1);

    uint8_t id = dense_pool_int_remove(pool, 0);
    munit_assert_uint8(id, ==, 0);
    id = dense_pool_int_remove(pool, 1);
    munit_assert_uint8(id, ==, 0);

    id = dense_pool_int_append(pool, 44);
    munit_assert_uint8(id, ==, 1);
    id = dense_pool_int_append(pool, 55);
    munit_assert_uint8(id, ==, 0);

    munit_assert_uint8(pool->dense[pool->sparse[0]], ==, 0);
    munit_assert_uint8(pool->dense[pool->sparse[1]], ==, 1);

    dense_pool_int_remove(pool, 1);
    dense_pool_int_remove(pool, 0);

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
        const int was_in_set = set_int_contains(&data_structures->int_set, values[i]);
        munit_assert_int(set_int_add(&data_structures->int_set, values[i]), ==, !was_in_set);
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
        vector_u64_get(&data_structures->uint64_vec, vector_u64_count(&data_structures->uint64_vec) - 1),
        ==,
        values[125]);

    unsigned counts[] = {
        hashmap_uint8_t_uint64_t_count(&data_structures->uint64_hashmap),
        hashmap_u16_to_u8_count(&data_structures->u8_hashmap),
        hashmap_int_int_count(&data_structures->int_hashmap),
    };
    for (
        int i = 300, j = 300, k = 300;
        i >= 0 && j >= 0 && k >= 0;
        i -= munit_rand_int_range(0, 5), j -= munit_rand_int_range(0, 5), k -= munit_rand_int_range(0, 5)
    ) {
        const int was_in_uint64_hashmap =
            hashmap_uint8_t_uint64_t_get(&data_structures->uint64_hashmap, (uint8_t)i) != NULL;
        if (was_in_uint64_hashmap) {
            counts[0]--;
        }
        if (hashmap_u16_to_u8_get(&data_structures->u8_hashmap, (uint16_t)i)) {
            counts[1]--;
        }
        if (hashmap_int_int_get(&data_structures->int_hashmap, i)) {
            counts[2]--;
        }
        munit_assert_int(
            hashmap_uint8_t_uint64_t_remove(&data_structures->uint64_hashmap, (uint8_t)i),
            ==,
            was_in_uint64_hashmap
        );
        munit_assert_false(hashmap_uint8_t_uint64_t_remove(&data_structures->uint64_hashmap, (uint8_t)i));
        munit_assert_false(hashmap_uint8_t_uint64_t_remove(&data_structures->uint64_hashmap, (uint8_t)i));
        hashmap_u16_to_u8_remove(&data_structures->u8_hashmap, (uint16_t)i);
        hashmap_u16_to_u8_remove(&data_structures->u8_hashmap, (uint16_t)i);
        hashmap_int_int_remove(&data_structures->int_hashmap, i);
    }

    munit_assert_uint(hashmap_uint8_t_uint64_t_count(&data_structures->uint64_hashmap), ==, counts[0]);
    munit_assert_uint(hashmap_u16_to_u8_count(&data_structures->u8_hashmap), ==, counts[1]);
    munit_assert_uint(hashmap_int_int_count(&data_structures->int_hashmap), ==, counts[2]);

    const unsigned total = set_int_count(&data_structures->int_set);
    int removed = 0;
    for (int i = 0; i < 100; i++) {
        int should_contain = 1;
        for (int j = 0; j < i; j++) {
            if (values[j] == values[i]) {
                // If the value appears more than once, then it's already removed.
                should_contain = 0;
                break;
            }
        }
        if (should_contain) {
            removed++;
        }
        munit_assert_int(set_int_contains(&data_structures->int_set, values[i]), ==, should_contain);
        const unsigned count = set_int_count(&data_structures->int_set);
        munit_assert_int(set_int_remove(&data_structures->int_set, values[i]), ==, should_contain);
        munit_assert_false(set_int_remove(&data_structures->int_set, values[i]));
        munit_assert_int(count - should_contain, ==, set_int_count(&data_structures->int_set));
    }
    munit_assert_uint32(set_int_count(&data_structures->int_set), ==, total - removed);

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

        munit_assert_true(hashmap_uint8_t_uint64_t_set(
            &data_structures->uint64_hashmap,
            (uint8_t)i,
            values[i]
        ));
        unsigned count = hashmap_uint8_t_uint64_t_count(&data_structures->uint64_hashmap);
        munit_assert_false(hashmap_uint8_t_uint64_t_set(
            &data_structures->uint64_hashmap,
            (uint8_t)i,
            values[i]
        ));
        munit_assert_int(count, ==, hashmap_uint8_t_uint64_t_count(&data_structures->uint64_hashmap));

        hashmap_u16_to_u8_set(&data_structures->u8_hashmap, (uint16_t)i, (uint8_t)values[i]);
        count = hashmap_u16_to_u8_count(&data_structures->u8_hashmap);
        hashmap_u16_to_u8_set(&data_structures->u8_hashmap, (uint16_t)i, (uint8_t)values[i]);
        munit_assert_int(count, ==, hashmap_u16_to_u8_count(&data_structures->u8_hashmap));

        hashmap_int_int_set(&data_structures->int_hashmap, i, values[i]);
        count = hashmap_int_int_count(&data_structures->int_hashmap);
        hashmap_int_int_set(&data_structures->int_hashmap, i, values[i]);
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

static MunitResult queue_fifo_and_wrap(const MunitParameter* params, void* fixture) {
    (void)params;
    test_data_structures_t* data_structures = fixture;
    queue_int* queue = &data_structures->int_queue;

    munit_assert_uint32(queue_int_count(queue), ==, 0);
    munit_assert_null(queue_int_front(queue));

    queue_int_reserve(queue, 4);
    for (int i = 0; i < 4; i++) {
        queue_int_push(queue, i);
    }
    munit_assert_int(queue_int_pop(queue), ==, 0);
    munit_assert_int(queue_int_pop(queue), ==, 1);

    queue_int_push(queue, 4);
    queue_int_push(queue, 5);
    munit_assert_uint32(queue->capacity, ==, 4);
    munit_assert_uint32(queue->head, ==, 2);
    for (int expected = 2; expected < 6; expected++) {
        munit_assert_int(*queue_int_front(queue), ==, expected);
        munit_assert_int(queue_int_pop(queue), ==, expected);
    }

    munit_assert_null(queue_int_front(queue));
    munit_assert_uint32(queue_int_count(queue), ==, 0);
    return MUNIT_OK;
}

static MunitResult queue_growth_and_storage(const MunitParameter* params, void* fixture) {
    (void)params;
    test_data_structures_t* data_structures = fixture;
    small_queue* queue = &data_structures->small_queue;

    for (uint16_t i = 0; i < 4; i++) {
        small_queue_push(queue, i);
    }
    munit_assert_uint8(queue->capacity, ==, 4);
    small_queue_push(queue, 4);
    munit_assert_uint8(queue->capacity, ==, 8);
    munit_assert_uint8(queue->head, ==, 0);

    munit_assert_uint16(small_queue_pop(queue), ==, 0);
    munit_assert_uint16(small_queue_pop(queue), ==, 1);
    for (uint16_t i = 5; i < 10; i++) {
        small_queue_push(queue, i);
    }
    munit_assert_uint8(queue->head, ==, 2);
    small_queue_push(queue, 10);
    munit_assert_uint8(queue->capacity, ==, 16);
    munit_assert_uint8(queue->head, ==, 0);
    for (uint16_t expected = 2; expected <= 10; expected++) {
        munit_assert_uint16(small_queue_pop(queue), ==, expected);
    }

    small_queue_reserve(queue, 12);
    small_queue_push(queue, 20);
    small_queue_push(queue, 21);
    small_queue_reclaim(queue);
    munit_assert_uint8(queue->capacity, ==, 2);
    munit_assert_uint8(queue->head, ==, 0);
    munit_assert_uint16(small_queue_pop(queue), ==, 20);
    munit_assert_uint16(small_queue_pop(queue), ==, 21);
    small_queue_reclaim(queue);
    munit_assert_null(queue->array);
    munit_assert_uint8(queue->capacity, ==, 0);

    small_queue_push(queue, 30);
    small_queue_clear(queue);
    munit_assert_uint8(queue->count, ==, 0);
    munit_assert_not_null(queue->array);
    return MUNIT_OK;
}

static MunitResult queue_cleanup(const MunitParameter* params, void* fixture) {
    (void)params;
    test_data_structures_t* data_structures = fixture;
    cleanup_queue* queue = &data_structures->cleanup_queue;

    queue_values_finalized = 0;
    cleanup_queue_push(queue, 1);
    cleanup_queue_push(queue, 2);
    cleanup_queue_push(queue, 3);
    munit_assert_int(cleanup_queue_pop(queue), ==, 1);
    munit_assert_uint(queue_values_finalized, ==, 0);
    cleanup_queue_clear(queue);
    munit_assert_uint(queue_values_finalized, ==, 2);

    cleanup_queue_push(queue, 4);
    cleanup_queue_push(queue, 5);
    cleanup_queue_fini(queue);
    munit_assert_uint(queue_values_finalized, ==, 4);
    return MUNIT_OK;
}

static MunitResult bitset(const MunitParameter* params, void* fixture) {
    (void)params;
    (void)fixture;

    bitset_1_t one = { 0 };
    munit_assert_false(bitset_1_t_any(&one));
    munit_assert_true(bitset_1_t_none(&one));
    munit_assert_false(bitset_1_t_get(&one, 0));
    bitset_1_t_set_all(&one);
    munit_assert_true(bitset_1_t_any(&one));
    munit_assert_false(bitset_1_t_none(&one));
    munit_assert_true(bitset_1_t_get(&one, 0));
    bitset_1_t_clear(&one, 0);
    munit_assert_false(bitset_1_t_any(&one));
    munit_assert_true(bitset_1_t_none(&one));

    bitset_64_t exact_word = { 0 };
    bitset_64_t_set(&exact_word, 0);
    bitset_64_t_set(&exact_word, 63);
    munit_assert_true(bitset_64_t_get(&exact_word, 0));
    munit_assert_true(bitset_64_t_get(&exact_word, 63));
    bitset_64_t_clear_all(&exact_word);
    munit_assert_false(bitset_64_t_any(&exact_word));
    munit_assert_true(bitset_64_t_none(&exact_word));

    bitset_65_t across_words = { 0 };
    bitset_65_t_set(&across_words, 63);
    bitset_65_t_set(&across_words, 64);
    munit_assert_true(bitset_65_t_get(&across_words, 63));
    munit_assert_true(bitset_65_t_get(&across_words, 64));
    bitset_65_t_clear(&across_words, 63);
    munit_assert_false(bitset_65_t_get(&across_words, 63));
    munit_assert_true(bitset_65_t_any(&across_words));
    munit_assert_false(bitset_65_t_none(&across_words));

    bitset_65_t_set_all(&across_words);
    for (uint32_t i = 0; i < 65; i++) {
        munit_assert_true(bitset_65_t_get(&across_words, i));
        bitset_65_t_clear(&across_words, i);
    }
    munit_assert_false(bitset_65_t_any(&across_words));
    munit_assert_true(bitset_65_t_none(&across_words));

    small_bitset small = { 0 };
    small_bitset_set_all(&small);
    for (uint32_t i = 0; i < 10; i++) {
        munit_assert_true(small_bitset_get(&small, i));
        small_bitset_clear(&small, i);
    }
    munit_assert_false(small_bitset_any(&small));
    munit_assert_true(small_bitset_none(&small));

    return MUNIT_OK;
}

#define TDS_TEST(fun) {\
    .name = "/"#fun,\
    .test = (fun),\
    .setup = setup,\
    .tear_down = tear_down,\
}

#define TDS_DENSE_POOL_TEST(fun) {\
    .name = "/"#fun,\
    .test = (fun),\
    .setup = setup_dense_pool,\
    .tear_down = tear_down_dense_pool,\
}

int main(const int argc, char* const* argv) {
    MunitTest containers[] = {
        TDS_TEST(append),
        TDS_TEST(remove_test),
        TDS_TEST(get_set),
        TDS_TEST(count),
        { 0 },
    };

    MunitTest dense_pool[] = {
        TDS_DENSE_POOL_TEST(append_and_remove_dense_pool),
        TDS_DENSE_POOL_TEST(dense_pool_id_reuse),
        { 0 },
    };

    MunitTest bitsets[] = {
        {
            .name = "/operations",
            .test = bitset,
        },
        { 0 },
    };

    MunitTest queues[] = {
        TDS_TEST(queue_fifo_and_wrap),
        TDS_TEST(queue_growth_and_storage),
        TDS_TEST(queue_cleanup),
        { 0 },
    };

    MunitSuite suites[] = {
        {
            .prefix = "/containers",
            .tests = containers,
        },
        {
            .prefix = "/dense-pool",
            .tests = dense_pool,
        },
        {
            .prefix = "/bitset",
            .tests = bitsets,
        },
        {
            .prefix = "/queue",
            .tests = queues,
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
