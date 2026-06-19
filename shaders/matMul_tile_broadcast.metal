#include <metal_stdlib>
using namespace metal;

kernel void matMul(
	device const float* A [[buffer(0)]],
	device const float* B	[[buffer(1)]],
	device float* result    [[buffer(2)]],

	constant int& M [[buffer(3)]],
	constant int& N [[buffer(4)]],
	constant int& K [[buffer(5)]],
	uint3 gid [[threadgroup_position_in_grid]],
	uint3 tid [[thread_position_in_threadgroup]]
) {
	constexpr int TILE_M = 8;
	constexpr int TILE_N = 8;
	constexpr int TILE_K = 8;

	// declare two submatrices
	threadgroup float ASubMtx[TILE_M][TILE_K];
	threadgroup float BSubMtx[TILE_K][TILE_N];

	int tileRowPos = gid.y;
	int tileColPos = gid.x;

	int row = tileRowPos * TILE_M + tid.y;
	int col = tileColPos * TILE_N + tid.x;

	// same optional for edges

	float accumulator = 0.0f;

	// NOTE: we change how the loop work here
	// this looks very similar to loop unrolling
	
	for (int k = 0; k < K; k += TILE_K) {
		// first step we want to load the values into thread_local
		// scratchpad memory
		ASubMtx[tid.y][tid.x] = A[row * K + (k + tid.x)];
		BSubMtx[tid.y][tid.x] = B[(k + tid.y) * N + col];

		// setup mem barrier so we cannot continue until everything 
		// is loaded
		threadgroup_barrier(mem_flags::mem_threadgroup);
		// now we can compute with the broadcasted tiles 
		for (int k0 = 0; k0 < TILE_K;k0++){
			accumulator += ASubMtx[tid.y][k0] * BSubMtx[k0][tid.x];
		}
		// now we want ALL threads to finish computing before we continue
        threadgroup_barrier(mem_flags::mem_threadgroup);
    }
	
    if (row < M && col < N) {

    	result[row * N + col] = accumulator;
	}



}
