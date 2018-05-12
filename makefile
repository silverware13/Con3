all :
	gcc -std=c99 -pthread -o concurrency3 concurrency3.c

clean:
	rm concurrency3
