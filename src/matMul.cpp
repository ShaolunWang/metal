#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#include "matMul.h"
#include "assert.h"
#include "fmt/base.h"
#include "spdlog/spdlog.h"

MatMul::MatMul() {
  m_device = NS::TransferPtr(MTL::CreateSystemDefaultDevice());
  if (!m_device) {
    fmt::report_error("no metal device found.");
    exit(-1);
  }

  m_queue = NS::TransferPtr(m_device->newCommandQueue());
  auto *libPath = NS::String::string("matMul_tile_broadcast.metallib",
                                     NS::ASCIIStringEncoding);

  m_library = NS::TransferPtr(m_device->newLibrary(libPath, &m_error));
  if (!m_library) {
    fmt::report_error("failed to find matmul metal lib.");
    exit(-1);
  }
  auto *funcName = NS::String::string("matMul", NS::ASCIIStringEncoding);
  m_function = NS::TransferPtr(m_library->newFunction(funcName));

  m_pso = NS::TransferPtr(
      m_device->newComputePipelineState(m_function.get(), &m_error));
}

void MatMul::run() {

  m_cmd = NS::TransferPtr(m_queue->commandBuffer());
  m_enc = NS::TransferPtr(m_cmd->computeCommandEncoder());

  constexpr int M = 1024, N = 1024, K = 1024;
  constexpr size_t sizeA = M * K * sizeof(float);
  constexpr size_t sizeB = K * N * sizeof(float);
  constexpr size_t sizeC = M * N * sizeof(float);

  spdlog::info("M = {}, N = {}, K = {}", M, N, K);

  if (m_device.get() == nullptr) {
    exit(-1);
  }
  auto A = NS::TransferPtr(
      m_device->newBuffer(sizeA, MTL::ResourceStorageModeShared));
  auto B = NS::TransferPtr(
      m_device->newBuffer(sizeB, MTL::ResourceStorageModeShared));
  auto result = NS::TransferPtr(
      m_device->newBuffer(sizeC, MTL::ResourceStorageModeShared));
  float *a_ptr = static_cast<float *>(A->contents());
  float *b_ptr = static_cast<float *>(B->contents());

  for (size_t i = 0; i < 1024 * 1024; i++) {
    a_ptr[i] = static_cast<float>(rand()) / RAND_MAX;
    b_ptr[i] = static_cast<float>(rand()) / RAND_MAX;
  }

  m_enc->setComputePipelineState(m_pso.get());
  m_enc->setBuffer(A.get(), 0, 0);
  m_enc->setBuffer(B.get(), 0, 1);
  m_enc->setBuffer(result.get(), 0, 2);
  m_enc->setBytes(&M, sizeof(int), 3);
  m_enc->setBytes(&N, sizeof(int), 4);
  m_enc->setBytes(&K, sizeof(int), 5);

  // set them to arbitrary values for now
  constexpr int TILE_M = 8;
  constexpr int TILE_N = 8;
  constexpr int TILE_K = 8;

  spdlog::info("TILE_M = {}, TILE_N = {}, TILE_K = {}", TILE_M, TILE_N, TILE_K);

  MTL::Size gridSize{(N + TILE_N - 1) / TILE_N, // columns
                     (M + TILE_M - 1) / TILE_M, // rows
                     1};

  spdlog::info("Grid size: Width = {}, Height = {}, Depth = {}", gridSize.width,
               gridSize.height, gridSize.depth);

  MTL::Size threadgroupSize(TILE_M, TILE_N, 1);
  spdlog::info("threadgroupSize size: width = {}, height = {} depth = {}",
               threadgroupSize.width, threadgroupSize.height,
               threadgroupSize.depth);

  m_enc->dispatchThreads(gridSize, threadgroupSize);

  m_enc->endEncoding();
  m_cmd->commit();
  m_cmd->waitUntilCompleted();

  float *pr = static_cast<float *>(result->contents());

  // for (int r = 0; r < M; r++) {
  //   for (int c = 0; c < N; c++) {
  //     fmt::print("{:8.2f} ", pr[r * N + c]);
  //   }
  //   fmt::print("\n");
  // }
}
