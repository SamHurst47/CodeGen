#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "compiler.h"

typedef enum {
    program,
    class,
    constructor,
    method,
    function
} Scope;

typedef enum {
    _static,
    field,
    argument,
    var,
    subroutine,
    non
} DataKinds;

typedef struct {
    char name[128];
    char type[128]; // Changed to char array for type
    DataKinds kind;
    int address;
} Symbol;

typedef struct node_s {
    Symbol cur_symbol;
    struct node_s* next;
} node_symbol;

typedef struct node_st {
    char name[128];
    Scope location;
    node_symbol* head_of_tree;
    struct node_st* next;
} node_symboltree;

typedef struct token_node {
    Token token;
    struct token_node* next;
} TokenNode;

void start_symbol_table_tree();
void stop_symbol_table_tree();
node_symboltree* get_symbol_table_tree_head();
Symbol populate_symbol(const char* name, const char* type, DataKinds kind, int address);
void add_symbol_node(Symbol symbol);
void add_symboltree_node(const char* name, Scope location);
void add_symbol_node_toptree(Symbol symbol);
void delete_last_symboltree_node();
void clear_symbol_list(node_symbol** head_ref);
void clear_symboltree_list(node_symboltree** head_ref);
int find_symbol(const char* name,const char* prg);
int find_symbol_local(const char* name);
int find_symbol_all(const char* name);
int find_symbol_top(const char* name);
void add_token_to_sub_check(Token t,int cat);
Token do_sub_check(int cat);
void reset_sub(int cat);
ParserInfo check_all_subs();
Symbol* get_top_symbol();
#endif /* SYMBOLS_H */

