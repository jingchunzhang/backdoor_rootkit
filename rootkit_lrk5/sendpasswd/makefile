TARGET: send_pass

ALL= send_pass

OBJ= client.o get_ip_port.o

CFLAGS += -I./ -I../include 

send_pass:client.o get_ip_port.o
	gcc -I./ -I../include client.o get_ip_port.o -o send_pass 

clean:
	rm -rf $(ALL) $(OBJ)
