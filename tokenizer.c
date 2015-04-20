/*
 * tokenizer.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>

/*
 * Tokenizer type.  You need to fill in the type as part of your implementation.
 */
struct TokenizerT_ {
  //Implemented as a linked list with String values
  char *value;
  struct TokenizerT_ *next;
};

typedef struct TokenizerT_ TokenizerT;

/*
 *Returns the appropriate character based on the escape sequence or regular character
 */
char getEscapeChar(char single_char) {
  switch (single_char) {
  case 'n':
    return '\n';
  case 't':
    return '\t';
  case 'v':
    return '\v';
  case 'b':
    return '\b';
  case 'r':
    return '\r';
  case 'f':
    return '\f';
  case 'a':
    return '\a';
  case '\\':
    return '\\';
  case '\?':
    return '\?';
  case '\'':
    return '\'';
  case '\"':
    return '\"';
  default:
    return single_char;
  }
}

/*
 *Takes a string and returns a string with appropriate escape sequence
 *characters inserted.
 */
char *cleanCopyString(char *input_string) {
  int i = 0;
  char next_char;
  char *temp = (char *)malloc(sizeof(char) * strlen(input_string) + 1);
  char *result;
  int temp_i = 0;
  for(i = 0; i < strlen(input_string); i++, temp_i++) {
    if (input_string[i] == '\\') { 
      // Possible escape sequence found
      next_char = input_string[++i];
      temp[temp_i] = getEscapeChar(next_char); 
    }
    else {
      temp[temp_i] = input_string[i];
    }
  }
  temp[temp_i] = '\0';
  result = (char *)malloc(sizeof(char) * strlen(temp) + 1);
  strcpy(result, temp);
  free(temp);
  return result;
}

/*
 *Returns a sub string containing all information between the two
 *pointers in memory
 */
char *TKSubString(char *start, char *end) {
  char *result = malloc(sizeof(char) * abs(start-end) + 1);
  char *result_ptr;
  char *input_ptr;
  for(input_ptr = start, result_ptr = result; input_ptr < end; input_ptr++, result_ptr++) {
    *result_ptr = *input_ptr;
  }
  *result_ptr = '\0';
  return result;
}



/*
 * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
 *
 * You need to fill in this function as part of your implementation.
 */

void TKDestroy(TokenizerT *tk) {
  free(tk->value);
  free(tk);
}

/*
 * TKCreate creates a new TokenizerT object for a given set of separator
 * characters (given as a string) and a token stream (given as a string).
 * 
 * TKCreate should copy the two arguments so that it is not dependent on
 * them staying immutable after returning.  (In the future, this may change
 * to increase efficiency.)
 *
 * If the function succeeds, it returns a non-NULL TokenizerT.
 * Else it returns NULL.
 *
 * You need to fill in this function as part of your implementation.
 */
TokenizerT *TKCreate(char *separators, char *token_string) {
  //For each token, create a TokenizerT struct
  char *ptr;
  char *substring_start;
  char *token;
  TokenizerT *new_node;
  TokenizerT *head = malloc(sizeof(TokenizerT)); // head will start as a dummy node
  TokenizerT *tail = head;
  TokenizerT *result;
  head->value = NULL;
  head->next = NULL;
  for(ptr = substring_start = token_string; *ptr; ptr += sizeof(char)) {
    if(strchr(separators, *ptr) != NULL) { // If current character is a separator
      // Create a token with parsed word
      if(substring_start != ptr) {
	token = TKSubString(substring_start, ptr);
	new_node = malloc(sizeof(TokenizerT));
	new_node->value = token;
	new_node->next = NULL;
	tail->next = new_node;
	tail = tail->next;
      }
      substring_start = ptr + 1; //shifts substring_start past current separator
    }        
  }
  if(substring_start != ptr) {
    new_node = malloc(sizeof(TokenizerT));
    new_node->value = TKSubString(substring_start, ptr);
    new_node->next = NULL;
    tail->next = new_node;
    tail = tail->next;
  }
  result = head->next; // Chop off the dummy head
  TKDestroy(head);    
    
  return result;
}

/*
 * TKGetNextToken returns the next token from the token stream as a
 * character string.  Space for the returned token should be dynamically
 * allocated.  The caller is responsible for freeing the space once it is
 * no longer needed.
 *
 * If the function succeeds, it returns a C string (delimited by '\0')
 * containing the token.  Else it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */

TokenizerT *TKGetNextToken(TokenizerT *tk) {
  TokenizerT *new_head = tk->next;
  TKDestroy(tk); // Will only destroy the first node of the list
  return new_head;
}

/*
 * main will have two string arguments (in argv[1] and argv[2]).
 * The first string contains the separator characters.
 * The second string contains the tokens.
 * Print out the tokens in the second string in left-to-right order.
 * Each token should be printed on a separate line.
 */
void printUsage(int out) {
  if(out == 0) {
    fprintf(stdout, "Usage: tokenizer \"<separators>\" \"<string>\"\n");
  }
  else if(out == 1) {
    fprintf(stderr, "Usage: tokenizer \"<separators>\" \"<string>\"\n");
  }
}

/*
 * Print function which iterates through each character and printing
 * to screen. Escape sequence characters are printed as their hex
 * values.
 */
void printCharacters(char *value) {
  int i = 0;
  for(i = 0; i < strlen(value); i++) {
    switch(value[i]) {
    case '\n':
    case '\t':
    case '\v':
    case '\b':
    case '\r':
    case '\f':
    case '\a':
    case '\\':
    case '\?':
    case '\'':
    case '\"':
      fprintf(stdout, "[0x%.2x]", value[i]);
      break;
    default:
      fprintf(stdout, "%c", value[i]);
      break;
    }
  }
  fprintf(stdout, "\n");
}

int main(int argc, char **argv) {
  TokenizerT *token_list;  
  char *separators_copy;
  char *token_string_copy;
  if(argc != 3) {
    fprintf(stderr, "Incorrect number of arguments.\n");
    printUsage(1);
    exit(1);
  }

  // Next two lines make copies of input strings
  separators_copy = cleanCopyString(argv[1]);
  token_string_copy = cleanCopyString(argv[2]);
  // TKCreate will create a linked list of tokenized words
  token_list = TKCreate(separators_copy, token_string_copy); 

  while( (token_list) != NULL ) { // Start iterating through token list
    // For each token, print it on a new line
    printCharacters(token_list->value);
    token_list = TKGetNextToken(token_list); 
  }
  
  return 0;
}
