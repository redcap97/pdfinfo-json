.PHONY: all clean install

target = pdfinfo-json
objects = main.o JSInfo.o parseargs.o is_utf8.o
dependencies = $(objects:%.o=%.d)

CPPFLAGS = -g -O2 -MD -MP $$(pkg-config poppler --cflags) -Irapidjson/include
LDFLAGS = $$(pkg-config poppler --libs)

ifdef PREFIX
	export PKG_CONFIG_PATH := $(PREFIX)/lib/pkgconfig:$(PKG_CONFIG_PATH)
	LDFLAGS += -Wl,-rpath,$(PREFIX)/lib
else
	PREFIX = /usr/local
endif

all: $(target)

test: all
	ruby test/test.rb

install: all
	install -d $(PREFIX)/bin
	install -m 755 $(target) $(PREFIX)/bin

clean:
	rm -f $(target) $(objects) $(dependencies)

$(target): $(objects)
	$(CXX) -o $@ $^ $(LDFLAGS)

-include $(dependencies)
