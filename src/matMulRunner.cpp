#include "matMulRunner.h"

#include "ctrack.hpp"
#include "fmt/format.h"

MatMulTileBase::MatMulTileBase(const std::string &fileName, MatMulConfig config)
    : MatMul(fileName, config) {

  m_gridSize = {(m_config.N + m_config.TILE_N - 1) / m_config.TILE_N, // columns
                (m_config.M + m_config.TILE_M - 1) / m_config.TILE_M, // rows
                1};
  m_threadgroupSize = {m_config.TILE_N, m_config.TILE_M, 1};
}
void MatMulTileBase::run(NS::SharedPtr<MTL::CommandBuffer> cmd,
                         NS::SharedPtr<MTL::ComputeCommandEncoder> enc,
                         ctrack::EventHandler *h) {
  // set them to arbitrary values for now

  enc->dispatchThreadgroups(m_gridSize, m_threadgroupSize);
  enc->endEncoding();
  {
    cmd->commit();
    cmd->waitUntilCompleted();
    delete h;
  }
  // verify();
}
void MatMulTile::benchmark() {
  warmup();
  for (int i = 0; i < 100; i++) {
    auto [cmd, enc] = setup();
    run(cmd, enc,
        new ctrack::EventHandler{__builtin_LINE(), __builtin_FILE(), "tiled_prototype"});
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

// void MatMulTile::benchmark() {
//   warmup();
//   for (int i = 0; i < 1; i++) {
//     auto [cmd, enc] = setup();
//     run(cmd, enc,
//         new ctrack::EventHandler{__builtin_LINE(), __builtin_FILE(),
//         "tiled"});
//   }
// }

void MatMulNaive::benchmark() {
  warmup();
  for (int i = 0; i < 100; i++) {
    auto [cmd, enc] = setup();
    run(cmd, enc,
        new ctrack::EventHandler{__builtin_LINE(), __builtin_FILE(), "naive"});
  }
}
