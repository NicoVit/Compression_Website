#include "heap.h"
#include "node.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// creates the function for swapping
void swap(Node *one, Node *two) {
  // sets the temp
  Node temp = *one;
  // does the swapping
  *one = *two;
  *two = temp;
}

// produces values baased on formula
int l_child(int x) { return 2 * x + 1; }
// produces values based on formula
int r_child(int x) { return 2 * x + 2; }
// produces output based on formula
int parent(int x) { return (x - 1) / 2; }
// creates the function for up heap
void up_heap(Node **arr, int n) {
  // loop that checks for the index to be above 0
  // and for the n index of array to be less than the parent
  // index inside of the array
  while (n > 0 && node_cmp(arr[n], arr[parent(n)]) == false) {
    // swaps the array n indexc and the parent
    // formulated one
    swap(arr[n], arr[parent(n)]);
    // makes n equal the parent formulation of n
    n = parent(n);
  }
}
// creates the function of down heap
void down_heap(Node **arr, int heap_size) {
  // creates the vars to hold index and condtional num
  int n = 0;
  int bigger = 0;
  // looops while the child formulated index is less than the size
  while (l_child(n) < heap_size) {
    // checks if the child formulated index is equal to the size
    if (r_child(n) == heap_size) {
      // makes the temp var hold the child formulated index
      bigger = l_child(n);
    } else {
      // checks if array index of child formulated index is
      // less than the same of the other side
      if (node_cmp(arr[l_child(n)], arr[r_child(n)]) == false) {
        // holds the formulated new var
        bigger = l_child(n);
      } else {
        // holds the right side formulation
        bigger = r_child(n);
      }
    }
    // checks index of array and temp index of array
    // if smaller than it will break
    if (node_cmp(arr[n], arr[bigger]) == false) {
      break;
    }
    // swaps temp index and index of array
    swap(arr[bigger], arr[n]);
    // holds the temp as the index
    n = bigger;
  }
}
