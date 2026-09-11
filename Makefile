.PHONY: conan conand cmake cmaked gui guid dump fresh

# Install Release dependencies
conan:
	conan install . --output-folder=build -s build_type=Release --build=missing

# Install Debug dependencies
conand:
	conan install . --output-folder=build -s build_type=Debug --build=missing

# Configure CMake for Release
cmake: conan
	cmake -B build -S . \
		-DCMAKE_TOOLCHAIN_FILE=build/build/Release/generators/conan_toolchain.cmake \
		-DCMAKE_BUILD_TYPE=Release

# Configure CMake for Debug
cmaked: conand
	cmake -B build -S . \
		-DCMAKE_TOOLCHAIN_FILE=build/build/Debug/generators/conan_toolchain.cmake \
		-DCMAKE_BUILD_TYPE=Debug

gui:
	cmake --build build --target GUI
	./bin/GUI

guid: cmaked
	cmake --build build --target GUI
	gdb ./bin/GUI

dump:
	rm -rf build bin/GUI bin/Server

fresh: dump cmake
