/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
Lexer Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Sam Hurst
Student ID: 201606431
Email: sc222sh@leeds.ac.uk
Date Work Commenced: 10 / 02 / 24

 *************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

#define MAXLINE 128

// Linked list node def
typedef struct node {
  Token cur_token;
  struct node * next;
}
node_token;

node_token * head = NULL;

// Checks if the selceted input is a symbole
int symcheck(char input) {
  switch (input) {
  case '+':
  case '-':
  case '*':
  case '/':
  case '&':
  case '|':
  case '~':
  case '<':
  case '>':
  case ',':
  case '.':
  case ';':
  case ':':
  case '{':
  case '}':
  case '[':
  case ']':
  case '=':
  case '(':
  case ')':
    return 1;
  default:
    return 0;
  }
}

// Checks if the selceted input is a resword
int reswordcheck(char * input) {
  // Sets up a list of resserved words
  const char * reservedWords[] = {
    "class", "constructor","method",
    "function","int","boolean",
    "char","void","var",
    "static","field","let",
    "do","if","else",
    "while","return","true",
    "false","null","this"
  };
  // Calculates number of reswords
  int numReservedWords = sizeof(reservedWords) / sizeof(reservedWords[0]);
  // Itteratse thorught all the reswords compareing them to input to see if there is a match
  for (int i = 0; i < numReservedWords; ++i) {
    if (strcmp(input, reservedWords[i]) == 0) {
      return 1;
    }
  }
  return 0;
}

// Checks if the selceted input is a int
int intcheck(char * input) {
  while ( * input) {
    if ( * input < '0' || * input > '9') {
      return 0;
    }
    input++;
  }
  return 1;
}

// Checks if the selceted input is a id
int idcheck(char * input) {
  // Check if the first character is a letter or underscore
  if (!(( * input >= 'A' && * input <= 'Z') || ( * input >= 'a' && * input <= 'z') || ( * input == '_'))) {
    return 0;
  }
  input++;

  // Check the remaining characters if they are letter pr number or underscore
  while ( * input) {
    if (!(( * input >= 'A' && * input <= 'Z') || ( * input >= 'a' && * input <= 'z') || ( * input == '_') || ( * input >= '0' && * input <= '9'))) {
      return 0;
    }
    input++;
  }
  return 1;
}

// Checks if the selceted input is a an illigal symbole
int illgesym(char input) {
  if (input == '?') {
    return 1;
  }
  return 0;
}

// Select Token type using predefined checks and mutiple selections statements
TokenType select_tokentype(char * input) {
  if (input[1] == '\0' && symcheck(input[0])) {
    return SYMBOL;
  } else if (reswordcheck(input)) {
    return RESWORD;
  } else if (input[0] == '"') {
    return STRING;
  } else if (intcheck(input)) {
    return INT;
  } else if (idcheck(input)) {
    return ID;
  } else if (input[0] == ' ') {
    return EOFile;
  }
  return ERR;
}

// Genrates a token for an error using verious inputs
Token createErToken(char * error_Mes, int line, char * file_name) {
  Token t;
  t.tp = ERR;
  for (int i = 0; i < MAXLINE; i++) {
    t.lx[i] = '\0'; // Initialize the lexeme field of the token
  }
  for (int i = 0; i < 32; i++) {
    t.fl[i] = '\0'; // Initialize the filename field of the token
  }
  for (int curpos = 0; file_name[curpos] != '\0'; curpos++) {
    t.fl[curpos] = file_name[curpos];  // Copy the filename into the token
  }
  t.ln = line; // Set the line number field of the token
  for (int curpos = 0; error_Mes[curpos] != '\0'; curpos++) {
    t.lx[curpos] = error_Mes[curpos]; // Copy the characters of the error code to the lexeme
  }
  return t;
}

// Creates the token using the all inputs from file
Token createToken(char * chartoken, int line, char * file_name) {
  Token t;
  t.tp = select_tokentype(chartoken); // Set token type based on the input token
  for (int i = 0; i < MAXLINE; i++) {
    t.lx[i] = '\0'; // Initialize the lexeme field of the token
  }
  for (int i = 0; i < 32; i++) {
    t.fl[i] = '\0'; // Initialize the filename field of the token
  }
  for (int curpos = 0; file_name[curpos] != '\0'; curpos++) {
    t.fl[curpos] = file_name[curpos]; // Copy the filename into the token
  }
  t.ln = line; // Set the line number field of the token
  if (t.tp == STRING) { // If the token type is STRING
    for (int curpos = 0; chartoken[curpos + 1] != '\0'; curpos++) {
      t.lx[curpos] = chartoken[curpos + 1]; // Copy the characters of the string (excluding the opening quote) into the lexeme field
    }
  } else if (t.tp == EOFile) { // If the token type is End of File
    char * eof = "End of File ";
    for (int curpos = 0; eof[curpos] != '\0'; curpos++) {
      t.lx[curpos] = eof[curpos]; // Copy "End of File" into the lexeme field
    }
  } else { // For other token types
    for (int curpos = 0; chartoken[curpos] != '\0'; curpos++) {
      t.lx[curpos] = chartoken[curpos]; // Copy the characters of the token into the lexeme field
    }
  }
  return t; // Return the created token
}

