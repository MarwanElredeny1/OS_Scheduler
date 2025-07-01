#ifndef MEMORY_H
#define MEMORY_H

#define CODE_OFFSET      5
#define VARIABLES_OFFSET 15

#include "pcb.h"

typedef enum {
  TYPE_NULL = 0,
  TYPE_INT,
  TYPE_STRING
} ValueType;

typedef struct {
  char* name;
  char* valueStr;
  int valueInt;
  ValueType type;
} Word;

int load_int_word_from_memory(const PCB *pcb, const char *wordName);
char *load_str_word_from_memory(const PCB *pcb, const char *wordName);

void update_int_word_in_memory(const PCB *pcb, const char *wordName, const int value);
void update_str_word_in_memory(const PCB *pcb, const char *wordName, const char *value);

void updatePCBInMemory(PCB* pcb);

#endif
