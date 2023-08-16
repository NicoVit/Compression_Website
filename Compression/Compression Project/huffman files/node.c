#include "node.h"
#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
// creates the structure for node
typedef struct Node Node;
// creates the function to create nodes
Node *node_create(uint8_t symbol, uint64_t frequency) {
  // makes a node using the stuct type and allocates memory for it
  Node *node = (Node *)malloc(sizeof(Node));
  // checks that the node was made correctly
  if (node == NULL) {
    // if the node is incorrect i will free the mem
    free(node);
    // sets the node to null to be returned
    node = NULL;
    // returns the node
    return node;
  }
  // sets the symbol field to the symbol passed in
  node->symbol = symbol;
  // sets the frequency feild to the frequency passed in
  node->frequency = frequency;
  // sets the left child to null
  node->left = NULL;
  // sets the right child to null
  node->right = NULL;
  // returns the made node
  return node;
}
// creates the function for deleting the node
void node_delete(Node **n) {
  // checks that the node passed is actually valid
  if ((*n) == NULL) {
    // returns if the node is not real
    return;
  }
  // frees the nodes memory
  free(*n);
  // sets the node to null
  *n = NULL;
}
// creates the function to join to nodes into a parent
Node *node_join(Node *left, Node *right) {
  // checks that the left node passed is real
  if (left == NULL) {
    // frees the left node if invalid
    free(left);
    // sets the node to null
    left = NULL;
    // returns a null node
    return left;
  }
  // checks that the right node was valid
  if (right == NULL) {
    // frees the memory for it if invalid
    free(right);
    // sets the right node to null
    right = NULL;
    // returns a null node
    return right;
  }
  // makes a node to be used as a parent
  Node *node = node_create(0, 0);
  // sets the parents left child to the left node passed
  node->left = left;
  // sets the parents right child as the right node passed
  node->right = right;
  // sets the parents symbol to the parent symbol sign "$"
  node->symbol = 36;
  // sets the frequency of the parent as the frequency of the children combined
  node->frequency = left->frequency + right->frequency;
  // returns the parent node
  return node;
}
// creates the function to print the node
void node_print(Node *n) {
  // checks that the node passed in is valid
  if (n == NULL) {
    // returns if node is invalid
    return;
  }
  // creates a flag for the results of the conditionals
  uint8_t sym_flag = 0;
  // checks if the symbol of the node quals as a cntrl
  if (iscntrl(n->symbol) == 1) {
    // flags it
    sym_flag += 1;
  }
  // checks if it is normal symbol
  if (isprint(n->symbol) == 1) {
    // doesnt alter flag
    sym_flag = 0;
  }
  // checks which result there is (this one is cntrl)
  if (sym_flag == 1) {
    // prints using the format given in piazza
    printf("0x%02" PRIx8 "\n", n->symbol);
  }
  // checks if the symbol is normal
  if (sym_flag == 0) {
    // prints normally
    printf("%c\n", n->symbol);
  }
  // prints the nodes frequency
  printf("%" PRIu64 "\n", n->frequency);
}
// creates the function to compare nodes
bool node_cmp(Node *node1, Node *node2) {
  // returns the result of which frequency is higher
  return node1->frequency > node2->frequency;
}
// creates the function to print node symbol only
void node_print_sys(Node *node) {
  // checks that the node is valid
  if (node == NULL) {
    // returns if not valid
    return;
  }
  // creates the same flag as other print
  uint8_t sym_flag = 0;
  // checks the symbol type between control and normal and flags it
  if (iscntrl(node->symbol) == 1) {
    sym_flag += 1;
  }
  if (isprint(node->symbol) == 1) {
    sym_flag = 0;
  }
  // checks the flag raised and prints based on the type
  if (sym_flag == 1) {
    printf("0x%02" PRIx8 "\n", node->symbol);
  }
  if (sym_flag == 0) {
    printf("%c\n", node->symbol);
  }
}
