.PHONY: all clean install

target = pdfinfo-json
objects = main.o JSInfo.o parseargs.o is_utf8.o
dependencies = $(objects:%.o=%.d)

PREFIX = /usr/local

CPPFLAGS = -g -O2 -MD -MP $$(pkg-config poppler --cflags) -Irapidjson/include
LDFLAGS = $$(pkg-config poppler --libs)

all: $(target)

install:
	install -d $(PREFIX)/bin
	install -m 755 $(target) $(PREFIX)/bin

clean:
	rm -f $(target) $(objects) $(dependencies)

$(target): $(objects)
	$(CXX) -o $@ $^ $(LDFLAGS)

-include $(dependencies)
