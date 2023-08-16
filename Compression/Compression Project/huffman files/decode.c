#include "code.h"
#include "defines.h"
#include "header.h"
#include "heap.h"
#include "huffman.h"
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
#include <sys/stat.h>
#include <unistd.h>

// creates the options to be parsed
#define OPTIONS "hi:o:v"
// creates the main function
int main(int argc, char **argv) {
  // makes the var for the files, defaulting as stdin
  int infile = STDIN_FILENO;
  int outfile = STDOUT_FILENO;
  // creates the var to use as a flag if the stats option is picked
  int stats = 0;
  // var to parse through options
  int opt;
  // loops through the command line options
  while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
    // switches between the option's cases
    switch (opt) {
    // case h
    case 'h':
      // prints the help messages to stderr
      fprintf(stderr, "SYNOPSIS\n");
      fprintf(stderr, "  A Huffman decoder.\n");
      fprintf(stderr,
              "  Decompresses a file using the Huffman coding algorithm.\n\n");
      fprintf(stderr, "USAGE\n");
      fprintf(stderr, "  ./decode [-h] [-i infile] [-o outfile]\n\n");
      fprintf(stderr, "OPTIONS\n");
      fprintf(stderr, "  -h             Program usage and help.\n");
      fprintf(stderr, "  -v             Print compression statistics.\n");
      fprintf(stderr, "  -i infile      Input file to decompress.\n");
      fprintf(stderr, "  -o outfile     Output of decompressed data.\n");
      return 0;
    // case i
    case 'i':
      // opens the file passed in through optarg as read only
      infile = open(optarg, O_RDONLY);
      // if the return is -1 that means it failed and I will return
      if (infile == -1) {
        return 0;
      }
      // continues
      continue;
    // case o
    case 'o':
      // opens the outfile as write only
      outfile = open(optarg, O_WRONLY);
      // checks for success in opening
      if (outfile == -1) {
        // makes a file and opens if no file exists
        outfile = open(optarg, O_WRONLY | O_CREAT);
      }
      // continues
      continue;
    // case v
    case 'v':
      // flags the stat var and continues
      stats += 1;
      continue;
    // default case
    default:
      // prints the help options
      fprintf(stderr, "SYNOPSIS\n");
      fprintf(stderr, "  A Huffman decoder.\n");
      fprintf(stderr,
              "  Decompresses a file using the Huffman coding algorithm.\n\n");
      fprintf(stderr, "USAGE\n");
      fprintf(stderr, "  ./decode [-h] [-i infile] [-o outfile]\n\n");
      fprintf(stderr, "OPTIONS\n");
      fprintf(stderr, "  -h             Program usage and help.\n");
      fprintf(stderr, "  -v             Print compression statistics.\n");
      fprintf(stderr, "  -i infile      Input file to decompress.\n");
      fprintf(stderr, "  -o outfile     Output of decompressed data.\n");
      return 1;
    }
  }
  // makes a flag for the case in which stdin is used
  int std = 0;
  // checks if the file is in stdin
  if (infile == STDIN_FILENO) {
    // flags this situation
    std += 1;
  }
  // makes a buffer to read in the header with the proper size
  uint8_t *head = (uint8_t *)calloc(16, sizeof(uint8_t));
  // reads the 16 header bytes in from the infile to the header buffer
  read_bytes(infile, head, 16);
  // makes a buffer for the magic number portion
  uint8_t *buf = (uint8_t *)calloc(4, sizeof(uint8_t));
  // makes a var to hold the magic number
  uint32_t mag = 0;
  // copies the first 4 bytes from the header to the magic buffer
  memcpy(buf, head, 4);
  // copies the magic buffer info into the var for it
  memcpy(&mag, buf, 4);
  // frees the magic buffer
  free(buf);
  // checks that the magic numbers allign
  if (mag != MAGIC) {
    // prints helpful error message if the numbers do not allign
    fprintf(stderr, "Invalid file, mismatched magic number\n");
    // returns with error
    return 1;
  }
  // makes a buffer for the permissions portion
  uint8_t *buf1 = (uint8_t *)calloc(2, sizeof(uint8_t));
  // makes the var to hold the perms
  uint16_t perms = 0;
  // copies the next 2 bytes from the header to the perms buffer
  memcpy(buf1, head + 4, 2);
  // copies the info from the perms buffer to the var
  memcpy(&perms, buf1, 2);
  // frees the perm buffer
  free(buf1);
  // makes the buffer for the tree size
  uint8_t *buf2 = (uint8_t *)calloc(2, sizeof(uint8_t));
  // makes the var to hold the tree size
  uint16_t tree_size = 0;
  // copies the tree size (next 2 bytes) from header
  memcpy(buf2, head + 6, 2);
  // copies the info to the var
  memcpy(&tree_size, buf2, 2);
  // frees the buffer for the tree size
  free(buf2);
  // makes the buffer for the file size
  uint8_t *buf3 = (uint8_t *)calloc(8, sizeof(uint8_t));
  // makes the file size var
  uint64_t file_size = 0;
  // copies the file size (next 8 bytes) from header
  memcpy(buf3, head + 8, 8);
  // copies the info to the var
  memcpy(&file_size, buf3, 8);
  // frees the buffer for the file size
  free(buf3);
  // sets the file permissions for the outfile using the var I made
  fchmod(outfile, perms);
  // makes a buffer for the tree dump using the size var
  uint8_t *dump = (uint8_t *)calloc(tree_size, sizeof(uint8_t));
  // reads the tree dump in from the file
  read_bytes(infile, dump, tree_size);
  // makes a root node for next use
  Node *root;
  // calls rebuild tree function using the size of the tree and the buffer I
  // made sets the root to the returned node
  root = rebuild_tree(tree_size, dump);
  // frees the dump buffer
  free(dump);
  // makes a bit to use in the loop below
  uint8_t bit = 0;
  // makes a node to hold the current working node in the loop
  Node *node;
  // makes the current node the root node
  node = root;
  // makes a buffer to hold symbols in the loop that will be sent to be written
  uint8_t *sym = (uint8_t *)calloc(1, sizeof(uint8_t));
  // loops while the file still has bytes to be written
  while (file_size != bytes_written) {
    // reads in a bit and sets the bit passed in to be used below
    read_bit(infile, &bit);
    // checks the bits value after being set
    if (bit == 0) {
      // if the bit is 0 I will move to the nodes left child
      node = node->left;
    } else {
      // if the bit is 1 I will move to the nodes right child
      node = node->right;
    }
    // checks if the node is a leaf by checking that it has no chuldren
    if (node->right == NULL && node->left == NULL) {
      // sets the buffer to the symbol of the leaf node
      sym[0] = node->symbol;
      // writes the symbol into the file using the buffer and one byte
      write_bytes(outfile, sym, 1);
      // sets the node back to the top of the tree
      node = root;
    }
  }
  // frees the symbol buffer
  free(sym);
  // checks if the stat flag is active
  if (stats == 1) {
    // checks if the file is stdin or not
    if (std == 1) {
      // prints the stats for the file using the externs and the formula given
      fprintf(stderr, "Compressed file size: %lu bytes\n", bytes_read);
      fprintf(stderr, "Decompressed file size: %lu bytes\n", bytes_written);
      float percent = (100 * (1 - ((float)bytes_read / (float)bytes_written)));
      fprintf(stderr, "Space saving: %0.2f%%\n", percent);
    } else {
      // printfs the stats for the non stdin files using the same externs
      fprintf(stderr, "Compressed file size: %lu bytes\n", bytes_read);
      fprintf(stderr, "Decompressed file size: %lu bytes\n", bytes_written);
      float percent = (100 * (1 - ((float)bytes_read / (float)bytes_written)));
      fprintf(stderr, "Space saving: %0.2f%%\n", percent);
    }
  }
  // frees the header buffer
  free(head);
  // deletes the tree
  delete_tree(&root);
  // closes the files and returns
  close(infile);
  close(outfile);
  return 0;
}
