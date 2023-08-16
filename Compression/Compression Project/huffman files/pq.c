#include "pq.h"
#include "heap.h"
#include "node.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// makes the stucture for the object pq
typedef struct PriorityQueue {
  // creates the fields
  uint64_t size;
  uint64_t capacity;
  Node **arr;
} PriorityQueue;
// creates the function that makes the pqs
PriorityQueue *pq_create(uint32_t capacity) {
  // allocates memory for the pq while making the object
  PriorityQueue *q = (PriorityQueue *)malloc(sizeof(PriorityQueue));
  // checks that the mem was allocated correctly
  if (q == NULL) {
    // frees the mem if done wrong
    free(q);
    // sets the pq to null
    q = NULL;
    // returns a null q
    return q;
  }
  // makes an array of nodes and allocates mem for it
  Node **arr = (Node **)calloc(capacity, sizeof(Node *));
  // checks that the arr was allocated right
  if (arr == NULL) {
    // frees the mem if done wrong
    free(arr);
    // makes the array null
    arr = NULL;
    // frees the q because it cannot proceed
    free(q);
    // sets it to null
    q = NULL;
    // returns null pq
    return q;
  }
  // makes the size field equal to 0
  q->size = 0;
  // makes the capacity field equal to the passed in capacity
  q->capacity = capacity;
  // makes the array field equal to the array made
  q->arr = arr;
  // returns the finished pq
  return q;
}
// creates the function to delete the pqs
void pq_delete(PriorityQueue **q) {
  // checks that the pq passed in is valid
  if ((*q) == NULL) {
    // returns if invalid
    return;
  }
  // checks the arr to see what operations need to be done
  if ((*q)->arr == NULL) {
    // frees the q if the array is already null
    free(q);
    // sets the pq to null
    q = NULL;
    // returns to end
    return;
  }
  // frees the array within the pq if it is there
  free((*q)->arr);
  // frees the pq
  free(*q);
  // sets the pq to null to finish
  *q = NULL;
}
// creates the function to check if the pq is empty
bool pq_empty(PriorityQueue *q) {
  // checks that the pq is valid
  if (q == NULL) {
    // returns false if it is invalid
    return false;
  }
  // checks if the size is 0 meaning empty
  if (q->size == 0) {
    // returns true meaning its empty
    return true;
  }
  // returns false meaning not empty
  return false;
}
// creates the function to check if the pq is full
bool pq_full(PriorityQueue *q) {
  // checks if the pq is valid or not
  if (q == NULL) {
    // returns false if invalid
    return false;
  }
  // checks if the size is at capacity
  if (q->size == q->capacity) {
    // returns true if there is no more space
    return true;
  }
  // returns false meaning not full
  return false;
}
// creates the function to get the size of the current pq
uint32_t pq_size(PriorityQueue *q) {
  // checks that the pq is valid
  if (q == NULL) {
    // returns 0 if invlaid
    return 0;
  }
  // returns the size field of the pq
  return q->size;
}
// creates the function that enqueues a node
bool enqueue(PriorityQueue *q, Node *n) {
  // checks if the pq is valid or not
  if (q == NULL) {
    // returns false if invalid
    return false;
  }
  // checks if the pq is at capacity
  if (pq_full(q) == true) {
    // returns false because there is no more space
    return false;
  }
  // sets the next element in the array to the node
  q->arr[q->size] = n;
  // calls up heap to fix the pq
  up_heap(q->arr, q->size);
  // adds one to the size because there is a new item
  q->size += 1;
  // returns true for success
  return true;
}
// creates the function to dequeue a node
bool dequeue(PriorityQueue *q, Node **n) {
  // checks that the pq is valid
  if (q == NULL) {
    // returns false if the pq is invalid
    return false;
  }
  // checks if the pq is empty meaning nothing to dequeue
  if (pq_empty(q) == true) {
    // returns false for unsuccessful call
    return false;
  }
  // makes the node equal to the first element in the pq
  *n = q->arr[0];
  // makes the first element now equal the last for resorting
  q->arr[0] = q->arr[q->size - 1];
  // takes on from the size to clear the duplicate
  q->size -= 1;
  // calls downheap to resort everything
  down_heap(q->arr, q->size);
  // returns true for the successful call
  return true;
}
// creates the function to print pqs
void pq_print(PriorityQueue *q) {
  // checks that the pq is valid
  if (q == NULL) {
    // returns if invalid
    return;
  }
  // makes an index for the loop to use
  uint64_t index = 0;
  // loops until a break is reached
  while (true) {
    // prints the node at the index in the array
    node_print(q->arr[index]);
    // increases the index
    index += 1;
    // makes sure the index is still valid
    if (index >= q->size) {
      // if done printing it will return
      return;
    }
  }
}
