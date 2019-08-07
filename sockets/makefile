ROOT=..
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

ifeq "$(PLATFORM)" "solaris"
  EXTRALIBS = -lsocket -lnsl
endif

PROGS = ruptime ruptimed ruptimed-fd ruptimed-dg ruptimed-reuse
MOREPROGS = findsvc ruptime-dg

all:	$(PROGS) $(MOREPROGS) clconn.o clconn2.o initsrv1.o initsrv2.o my_daemonize.o

%:	%.c $(LIBAPUE)

ruptime:	ruptime.o  $(LIBAPUE) 
		$(CC) $(CFLAGS) -o ruptime ruptime.o  $(LDFLAGS) $(LDLIBS) 

ruptimed:	ruptimed.o initsrv2.o $(LIBAPUE) my_daemonize.o
		$(CC) $(CFLAGS) -o ruptimed ruptimed.o initsrv2.o $(LDFLAGS) $(LDLIBS) my_daemonize.o

ruptimed-fd:	ruptimed-fd.o initsrv2.o $(LIBAPUE)
		$(CC) $(CFLAGS) -o ruptimed-fd ruptimed-fd.o initsrv2.o $(LDFLAGS) $(LDLIBS)

ruptimed-dg:	ruptimed-dg.o initsrv2.o $(LIBAPUE)
		$(CC) $(CFLAGS) -o ruptimed-dg ruptimed-dg.o initsrv2.o $(LDFLAGS) $(LDLIBS)

ruptimed-reuse:	ruptimed-reuse.o  $(LIBAPUE) my_daemonize.o 
		$(CC) $(CFLAGS) -o ruptimed-reuse ruptimed-reuse.o $(LDFLAGS) $(LDLIBS) my_daemonize.o

clean:
	rm -f $(PROGS) $(MOREPROGS) $(TEMPFILES) *.o

include $(ROOT)/Make.libapue.inc
