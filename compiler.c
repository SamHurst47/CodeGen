/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
The Compiler Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Sam Hurst
Student ID: 201606431
Email: sc222sh@leeds.ac.uk
Date Work Commenced: 30/03/24
*************************************************************************/
#include "compiler.h"
#include "lexer.h"
#include "parser.h"
#include "symbols.h"

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>


int InitCompiler ()
{
	start_symbol_table_tree();
    return 1;
}

// Global variable for the head of the linked list
vm_node *top = NULL;

// Function to add a new node to the linked list
void add_to_list(const char *vmcommands, const char *mid, int num) {
    // Allocate memory for the new node
    vm_node *new_node = (vm_node *)malloc(sizeof(vm_node));
    if (new_node == NULL) {
        perror("Memory allocation failed");
        return;
    }

    // Assign the data to the new node
    strcpy(new_node->cur_vm.vmcommands, vmcommands);
    strcpy(new_node->cur_vm.mid, mid);
    new_node->cur_vm.num = num;
    new_node->next = NULL;

    // If the list is empty, set the new node as the head
    if (top == NULL) {
        top = new_node;
        return;
    }

    // Traverse the list to find the last node
    vm_node *temp = top;
    while (temp->next != NULL) {
        temp = temp->next;
    }

    // Attach the new node to the last node
    temp->next = new_node;
}


// is_regular_file function definition
int is_regular_file(const char *path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        // Failed to get file status
        return 0;
    }
    return S_ISREG(path_stat.st_mode); // Returns non-zero if it's a regular file
}


void delete_vm_files(const char *directory) {
    DIR *dir;
    struct dirent *entry;

    // Open directory
    dir = opendir(directory);
    if (dir == NULL) {
        perror("Unable to open directory");
        return;
    }

    // Iterate over each file in the directory
    while ((entry = readdir(dir)) != NULL) {
        // Check if the file has a .vm extension
        size_t len = strlen(entry->d_name);
        if (len >= 3 && strcmp(entry->d_name + len - 3, ".vm") == 0) {
            // Construct full file path
            char *file_path = malloc(strlen(directory) + 1 + strlen(entry->d_name) + 1);
            if (file_path == NULL) {
                perror("Memory allocation failed");
                closedir(dir);
                return;
            }
            snprintf(file_path, strlen(directory) + 1 + strlen(entry->d_name) + 1, "%s/%s", directory, entry->d_name);

            // Delete the file
            if (remove(file_path) != 0) {
                perror("Failed to delete file");
            } else {
                printf("Deleted file: %s\n", file_path);
            }

            free(file_path);
        }
    }

    // Close directory
    closedir(dir);
}
void create_vm_file(const char *directory, const char *filename) {
    // Calculate the length of the full file path
    size_t dir_len = strlen(directory);
    size_t filename_len = strlen(filename);
    size_t extension_len = strlen(".vm");
    size_t full_path_len = dir_len + 1 + filename_len - strlen(".jack") + extension_len + 1; // +1 for directory separator, +1 for null terminator

    // Allocate memory for the full file path
    char *full_path = malloc(full_path_len);
    if (full_path == NULL) {
        perror("Memory allocation failed");
        return;
    }

    // Remove ".jack" extension from filename
    char *vm_filename = malloc(filename_len - strlen(".jack") + 1); // +1 for null terminator
    strncpy(vm_filename, filename, filename_len - strlen(".jack"));
    vm_filename[filename_len - strlen(".jack")] = '\0'; // Null terminate the string

    // Construct the full file path
    snprintf(full_path, full_path_len, "%s/%s.vm", directory, vm_filename);

    // Create and open the file
    FILE *file = fopen(full_path, "w");
    if (file == NULL) {
        perror("Failed to create .vm file");
        free(full_path);
        free(vm_filename);
        return;
    }

    // Write VM commands to the file
    vm_node *current = top;
    while (current != NULL) {
        fprintf(file, "%s\n", current->cur_vm.vmcommands);
        current = current->next;
    }

    // Close the file
    fclose(file);
    free(full_path);
    free(vm_filename);

    // Free and clear the VM linked list
    current = top;
    while (current != NULL) {
        vm_node *temp = current;
        current = current->next;
        free(temp);
    }
    top = NULL; // Reset the head pointer
}


