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
// defines the options used in parsing below
#define OPTIONS "hi:o:v"

// creates the main function
int main(int argc, char **argv) {
  // sets the files using stdin and out as default
  int infile = STDIN_FILENO;
  int outfile = STDOUT_FILENO;
  // creates the stats flag for use later
  int stats = 0;
  // creates the opt var to parse through options
  int opt;
  // loops while there are still options
  while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
    // switches to the option passed
    switch (opt) {
    // case h
    case 'h':
      // prints the help message for the file
      fprintf(stderr, "SYNOPSIS\n");
      fprintf(stderr, "  A Huffman encoder.\n");
      fprintf(stderr,
              "  Compresses a file using the Huffman coding algorithm.\n\n");
      fprintf(stderr, "USAGE\n");
      fprintf(stderr, "  ./encode [-h] [-i infile] [-o outfile]\n\n");
      fprintf(stderr, "OPTIONS\n");
      fprintf(stderr, "  -h             Program usage and help.\n");
      fprintf(stderr, "  -v             Print compression statistics.\n");
      fprintf(stderr, "  -i infile      Input file to compress.\n");
      fprintf(stderr, "  -o outfile     Output of compressed data.\n");
      return 0;
    // case i
    case 'i':
      // opens the file with the the name passed in
      infile = open(optarg, O_RDONLY);
      // checks that the file is opened successfully
      if (infile == -1) {
        // returns if the file isnt valid
        return 0;
      }
      // continues
      continue;
    // case o
    case 'o':
      // opens the outfile with the name passed in
      outfile = open(optarg, O_WRONLY);
      // checks if the file opened properly
      if (outfile == -1) {
        // if the file isnt real it will make one
        outfile = open(optarg, O_WRONLY | O_CREAT);
      }
      // continues
      continue;
    // case v
    case 'v':
      // flags the stats if need be
      stats += 1;
      continue;
    // default case
    default:
      // prints the help message
      fprintf(stderr, "SYNOPSIS\n");
      fprintf(stderr, "  A Huffman encoder.\n");
      fprintf(stderr,
              "  Compresses a file using the Huffman coding algorithm.\n\n");
      fprintf(stderr, "USAGE\n");
      fprintf(stderr, "  ./encode [-h] [-i infile] [-o outfile]\n\n");
      fprintf(stderr, "OPTIONS\n");
      fprintf(stderr, "  -h             Program usage and help.\n");
      fprintf(stderr, "  -v             Print compression statistics.\n");
      fprintf(stderr, "  -i infile      Input file to compress.\n");
      fprintf(stderr, "  -o outfile     Output of compressed data.\n");
      return 1;
    }
  }
  // makes a var to show it is still stdin
  int std = 0;
  // makes a buffer for bytes read
  uint8_t buf[BLOCK];
  // makes a histogram for the bytes
  uint64_t hist[ALPHABET];
  // makes the infile temp for the mkstemp function
  char in_file[] = "/tmp/tmp.myfile.XXXXXX";
  // opens temp file for the stdin info to be held in
  int temp = mkstemp(in_file);
  // loops while there are still bytes to be read
  while (read_bytes(infile, buf, 1) != 0) {
    // goes to the location of the buffer info in the histogram and adds one to
    // the value
    hist[buf[0]] += 1;
    // checks if the file is stdin
    if (infile == STDIN_FILENO) {
      // if it is stdin it will write the bytes to the temp file
      int check = write_bytes(temp, buf, 1);
      // checks that write bytes worked
      if (check == 0) {
        // breaks if need be
        break;
      }
    }
    // resets the buffer index to 0
    buf[0] = 0;
  }
  // if the file is stdin it will enter the statment
  if (infile == STDIN_FILENO) {
    // flags the stdin use
    std += 1;
    // makes the infile the written temp file
    infile = temp;
    // sets the file stream to the start of the file
    lseek(infile, 0, SEEK_SET);
  }
  // clears the entire buffer incase of not zero by looping through
  // and setting each index to 0
  for (int i = 0; i < BLOCK; i += 1) {
    buf[i] = 0;
  }
  // checks if the index 0 of the histogram is 0
  if (hist[0] == 0) {
    // sets the index 0 to 1
    hist[0] = 1;
  }
  // checks if the index 1 of the histogram is 0
  if (hist[1] == 0) {
    // sets the first index to 1
    hist[1] = 1;
  }
  // makes the node to hold the root
  Node *root;
  // calls build tree using the histogram and sets the root node with the return
  root = build_tree(hist);
  // creates the array for codes
  Code code[ALPHABET];
  // loops through the array
  for (int i = 0; i < ALPHABET; i += 1) {
    // sets the index to actual code object
    code[i] = code_init();
  }
  // builds the codes using the function i made, the root node, and the code
  // array
  build_codes(root, code);
  // makes the header object
  Header head;
  // sets the magic number to the defined one
  head.magic = MAGIC;
  // creates the stat object
  struct stat perm;
  // gets the information about the file and sets perm
  fstat(infile, &perm);
  // sets the header permission field using the stat object
  head.permissions = perm.st_mode;
  // sets the actual file perms using the stat object
  fchmod(outfile, perm.st_mode);
  // creates a var to hold the amount of uniques
  int count = 0;
  // loops through the hist
  for (int i = 0; i < ALPHABET; i += 1) {
    // checks if the index value is not 0
    if (hist[i] != 0) {
      // adds one to the counter
      count += 1;
    }
  }
  // sets the tree size field using the uniques and the formula given
  head.tree_size = ((count * 3) - 1);
  // sets the file size field using the stats object
  head.file_size = perm.st_size;
  // casts and flattens the header into uint8 arr
  uint8_t *tempor = (uint8_t *)&head;
  // writes the header into the file
  write_bytes(outfile, tempor, 16);
  // calls dump tree using the root i got earlier and the file
  dump_tree(outfile, root);
  // sets the file stream to 0
  lseek(infile, 0, SEEK_SET);
  // makes the var to check outputs from read bytes
  int out = -1;
  // sets the extern to zero so it only counts the actual file read
  bytes_read = 0;
  // loops infinitely
  while (true) {
    // reads a bytes into the buf from the file
    // and stores the return as out
    out = read_bytes(infile, buf, 1);
    // checks that out read something
    if (out == 1) {
      // writes the code for the bytes
      write_code(outfile, &code[buf[0]]);
      // continues in loop
      continue;
    }
    // breaks if the case was not met
    break;
  }
  // flushes the remaining codes into the outfile
  flush_codes(outfile);
  // checks if the stats flag was raised
  if (stats == 1) {
    // checks if the file was stdin
    if (std == 1) {
      // prints the stats using the externs and the formula
      fprintf(stderr, "Uncompressed file size: %lu bytes\n", bytes_read);
      bytes_written -= bytes_read;
      fprintf(stderr, "Compressed file size: %lu bytes\n", bytes_written);
      float percent = (100 * (1 - ((float)bytes_written / (float)bytes_read)));
      fprintf(stderr, "Space saving: %0.2f%%\n", percent);
    } else {
      // prints the stats for the normal file using the same
      fprintf(stderr, "Uncompressed file size: %lu bytes\n", bytes_read);
      fprintf(stderr, "Compressed file size: %lu bytes\n", bytes_written);
      float percent = (100 * (1 - ((float)bytes_written / (float)bytes_read)));
      fprintf(stderr, "Space saving: %0.2f%%\n", percent);
    }
  }
  // checks if the file is stdin
  if (STDIN_FILENO == infile) {
    // unlinks the temp
    unlink(in_file);
  }
  // closes all files
  close(infile);
  close(outfile);
  // deletes the tree and returns
  delete_tree(&root);
  return 0;
}
