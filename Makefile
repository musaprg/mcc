mcc: main.c
	gcc main.c -o mcc

test: mcc
	sh test.sh

clean:
	rm -f mcc tmp*
