#include "huffman.h"
#include "code.h"
#include "heap.h"
#include "io.h"
#include "node.h"
#include "pq.h"
#include "stack.h"
#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// creates the code but not init'ed
Code temp;
// creates the var to track bypasses for initing code
static bool bypass = true;

// creates the function for building the tree
Node *build_tree(uint64_t hist[static ALPHABET]) {
  // makes a priority queue for the functions use
  PriorityQueue *pq = pq_create(ALPHABET);
  // checks that the pq was made successfully
  if (pq == NULL) {
    // frees the pq if made wrong
    free(pq);
    // sets the pq to null
    pq = NULL;
    // seys the return node to null
    Node *out = NULL;
    // returns the null node
    return out;
  }
  // loops through the histogram passed in
  for (int i = 0; i < ALPHABET; i += 1) {
    // checks if the index of the hist is not 0
    if (hist[i] != 0) {
      // creates a node using the index and the hist value
      Node *node = node_create(i, hist[i]);
      // enqueues the new node
      enqueue(pq, node);
    }
  }
  // loops while the size of the pq is still greater than 1
  while (pq_size(pq) > 1) {
    // makes a temp left node
    Node *temp_left;
    // dequeues a node and sets the left temp
    dequeue(pq, &temp_left);
    // makes a temp right node
    Node *temp_right;
    // deuqeues a node and sets the right temp
    dequeue(pq, &temp_right);
    // makes a node to set as new parents
    Node *parent;
    // joins the left and right node and sets the parent node as the return
    parent = node_join(temp_left, temp_right);
    // enqueues the new parent node
    enqueue(pq, parent);
  }
  // makes a return node
  Node *out;
  // dequeues a node and sets the temp return one
  dequeue(pq, &out);
  // deletes the pq
  pq_delete(&pq);
  // returns the node
  return out;
}
// creates the function to build the codes
void build_codes(Node *root, Code table[static ALPHABET]) {
  // creates the var to hold the flagged results
  int flag = 0;
  // checks if the node passed is valid
  if (root == NULL) {
    // returns if invalid
    return;
  }
  // checks if the bypass has been flagged or not
  if (bypass == true) {
    // if still fresh it will init the code
    temp = code_init();
    // flags the bypass has been used
    bypass = false;
  }
  // checks if the node has a left child
  if (root->left == NULL) {
    // flags this child
    flag += 1;
  }
  // checks if the node has a right child
  if (root->right == NULL) {
    // flags this child
    flag += 1;
  }
  // checks if the flag was raised twice
  if (flag == 2) {
    // sets the symbols index to the code
    table[root->symbol] = temp;
  } else {
    // pushes 0 to the code
    code_push_bit(&temp, 0);
    // calls the build codes again with the left child and the same table
    build_codes(root->left, table);
    // makes a bit to be set
    uint8_t bit1 = 0;
    // pops the bit from the code and sets the bit temp
    code_pop_bit(&temp, &bit1);
    // pushes 1 onto the code
    code_push_bit(&temp, 1);
    // calls build codes again with the right child now and the same table
    build_codes(root->right, table);
    // makes bit to be set
    uint8_t bit2 = 0;
    // pops the bit and sets the temp
    code_pop_bit(&temp, &bit2);
  }
}
// creates the dump tree function
void dump_tree(int outfile, Node *root) {
  // checks that the node passed is valid
  if (root == NULL) {
    // returns if the node is invalid
    return;
  }
  // makes a temp for the left child
  Node *left = root->left;
  // makes a temp for the right child
  Node *right = root->right;
  // calls the dumptree function using the left temp and the file
  dump_tree(outfile, left);
  // calls dump with the right child and the file passed
  dump_tree(outfile, right);
  // checks that both the right and left child are null
  if (left == NULL && right == NULL) {
    // makes a temp buffer for the info to be written
    uint8_t *buf = (uint8_t *)calloc(2, sizeof(uint8_t));
    // sets the buffers index 0 to L
    buf[0] = 'L';
    // sets the buffers index 1 to the symbol
    buf[1] = root->symbol;
    // writes the filled buffer to the file
    write_bytes(outfile, buf, 2);
    // frees the buffer
    free(buf);
  } else {
    // makes a buffer to hold the info to be written
    uint8_t *buf = (uint8_t *)calloc(1, sizeof(uint8_t));
    // sets the buffer to I
    buf[0] = 'I';
    // writes the buffer to the file
    write_bytes(outfile, buf, 1);
    // frees the buffer
    free(buf);
  }
}
// creates the rebuild tree function
Node *rebuild_tree(uint16_t nbytes, uint8_t tree_dump[static nbytes]) {
  // creates a stack to be used (with the size passed in)
  Stack *stack = stack_create(nbytes);
  // checks that the stack was made correctly
  if (stack == NULL) {
    // sets a return node to null
    Node *out = NULL;
    // frees the stack
    free(stack);
    // sets the stack to null
    stack = NULL;
    // returns the null node
    return out;
  }
  // makes an index place holder
  uint16_t index = 0;
  // loops while the index is lower than the bytes meant to be read
  while (index < nbytes) {
    // checks if the index of the passed in tree dump is L
    if (tree_dump[index] == 'L') {
      // creates a node to be pushed using the index after the current of the
      // dump and 0
      Node *temp = node_create(tree_dump[index + 1], 0);
      // pushes the temp node onto the stack
      stack_push(stack, temp);
      // updates the index by 2
      index += 2;
      // continues;
      continue;
    }
    // checks if the tree dump value at index is I
    if (tree_dump[index] == 'I') {
      // makes a right node
      Node *right = NULL;
      // pops a node and sets it to the right temp
      stack_pop(stack, &right);
      // makes a left node
      Node *left = NULL;
      // pops a node and sets it to the left temp
      stack_pop(stack, &left);
      // makes a parent node and joins the left and right to make it
      Node *par = node_join(left, right);
      // pushes this parent to the stack
      stack_push(stack, par);
      // increases index by 1
      index += 1;
      continue;
    }
  }
  // makes a return node
  Node *out;
  // pops the node on the stack to set the return
  stack_pop(stack, &out);
  // deletes the stack
  stack_delete(&stack);
  // returns the popped node
  return out;
}
// creates the delete tree function
void delete_tree(Node **root) {
  // checks if the node is valid
  if ((*root) == NULL) {
    // returns if node is invalid
    return;
  }
  // checks if the node has a left child
  if ((*root)->left != NULL) {
    // calls delete again with the child
    delete_tree(&(*root)->left);
  }
  // checks if the node has a right child
  if ((*root)->right != NULL) {
    // calls delete again witht the right child
    delete_tree(&(*root)->right);
  }
  // deletes the final nodes of calls
  node_delete(root);
  // frees the root
  free(*root);
  // sets the root to null
  *root = NULL;
}
