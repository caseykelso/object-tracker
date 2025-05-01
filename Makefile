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

run: .FORCE
	LD_LIBRARY_PATH=$(INSTALLED.HOST.DIR)/lib $(INSTALLED.HOST.DIR)/bin/tracker_rbr --input=$(DATA.DIR)/frames.json --output=$(BASE.DIR)/tracks.json --vis-dir=$(BASE.DIR)/docs

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


