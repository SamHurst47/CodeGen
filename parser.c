#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "symbols.h"
#include "lexer.h"
#include "parser.h"

// you can declare prototypes of parser functions below

void classDeclar();
void memberDeclar();
void classVarDeclar(char * kind);
void subroutineDeclar(char * scope);
void type();
void paramList();
void subroutineBody();
void statement();
void varDeclarStatement();
void letStatement();
void ifStatement();
void whileStatement();
void doStatement();
void subroutineCall();
void expressionList();
void returnStatement();
void expression();
void relationalExpression();
void arithmeticExpression();
void term();
void factor();
void operand();

int ter = 0;
char * prg;
ParserInfo returnInfo;

// Global variables and other declarations can go here
void error(SyntaxErrors msg, Token t) {
  if (!ter) {
    ParserInfo pi;
    pi.er = msg;
    pi.tk = t;
    returnInfo = pi;
    ter = 1;
  }
}

int checkResword(Token t, char* strInput) {
  return strcmp(strInput, t.lx);
}

int checkId(Token t) {
  if (t.tp == ID)
    return 0;
  return 1;
}

int checkSym(Token t, char charInput) {
  if (t.tp == SYMBOL && t.lx[0] == charInput)
    return 1;
  return 0;
}

void cbo() {
  Token t = GetNextToken();
  if (!(checkSym(t, '{')))
    error(openBraceExpected, t);
}

void cbc() {
  Token t = GetNextToken();
  if (!(checkSym(t, '}')))
    error(closeBraceExpected, t);
}

int repCallcbc() {
  Token t = PeekNextToken();
  if (t.tp != EOFile && (t.lx[0] != '}')) {
    return 1;
  }
  return 0;
}

int repCalle() {
  Token t = PeekNextToken();
  if (t.tp != EOFile && (t.lx[0] != ';')) {
    return 1;
  }
  return 0;
}

int repCallb() {
  Token t = PeekNextToken();
  if (t.tp != EOFile && (t.lx[0] != ')')) {
    return 1;
  }
  return 0;
}

// Start of functions

void classDeclar() {
 Token t = GetNextToken();
 if (t.tp == ERR)
   return error(lexerErr, t);
 if (checkResword(t, "class"))
   return error(classExpected, t);
 t = GetNextToken();
 if (checkId(t))
   return error(idExpected, t);
 prg = t.lx;
 add_symboltree_node(t.lx,class);
 cbo();
 if (ter)
   return;
 while (repCallcbc()) {
   memberDeclar();
   if (ter)
     return;
 }
 cbc();
}


void memberDeclar() {
  Token t = GetNextToken();
  if (!(checkResword(t, "field")) || !(checkResword(t, "static"))) {
    classVarDeclar(t.lx);
  } else if (!(checkResword(t, "constructor")) || !(checkResword(t, "method")) || !(checkResword(t, "function"))) {
    subroutineDeclar(t.lx);
  } else {
    error(memberDeclarErr, t);
  }
}

void classVarDeclar(char * kind) {
  Token t = PeekNextToken();
  char * sy_ty= t.lx;
  DataKinds sy_kind;
  if (!(strcmp(kind,"field"))) {
    sy_kind = field;
  } else {
    sy_kind = _static;
  }
  type();
  if (ter)
    return;
  t = GetNextToken();
  if (checkId(t))
    return error(idExpected, t);
  if (find_symbol(t.lx,prg) != -1)
    return error(redecIdentifier, t);
  Symbol s = populate_symbol(t.lx,sy_ty,sy_kind,0);
  add_symbol_node(s);
  while (repCalle()) {
    t = GetNextToken();
    if (!(checkSym(t, ',')))
      return error(classVarErr, t);
    t = GetNextToken();
    if (checkId(t))
      return error(idExpected, t);
    if (find_symbol(t.lx,prg) != -1)
      return error(redecIdentifier, t);
    Symbol s = populate_symbol(t.lx,sy_ty,sy_kind,0);
    add_symbol_node(s);
  }
  t = GetNextToken();
  if (!(checkSym(t, ';')))
    return error(semicolonExpected, t);
}

