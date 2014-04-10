OBJ=d3des.o exdes.o rkcrypt.o util_ip.o mailfile.o

TARGET: librkcrypt.a test de

ALL= librkcrypt.a test de

CFLAGS += -I./ -I../include 

librkcrypt.a:$(OBJ)
	ar rcu $@ $(OBJ)

test:sendmail.o
	gcc -I./ -I../include sendmail.o -o test -L./ -lrkcrypt
de:de.o
	gcc -I./ -I../include de.o -o de -L./ -lrkcrypt

clean:
	rm -rf $(ALL) $(OBJ) *.o
