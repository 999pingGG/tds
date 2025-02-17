#include "private/common.h"
#include "private/begin.inc"

#ifndef TDS_TYPE
#define TDS_TYPE TDS_DEFAULT_TYPE_W_KEY_VALUE(hashmap)
#endif

#define TDS_ENTRY_T TDS_JOIN2(TDS_TYPE, _entry)

#ifdef TDS_DECLARE
typedef struct TDS_ENTRY_T {
  uint64_t hash;
  TDS_SIZE_T dist; // Probe distance from ideal slot.
  TDS_KEY_T key;
  TDS_VALUE_T value;
  char occupied;
} TDS_ENTRY_T;

typedef struct TDS_TYPE {
  TDS_ENTRY_T* buckets;
  TDS_SIZE_T count;
  TDS_SIZE_T capacity; // Total capacity, a prime number.
} TDS_TYPE;

void TDS_FUNCTION(put)(TDS_TYPE* map, TDS_KEY_T key, TDS_VALUE_T value);
TDS_VALUE_T* TDS_FUNCTION(get)(const TDS_TYPE* map, TDS_KEY_T key);
TDS_SIZE_T TDS_FUNCTION(count)(const TDS_TYPE* map);
void TDS_FUNCTION(clear)(TDS_TYPE* map);
void TDS_FUNCTION(fini)(TDS_TYPE* map);
#endif

#ifdef TDS_IMPLEMENT
#define TDS_HASHMAP_LOAD_FACTOR 0.85

void TDS_FUNCTION(put)(TDS_TYPE* map, const TDS_KEY_T key, const TDS_VALUE_T value) {
  if (!map->buckets) {
    // Yeah, we ignore TDS_INITIAL_CAPACITY here because we need the capacity to be a prime number.
    map->capacity = 11;
    map->buckets = TDS_CALLOC(map->capacity, sizeof(TDS_ENTRY_T));
  }

  // Ensure the map has room for at least one more entry.
  if ((double)(map->count + 1) > (double)map->capacity * TDS_HASHMAP_LOAD_FACTOR) {
    // Find the smaller prime number that is at least as big as the required capacity.
    static const unsigned long long prime_list[64] = {
      2, 3, 5, 11, 17, 37, 67, 131, 257, 521, 1031, 2053, 4099, 8209, 16411, 32771, 65537, 131101, 262147, 524309,
      1048583, 2097169, 4194319, 8388617, 16777259, 33554467, 67108879, 134217757, 268435459, 536870923, 1073741827,
      2147483659, 4294967311, 8589934609, 17179869209, 34359738421, 68719476767, 137438953481, 274877906951,
      549755813911, 1099511627791, 2199023255579, 4398046511119, 8796093022237, 17592186044423, 35184372088891,
      70368744177679, 140737488355333, 281474976710677, 562949953421381, 1125899906842679, 2251799813685269,
      4503599627370517, 9007199254740997, 18014398509482143, 36028797018963971, 72057594037928017, 144115188075855881,
      288230376151711813, 576460752303423619, 1152921504606847009, 2305843009213693967, 4611686018427388039,
      9223372036854775837ull,
    };

    TDS_SIZE_T new_capacity = map->capacity * 2;
    char found = 0;
    for (int i = 0; i < 64; i++) {
      if (prime_list[i] < new_capacity) {
        continue;
      }

      new_capacity = (TDS_SIZE_T)prime_list[i];
      found = 1;
      break;
    }

    if (!found) {
      new_capacity = TDS_MAX_VALUE(TDS_SIZE_T);
    }

    // Rehashing.
    TDS_ENTRY_T* new_buckets = TDS_CALLOC(new_capacity, sizeof(TDS_ENTRY_T));
    for (TDS_SIZE_T i = 0; i < map->capacity; i++) {
      TDS_ENTRY_T entry = map->buckets[i];
      if (entry.occupied) {
        entry.dist = 0;

        // Insert entry.
        TDS_SIZE_T index = entry.hash % new_capacity;
        while (1) {
          TDS_ENTRY_T* cur = new_buckets + index;
          if (!cur->occupied) {
            new_buckets[index] = entry;
            break;
          }
          // Robin Hood: Swap if our probe distance is higher.
          if (cur->dist < entry.dist) {
            const TDS_ENTRY_T temp = *cur;
            *cur = entry;
            entry = temp;
          }
          index = (index + 1) % new_capacity;
          entry.dist++;
          TDS_ASSERT(entry.dist < new_capacity);
        }
      }
    }
    TDS_FREE(map->buckets);
    map->buckets = new_buckets;
    map->capacity = new_capacity;
  }

  // Hash the key using rapidhash.
  const uint64_t hash = rapidhash(&key, sizeof(key));
  TDS_ENTRY_T new_entry = {
    .hash = hash,
    .dist = 0,
    .key = key,
    .value = value,
    .occupied = 1,
  };

  TDS_SIZE_T index = hash % map->capacity;
  TDS_SIZE_T dist = 0;
  while (1) {
    TDS_ENTRY_T* cur = &map->buckets[index];
    if (!cur->occupied) {
      new_entry.dist = dist;
      map->buckets[index] = new_entry;
      map->count++;
      return;
    }

    if (cur->hash == hash && cur->key == key) {
      // Key matches; update the value.
      cur->value = value;
      return;
    }

    if (cur->dist < dist) {
      // Robin Hood swaps our entry with the one already here.
      const TDS_ENTRY_T temp = *cur;
      *cur = new_entry;
      new_entry = temp;
      dist = cur->dist;
    }
    index = (index + 1) % map->capacity;
    dist++;
    new_entry.dist = dist;
    TDS_ASSERT(dist < map->capacity);
  }
}

TDS_VALUE_T* TDS_FUNCTION(get)(const TDS_TYPE* map, const TDS_KEY_T key) {
  if (!map->buckets) {
    return NULL;
  }

  const uint64_t hash = rapidhash(&key, sizeof(key));
  TDS_SIZE_T index = hash % map->capacity;
  TDS_SIZE_T dist = 0;
  while (1) {
    TDS_ENTRY_T* cur = &map->buckets[index];
    if (!cur->occupied) {
      // Key not found.
      return NULL;
    }

    if (cur->hash == hash && cur->key == key) {
      // Key found.
      return &cur->value;
    }

    TDS_ASSERT(cur->dist >= dist);

    index = (index + 1) % map->capacity;
    dist++;
  }
}

TDS_SIZE_T TDS_FUNCTION(count)(const TDS_TYPE* map) {
  return map->count;
}

void TDS_FUNCTION(clear)(TDS_TYPE* map) {
  if (map->buckets) {
    TDS_MEMSET(map->buckets, 0, sizeof(TDS_ENTRY_T) * map->capacity);
  }
  map->count = 0;
}

void TDS_FUNCTION(fini)(TDS_TYPE* map) {
  TDS_FREE(map->buckets);
  *map = (TDS_TYPE){ 0 };
}
#endif

#undef TDS_TYPE
#undef TDS_ENTRY_T
#include "private/end.inc"
