CYGWIN=$(findstring CYGWIN,$(shell uname -a))

ifeq "$(CYGWIN)" ""
  EXT=
else
  EXT=.exe
  CFLAGS+=-D_WIN32
endif

PROGRAMS=$(patsubst %.c,%$(EXT),$(wildcard *.c))
BINDIR=/usr/local/bin
MANDIR=/usr/share/man/man1

all: $(PROGRAMS)

clean:
	make -C Docs clean
	rm -f *~ */*~
	rm -f $(patsubst %.c,%.o,$(wildcard Modules/*.c))
	rm -f $(patsubst %.c,%$(EXT),$(wildcard *.c))

needroot:
	@if [ ! $$UID -eq 0 ]; then \
	  echo You must be root to do this.; \
	  exit 1; \
	fi

install: all needroot
	make -C Docs all
	install -g root -o root -m 644 Docs/lnpdump.1.gz $(MANDIR)
	install -g root -o root -m 755 lnpdump$(EXT) $(BINDIR)

remove: needroot
	rm -f $(MANDIR)/lnpdump.1.gz
	rm -f $(BINDIR)/lnpdump$(EXT)

lnpdump$(EXT): lnpdump.c Modules/getopt.o Modules/lnphost.o
	$(CC) $(CFLAGS) -Wall -IInclude -o $@ $+ -lpthread

Modules/%.o: Modules/%.c Include/%.h
	$(CC) $(CFLAGS) -Wall -IInclude -c -o $@ $<


posrecv$(EXT): posrecv.c Modules/getopt.o Modules/lnphost.o
	$(CC) $(CFLAGS) -Wall -IInclude -o $@ $+ -lpthread