ParserInfo compile(char* dir_name) {
    ParserInfo p;

    // Add the directory to the symbol tree as a program scope
    add_symboltree_node(dir_name, program);
    
    // List of standard library files
    char* stdLibFiles[] = {
        "Math.jack",
        "Array.jack",
        "Memory.jack",
        "String.jack",
        "Screen.jack",
        "Keyboard.jack",
        "Sys.jack",
        "Output.jack"
    };
    int stdLibFilesCount = sizeof(stdLibFiles) / sizeof(stdLibFiles[0]);

    // Precompile all the standard library files
    for (int i = 0; i < stdLibFilesCount; i++) {
        // Initialize lexer for the file
        if (InitLexer(stdLibFiles[i]) != 1) {
            p.er = lexerErr;
            return p;
        }

        // Add symbol node for the standard library file (without .jack extension)
        char symbol_name[256];
        strncpy(symbol_name, stdLibFiles[i], strlen(stdLibFiles[i]) - strlen(".jack"));
        symbol_name[strlen(stdLibFiles[i]) - strlen(".jack")] = '\0'; // Null-terminate the string
        Symbol s = populate_symbol(symbol_name, "class", non, 0);
        add_symbol_node_toptree(s);

        // Parse the file
        p = Parse();

        // Stop lexer for the file
        StopLexer();

        // Check if parsing was successful
        if (p.er != none) {
            // Parsing failed, return with the error
            return p;
        }
    }

    // Add symbols for all files in the directory (without .jack extension)
    DIR* dir;
    struct dirent* entry;

    dir = opendir(dir_name);
    if (dir == NULL) {
        // Directory not found
        p.er = syntaxError;
        return p;
    }
    delete_vm_files(dir_name);
    // Iterate over each file in the directory to add symbols

    while ((entry = readdir(dir)) != NULL) {
        // Check if the entry name ends with ".jack" to identify regular files
        size_t entry_len = strlen(entry->d_name);
        const char *extension = ".jack";
        size_t extension_len = strlen(extension);

        if (entry_len > extension_len && 
            strcmp(entry->d_name + entry_len - extension_len, extension) == 0) {
            // Construct full file path
            char *file_path = malloc(strlen(dir_name) + strlen("/") + strlen(entry->d_name) + 1);
            if (file_path == NULL) {
                return p;
            }
            snprintf(file_path, strlen(dir_name) + strlen("/") + strlen(entry->d_name) + 1, "%s/%s", dir_name, entry->d_name);

            // Add symbol node for the file (without .jack extension)
            char symbol_name[256];
            strncpy(symbol_name, entry->d_name, entry_len - extension_len);
            symbol_name[entry_len - extension_len] = '\0'; // Null-terminate the string
            Symbol s = populate_symbol(symbol_name, "class", non, 0);
            add_symbol_node_toptree(s);
            free(file_path);
        }
    }

    // Close the directory after adding symbols
    closedir(dir);

    // Reopen the directory for parsing
    dir = opendir(dir_name);
    if (dir == NULL) {
        // Directory not found
        p.er = syntaxError;
        return p;
    }
    // Iterate over each file in the directory for parsing
    while ((entry = readdir(dir)) != NULL) {
        char *file_path = malloc(strlen(dir_name) + strlen("/") + strlen(entry->d_name) + 1);
        if (file_path == NULL) {
            return p;
        }
        snprintf(file_path, strlen(dir_name) + strlen("/") + strlen(entry->d_name) + 1, "%s/%s", dir_name, entry->d_name);

        // Check if the entry is a regular file and not "Main.jack"
        if (is_regular_file(file_path) && strcmp(entry->d_name, "Main.jack") != 0) {
            // Initialize lexer for the file
            if (InitLexer(file_path) != 1) {
                p.er = lexerErr;
                closedir(dir);
                reset_sub(1);
                return p;
            }
            free(file_path);
            // Parse the file
            p = Parse();

            // Stop lexer for the file
            StopLexer();

            // Check if parsing was successful
            if (p.er != none) {
                // Parsing failed, return with the error
                closedir(dir);
                reset_sub(1);
                return p;
            }
            create_vm_file(dir_name,entry->d_name);
        }
    }
	closedir(dir);
		
	// Reopen the directory
	dir = opendir(dir_name);
	if (dir == NULL) {
		// Directory not found
		p.er = syntaxError;
		reset_sub(1);
		return p;
	}

	// Find and parse Main.jack if it exists
	while ((entry = readdir(dir)) != NULL) {
        char *file_path = malloc(strlen(dir_name) + strlen("/") + strlen(entry->d_name) + 1);
        if (file_path == NULL) {
            // Handle memory allocation failure
            // For example, you can return an error or exit the function
            return p;
        }
        snprintf(file_path, strlen(dir_name) + strlen("/") + strlen(entry->d_name) + 1, "%s/%s", dir_name, entry->d_name);


        // Check if the entry is a regular file and its name is "Main.jack"
        if (is_regular_file(file_path) && strcmp(entry->d_name, "Main.jack") == 0) {
            // Initialize lexer for Main.jack
            if (InitLexer(file_path) != 1) {
                p.er = lexerErr;
                closedir(dir);
                reset_sub(1);
                return p;
            }
            free(file_path);
            // Parse Main.jack
            p = Parse();

            // Stop lexer for Main.jack
            StopLexer();

            // Check if parsing was successful
            if (p.er != none) {
                // Parsing failed, return with the error
                closedir(dir);
                return p;
            }
            create_vm_file(dir_name,entry->d_name);
            // Break the loop after parsing Main.jack
            break;
        }
    }
	p = check_all_subs();
	if (p.er != none) {
		closedir(dir);
		reset_sub(1);
		return p;
	}
	reset_sub(1);
	closedir(dir);
	return p;
}

int StopCompiler ()
{
    stop_symbol_table_tree();
    return 1;
}

#ifndef TEST_COMPILER
int main ()
{
	InitCompiler ();
	ParserInfo p = compile("Pong");
	PrintError (p);
	StopCompiler ();
	return 1;
}
#endif

