#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include "../Foundation/Foundation.hpp"
#include "../Metal/Metal.hpp"
#include "../QuartzCore/QuartzCore.hpp"
#include "fmt/format.h"
#include <cstdio>
#include <cstdlib>

int run() {
  constexpr size_t arrayLength = 1 << 20;
  const size_t bufferSize = arrayLength * sizeof(float);
  MTL::Device *device = MTL::CreateSystemDefaultDevice();
  if (!device) {
    fmt::report_error("no metal device found.");
    return -1;
  }
  MTL::CommandQueue *queue = device->newCommandQueue();
  NS::Error *error = nullptr;
  auto libPath =
      NS::String::string("default.metallib", NS::ASCIIStringEncoding);
  MTL::Library *library = device->newLibrary(libPath, &error);
  if (!library) {
    fmt::report_error("failed to find default metal lib.");
    return -1;
  }
  auto funcName = NS::String::string("vecAdd", NS::ASCIIStringEncoding);
  MTL::Function *function = library->newFunction(funcName);
  MTL::ComputePipelineState *pso =
      device->newComputePipelineState(function, &error);
  MTL::Buffer *a =
      device->newBuffer(bufferSize, MTL::ResourceStorageModeShared);
  MTL::Buffer *b =
      device->newBuffer(bufferSize, MTL::ResourceStorageModeShared);
  MTL::Buffer *result =
      device->newBuffer(bufferSize, MTL::ResourceStorageModeShared);

  float *pa = static_cast<float *>(a->contents());
  float *pb = static_cast<float *>(b->contents());

  // init some values
  for (size_t i = 0; i < arrayLength; i++) {
    pa[i] = (float)rand() / RAND_MAX;
    pb[i] = (float)rand() / RAND_MAX;
  }
  MTL::CommandBuffer *cmdBuffer = queue->commandBuffer();
  MTL::ComputeCommandEncoder *enc = cmdBuffer->computeCommandEncoder();

  enc->setComputePipelineState(pso);

  enc->setBuffer(a, 0, 0);
  enc->setBuffer(b, 0, 1);
  enc->setBuffer(result, 0, 2);
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
