#include <stdlib.h>

#include "memory.h"
#include "vm.h"

void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
  if (newSize == 0) {
    free(pointer);
    return NULL;
  }

  void* result = realloc(pointer, newSize);
  if (result == NULL) exit(1);
  return result;
}
static void freeObject(Obj* object) {
  if (object->type == OBJ_STRING) {
    ObjString* string = (ObjString*)object;
    size_t size = sizeof(ObjString);
    if (string->ownsChars) size += (size_t)string->length + 1;
    reallocate(object, size, 0);
  }
}
void freeObjects() {
  Obj* object = vm.objects;
  while (object != NULL) {
    Obj* next = object->next;
    freeObject(object);
    object = next;
  }
}