// Adds token by traversing to end and setting to next item of the linked list
void add_token(Token new_token) {
  node_token * current = head; // Initialize a pointer to traverse the linked list

  if (head != NULL) { // If the linked list is not empty
    while (current -> next != NULL) { // Traverse to the last node
      current = current -> next;
    }
    current -> next = (node_token * ) malloc(sizeof(node_token)); // Create a new node at the end of the list
    current -> next -> cur_token = new_token; // Set the current token of the new node
    current -> next -> next = NULL; // Set the next pointer of the new node to NULL
  } else { // If the linked list is empty
    head = (node_token * ) malloc(sizeof(node_token)); // Create a new node as the head
    head -> next = NULL; // Set the next pointer of the head node to NULL
    head -> cur_token = new_token; // Set the current token of the head node
  }
}

// Clear memory of the string
void clearString(char * str) {
  memset(str, 0, strlen(str));
}

//***********************************

int InitLexer(char * file_name) {
  // Stop the lexer first if it's already running and to ensure clear linked list
  StopLexer();
  //
  FILE * fp = fopen(file_name, "r");
  if (fp == NULL) {
    return 0; // Error opening file
  }
  // Intialies all the varibles
  char strbuff[MAXLINE];
  int line_num = 0;
  int in_comment = 0;
  int in_string = 0;
  char string_token[MAXLINE];
  int string_pos = 0;
  while (fgets(strbuff, MAXLINE, fp)) {
    line_num++;
    // Iterate through each character in the current line
    for (int i = 0; strbuff[i] != '\0'; i++) {
      if (in_comment) {
        // Check for end of comment
        if (strbuff[i] == '*' && strbuff[i + 1] == '/') {
          in_comment = 0;
          i++; // Skip the '/' character
        }
      } else if (in_string) {
        // Check for end of string
        if (strbuff[i] == '"') { // If a string termanting char is found ends string and adds token to linked list
          string_token[string_pos++] = '\0';
          Token t = createToken(string_token, line_num, file_name);
          add_token(t);
          // Resest all string varibles
          in_string = 0;
          clearString(string_token);
          string_pos = 0;
        } else {
          if (strbuff[i] == '\0' || strbuff[i] == '\n') {
            Token t = createErToken("Error: new line in string constant", line_num, file_name);
            add_token(t);
          }
          string_token[string_pos] = strbuff[i];
          string_pos++;
        }
      } else if (illgesym(strbuff[i])) {
        Token t = createErToken("Error: illegal symbol in source file", line_num, file_name);
        add_token(t);
      } else {
        // Check for start of comment
        if (strbuff[i] == '/' && strbuff[i + 1] == '*') {
          in_comment = 1;
          i++; // Skip the '*' character
        } else if (strbuff[i] == '/' && strbuff[i + 1] == '/') {
          strbuff[i + 1] = '\0';
        } else if (strbuff[i] == '"') {
          in_string = 1;
          string_token[0] = '"';
          string_pos++;
        } else if (!isspace(strbuff[i])) { // Check if character is not a whitespace
          if (symcheck(strbuff[i])) { // Check if character is a symbol
            // Tokenize the symbol and add it to the linked list
            char tokenbuff[2];
            tokenbuff[0] = strbuff[i];
            tokenbuff[1] = '\0';
            Token t = createToken(tokenbuff, line_num, file_name);
            add_token(t);
          } else {
            char tokenbuff[MAXLINE];
            int token_pos = 0;
            // Start extracting the token
            while (!isspace(strbuff[i]) && strbuff[i] != '\0' && !symcheck(strbuff[i])) {
              tokenbuff[token_pos++] = strbuff[i++];
            }
            tokenbuff[token_pos] = '\0'; // Null-terminate the token
            if (symcheck(strbuff[i])) {
              i--; // Reveres to ensure no skip char
            }
            // Create a token and add it to the linked list
            Token t = createToken(tokenbuff, line_num, file_name);
            add_token(t);
          }
        }
      }
    }
    clearString(strbuff); // Clear the string to ensure no data is left over
  }
  // Close flie to ensure mermory integruity
  fclose(fp);
  if (in_comment) { // Adds error tokens if in comment at eof
    Token t = createErToken("Error: unexpected eof in comment", line_num + 1, file_name);
    add_token(t);
  } else if (in_string) { // Adds error tokens if in string at eof
    Token t = createErToken("Error: unexpected eof in string constant", line_num, file_name);
    add_token(t);
  }
  // Adds end of file token
  Token t = createToken("  ", line_num, file_name);
  add_token(t);

  return 1; // Successfully initialized lexer
}

// Get the next token from the source file
Token GetNextToken() {
  Token re; // Intilises token
  node_token * tmp = head; // Sets the head node to temp varble
  re = tmp -> cur_token; // Saves token that is extracted for return
  head = tmp -> next; // Sets the head to the second node of linked list
  free(tmp); // Frees temapry node
  return re;
}

// peek (look) at the next token in the source file without removing it from the stream
Token PeekNextToken() {
  node_token * tmp = head; // Sets the head node to temp varble
  Token t;
  t = tmp -> cur_token; // Extract the token form the head to return
  return t;
}

// clean out at end, e.g. close files, free memory, ... etc
int StopLexer() {
  if (head != NULL) { // Checks that the head isnt intialised
    node_token * tmp;
    while (head != NULL) { // Ittrates through freeing the head
      tmp = head;
      head = head -> next;
      free(tmp);
    }
  }
  head = NULL; // Resets the head to Null
  return 1;
}


