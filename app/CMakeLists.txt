add_executable(metal main.cpp)
target_compile_features(metal PRIVATE cxx_std_17)
target_link_libraries(metal PRIVATE MetalGPU_lib)

