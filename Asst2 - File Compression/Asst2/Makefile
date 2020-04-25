all: fileCompressor.c huffman.o tokenizer.o
	gcc -fsanitize=address -o fileCompressor fileCompressor.c huffman.o tokenizer.o

huffman.o: huffman.c
	gcc -c huffman.c

tokenizer.o: tokenizer.c
	gcc -c tokenizer.c

clean:
	rm -f huffman.o
	rm -f tokenizer.o
	rm -f fileCompressor
	rm -f HuffmanCodebook

