####
## Makefile for openpst/readmbn
####

all: default

default:
	if [ ! -d "./build" ]; then mkdir -p build;  fi
	if [ ! -d "./lib/libopenpst/include" ]; then git submodule init && git submodule update;  fi
	$(CXX) -I./lib/libopenpst/include -I./lib/tclap/include -I./lib/openssl/include -I./src -std=gnu++11 $(CXX_FLAGS) \
		./src/mbn_parser.cpp ./src/main.cpp -o build/readmbn 
clean:
	rm -rf build/*