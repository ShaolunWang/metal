
#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#include "matMul.h"
#include "ctrack.hpp"
#include "fmt/format.h"
#include <string>

MatMul::MatMul(const std::string &fileName, MatMulConfig config)
    : m_metalfileName{fileName}, m_config{config} {
  m_device = NS::TransferPtr(MTL::CreateSystemDefaultDevice());
  if (!m_device) {
    fmt::report_error("no metal device found.");
    exit(-1);
  }

  m_queue = NS::TransferPtr(m_device->newCommandQueue());
  const std::string name = fileName + ".metallib";
  auto *libPath = NS::String::string(name.c_str(), NS::ASCIIStringEncoding);

  m_library = NS::TransferPtr(m_device->newLibrary(libPath, &m_error));
  if (!m_library) {
    fmt::report_error("failed to find matmul metal lib.");
    exit(-1);
  }
  auto *funcName = NS::String::string("matMul", NS::ASCIIStringEncoding);
  m_function = NS::TransferPtr(m_library->newFunction(funcName));

  m_pso = NS::TransferPtr(
      m_device->newComputePipelineState(m_function.get(), &m_error));
  m_gridSize = {m_config.N, m_config.M, 1};
  // NOTE: this is purely for scheduling purposes
  m_threadgroupSize = {m_config.TILE_M, m_config.TILE_N, 1};

  A = NS::TransferPtr(
      m_device->newBuffer(m_config.sizeA, MTL::ResourceStorageModeShared));
  B = NS::TransferPtr(
      m_device->newBuffer(m_config.sizeB, MTL::ResourceStorageModeShared));
  result = NS::TransferPtr(
      m_device->newBuffer(m_config.sizeC, MTL::ResourceStorageModeShared));

  if (m_device.get() == nullptr) {
    exit(-1);
  }
  const auto &buffers = BufferGen::instance();
  std::memcpy(A->contents(), buffers.A(), m_config.sizeA);
  std::memcpy(B->contents(), buffers.B(), m_config.sizeB);
}

std::pair<NS::SharedPtr<MTL::CommandBuffer>,
          NS::SharedPtr<MTL::ComputeCommandEncoder>>
MatMul::setup() {
  auto cmd = NS::TransferPtr(m_queue->commandBuffer());
  auto enc = NS::TransferPtr(cmd->computeCommandEncoder());

  enc->setComputePipelineState(m_pso.get());
  enc->setBuffer(A.get(), 0, 0);
  enc->setBuffer(B.get(), 0, 1);
  enc->setBuffer(result.get(), 0, 2);
  enc->setBytes(&m_config.M, sizeof(int), 3);
  enc->setBytes(&m_config.N, sizeof(int), 4);
  enc->setBytes(&m_config.K, sizeof(int), 5);
  return {cmd, enc};
}

void MatMul::run(NS::SharedPtr<MTL::CommandBuffer> cmd,
                 NS::SharedPtr<MTL::ComputeCommandEncoder> enc,
                 ctrack::EventHandler *h) {
  // set them to arbitrary values for now

  enc->dispatchThreads(m_gridSize, m_threadgroupSize);
  enc->endEncoding();
  {
    cmd->commit();
    cmd->waitUntilCompleted();
    delete h;
  }
}
void MatMul::warmup() {
  for (int i = 0; i < 10; i++) {
    auto cmd = NS::TransferPtr(m_queue->commandBuffer());
    auto enc = NS::TransferPtr(cmd->computeCommandEncoder());
    enc->setComputePipelineState(m_pso.get());
    enc->dispatchThreads(m_gridSize, m_threadgroupSize);
    enc->endEncoding();
    cmd->commit();
    cmd->waitUntilCompleted();
  }
}

void MatMul::benchmark() {
  warmup();
  for (int i = 0; i < 100; i++) {
    auto [cmd, enc] = setup();
    run(cmd, enc,
        new ctrack::EventHandler{__builtin_LINE(), __builtin_FILE(), ""});
  }
}
