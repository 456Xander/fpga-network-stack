FPGA_PART ?= xcu55c-fsvh2892-2L-e

all: build/Makefile
	$(MAKE) -C build ip

clean:
	rm -rf build

build/Makefile:
	@echo "Building FPGA Network Stack for ${FPGA_PART}"
	mkdir build
	cd build && cmake .. -DEN_RDMA=1 -DFPGA_PART=${FPGA_PART} || rmdir build


.PHONY: all clean
