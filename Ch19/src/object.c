#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

static Obj* allocateObject(size_t size, ObjType type) {
  Obj* object = (Obj*)reallocate(NULL, 0, size);
  object->type = type;

  object->next = vm.objects;
  vm.objects = object;
  return object;
}

ObjString* makeString(bool ownsChars, const char* chars, int length) {
  size_t size = sizeof(ObjString);
  if (ownsChars) size += (size_t)length + 1;

  ObjString* string = (ObjString*)allocateObject(size, OBJ_STRING);
  string->ownsChars = ownsChars;
  string->length = length;
  string->externalChars = ownsChars ? NULL : chars;

  if (ownsChars) {
    if (chars != NULL) memcpy(string->chars, chars, (size_t)length);
    string->chars[length] = '\0';
  }

  return string;
}
void printObject(Value value) {
  if (OBJ_TYPE(value) == OBJ_STRING) {
    printf("%.*s", AS_STRING(value)->length, AS_CSTRING(value));
  }
}
