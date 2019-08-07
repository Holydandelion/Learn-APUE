ROOT=..
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

ifeq "$(PLATFORM)" "freebsd"
  GETPW = getpw44bsd
  FMEM = fmemopen.o
  EXTRALIBS=-pthread
endif
ifeq "$(PLATFORM)" "linux"
  GETPW = getpwsvr4
  FMEM =
  EXTRALIBS=-pthread
endif
ifeq "$(PLATFORM)" "macos"
  GETPW = getpw44bsd
  FMEM = fmemopen.o
endif
ifeq "$(PLATFORM)" "solaris"
  GETPW = getpwsvr4
  FMEM =
endif

PROGS =	bo fifo1 getlogin goodexit longpath pendlock prtime sizepipe vfork3 zombie test_15.12 test_15.15 test_15.18  test_16.1 test_16.2 test_16.3 test_16.4 test_16.5

all:	$(PROGS) asyncsocket.o $(FMEM) openmax.o sleep.o sleepus_poll.o sleepus_select.o $(GETPW)

%:	%.c $(LIBAPUE)
	$(CC) $(CFLAGS) $@.c -o $@ $(LDFLAGS) $(LDLIBS)

test_16.3:	test_16.3.o  $(LIBAPUE) my_daemonize.o initsrv2.o 
		$(CC) $(CFLAGS) -o test_16.3 test_16.3.o $(LDFLAGS) $(LDLIBS) my_daemonize.o initsrv2.o 

test_16.4:	test_16.4.o  $(LIBAPUE) my_daemonize.o initsrv2.o 
		$(CC) $(CFLAGS) -o test_16.4 test_16.4.o $(LDFLAGS) $(LDLIBS) my_daemonize.o initsrv2.o 

test_16.5:	test_16.5.o  $(LIBAPUE) my_daemonize.o  
		$(CC) $(CFLAGS) -o test_16.5 test_16.5.o $(LDFLAGS) $(LDLIBS) my_daemonize.o 

clean:
	rm -f $(PROGS) $(TEMPFILES) *.o $(GETPW)

include $(ROOT)/Make.libapue.inc
