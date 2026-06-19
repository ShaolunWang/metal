conf:
	cmake -B build_debug -D CMAKE_BUILD_TYPE=Debug -G Ninja
debug:
	cmake --build build_debug
xcode:
	cmake -B build_xcode -D CMAKE_BUILD_TYPE=Debug -G Xcode
metal target:
	xcrun -sdk macosx metal -c shaders/{{target}}.metal -o {{target}}.air && xcrun -sdk macosx metallib {{target}}.air -o {{target}}.metallib
