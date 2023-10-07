CC=g++ 
CFLAGS=  -O3 -Wall  -DHAVE_CONFIG_H -DBUILD_DIVSUFSORT64=1
libdivsufsort.a:divsufsort.o sssort.o trsort.o utils.o
	ar rc libdivsufsort.a divsufsort.o sssort.o trsort.o utils.o

%.o:%.c *.h
	$(CC) -c $(CFLAGS) $< -o $@
clean:
	rm *.a *.o