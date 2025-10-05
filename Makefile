.PHONY: build

all: setup cmake

setup:
	git submodule update --init --recursive

cmake:
	cmake -B build -S . -G "Unix Makefiles" ../

c: bc rc

s: bs rs

bc:
	make -C build Client

bs:
	make -C build Server

rc:
	./bin/Client

rs:
	./bin/Server
