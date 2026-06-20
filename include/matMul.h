#pragma once

#include "Foundation/Foundation.hpp"
#include "Metal/Metal.hpp"
#include "QuartzCore/QuartzCore.hpp"
#include "bufferGen.h"
#include "ctrack.hpp"
#include <string>

class MatMul {
public:
  MatMul(const std::string &fileName, MatMulConfig config = {});
  virtual void run(NS::SharedPtr<MTL::CommandBuffer>,
                   NS::SharedPtr<MTL::ComputeCommandEncoder>,
                   ctrack::EventHandler *h = new ctrack::EventHandler());
  virtual void benchmark();
  void warmup();
  std::string m_metalfileName;

protected:
  NS::SharedPtr<MTL::Device> m_device;
  NS::SharedPtr<MTL::Library> m_library;
  NS::SharedPtr<MTL::ComputePipelineState> m_pso;
  NS::SharedPtr<MTL::CommandQueue> m_queue;
  NS::SharedPtr<MTL::Function> m_function;
  NS::SharedPtr<MTL::Buffer> A;
  NS::SharedPtr<MTL::Buffer> B;
  NS::SharedPtr<MTL::Buffer> result;
  NS::Error *m_error{nullptr};

  MatMulConfig m_config;

  MTL::Size m_gridSize{};

  MTL::Size m_threadgroupSize;

  virtual std::pair<NS::SharedPtr<MTL::CommandBuffer>,
                    NS::SharedPtr<MTL::ComputeCommandEncoder>>
  setup();
};
