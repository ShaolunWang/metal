#include <metal_stdlib>
using namespace metal;

kernel void vecAdd(
	device const float* inA,
	device const float* inB	,
	device float* result,
	//device const float size N
	uint3 gid [[thread_position_in_grid]],
	uint3 lid [[thread_position_in_threadgroup]]
) {

	// naive
	// 	for (int i = 0; i < N;i++) {
	//  	result[i] = A[i] + B[i]
	// }

	int offset = lid.x << 2; // load 4 elements
	// SM memory
	float4 vec0 = *((device const float4*)(inA + offset));
	// SM memory
	float4 vec1 = *((device const float4*)(inB + offset));
	*(device float4*)(result + offset) =  float4((vec0.x + vec1.x), (vec0.y + vec1.y), (vec0.z + vec1.z), (vec0.w + vec0.w));
}
