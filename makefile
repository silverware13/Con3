all :
	gcc -std=c99 -pthread -o con3_p2 con3_p2.c
	gcc -std=c99 -pthread -o con3_p1 con3_p1.c

clean:
	rm con3_p2
	rm con3_p1
