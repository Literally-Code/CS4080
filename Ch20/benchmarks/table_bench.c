#include <stdio.h>
#include <time.h>

#include "table.h"

#define BENCHMARK_SIZE 100000
#define LOOKUPS 1000000

static uint32_t nextRandom(uint32_t* state) {
  *state = *state * 1664525u + 1013904223u;
  return *state;
}

static double elapsed(clock_t start) {
  return (double)(clock() - start) / CLOCKS_PER_SEC;
}

static void benchmarkSequential(void) {
  Table table;
  initTable(&table);
  clock_t start = clock();

  for (int i = 0; i < BENCHMARK_SIZE; i++) {
    tableSet(&table, NUMBER_VAL(i), NUMBER_VAL(i * 2));
  }
  for (int i = 0; i < LOOKUPS; i++) {
    Value value;
    tableGet(&table, NUMBER_VAL(i % BENCHMARK_SIZE), &value);
  }

  printf("sequential insert/get: %.3f seconds\n", elapsed(start));
  freeTable(&table);
}

static void benchmarkMixedKeys(void) {
  Table table;
  initTable(&table);
  clock_t start = clock();

  for (int i = 0; i < BENCHMARK_SIZE; i++) {
    tableSet(&table, NUMBER_VAL(i), NIL_VAL);
    tableSet(&table, BOOL_VAL((i & 1) != 0), NUMBER_VAL(i));
  }
  tableSet(&table, NIL_VAL, NUMBER_VAL(42));

  for (int i = 0; i < LOOKUPS; i++) {
    Value value;
    tableGet(&table, (i & 1) ? BOOL_VAL(true) : NIL_VAL, &value);
    tableGet(&table, NUMBER_VAL(i % BENCHMARK_SIZE), &value);
  }

  printf("mixed primitive keys: %.3f seconds\n", elapsed(start));
  freeTable(&table);
}

static void benchmarkRandom(void) {
  Table table;
  initTable(&table);
  clock_t start = clock();
  uint32_t random = 7;

  for (int i = 0; i < BENCHMARK_SIZE; i++) {
    uint32_t key = nextRandom(&random);
    tableSet(&table, NUMBER_VAL(key), NUMBER_VAL(i));
  }
  random = 7;
  for (int i = 0; i < LOOKUPS; i++) {
    Value value;
    tableGet(&table, NUMBER_VAL(nextRandom(&random)), &value);
  }

  printf("random insert/get: %.3f seconds\n", elapsed(start));
  freeTable(&table);
}

static void benchmarkDeletion(void) {
  Table table;
  initTable(&table);
  clock_t start = clock();
  uint32_t random = 1;

  for (int i = 0; i < BENCHMARK_SIZE; i++) {
    tableSet(&table, NUMBER_VAL(i), BOOL_VAL(true));
  }
  for (int i = 0; i < BENCHMARK_SIZE / 2; i++) {
    tableDelete(&table, NUMBER_VAL(nextRandom(&random) % BENCHMARK_SIZE));
  }
  for (int i = 0; i < LOOKUPS; i++) {
    Value value;
    tableGet(&table, NUMBER_VAL(nextRandom(&random) % BENCHMARK_SIZE),
             &value);
  }

  printf("deletion/tombstone lookups: %.3f seconds\n", elapsed(start));
  freeTable(&table);
}

int main(void) {
  benchmarkSequential();
  benchmarkMixedKeys();
  benchmarkRandom();
  benchmarkDeletion();
  return 0;
}
