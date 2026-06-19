#pragma once

#include "Foundation/Foundation.hpp"
#include "Metal/Metal.hpp"
#include "QuartzCore/QuartzCore.hpp"

class MatMul {
public:
  MatMul();
  void run();

private:
  NS::SharedPtr<MTL::Device> m_device;
  NS::SharedPtr<MTL::Library> m_library;
  NS::SharedPtr<MTL::ComputePipelineState> m_pso;
  NS::SharedPtr<MTL::CommandQueue> m_queue;
  NS::SharedPtr<MTL::Function> m_function;
  NS::SharedPtr<MTL::ComputeCommandEncoder> m_enc;
  NS::SharedPtr<MTL::CommandBuffer> m_cmd;
  NS::Error *m_error{nullptr};
};
