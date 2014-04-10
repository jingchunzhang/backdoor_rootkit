TARGET: ktpage1_r ktpage0_r

ALL= ktpage1_r ktpage0_r

CFLAGS += -DLINUX 

ktpage1_r:ktpage1_r.o
	gcc -DLINUX -I./ ktpage1_r.o -o ktpage1_r -lutil
ktpage0_r: ktpage0_r.o 
	gcc -DLINUX -I./ ktpage0_r.o -o ktpage0_r -lutil

clean:
	rm -rf $(ALL) $(OBJ) *.o
