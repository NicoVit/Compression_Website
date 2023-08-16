#pragma once
#ifndef __HEAP_H__
#define __HEAP_H__
#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include "node.h"
void swap(Node *one, Node *two);
int l_child(int x);
int r_child(int x);
int parent(int x);
void up_heap(Node **arr, int n);
void down_heap(Node **arr, int heap_size);
#endif
