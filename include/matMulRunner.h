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
};
class MatMulTile : public MatMulTileBase {
public:
  explicit MatMulTile(MatMulConfig config = {})
      : MatMulTileBase("matMul_tiling", config) {};

  void benchmark() override;
};
class MatMulTileBroadcast : public MatMulTileBase {
public:
  explicit MatMulTileBroadcast(MatMulConfig config = {})
      : MatMulTileBase("matMul_tile_broadcast", config) {};

  void benchmark() override;
};
