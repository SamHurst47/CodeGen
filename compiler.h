#ifndef COMPILER_H
#define COMPILER_H

#define TEST_COMPILER    // uncomment to run the compiler autograder

#include "parser.h"
#include "symbols.h"

typedef struct {
    char vmcommands[128];
    char mid[128]; // Changed to char array for type
    int num;
} vmline;

// Linked list node def
typedef struct vmnode {
  vmline cur_vm;
  struct vmnode * next;
} vm_node;

int InitCompiler ();
void add_to_list(const char *vmcommands, const char *mid, int num);
ParserInfo compile (char* dir_name);
int StopCompiler();

#endif
