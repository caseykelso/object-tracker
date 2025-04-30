SHELL=/bin/bash

ifndef J_OVERRIDE
J=$(shell nproc --all)
else
J=$(J_OVERRIDE)
endif
$(info building with $(J) threads)

BASE.DIR=$(PWD)
DATA.DIR=$(BASE.DIR)/data
BUILD.APP.DIR=$(BASE.DIR)/build
BUILD.TESTS.DIR=$(BASE.DIR)/build.tests
SOURCE.DIR=$(BASE.DIR)/source
TESTS.DIR=$(BASE.DIR)/tests
DOWNLOADS.DIR=$(BASE.DIR)/downloads
SCRIPTS.DIR=$(BASE.DIR)/scripts
MUNKRES.VERSION=1.0.0
MUNKRES.ARCHIVE=v$(MUNKRES.VERSION).tar.gz
MUNKRES.URL=https://github.com/saebyn/munkres-cpp/archive/refs/tags/$(MUNKRES.ARCHIVE)
MUNKRES.URL=https://github.com/saebyn/munkres-cpp/archive/refs/tags/$(MUNKRES.ARCHIVE)
MUNKRES.BUILD=$(DOWNLOADS.DIR)/build.munkres
MUNKRES.DIR=$(DOWNLOADS.DIR)/munkres-cpp-$(MUNKRES.VERSION)
ifndef INSTALLED_HOST_DIR
INSTALLED.HOST.DIR=$(BASE.DIR)/installed.host
else
INSTALLED.HOST.DIR=$(INSTALLED_HOST_DIR)
endif

ci: init build

build: .FORCE
	mkdir -p $(BUILD.APP.DIR)
	cd $(BUILD.APP.DIR) && cmake -DCMAKE_INSTALL_PREFIX=$(INSTALLED.HOST.DIR) -DCMAKE_PREFIX_PATH=$(INSTALLED.HOST.DIR) $(SOURCE.DIR) && make -j$(J) install

init: .FORCE
	mkdir -p $(DOWNLOADS.DIR)
	mkdir -p $(INSTALLED.HOST.DIR)
	$(MAKE) munkres

munkres: .FORCE
	rm -rf $(MUNKRES.BUILD) && mkdir -p $(MUNKRES.BUILD)
	cd $(DOWNLOADS.DIR) && rm -f $(MUNKRES.ARCHIVE) && wget $(MUNKRES.URL) && tar xvf $(MUNKRES.ARCHIVE)
	patch -p0 < munkres.patch $(MUNKRES.DIR)/CMakeLists.txt
	patch -p0 < munkres_adapters.patch $(MUNKRES.DIR)/src/adapters/CMakeLists.txt
	cd $(MUNKRES.BUILD) && cmake -DBOOST_MATRIX_ADAPTER=ON -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=$(INSTALLED.HOST.DIR) -DCMAKE_PREFIX_PATH=$(INSTALLED.HOST.DIR) $(MUNKRES.DIR) && make -j$(J) install
	cp $(MUNKRES.DIR)/src/adapters/boostmatrixadapter.h $(INSTALLED.HOST.DIR)/include/munkres/adapters # TODO: patch the CMakeLists.txt further and avoid this janky copy
	cp $(MUNKRES.DIR)/src/adapters/boostmatrixadapter.h $(INSTALLED.HOST.DIR)/include/munkres/adapters


run: .FORCE
	LD_LIBRARY_PATH=$(INSTALLED.HOST.DIR)/lib $(INSTALLED.HOST.DIR)/bin/tracker_rbr --input=$(DATA.DIR)/frames.json --output=$(BASE.DIR)/tracks.json

viz: build
	$(INSTALLED.HOST.DIR)/bin/viz

tests: .FORCE
	mkdir -p $(BUILD.TESTS.DIR)
	cd $(BUILD.TESTS.DIR) && cmake -DCMAKE_INSTALL_PREFIX=$(INSTALLED.HOST.DIR) -DCMAKE_PREFIX_PATH=$(INSTALLED.HOST.DIR) $(TESTS.DIR) && make -j$(J) install
	LD_LIBRARY_PATH=$(INSTALLED.HOST.DIR)/lib $(INSTALLED.HOST.DIR)/bin/tracker_rbr_tests

version: .FORCE
	$(INSTALLED.HOST.DIR)/bin/tracker_rbr --version

help: .FORCE
	$(INSTALLED.HOST.DIR)/bin/tracker_rbr --help

clean: .FORCE
	rm -rf $(DOWNLOADS.DIR) && rm -rf $(INSTALLED.HOST.DIR) && rm -rf $(BUILD.APP.DIR) && rm -f $(BASE.DIR)/tags && rm -rf $(BUILD.TESTS.DIR)

ctags: .FORCE
	cd $(BASE.DIR) && ctags -R --exclude=.git --exclude=downloads --exclude=installed.host --exclude=installed.target --exclude=documents  --exclude=build.*  .

clangformat: .FORCE
	clang-format -i source/*.h source/*.cpp --style=Microsoft
.FORCE:


