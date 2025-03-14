all: build/Makefile
	$(MAKE) -C build ip

clean:
	rm -rf build

build/Makefile:
	mkdir build
	cd build && cmake .. -DEN_RDMA=1 -DFPGA_PART=xcu55c-fsvh2892-2L-e || rmdir build


.PHONY: all clean
