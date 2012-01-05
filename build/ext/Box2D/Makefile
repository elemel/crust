INSTALL_DIR = ../../../ext/Box2D
SRC_DIR = Box2D_v2.2.1
SRC_ZIP = $(SRC_DIR).zip

install.made: build.made
	mkdir -p $(INSTALL_DIR)/include $(INSTALL_DIR)/lib
	(cd $(SRC_DIR) && tar cf - `find Box2D -name "*.h" -print`) | \
		(cd $(INSTALL_DIR)/include && tar xBf -)
	cp $(SRC_DIR)/Build/gmake/bin/Release/libBox2D.a $(INSTALL_DIR)/lib
	touch install.made

build.made: configure.made
	cd $(SRC_DIR)/Build/gmake && make Box2D config=release
	touch build.made

configure.made: extract.made
	cd $(SRC_DIR) && premake4 gmake
	touch configure.made

extract.made: download.made
	rm -fr $(SRC_DIR)
	unzip -q $(SRC_ZIP)
	touch extract.made

download.made:
	rm -f $(SRC_ZIP)
	curl -O http://box2d.googlecode.com/files/$(SRC_ZIP)
	touch download.made

clean:
	rm -f *.made $(SRC_ZIP)
	rm -fr $(SRC_DIR)