void subroutineDeclar(char * scope) {
  Token t = PeekNextToken();
  int add = 0;
  Scope sc;
  char * typ = t.lx;
  if (!(strcmp(scope,"constructor"))) {
    Symbol s = populate_symbol(t.lx,"new",subroutine,0);
    add_symbol_node(s);
    add_symboltree_node(t.lx,constructor);
    s = populate_symbol("this",t.lx,argument,0);
    add_symbol_node(s);
    add = 1;
  }
  if (checkResword(t, "void")) {
    t = GetNextToken();
  } else {
    type();
    if (ter)
      return;
  }
  t = GetNextToken();
  if (checkId(t))
    return error(idExpected, t);
  if (!(strcmp(scope,"method"))) {
    sc = method;
  } else if (!(strcmp(scope,"function"))) {
    sc = function;
  }
  if (add==0) {
    if (find_symbol(t.lx,prg) != -1)
      return error(redecIdentifier, t);
    Symbol s = populate_symbol(t.lx,typ,subroutine,0);
    add_symbol_node(s);
    add_symboltree_node(prg,sc);
    s = populate_symbol("this",t.lx,argument,0);
    add_symbol_node(s);
  }
  t = GetNextToken();
  if (!(checkSym(t, '(')))
    return error(openParenExpected, t);
  t = PeekNextToken();
  if (!(checkSym(t, ')')))
    paramList();
  if (ter)
    return;
  t = GetNextToken();
  if (!(checkSym(t, ')')))
    return error(closeParenExpected, t);
  subroutineBody();
  if (ter)
    return;
  delete_last_symboltree_node();
}

void type() {
  Token t = GetNextToken();
  if (!((checkResword(t, "int")) || (checkResword(t, "char")) || (checkResword(t, "boolean")) || (checkId(t)))) {
    error(illegalType, t);
  }
  if (!(checkId(t))) {
    if (find_symbol_top(t.lx) == -1)
      return error(undecIdentifier, t);
  }
}

void paramList() {
  Token t = PeekNextToken();
  char * sy_ty= t.lx;
  if (checkSym(t, '{'))
    return error(closeParenExpected, t);
  type();
  if (ter)
    return;
  t = GetNextToken();
  if (checkId(t))
    return error(closeParenExpected, t);
  if (find_symbol(t.lx,prg) != -1)
    return error(redecIdentifier, t);
  Symbol s = populate_symbol(t.lx,sy_ty,argument,0);
  add_symbol_node(s);
  while (repCallb()) {
    t = GetNextToken();
    if (!(checkSym(t, ',')))
      return error(classVarErr, t);
    t = PeekNextToken();
    sy_ty= t.lx;
    type();
    if (ter)
      return;
    t = GetNextToken();
    if (checkId(t))
      return error(idExpected, t);
    if (find_symbol(t.lx,prg) != -1)
        return error(redecIdentifier, t);
    Symbol s = populate_symbol(t.lx,sy_ty,argument,0);
    add_symbol_node(s);
  }
}

void subroutineBody() {
  cbo();
  if (ter)
    return;
  while (repCallcbc()) {
    statement();
    if (ter)
      return;
  }
  cbc();
  if (ter)
    return;
}

void statement() {
  Token t = GetNextToken();
  if (!(checkResword(t, "var"))) {
    varDeclarStatement();
  } else if (!(checkResword(t, "let"))) {
    letStatement();
  } else if (!(checkResword(t, "if"))) {
    ifStatement();
  } else if (!(checkResword(t, "while"))) {
    whileStatement();
  } else if (!(checkResword(t, "do"))) {
    doStatement();
  } else if (!(checkResword(t, "return"))) {
    returnStatement();
  } else {
    return error(syntaxError, t);
  }
}

