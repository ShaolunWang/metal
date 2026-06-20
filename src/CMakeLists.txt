add_library(MetalGPU_lib vecAdd.cpp matMul.cpp matMulRunner.cpp bm.cpp)
target_link_libraries(MetalGPU_lib 
	PRIVATE 
	fmt::fmt 
	spdlog
	ctrack
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
# making ctrack happy
target_compile_definitions(MetalGPU_lib PRIVATE CTRACK_DISABLE_EXECUTION_POLICY)
