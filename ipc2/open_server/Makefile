ROOT=../..
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

ifeq "$(PLATFORM)" "solaris"
  EXTRALIBS=-lsocket -lnsl
endif

PROGS =	open_server

all:	$(PROGS)

daemonize.o:  daemonize.h
server_unix_sock.o: server_unix_sock.h
buf_args.o: buf_args.h
wrtien.o:  writen.h

handle_request.o : handle_request.h

open_server:	main.o server_unix_sock.o daemonize.o loop.o  handle_request.o client_management.o buf_args.o  writen.o 
	$(CC) $(CFLAGS) -o open_server main.o server_unix_sock.o daemonize.o loop.o handle_request.o client_management.o buf_args.o   writen.o 	

clean:
	rm -f $(PROGS) $(TEMPFILES) *.o

include $(ROOT)/Make.libapue.inc