void varDeclarStatement() {
  Token t = PeekNextToken();
  char * sy_ty= t.lx;
  type();
  if (ter)
    return;
  t = GetNextToken();
  if (checkId(t))
    return error(idExpected, t);
  if (find_symbol_local(t.lx) != -1)
    return error(redecIdentifier, t);
  Symbol s = populate_symbol(t.lx,sy_ty,argument,0);
  add_symbol_node(s);
  while (repCalle()) {
    t = GetNextToken();
    if (!(checkSym(t, ',')))
      return error(syntaxError, t);
    t = GetNextToken();
    if (checkId(t))
      return error(idExpected, t);
    if (find_symbol_local(t.lx) != -1)
      return error(redecIdentifier, t);
    s = populate_symbol(t.lx,sy_ty,argument,0);
    add_symbol_node(s);
  }
  t = GetNextToken();
  if (!(checkSym(t, ';')))
    return error(semicolonExpected, t);
}

void letStatement() {
  Token t = GetNextToken();
  if (checkId(t))
    return error(idExpected, t);
  if (find_symbol(t.lx,prg) == -1)
      return error(undecIdentifier, t);
  t = PeekNextToken();
  if (checkSym(t, '[')) {
    t = GetNextToken();
    expression();
    if (ter)
      return;
    t = GetNextToken();
    if (!(checkSym(t, ']')))
      return error(closeBracketExpected, t);
  }
  t = GetNextToken();
  if (!(checkSym(t, '=')))
    return error(equalExpected, t);
  expression();
  if (ter)
    return;
  t = GetNextToken();
  if (!(checkSym(t, ';')))
    return error(semicolonExpected, t);
}

void ifStatement() {
  Token t = GetNextToken();
  if (!(checkSym(t, '(')))
    return error(openParenExpected, t);
  expression();
  if (ter)
    return;
  t = GetNextToken();
  if (!(checkSym(t, ')')))
    return error(closeParenExpected, t);
  t = GetNextToken();
  if (!(checkSym(t, '{')))
    return error(openBraceExpected, t);
  while (repCallcbc()) {
    statement();
    if (ter)
      return;
  }
  t = GetNextToken();
  if (!(checkSym(t, '}')))
    return error(closeBraceExpected, t);
  t = PeekNextToken();
  if (!(checkResword(t, "else"))) {
    t = GetNextToken();
    t = GetNextToken();
    if (!(checkSym(t, '{')))
      return error(openBraceExpected, t);
    while (repCallcbc()) {
      statement();
      if (ter)
        return;
    }
    t = GetNextToken();
    if (!(checkSym(t, '}')))
      return error(closeBraceExpected, t);
  }
}

void whileStatement() {
  Token t = GetNextToken();
  if (!(checkSym(t, '(')))
    return error(openParenExpected, t);
  expression();
  if (ter)
    return;
  t = GetNextToken();
  if (!(checkSym(t, ')')))
    return error(closeParenExpected, t);
  t = GetNextToken();
  if (!(checkSym(t, '{')))
    return error(openBraceExpected, t);
  while (repCallcbc()) {
    statement();
    if (ter)
      return;
  }
  t = GetNextToken();
  if (!(checkSym(t, '}')))
    return error(closeBraceExpected, t);
}

void doStatement() {
  subroutineCall();
  if (ter)
    return;
  Token t = GetNextToken();
  if (!(checkSym(t, ';')))
    return error(semicolonExpected, t);
}

void subroutineCall() {
  Token t = GetNextToken();
  int cat = 0;
  if (find_symbol_top(t.lx) == -1 && strcmp(t.lx, prg) != 0) {
    cat = 1;
  }
  if (checkId(t))
    return error(idExpected, t);
  if (find_symbol(t.lx,prg) == -1)
    add_token_to_sub_check(t,0);
  t = PeekNextToken();
  if (checkSym(t, '.')) {
    t = GetNextToken();
    t = GetNextToken();
    if (checkId(t))
      return error(idExpected, t);
    if (find_symbol(t.lx,prg) == -1) {
      add_token_to_sub_check(t,cat);
    }
  } 
  t = GetNextToken();
  if (!(checkSym(t, '(')))
    return error(openParenExpected, t);
  expressionList();
  if (ter)
    return;
  t = GetNextToken();
  if (!(checkSym(t, ')')))
    return error(closeParenExpected, t);
}

