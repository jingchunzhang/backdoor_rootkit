TARGET: ktpage1 ktpage0

ALL= ktpage1 ktpage0

CFLAGS += -DLINUX 

ktpage1:ktpage1.o
	gcc -DLINUX -I./ ktpage1.o -o ktpage1 -lutil
ktpage0: ktpage0.o 
	gcc -DLINUX -I./ ktpage0.o -o ktpage0 -lutil

clean:
	rm -rf $(ALL) $(OBJ) *.o
