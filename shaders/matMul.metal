#include <metal_stdlib>
using namespace metal;

kernel void matMul(
	device const float* inA [[buffer(0)]],
	device const float* inB	[[buffer(1)]],
	device float* result    [[buffer(2)]],

	constant int& M [[buffer(3)]],
	constant int& N [[buffer(4)]],
	constant int& K [[buffer(5)]],
	uint3 gid [[thread_position_in_grid]],
	uint3 lid [[thread_position_in_threadgroup]]
) {
	int row = gid.y;
	int col = gid.x;
    if (row >= M || col >= N) return;
	float accumulator = 0.0f;
	// Matrix A: M * K
	// Matrix B: K * N
	for (int k = 0; k < K; k++){
		// row * K -> say row * K gives [row,  k] of A
		// i.e. A[row][k]

		// since Matrix K is K * N, this is effectively
		// doing the same thing as A, but we are accessing it
		// by col: B[k][col]
		//
	    // NOTE: acc = \sum_{K}^{k = 0} A[row][k] * [k][col]
		
		// The accumulator basically says that we should 
		//
		thread_local float = load A[row * K + k]; 
		// ^---- this gets shared on the fixed k across all threads

		accumulator += inA[row * K + k] * inB[k * N + col];

	}


	// col is based on N value
	// effectively this is saying result[row][col] = accumulator
    result[row * N + col] = accumulator;
}
