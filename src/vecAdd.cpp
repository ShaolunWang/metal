#include "Foundation/Foundation.hpp"
#include "Metal/Metal.hpp"
#include "fmt/format.h"
#include <cstdio>
#include <cstdlib>

int run() {
  constexpr size_t arrayLength = 1 << 20;
  const size_t bufferSize = arrayLength * sizeof(float);
  NS::SharedPtr<MTL::Device> device =
      NS::TransferPtr(MTL::CreateSystemDefaultDevice());
  if (!device) {
    fmt::report_error("no metal device found.");
    return -1;
  }

  auto queue = NS::TransferPtr(device->newCommandQueue());
  NS::Error *error = nullptr;
  auto libPath =
      NS::String::string("default.metallib", NS::ASCIIStringEncoding);
  NS::SharedPtr<MTL::Library> library =
      NS::TransferPtr(device->newLibrary(libPath, &error));
  if (!library) {
    fmt::report_error("failed to find default metal lib.");
    return -1;
  }
  auto funcName = NS::String::string("vecAdd", NS::ASCIIStringEncoding);
  auto function = NS::TransferPtr(library->newFunction(funcName));
  NS::SharedPtr<MTL::ComputePipelineState> pso =
      NS::TransferPtr(device->newComputePipelineState(function.get(), &error));
  NS::SharedPtr<MTL::Buffer> a = NS::TransferPtr(
      device->newBuffer(bufferSize, MTL::ResourceStorageModeShared));
  NS::SharedPtr<MTL::Buffer> b = NS::TransferPtr(
      device->newBuffer(bufferSize, MTL::ResourceStorageModeShared));
  NS::SharedPtr<MTL::Buffer> result = NS::TransferPtr(
      device->newBuffer(bufferSize, MTL::ResourceStorageModeShared));

  float *pa = static_cast<float *>(a->contents());
  float *pb = static_cast<float *>(b->contents());

  // init some values
  for (size_t i = 0; i < arrayLength; i++) {
    pa[i] = static_cast<float>(rand()) / RAND_MAX;
    pb[i] = static_cast<float>(rand()) / RAND_MAX;
  }
  NS::SharedPtr<MTL::CommandBuffer> cmdBuffer =
      NS::TransferPtr(queue->commandBuffer());
  NS::SharedPtr<MTL::ComputeCommandEncoder> enc =
      NS::TransferPtr(cmdBuffer->computeCommandEncoder());

  enc->setComputePipelineState(pso.get());

  enc->setBuffer(a.get(), 0, 0);
  enc->setBuffer(b.get(), 0, 1);
  enc->setBuffer(result.get(), 0, 2);
  MTL::Size gridSize(arrayLength, 1, 1);
  size_t maxThreads = pso->maxTotalThreadsPerThreadgroup();
  if (maxThreads > arrayLength) {
    maxThreads = arrayLength;
  }

  MTL::Size threads(maxThreads, 1, 1);
  enc->dispatchThreads(gridSize, threads);

  enc->endEncoding();
  cmdBuffer->commit();
  cmdBuffer->waitUntilCompleted();

  float *pr = static_cast<float *>(result->contents());

  for (size_t i = 0; i < 10; i++) {
    printf("%f + %f = %f\n", pa[i], pb[i], pr[i]);
  }
  return 0;
}
