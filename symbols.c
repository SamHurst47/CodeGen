
/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
The Symbol Tables Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Sam Hurst
Student ID: 201606431
Email: sc222sh@leeds.ac.uk
Date Work Commenced: 30/03/24
*************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"
#include "symbols.h"

// Define a global variable to store the head of the symbol tree
node_symboltree* symbol_tree_head = NULL;
TokenNode* token_list_head = NULL;
TokenNode* token_list_end_head = NULL;

// Function to set the head of the symbol table tree
void start_symbol_table_tree() {
    // Set the head of the symbol table tree
    symbol_tree_head = NULL;
}

// Function to clear the symbol table tree and reset the head
void stop_symbol_table_tree() {
    // Clear the symbol table tree
    clear_symboltree_list(&symbol_tree_head);
    symbol_tree_head = NULL;
}

// Function to get the head of the symbol table tree
node_symboltree* get_symbol_table_tree_head() {
    // Return the head of the symbol table tree
    return symbol_tree_head;
}

// Function to populate a symbol
Symbol populate_symbol(const char* name, const char* type, DataKinds kind, int address) {
    Symbol sym;
    strcpy(sym.name, name);
    strcpy(sym.type, type);
    sym.kind = kind;
    sym.address = address;
    return sym;
}

// Function to add a symbol node to the end of the symbol list within a specific node of the symbol tree list
void add_symbol_node(Symbol symbol) {
    // Find the node in the symbol tree list
    node_symboltree* current_node = symbol_tree_head;
    while (current_node->next != NULL) {
        current_node = current_node->next;
    }

    // Traverse the symbol linked list of the node to reach the last node
    node_symbol* current_symbol = current_node->head_of_tree;
    if (current_symbol == NULL) { // If the list is empty, make the new node the head
        current_node->head_of_tree = (node_symbol*)malloc(sizeof(node_symbol));
        if (current_node->head_of_tree == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        current_node->head_of_tree->cur_symbol = symbol;
        current_node->head_of_tree->next = NULL;
        return;
    }

    while (current_symbol->next != NULL) {
        current_symbol = current_symbol->next; // Traverse to the last node
    }

    // Create a new node and add it to the end of the list
    current_symbol->next = (node_symbol*)malloc(sizeof(node_symbol));
    if (current_symbol->next == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    current_symbol->next->cur_symbol = symbol;
    current_symbol->next->next = NULL;
}

// Function to add a node to the symbol tree
void add_symboltree_node(const char* name, Scope location) {
    node_symboltree* new_node = (node_symboltree*)malloc(sizeof(node_symboltree));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    } 
    strcpy(new_node->name, name);
    new_node->location = location;
    new_node->head_of_tree = NULL; // Initialize head_of_tree to NULL
    new_node->next = NULL;

    if (symbol_tree_head != NULL) { // If the linked list is not empty
        node_symboltree* current = symbol_tree_head;
        while (current->next != NULL) { // Traverse to the last node
            current = current->next;
        }
        current->next = new_node;
    } else { // If the linked list is empty
        symbol_tree_head = new_node;
    }
}

// Function to add a symbol node to the bottom of the symbol list referenced in the first item of the symbol tree
void add_symbol_node_toptree(Symbol symbol) {
    // Check if the symbol tree is empty
    if (symbol_tree_head == NULL) {
        fprintf(stderr, "Symbol tree is empty\n");
        return;
    }

    // Get the head of the symbol linked list from the first item in the symbol tree
    node_symbol* symbol_list_head = symbol_tree_head->head_of_tree;

    // Create a new symbol node
    node_symbol* new_symbol_node = (node_symbol*)malloc(sizeof(node_symbol));
    if (new_symbol_node == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the new symbol node with the provided symbol
    new_symbol_node->cur_symbol = symbol;
    new_symbol_node->next = NULL; // The new node will be the last node, so its next should be NULL

    // If the symbol list is empty, make the new node the head of the list
    if (symbol_list_head == NULL) {
        symbol_tree_head->head_of_tree = new_symbol_node;
        return;
    }

    // Traverse the symbol linked list to reach the last node
    node_symbol* current_symbol = symbol_list_head;
    while (current_symbol->next != NULL) {
        current_symbol = current_symbol->next;
    }

    // Add the new symbol node to the bottom of the list
    current_symbol->next = new_symbol_node;
}


void delete_last_symboltree_node() {
    if (symbol_tree_head == NULL) {
        return; // Empty list
    }

    if (symbol_tree_head->next == NULL) {
        free(symbol_tree_head);
        symbol_tree_head = NULL;
        return;
    }

    node_symboltree* current = symbol_tree_head;
    node_symboltree* previous = NULL;

    while (current->next != NULL) {
        previous = current;
        current = current->next;
    }

    free(current);
    if (previous != NULL) {
        previous->next = NULL;
    } else {
        symbol_tree_head = NULL; // If only one node in the list
    }
}

// Function to clear the symbol list
void clear_symbol_list(node_symbol** head_ref) {
    node_symbol* current = *head_ref;
    node_symbol* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    *head_ref = NULL; // Optional: Set head_ref to NULL after clearing the list
}

// Function to clear the symbol tree list
void clear_symboltree_list(node_symboltree** head_ref) {
    node_symboltree* current = *head_ref;
    node_symboltree* next;

    while (current != NULL) {
        next = current->next;
        clear_symbol_list(&(current->head_of_tree)); // Clear the associated symbol list
        free(current);
        current = next;
    }

    *head_ref = NULL; // Optional: Set head_ref to NULL after clearing the list
}

// Function to find a symbol in the symbol tree
int find_symbol(const char* name, const char *prg) {
    node_symboltree* head = symbol_tree_head;
    while (head != NULL) {
        // Check if the current node in the symbol tree matches the program name
        if (strcmp(head->name, prg) == 0 || head->location == program) {
            node_symbol* current_symbol = head->head_of_tree;
            while (current_symbol != NULL) {
                // If a matching symbol name is found, return 1 (or true) to indicate success
                if (strcmp(current_symbol->cur_symbol.name, name) == 0) {
                    return 1; // Or you can return a unique identifier or index if needed
                }
                current_symbol = current_symbol->next;
            }
        }
        head = head->next;
    }
    return -1; // Symbol not found
}

// Function to find a symbol in the symbol local
int find_symbol_local(const char* name) {
    // Start with the head of the symbol tree
    node_symboltree* current = symbol_tree_head;
    // Check if the symbol tree is empty
    if (current == NULL) return -1; // Symbol tree is empty, so return -1

    // Iterate to find the last node in the symbol tree
    while (current->next != NULL) {
        current = current->next;
    }

    // Now, 'current' points to the last node in the symbol tree
    // Check if the last node's symbol table has the symbol
    node_symbol* current_symbol = current->head_of_tree;
    while (current_symbol != NULL) {
        if (strcmp(current_symbol->cur_symbol.name, name) == 0) {
            return 1; // Symbol found, return 1 (true)
        }
        current_symbol = current_symbol->next;
    }

    // If we reach here, the symbol was not found in the last symbol table
    return -1; // Symbol not found
}

int find_symbol_all(const char* name) {
    node_symboltree* current_tree_node = symbol_tree_head;

    // Iterate through each node in the symbol tree
    while (current_tree_node != NULL) {
        node_symbol* current_symbol = current_tree_node->head_of_tree;

        // Search through the symbol table of the current node
        while (current_symbol != NULL) {
            if (strcmp(current_symbol->cur_symbol.name, name) == 0) {
                return 1; // Symbol found, return 1 (true)
            }
            current_symbol = current_symbol->next;
        }

        // Move to the next node in the symbol tree
        current_tree_node = current_tree_node->next;
    }

    // Symbol not found in any symbol table
    return -1; // Symbol not found
}

int find_symbol_top(const char* name) {
    node_symbol* current_symbol = symbol_tree_head->head_of_tree;
    int index = 0;

    while (current_symbol != NULL) {
        if (strcmp(current_symbol->cur_symbol.name, name) == 0) {
            return index;
        }
        current_symbol = current_symbol->next;
        index++;
    }

    return -1; // Symbol not found
}


// Function to add a token to the end of the dynamic list
void add_token_to_sub_check(Token t,int cat) {
    // Allocate memory for the new node
    TokenNode* new_node = (TokenNode*)malloc(sizeof(TokenNode));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    
    // Set the token in the new node
    new_node->token = t;
    new_node->next = NULL; // Since it's the last node, its next should be NULL
    
    TokenNode* current;
    if (cat == 0) {
        if (token_list_head == NULL) {
            token_list_head = new_node;
            return;
        }

        // Traverse the list to find the last node
        current = token_list_head;
    } else {
        if (token_list_end_head == NULL) {
            token_list_end_head = new_node;
            return;
        }
        
        // Traverse the list to find the last node
        current = token_list_end_head;
    }
    // If the list is empty, set the new node as the head
    
    while (current->next != NULL) {
        current = current->next;
    }
    
    // Attach the new node to the last node
    current->next = new_node;
}

// Function to perform symbol check on tokens in the list
Token do_sub_check(int cat) {
    TokenNode* current;
    if (cat == 0) {
        current = token_list_head;
    } else {
        current = token_list_end_head;
    }
    TokenNode* next_node;
    Token error_token;
    int ss = 0;

    while (current != NULL) {
        const char* lexeme = current->token.lx; // Get the lexeme from the current token
        next_node = current->next; // Store the next node
        if (find_symbol_all(lexeme) == -1) {
            // Symbol not found, store the error token and break the loop
            error_token = current->token;
            ss = 1;
            break;
        }
        current = next_node; // Move to the next node
    }

    // Free the memory for all nodes except the one with the error token
    TokenNode* temp;
    current = token_list_head;
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }
    
    // Reset the head to NULL before returning
    token_list_head = NULL;
    
    // If an error token was found, return it; otherwise, return an empty token with the EOFile type
    if (ss == 1) {
        return error_token;
    } else {
        Token empty_token;
        empty_token.tp = EOFile;
        return empty_token;
    }
}

// Function to reset the token list and head
void reset_sub(int cat) {
    TokenNode* current;
    if (cat == 0) {
        current = token_list_head;
    } else {
        current = token_list_end_head;
    }
    TokenNode* temp;

    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }

    if (cat == 0) {
        token_list_head = NULL;
    } else {
        token_list_end_head = NULL;
    }
}

ParserInfo returnInfoS;

// Function to check all subroutines
ParserInfo check_all_subs() {
    // Execute the function call
    Token t  = do_sub_check(1);
    returnInfoS.er = none; 
    // Check if an error occurred
    if (t.tp == EOFile) {
        // Error occurred, return the error information
        return returnInfoS;
    } else {
        ParserInfo pi;
        pi.er = undecIdentifier;
        pi.tk = t;
        returnInfoS = pi;
        return returnInfoS;
    }
}

Symbol* get_top_symbol() {
    if (symbol_tree_head == NULL) {
        printf("Symbol tree is empty.\n");
        return NULL;
    }

    // Traverse the linked list of symbol trees to find the last symbol tree
    node_symboltree* current = symbol_tree_head;
    while (current->next != NULL) {
        current = current->next;
    }

    // Check if the symbol tree is empty
    if (current->head_of_tree == NULL) {
        printf("Symbol tree is empty.\n");
        return NULL;
    }

    // Return the symbol at the top of the last symbol tree
    return &(current->head_of_tree->cur_symbol);
}