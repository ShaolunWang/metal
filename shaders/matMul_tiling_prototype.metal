#include <metal_stdlib>
using namespace metal;

kernel void matMul(
	device const float* A [[buffer(0)]],
	device const float* B	[[buffer(1)]],
	device float* result    [[buffer(2)]],

	constant int& M [[buffer(3)]],
	constant int& N [[buffer(4)]],
	constant int& K [[buffer(5)]],
	uint2 gid [[threadgroup_position_in_grid]],
	uint2 tid [[thread_position_in_threadgroup]]
) {
	constexpr int TILE_M = 8;
	constexpr int TILE_N = 8;

	// this is to coordinate where our tile is at
	// since we define how many tiles are we having
	// using the grid

	int tileRowPos = gid.y;
	int tileColPos = gid.x;

	// now we calculate which row we are actually at with the above values
	// we are using threadpos to specify where we are 
	// within a tile
	int row = tileRowPos * TILE_M + tid.y;
	int col = tileColPos * TILE_N + tid.x;

	// same optional for edges
    if (row >= M || col >= N) return;

	float accumulator = 0.0f;
	// Matrix A: M * K
	// Matrix B: K * N
	// still the same reduction since we are only doing tiling right now
	for (int k = 0; k < K; k++) {
        accumulator += A[row * K + k] * B[k * N + col];
    }


	// col is based on N value
	// effectively this is saying result[row][col] = accumulator
    result[row * N + col] = accumulator;
}
