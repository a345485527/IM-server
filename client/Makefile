# The makefile of client program

objects=main_client.o client_handler.o process_protocol.o

client : $(objects)
	g++ -o client $(objects) -lpthread -g

main_client.o : main_client.cpp protocol.h client_handler.h process_protocol.h
	g++ -c main_client.cpp -g

client_handler.o : client_handler.cpp client_handler.h process_protocol.h protocol.h
	g++ -c client_handler.cpp -g

process_protocol.o : process_protocol.cpp process_protocol.h protocol.h client_handler.h
	g++ -c process_protocol.cpp -g

clean_objects : 
	rm -f $(objects)

clean:
	rm -f client

all : client clean_objects
