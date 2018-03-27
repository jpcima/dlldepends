
PREFIX ?= /usr/local

all:
	$(CXX) $(CXXFLAGS) -o dlldepends dlldepends.cc `llvm-config --cppflags --libs`

clean:
	rm -f dlldepends

install: all
	install -D -m 755 dlldepends $(DESTDIR)$(PREFIX)/bin/dlldepends

.PHONY: all clean install
