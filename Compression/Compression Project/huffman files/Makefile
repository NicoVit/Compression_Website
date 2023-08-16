CC = clang
CFLAGS = -Wall -Wpedantic -Werror -Wextra -gdwarf-4 -Ofast -g
.PHONY: clean spotless all
all: encode decode
encode: node.o heap.o pq.o code.o stack.o io.o huffman.o encode.o
	$(CC) -o $@ $^
decode: node.o heap.o pq.o code.o stack.o io.o huffman.o decode.o
	$(CC) -o $@ $^
%.o: %.c
	$(CC) $(CFLAGS) -c $<
clean: 
	rm -f *.o
spotless:
	rm -f encode decode *.o
format:
	clang-format -i -style=file *.[c]
scan-build: clean
	scan-build --use-cc=$(CC) make
