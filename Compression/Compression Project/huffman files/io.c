#include "io.h"
#include "code.h"
#include "heap.h"
#include "node.h"
#include "pq.h"
#include "stack.h"
#include <fcntl.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// creates the statics and externs
// tracks the bytes read
uint64_t bytes_read = 0;
// tracks the bytes written
uint64_t bytes_written = 0;
// tracks the index in read byte
static int read_place = 0;
// tracks the index in write byte
static int write_place = 0;
// tracks the index in read bit
static int bit_place = 0;
// temp block for diff functions
static uint8_t buff[BLOCK];
// tracks index and bytes in write code
static int flushing = 0;
// creates the function to read bytes in
int read_bytes(int infile, uint8_t *buf, int nbytes) {
  // creates the var to hold the bytes in this run
  int bytes = 0;
  // creates the index to hold amount of bytes left to read
  int iter_bytes = 0;
  // var to hold the new read bytes
  int add_bytes;
  // loops while the bytes read are less than the amount it is supposed to read
  while (bytes < nbytes) {
    // calculates the bytes left to read on this run
    iter_bytes = nbytes - bytes;
    // reads in bytes from file using the buffer manipulated by the amount read
    // and the bytes allowed sets the return to the var
    add_bytes = read(infile, buf + read_place, iter_bytes);
    // checks the returned bytes to make sure there are still bytes being read
    if (add_bytes == 0) {
      // if there are no more bytes being read it returns the bytes read so far
      return bytes;
    }
    // adds the new bytes to the extern total
    bytes_read += add_bytes;
    // adds the new bytes to the run total
    bytes += add_bytes;
    // adds the bytes to the index total
    read_place += add_bytes;
  }
  // resets the index for future runs
  read_place = 0;
  // returns the bytes read
  return bytes;
}
// creates the function to write bytes
int write_bytes(int outfile, uint8_t *buf, int nbytes) {
  // creates the var to hold the bytes written in the run
  int bytes = 0;
  // creates the var to track bytes left to write
  int iter_bytes = 0;
  // creates the var to set the new write bytes to
  int add_bytes;
  // loops while the bytes from this run are less than the allowed amount
  while (bytes < nbytes) {
    // calc the bytes that are still allowed to be written
    iter_bytes = nbytes - bytes;
    // writes the bytes to the file using the buffer manipulated by the amount
    // written and the amount left and sets new ones to the temp var
    add_bytes = write(outfile, buf + write_place, iter_bytes);
    // checks that the bytes are still being written
    if (add_bytes == 0) {
      // if no more bytes being written it returns the amount so far
      return bytes;
    }
    // adds the bytes written in the loop round to the total extern
    bytes_written += add_bytes;
    // adds the bytes written to the total for the call
    bytes += add_bytes;
    // adds the bytes to the index manipulator
    write_place += add_bytes;
  }
  // sets the index back to 0 for future runs
  write_place = 0;
  // returns the bytes written
  return bytes;
}
// creates the function for reading bits
bool read_bit(int infile, uint8_t *bit) {
  // creates the flag to be raised if either condition is met
  int flag = 0;
  // checks if the block needs to be filled
  if (bit_place == 0) {
    // raises flag for new block
    flag += 1;
  }
  // checks if the block is full
  if (bit_place == (BLOCK * 8)) {
    // raises flag for new block
    flag += 1;
  }
  // checks if any conditions for a new block were met
  if (flag > 0) {
    // loops through the buffer
    for (int i = 0; i < BLOCK; i += 1) {
      // sets each spot in the buffer to 0
      buff[i] = 0;
    }
    // sets the index to 0
    bit_place = 0;
    // creates the var to check that there are still new bytes coming in
    int check = 0;
    // calls read bytes to fill the buffer with new info
    check = read_bytes(infile, buff, BLOCK);
    // checks that there are new bytes
    if (check == 0) {
      // returns false if nothing new is being read
      return false;
    }
  }
  // gets the shift spots by using the index and the size of the values (uint8)
  uint64_t location = bit_place / 8;
  uint64_t position = bit_place % 8;
  // grabs the spots value
  uint64_t hold = buff[location];
  // or shifts at the spot
  hold |= 1UL << position;
  // checks if the new value is the same as the actual value
  if (hold == buff[location]) {
    // sets the bit to one if the bit was already set
    *bit = 1;
    // increases index
    bit_place = bit_place + 1 % (BLOCK * 8);
    // returns true for success
    return true;
  }
  // sets the bit to 0 if bit unset
  *bit = 0;
  // increases index
  bit_place = bit_place + 1 % (BLOCK * 8);
  // returns true for successful call
  return true;
}
// creates the function for the code writing
void write_code(int outfile, Code *c) {
  // makes a var to track if the end of the code is met
  uint32_t top = 0;
  // loops until a break is caught
  while (true) {
    // checks that the top has not been met
    if (top == c->top) {
      // breaks if at top
      break;
    }
    // gets the shift spots by using the index and the size of the uint8
    uint64_t location = flushing / 8;
    uint64_t position = flushing % 8;
    // checks if the bit bit at the top is equal to one
    if (code_get_bit(c, top) == 1) {
      // shifts the bit using or shift
      buff[location] |= 1UL << position;
      // increases the index by one
      flushing += 1;

    } else {
      // shifts the bit using and shift
      buff[location] &= ~(1UL << position);
      // ups index by one
      flushing += 1;
    }
    // checks if the buffer is at a spot in which it needs to write
    if (flushing % (BLOCK * 8) == 0) {
      // writes the bytes using the buffer
      write_bytes(outfile, buff, BLOCK);
      // loopsn through the buffer
      for (int i = 0; i < BLOCK; i += 1) {
        // resets each index to 0
        buff[i] = 0;
      }
      // resets my index
      flushing = 0;
    }
    // increases the top counter
    top += 1;
  }
}
// creates the function to flush leftover info
void flush_codes(int outfile) {
  // makes a bytes correcting var
  int fix = 0;
  // checks if the leftovers mod 8 are 0 or not
  if (flushing % 8 != 0) {
    // adds one to the fix if the leftovers are unbalanced
    fix += 1;
  }
  // writes the leftovers usinf the buffer and the size (with corrector)
  write_bytes(outfile, buff, ((flushing / 8) + fix));
}
