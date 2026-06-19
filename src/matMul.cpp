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
  auto *libPath =
      NS::String::string("matMul.metallib", NS::ASCIIStringEncoding);

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
  constexpr int M = 8, N = 8, K = 8;
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

  float tempA[64] = {
      3.14f,  -1.27f, 8.91f,  0.43f,  -5.62f, 7.18f,  2.05f,  -9.33f,
      -4.77f, 6.54f,  -2.88f, 1.96f,  9.12f,  -7.41f, 5.23f,  0.67f,
      8.45f,  -3.19f, 4.28f,  -6.75f, 1.34f,  2.87f,  -8.56f, 7.02f,
      -1.11f, 9.77f,  -5.44f, 3.68f,  -2.22f, 6.39f,  0.95f,  -4.81f,
      7.73f,  -8.02f, 1.57f,  5.91f,  -3.46f, 4.04f,  -6.63f, 2.29f,
      -9.14f, 0.88f,  7.36f,  -1.95f, 8.24f,  -5.17f, 3.03f,  6.71f,
      2.48f,  -4.93f, 9.05f,  -7.28f, 0.52f,  1.83f,  -3.75f, 8.67f,
      -6.16f, 5.49f,  -0.74f, 2.97f,  -8.38f, 7.84f,  4.11f,  -1.53f};

  float tempB[64] = {
      -7.25f, 2.61f,  -4.87f, 8.03f,  -1.49f, 5.72f,  -9.18f, 3.44f,
      6.92f,  -8.56f, 1.33f,  -2.78f, 7.15f,  -4.01f, 0.66f,  -5.89f,
      -3.27f, 9.41f,  -6.04f, 2.17f,  -8.72f, 1.58f,  4.93f,  -7.36f,
      5.14f,  -0.82f, 8.69f,  -3.55f, 6.28f,  -1.96f, 7.47f,  2.03f,
      -9.77f, 4.25f,  -2.11f, 5.83f,  -7.64f, 3.09f,  -0.57f, 8.88f,
      1.72f,  -6.39f, 7.94f,  -4.68f, 2.54f,  -8.13f, 5.31f,  -1.27f,
      8.46f,  -3.73f, 0.91f,  -9.05f, 4.62f,  6.17f,  -2.48f, 7.79f,
      -5.32f, 1.84f,  -7.58f, 3.26f,  -0.99f, 8.51f,  -4.35f, 6.08f};

  std::memcpy(a_ptr, tempA, sizeof(tempA));
  std::memcpy(b_ptr, tempB, sizeof(tempB));
  // NS::SharedPtr<MTL::Buffer> MBuf = NS::TransferPtr(
  //     m_device->newBuffer(&M, sizeof(int), MTL::ResourceStorageModeShared));
  // NS::SharedPtr<MTL::Buffer> NBuf = NS::TransferPtr(
  //     m_device->newBuffer(&N, sizeof(int), MTL::ResourceStorageModeShared));
  // NS::SharedPtr<MTL::Buffer> KBuf = NS::TransferPtr(
  //     m_device->newBuffer(&K, sizeof(int), MTL::ResourceStorageModeShared));

  m_enc->setComputePipelineState(m_pso.get());
  m_enc->setBuffer(A.get(), 0, 0);
  m_enc->setBuffer(B.get(), 0, 1);
  m_enc->setBuffer(result.get(), 0, 2);
  m_enc->setBytes(&M, sizeof(int), 3);
  m_enc->setBytes(&N, sizeof(int), 4);
  m_enc->setBytes(&K, sizeof(int), 5);

  MTL::Size gridSize(N, M, 1);
  spdlog::info("grid size: width = {}, height = {} depth = {}", gridSize.width,
               gridSize.height, gridSize.depth);
  MTL::Size threadgroupSize(8, 8, 1);
  spdlog::info("threadgroupSize size: width = {}, height = {} depth = {}",
               threadgroupSize.width, threadgroupSize.height,
               threadgroupSize.depth);

  m_enc->dispatchThreads(gridSize, threadgroupSize);

  m_enc->endEncoding();
  m_cmd->commit();
  m_cmd->waitUntilCompleted();

  float *pr = static_cast<float *>(result->contents());

  for (int r = 0; r < M; r++) {
    for (int c = 0; c < N; c++) {
      fmt::print("{:8.2f} ", pr[r * N + c]);
    }
    fmt::print("\n");
  }
}
