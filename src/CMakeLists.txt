add_library(MetalGPU_lib vecAdd.cpp matMul.cpp)
target_link_libraries(MetalGPU_lib 
	PRIVATE 
	fmt::fmt 
	spdlog
	"-framework Metal"
    "-framework Foundation"
    "-framework QuartzCore"
    "-framework Cocoa"
)
target_include_directories(MetalGPU_lib 
	PUBLIC 
	"${MetalGPU_SOURCE_DIR}/include"
	"${MetalGPU_SOURCE_DIR}/external/"

)

target_compile_features(MetalGPU_lib PUBLIC cxx_std_17)
