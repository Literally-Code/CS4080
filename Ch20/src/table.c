#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"

#define TABLE_MAX_LOAD 0.75

static uint32_t hashValue(Value value) {
  switch (value.type) {
    case VAL_NIL:
      return 2;
    case VAL_BOOL:
      return AS_BOOL(value) ? 3 : 1;
    case VAL_NUMBER: {
      double number = AS_NUMBER(value);
      if (number == 0) number = 0;

      uint64_t bits;
      memcpy(&bits, &number, sizeof bits);
      return (uint32_t)(bits ^ (bits >> 32));
    }
    case VAL_OBJ: {
      Obj* object = AS_OBJ(value);
      if (object->type == OBJ_STRING) {
        return ((ObjString*)object)->hash;
      }
      return (uint32_t)(uintptr_t)object;
    }
    default:
      return 0;
  }
}

void initTable(Table* table) {
  table->count = 0;
  table->capacity = 0;
  table->entries = NULL;
}
void freeTable(Table* table) {
  FREE_ARRAY(Entry, table->entries, table->capacity);
  initTable(table);
}
// NOTE: The "Optimization" chapter has a manual copy of this function.
// If you change it here, make sure to update that copy.
static Entry* findEntry(Entry* entries, int capacity, Value key) {
  uint32_t index = hashValue(key) % capacity;
  Entry* tombstone = NULL;

  for (;;) {
    Entry* entry = &entries[index];
    if (!entry->occupied) {
      if (!entry->tombstone) {
        return tombstone != NULL ? tombstone : entry;
      }
      if (tombstone == NULL) tombstone = entry;
    } else if (valuesEqual(entry->key, key)) {
      // We found the key.
      return entry;
    }

    index = (index + 1) % capacity;
  }
}
bool tableGet(Table* table, Value key, Value* value) {
  if (table->count == 0) return false;

  Entry* entry = findEntry(table->entries, table->capacity, key);
  if (!entry->occupied) return false;

  *value = entry->value;
  return true;
}
static void adjustCapacity(Table* table, int capacity) {
  Entry* entries = ALLOCATE(Entry, capacity);
  for (int i = 0; i < capacity; i++) {
    entries[i].occupied = false;
    entries[i].tombstone = false;
    entries[i].value = NIL_VAL;
  }

  table->count = 0;
  for (int i = 0; i < table->capacity; i++) {
    Entry* entry = &table->entries[i];
    if (!entry->occupied) continue;

    Entry* dest = findEntry(entries, capacity, entry->key);
    dest->key = entry->key;
    dest->value = entry->value;
    dest->occupied = true;
    dest->tombstone = false;
    table->count++;
  }

  FREE_ARRAY(Entry, table->entries, table->capacity);
  table->entries = entries;
  table->capacity = capacity;
}
bool tableSet(Table* table, Value key, Value value) {
  if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
    int capacity = GROW_CAPACITY(table->capacity);
    adjustCapacity(table, capacity);
  }

  Entry* entry = findEntry(table->entries, table->capacity, key);
  bool isNewKey = !entry->occupied;
  if (isNewKey) table->count++;

  entry->key = key;
  entry->value = value;
  entry->occupied = true;
  entry->tombstone = false;
  return isNewKey;
}
bool tableDelete(Table* table, Value key) {
  if (table->count == 0) return false;

  // Find the entry.
  Entry* entry = findEntry(table->entries, table->capacity, key);
  if (!entry->occupied) return false;

  // Place a tombstone in the entry.
  entry->occupied = false;
  entry->tombstone = true;
  entry->value = BOOL_VAL(true);
  table->count--;
  return true;
}
void tableAddAll(Table* from, Table* to) {
  for (int i = 0; i < from->capacity; i++) {
    Entry* entry = &from->entries[i];
    if (entry->occupied) {
      tableSet(to, entry->key, entry->value);
    }
  }
}
ObjString* tableFindString(Table* table, const char* chars,
                           int length, uint32_t hash) {
  if (table->count == 0) return NULL;

  uint32_t index = hash % table->capacity;
  for (;;) {
    Entry* entry = &table->entries[index];
    if (!entry->occupied) {
      // Stop if we find an empty non-tombstone entry.
      if (!entry->tombstone) return NULL;
    } else if (IS_STRING(entry->key) &&
        AS_STRING(entry->key)->length == length &&
        AS_STRING(entry->key)->hash == hash &&
        memcmp(AS_STRING(entry->key)->chars, chars, length) == 0) {
      // We found it.
      return AS_STRING(entry->key);
    }

    index = (index + 1) % table->capacity;
  }
}
