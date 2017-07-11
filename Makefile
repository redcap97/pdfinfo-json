.PHONY: all clean install
.NOTPARALLEL: check-poppler-version

target = pdfinfo-json
objects = main.o JSInfo.o ImageListDev.o parseargs.o is_utf8.o
dependencies = $(objects:%.o=%.d)
compatible_poppler_version = 0.56.0

CPPFLAGS = -g -O2 -MD -MP $$(pkg-config poppler --cflags) -Irapidjson/include
CXXFLAGS = -std=c++11
LDFLAGS = $$(pkg-config poppler --libs)

ifdef PREFIX
	export PKG_CONFIG_PATH := $(PREFIX)/lib/pkgconfig:$(PKG_CONFIG_PATH)
	LDFLAGS += -Wl,-rpath,$(PREFIX)/lib
else
	PREFIX = /usr/local
endif

all: check-poppler-version $(target)

test: all
	ruby test/test.rb

check-poppler-version:
	@echo checking whether version of poppler is $(compatible_poppler_version)...
	@pkg-config poppler --exact-version $(compatible_poppler_version)

update-test: all
	test/update-test

install: all
	install -d $(PREFIX)/bin
	install -m 755 $(target) $(PREFIX)/bin

clean:
	rm -f $(target) $(objects) $(dependencies)

$(target): $(objects)
	$(CXX) -o $@ $^ $(LDFLAGS)

-include $(dependencies)
