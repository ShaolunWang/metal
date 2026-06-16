# file(GLOB HEADER_LIST CONFIGURE_DEPENDS 
# 	"${MetalGPU_SOURCE_DIR}/external/**/*.hpp"
# )
# message("headers ${HEADER_LIST}")

add_library(MetalGPU_lib vecAdd.cpp #[[ ${HEADER_LIST} ]])
target_link_libraries(MetalGPU_lib 
	PRIVATE 
	fmt::fmt 
	"-framework Metal"
    "-framework Foundation"
    "-framework QuartzCore"
    "-framework Cocoa"
)
target_include_directories(MetalGPU_lib 
	PUBLIC 
	../include 
	"${MetalGPU_SOURCE_DIR}/external/"

)

target_compile_features(MetalGPU_lib PUBLIC cxx_std_17)
