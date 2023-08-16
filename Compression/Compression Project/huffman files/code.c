#include "code.h"
#include "heap.h"
#include "node.h"
#include "pq.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// creates the structure for this files object type
struct Code;
// creates the function to make the codes
Code code_init(void) {
  // creates the object to work with using the struct
  Code code;
  // makes an index to keep track of the array in the loop
  uint64_t index = 0;
  // loops while the index spot is less than the max allowed size
  while (index < MAX_CODE_SIZE) {
    // enters the index spot of the arr and sets the value to 0
    code.bits[index] = 0;
    // adds one to the index to move on
    index += 1;
  }
  // sets the objects "top" field to 0
  code.top = 0;
  // returns the object
  return code;
}
// creates the function for getting the code's size
uint32_t code_size(Code *c) {
  // makes sure the code passed in is valid
  if (c == NULL) {
    // returns 0 if the code is invalid
    return 0;
  }
  // returns the "top" field of the code for the size
  return c->top;
}
// creates the function to check if the code object is empty
bool code_empty(Code *c) {
  // makes sure the code is valid
  if (c == NULL) {
    // returns false if the code object passed in is not valid
    return false;
  }
  // creates a var to record the result and avoid 2 if statments and return
  // paths
  int flag = 0;
  // checks the value of the "top" field
  if (c->top == 0) {
    // marks if it is true
    flag += 1;
  }
  // returns the value of the condition based on the flagging
  return flag == 1;
}
// creates the function to check if the code is full
bool code_full(Code *c) {
  // checks if the code object is valid
  if (c == NULL) {
    // returns false if the object is not valid
    return false;
  }
  // creates a flag to track result to avoid 2 statments and control paths
  int flag = 0;
  // checks if the top field is at the max allowed space
  if (c->top == ALPHABET) {
    // flags the case in which it is full
    flag += 1;
  }
  // returns the condtion result
  return flag == 1;
}
// creates the function to set bits in the code
bool code_set_bit(Code *c, uint32_t i) {
  // checks to make sure that the code passed through is valid
  if (c == NULL) {
    // returns false if the code is not real
    return false;
  }
  // checks that the index passed through is within range of the allowed code
  if (i > ALPHABET || i > c->top) {
    // returns false if the index is out of range
    return false;
  }
  // gets the shifting spots by dividing and moduloing the index by the amount
  // of bits held by a uint8
  uint64_t location = i / 8;
  uint64_t position = i % 8;
  // shifts the bit at the spot using bit shift or
  c->bits[location] |= 1UL << (position);
  // returns true to signify success
  return true;
}
// creates the function to clear bits
bool code_clr_bit(Code *c, uint32_t i) {
  // checks that the code passed in is valid
  if (c == NULL) {
    // returns false if invalid
    return false;
  }
  // checks that the index passed in is within range
  if (i > ALPHABET || i > c->top) {
    // returns false if the index is not in range
    return false;
  }
  // gets the shifting places based on the index and the size of a uint8
  uint64_t location = i / 8;
  uint64_t position = i % 8;
  // bit shift ands to clear the bit at the spot
  c->bits[location] &= ~(1UL << (position));
  // returns true to signify success in the call
  return true;
}
// creates the function to get the bit at the index
bool code_get_bit(Code *c, uint32_t i) {
  // checks that the code is valid
  if (c == NULL) {
    // returns false if the code is not real
    return false;
  }
  // checks that the index passed in is within range
  if (i > ALPHABET || i > c->top) {
    // returns false if the index is out of range
    return false;
  }
  // gets the required details to shift using the index and the size of a uint8
  uint64_t location = i / 8;
  uint64_t position = i % 8;
  // makes a temp to hold the bit at the spot
  uint64_t hold = c->bits[location];
  // shifts the temp bit
  hold |= 1UL << (position);
  // creates the flag to keep track of the result
  uint64_t flag = 0;
  // checks if the shifted temp version is the same as the actual bit
  // this signifies that the bit is a 1
  if (hold == c->bits[location]) {
    // flags the result if needed
    flag += 1;
  }
  // returns the result of the conditional
  return flag == 1;
}
// creates the function to push bits in the code
bool code_push_bit(Code *c, uint8_t bit) {
  // checks that the code passed in is valid
  if (c == NULL) {
    // returns false if the code is invalid
    return false;
  }
  // checks that the code is not full using the other function I already made
  if (code_full(c) == true) {
    // returns false if it cannot push
    return false;
  }
  // checks if the bit is a 1
  if (bit == 1) {
    // sets the bit using the other function i made because the bit is a one
    code_set_bit(c, c->top);
    // increases the size of the code to signify adding one
    c->top += 1;
    // returns true for success
    return true;
  }
  // clears the bit using the function i made earlier because the bit passed is
  // 0
  code_clr_bit(c, c->top);
  // signifies the change in the code
  c->top += 1;
  // returns true for the successful call
  return true;
}
// creates the function to pop a bit
bool code_pop_bit(Code *c, uint8_t *bit) {
  // checks that the code passed in is valid
  if (c == NULL) {
    // returns false if the code is invalid
    return false;
  }
  // creates a uint version of the numbers because it was giving me a weird
  // error with adding normal numbers to the bit
  uint8_t one = 1;
  uint8_t zero = 0;
  // checks that the code is not empty using the function i made earlier
  if (!code_empty(c)) {
    // gets the boolean value from the return of the get bit function i made
    // this is used to figure out what the bit is
    bool hold = code_get_bit(c, c->top - 1);
    // checks if the bool is true or not to find the value of the bit
    if (hold) {
      // sets the bit to one if the bool is true
      *bit = one;
    } else {
      // sets the bit to 0 if the bool is false
      *bit = zero;
    }
    // clears the bit using the function i made earlier and the last bit in the
    // code
    code_clr_bit(c, c->top - 1);
    // signifies the loss of a bit using the field
    c->top -= 1;
    // returns true signifying a successful call
    return true;
  } else {
    // returns false if the call fails
    return false;
  }
}
// creates the print function for my own debugging purposes (free function)
void code_print(Code *c) {
  // makes sure the code is valid
  if (c == NULL) {
    // returns if not valid
    return;
  }
  // makes an index var to keep track
  uint64_t index = 0;
  // makes a temp var
  uint8_t hold;
  // loops through the size of the code
  while (index < code_size(c)) {
    // gets the bit using my function
    if (code_get_bit(c, index) == true) {
      // sets the temp to 1 to be printed
      hold = 1;
    } else {
      // sets the temp to 0 to be printed
      hold = 0;
    }
    // prints the bit using the temp
    printf("%" PRIu8 "", hold);
    // increases index
    index += 1;
  }
  // returns
  return;
}
