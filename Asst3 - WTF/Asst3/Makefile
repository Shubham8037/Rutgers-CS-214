all: first second
first: wtfclient.c
	-gcc wtfclient.c -o WTF -lpthread -lm
second: wtfserver.c
	-gcc wtfserver.c -o WTFserver -lpthread -lm 
clean:
	rm -rf WTF WTFserver
