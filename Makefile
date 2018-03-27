
PREFIX ?= /usr/local

all: dlldepends

dlldepends: dlldepends.cc
	$(CXX) $(CXXFLAGS) -o $@ $^ `llvm-config --cppflags --libs`

dlldepends.static: dlldepends.cc
	$(CXX) $(CXXFLAGS) -o $@ $^ `llvm-config --link-static --libs --system-libs object`

clean:
	rm -f dlldepends dlldepends.static

install: all
	install -D -m 755 dlldepends $(DESTDIR)$(PREFIX)/bin/dlldepends

.PHONY: all clean install
