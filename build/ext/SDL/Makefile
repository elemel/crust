ROOT_DIR = $(shell cd ../../.. && pwd)

install.made: build.made
	cd SDL && make install
	touch install.made

build.made: clean.made
	cd SDL && make
	touch build.made

clean.made: configure.made
	cd SDL && make clean
	touch clean.made

configure.made: clone.made
	cd SDL && ./configure --prefix=$(ROOT_DIR)/ext/SDL
	touch configure.made

clone.made:
	rm -fr SDL
	hg clone http://hg.libsdl.org/SDL SDL
	touch clone.made

clean:
	rm -f *.made
	rm -fr SDL
