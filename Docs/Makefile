MANPAGES=$(wildcard *.1)

all: $(patsubst %,%.gz,$(MANPAGES))

clean:
	rm -f $(patsubst %.1,%.html,$(MANPAGES))
	rm -f $(patsubst %.1,%.ps,$(MANPAGES))
	rm -f $(patsubst %.1,%.pdf,$(MANPAGES))
	rm -f $(patsubst %.1,%.1.gz,$(MANPAGES))
	rm -f *~

%.html: %.1
	groff -man -Thtml -P -l $< > $@

%.ps: %.1
	groff -man $< | psnup -2 > $@

%.pdf: %.ps
	ps2pdf $<

%.gz: %
	gzip -9c $< > $@

