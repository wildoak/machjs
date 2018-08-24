.PHONY: install clean build

ifneq ($(RELEASE), 1)
    BUILD_TYPE = Debug
else
    BUILD_TYPE = Release
endif

build: bin/$(BUILD_TYPE)/Makefile
	cd bin/$(BUILD_TYPE) \
	&& make

bin/$(BUILD_TYPE)/Makefile: bin/$(BUILD_TYPE) CMakeLists.txt
	(cd bin/$(BUILD_TYPE) && cmake -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) ../..)

bin:
	mkdir bin

bin/$(BUILD_TYPE): bin
	mkdir bin/$(BUILD_TYPE)

clean:
	rm -rf bin