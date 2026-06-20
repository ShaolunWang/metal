#include <metal_stdlib>
using namespace metal;

kernel void matMul(
    device const float* A [[buffer(0)]],
    device const float* B [[buffer(1)]],
    device float* C [[buffer(2)]],

    constant int& M [[buffer(3)]],
    constant int& N [[buffer(4)]],
    constant int& K [[buffer(5)]],

    uint2 gid [[threadgroup_position_in_grid]],
    uint2 tid [[thread_position_in_threadgroup]]
) {
    constexpr int TILE_M = 16;
    constexpr int TILE_N = 16;
    constexpr int TILE_K = 16;

    threadgroup float As[TILE_M][TILE_K];
    threadgroup float Bs[TILE_K][TILE_N];

    int row = gid.y * TILE_M + tid.y;
    int col = gid.x * TILE_N + tid.x;

    float acc = 0.0f;

    for (int k0 = 0; k0 < K; k0 += TILE_K) {

        int kA = k0 + tid.x; // A loads vary along K
        int kB = k0 + tid.y; // B loads vary along K

        As[tid.y][tid.x] = (row < M && kA < K) ? A[row * K + kA] : 0.0f;
        Bs[tid.y][tid.x] = (kB < K && col < N) ? B[kB * N + col] : 0.0f;

        threadgroup_barrier(mem_flags::mem_threadgroup);

        for (int k = 0; k < TILE_K; k++) {
            acc += As[tid.y][k] * Bs[k][tid.x];
        }

        threadgroup_barrier(mem_flags::mem_threadgroup);
    }

    if (row < M && col < N) {
        C[row * N + col] = acc;
    }
}
