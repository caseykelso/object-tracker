SHELL=/bin/bash

ifndef J_OVERRIDE
J=$(shell nproc --all)
else
J=$(J_OVERRIDE)
endif
$(info building with $(J) threads)

BASE.DIR=$(PWD)
BUILD.DIR=$(BASE.DIR)/build
SOURCE.DIR=$(BASE.DIR)/source
DOWNLOADS.DIR=$(BASE.DIR)/downloads
SCRIPTS.DIR=$(BASE.DIR)/scripts
ifndef INSTALLED_HOST_DIR
INSTALLED.HOST.DIR=$(BASE.DIR)/installed.host
else
INSTALLED.HOST.DIR=$(INSTALLED_HOST_DIR)
endif

ci: init build

build: .FORCE
	mkdir -p $(BUILD.DIR)

init: .FORCE
	mkdir -p $(DOWNLOADS.DIR)
	mkdir -p $(INSTALLED.HOST.DIR)

clean: .FORCE
	rm -rf $(DOWNLOADS.DIR) && rm -rf $(INSTALLED.HOST.DIR) && rm -rf $(BUILD.DIR) && rm -f $(BASE.DIR)/tags

ctags: .FORCE
	cd $(BASE.DIR) && ctags -R --exclude=.git --exclude=downloads --exclude=installed.host --exclude=installed.target --exclude=documents  --exclude=build.*  .

.FORCE:


