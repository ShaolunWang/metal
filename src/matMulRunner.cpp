#include "matMulRunner.h"

#include "ctrack.hpp"
#include "fmt/format.h"

MatMulTileBase::MatMulTileBase(const std::string &fileName, MatMulConfig config)
    : MatMul(fileName, config) {

  m_gridSize = {(m_config.N + m_config.TILE_N - 1) / m_config.TILE_N, // columns
                (m_config.M + m_config.TILE_M - 1) / m_config.TILE_M, // rows
                1};
  m_threadgroupSize = {m_config.TILE_M, m_config.TILE_N, 1};
}

void MatMulTile::benchmark() {
  warmup();
  for (int i = 0; i < 100; i++) {
    auto [cmd, enc] = setup();
    run(cmd, enc,
        new ctrack::EventHandler{__builtin_LINE(), __builtin_FILE(), "tiled"});
  }
}
void MatMulTileBroadcast::benchmark() {
  warmup();
  for (int i = 0; i < 100; i++) {
    auto [cmd, enc] = setup();
    run(cmd, enc,
        new ctrack::EventHandler{__builtin_LINE(), __builtin_FILE(),
                                 "tiled_broadcast"});
  }
}
void MatMulNaive::benchmark() {
  warmup();
  for (int i = 0; i < 100; i++) {
    auto [cmd, enc] = setup();
    run(cmd, enc,
        new ctrack::EventHandler{__builtin_LINE(), __builtin_FILE(), "naive"});
  }
}
