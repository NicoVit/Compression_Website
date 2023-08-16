#include "stack.h"
#include "code.h"
#include "heap.h"
#include "node.h"
#include "pq.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// creates the structure for the stack
typedef struct Stack {
  // sets the fields
  uint32_t top;
  uint32_t capacity;
  Node **items;
} Stack;
// creates the function to make stacks
Stack *stack_create(uint32_t capacity) {
  // uses the struct to make the stack object and allocates mem for it
  Stack *stack = (Stack *)malloc(sizeof(Stack));
  // checks that the stacks mem was allocated properly
  if (stack == NULL) {
    // frees the stack
    free(stack);
    // makes the stack null
    stack = NULL;
    // returns the null stack
    return stack;
  }
  // creates a array of nodes and allocates memory for them
  Node **arr = (Node **)calloc(capacity, sizeof(Node *));
  // makes sure that the array was allocated properly
  if (arr == NULL) {
    // frees the array if not allocated right
    free(arr);
    // frees the stack if the array was failed
    free(stack);
    // makes the stack null
    stack = NULL;
    // returns the null stack
    return stack;
  }
  // sets the items field to the array made
  stack->items = arr;
  // sets the top field to 0
  stack->top = 0;
  // sets the capacity field to the capcity passed in
  stack->capacity = capacity;
  // returns the finished stack
  return stack;
}
// creates the function to delete stacks
void stack_delete(Stack **s) {
  // checks that the stack passed is valid
  if ((*s) == NULL) {
    // returns if the stack is invalid
    return;
  }
  // frees the items field in the stack
  free((*s)->items);
  // frees the stack
  free(*s);
  // sets the stack to null
  (*s) = NULL;
  // returns for success
  return;
}
// creates the function for checking if the stack is empty
bool stack_empty(Stack *s) {
  // checks that the stack is valid
  if (s == NULL) {
    // returns false if the stack is invalid
    return false;
  }
  // checks if the top field is equal to 0
  if (s->top == 0) {
    // returns true to show that it is empty
    return true;
  }
  // returns false if the top is not equal to 0
  return false;
}
// creates the function to check if the stack is full
bool stack_full(Stack *s) {
  // checks if the stack passed in is valid
  if (s == NULL) {
    // returns false if the stacj is invalid
    return false;
  }
  // checks if the top field is equal to the capacity field
  if (s->top >= s->capacity) {
    // returns true if the stack is full
    return true;
  }
  // returns false if not full
  return false;
}
// creates the function to give the current stack size
uint32_t stack_size(Stack *s) {
  // checks if the stack is valid
  if (s == NULL) {
    // returns 0 if the stack is invalid
    return 0;
  }
  // returns the top field of the stack
  return s->top;
}
// creates the function to push a node on the stack
bool stack_push(Stack *s, Node *n) {
  // checks that the stack is valid
  if (s == NULL) {
    // returns false if the stack is invalid
    return false;
  }
  // checks if the stack is full so you cant push
  if (stack_full(s)) {
    // returns false if the stack cant have new node pushed
    return false;
  }
  // makes the last item in the items field equal the node passed in
  s->items[s->top] = n;
  // increaes the top field by one signifying the growth
  s->top += 1;
  // returns true for successful call
  return true;
}
// creates the function to pop the nodes
bool stack_pop(Stack *s, Node **n) {
  // checks that the stack is valid
  if (s == NULL) {
    // returns false if invalid
    return false;
  }
  // checks if the stack is empty meaning nothing to pop
  if (stack_empty(s) == true) {
    // returns false if there is nothing to pop
    return false;
  }
  // makes the node holder passed in equal to the last node in the stack
  *n = s->items[s->top - 1];
  // subtracts one from the length of the stack
  s->top -= 1;
  // returns true signifying success
  return true;
}
// creates the function to print the stack
void stack_print(Stack *s) {
  // checks that the stack is valid
  if (s == NULL) {
    // returns if invalid
    return;
  }
  // creates the index to use in the loop
  uint32_t index = 0;
  // loops through the length of the stack
  while (index < s->top) {
    // sends the current indexed node to the node print func
    node_print(s->items[index]);
    // increases index
    index += 1;
  }
  // returns to end
  return;
}
