.PHONY: conan cmake gui guid dump clean fresh

conan:
	conan install . --output-folder=build --build=missing

cmake: 
	cmake -B build -S . \
		-DCMAKE_TOOLCHAIN_FILE=build/build/Release/generators/conan_toolchain.cmake \
		-DCMAKE_PREFIX_PATH=build \
		-DCMAKE_BUILD_TYPE=Release

gui: 
	cmake --build build --target GUI
	./bin/GUI

guid:
	gdb ./bin/GUI

dump:
	rm -rf build bin/GUI bin/Server

fresh: dump conan cmake
