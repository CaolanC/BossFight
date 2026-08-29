gui:
	cmake --build build --target GUI
	./bin/GUI

guid:
	cmake -DCMAKE_BUILD_TYPE=Debug --build build --target GUI
	gdb ./bin/GUI
