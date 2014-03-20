# The makefile of server program

objects=main_server.o server_handler.o process_protocol.o

server : $(objects)
	g++ -o server $(objects) -lpthread -g

main_server.o : main_server.cpp protocol.h server_handler.h process_protocol.h
	g++ -c main_server.cpp -g

server_handler.o : server_handler.cpp server_handler.h process_protocol.h protocol.h
	g++ -c server_handler.cpp -g

process_protocol.o : process_protocol.cpp process_protocol.h protocol.h server_handler.h
	g++ -c process_protocol.cpp -g

clean_objects : 
	rm -f $(objects)

clean:
	rm -f server

all : server clean_objects