void expressionList() {
  Token t = PeekNextToken();
  if (checkSym(t, ')'))
    return;
  expression();
  if (ter)
    return;
  t = PeekNextToken();
  while (checkSym(t, ',')) {
    t = GetNextToken();
    expression();
    if (ter)
      return;
    t = PeekNextToken();
  }
}

void returnStatement() {
  Token t = PeekNextToken();
  if (!(checkSym(t, ';'))) {
    expression();
    if (ter) {
      if (returnInfo.er == syntaxError) {
        returnInfo.er = semicolonExpected;
      }
      return;
    }
  }
  t = GetNextToken();
  if (!(checkSym(t, ';')))
    return error(semicolonExpected, t);
}

void expression() {
  relationalExpression();
  if (ter)
    return;
  Token t = PeekNextToken();
  while (checkSym(t, '&') || checkSym(t, '|')) {
    t = GetNextToken();
    relationalExpression();
    if (ter)
      return;
    t = PeekNextToken();
  }
}

void relationalExpression() {
  arithmeticExpression();
  if (ter)
    return;
  Token t = PeekNextToken();
  while (checkSym(t, '=') || checkSym(t, '>') || checkSym(t, '<')) {
    t = GetNextToken();
    arithmeticExpression();
    if (ter)
      return;
    t = PeekNextToken();
  }
}

void arithmeticExpression() {
  term();
  if (ter)
    return;
  Token t = PeekNextToken();
  while (checkSym(t, '+') || checkSym(t, '-')) {
    t = GetNextToken();
    term();
    if (ter)
      return;
    t = PeekNextToken();
  }
}

void term() {
  factor();
  if (ter)
    return;
  Token t = PeekNextToken();
  while (checkSym(t, '*') || checkSym(t, '/')) {
    t = GetNextToken();
    factor();
    if (ter)
      return;
    t = PeekNextToken();
  }
}

void factor() {
  Token t = PeekNextToken();
  if (checkSym(t, '-')) {
    t = GetNextToken();
  } else if (checkSym(t, '~')) {
    t = GetNextToken();
  }
  operand();
}

void operand() {
  Token t = PeekNextToken();
  if (t.tp == INT) {
    t = GetNextToken();
  } else if (!(checkId(t))) {
    if (find_symbol_all(t.lx) == -1)
      return error(undecIdentifier, t);
    t = GetNextToken();
    t = PeekNextToken();
    if (checkSym(t,'.')) {
      t = GetNextToken();
      t = GetNextToken();
      if (checkId(t))
        return error(idExpected,t);
      if (find_symbol_all(t.lx) == -1)
        add_token_to_sub_check(t,0);
    }
    t = PeekNextToken();
    if (checkSym(t,'[')) {
      t = GetNextToken();
      expression();
      t = GetNextToken();
      if (!(checkSym(t,']')))
        return error(closeBracketExpected,t);
    } else if (checkSym(t,'(')) {
      t = GetNextToken();
      expressionList();
      if (ter) return;
      t = GetNextToken();
      if (!(checkSym(t,')')))
        return error(lexerErr,t);
    }
  } else if (checkSym(t,'(')) {
    t = GetNextToken();
    expression();
    if (ter) return;
    t = GetNextToken();
    if (!checkSym(t,')'))
      return error(closeParenExpected,t);
  } else if (t.tp == STRING) {
    t = GetNextToken();
  } else if (!(checkResword(t,"true"))) {
    t = GetNextToken();
  } else if (!(checkResword(t,"false"))) {
    t = GetNextToken();
  } else if (!(checkResword(t,"null"))) {
    t = GetNextToken();
  } else if (!(checkResword(t,"this"))) {
    t = GetNextToken();
  } else {
    error(syntaxError,t);
  }
}



int InitParser (char* file_name)
{
  return 1;
}


// Function called in tests
ParserInfo Parse ()
{
  ter = 0;
  returnInfo.er = none; 
  classDeclar();
  if (ter == 0) {
    Token t = do_sub_check(0);
    if (t.tp != EOFile) {
      error(undecIdentifier,t);
    } 
  } else {
    reset_sub(0);
  }
  StopParser();
	return returnInfo;
}


int StopParser() {
  return 1;
}
