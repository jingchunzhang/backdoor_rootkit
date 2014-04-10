TARGET: fix clearlog

ALL= fix clearlog

CFLAGS += -DLINUX 

fix:fix.o
	gcc -DLINUX -I./ fix.o -o fix -lutil
clearlog: clearlog.o 
	gcc -DLINUX -I./ clearlog.o -o clearlog -lutil

clean:
	rm -rf $(ALL) $(OBJ) *.o
