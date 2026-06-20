#include "bm.h"
#include "ctrack.hpp"
#include "matMulRunner.h"

void Runner::run() {

  {
    auto m = MatMulNaive();
    auto m1 = MatMulTile();
    auto m2 = MatMulTileBroadcast();
    m.benchmark();
    m1.benchmark();
    m2.benchmark();
    ctrack::result_print();
  }
}
