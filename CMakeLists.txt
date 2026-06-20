cmake_minimum_required(VERSION 3.20)
project(MetalGPU LANGUAGES CXX OBJCXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_OBJCXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
if (CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
	add_compile_options(-g)
endif()
if(CMAKE_BUILD_TYPE MATCHES Debug)
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)
	add_compile_definitions(Debug)
	add_compile_options(-fsanitize=address -g)
	add_link_options(-fsanitize=address)
endif()


set(DISABLE_EXAMPLES ON CACHE BOOL "" FORCE)
include(FetchContent)

FetchContent_Declare(
  fmtlib
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 12.1.0
  GIT_SHALLOW TRUE
)
FetchContent_Declare(
	spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
	GIT_TAG "v1.17.0"
)

FetchContent_Declare(
	ctrack
    GIT_REPOSITORY https://github.com/compaile/ctrack.git
	GIT_TAG "v1.1.0"
)

FetchContent_MakeAvailable(fmtlib spdlog ctrack)

add_subdirectory(src)
add_subdirectory(app)
