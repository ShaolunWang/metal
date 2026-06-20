#pragma once

#include "bufferGen.h"
#include "matMul.h"
#include <string>
class MatMulNaive : public MatMul {
public:
  explicit MatMulNaive(MatMulConfig config = {}) : MatMul("matMul", config) {};

  void benchmark() override;
};

class MatMulTileBase : public MatMul {
public:
  MatMulTileBase(const std::string &fileName, MatMulConfig config = {});
  void run(NS::SharedPtr<MTL::CommandBuffer> cmd,
           NS::SharedPtr<MTL::ComputeCommandEncoder> enc,
           ctrack::EventHandler *h) override;
};
class MatMulTile : public MatMulTileBase {
public:
  explicit MatMulTile(MatMulConfig config = {})
      : MatMulTileBase("matMul_tiling_prototype", config) {};

  void benchmark() override;
};
class MatMulTileBroadcast : public MatMulTileBase {
public:
  explicit MatMulTileBroadcast(MatMulConfig config = {})
      : MatMulTileBase("matMul_tiling_broadcast", config) {};

  void benchmark() override;
};
