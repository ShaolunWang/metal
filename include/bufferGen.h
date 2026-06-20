#pragma once

#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>

struct MatMulConfig {
  static constexpr size_t M = 1024;
  static constexpr size_t N = 1024;
  static constexpr size_t K = 1024;

  static constexpr size_t sizeA = M * K * sizeof(float);
  static constexpr size_t sizeB = K * N * sizeof(float);
  static constexpr size_t sizeC = M * N * sizeof(float);

  static constexpr size_t TILE_M = 16;
  static constexpr size_t TILE_N = 16;
  static constexpr size_t TILE_K = 16;
};

class BufferGen {
public:
  static BufferGen &instance() {
    static BufferGen instance;
    return instance;
  }

  const float *A() const { return m_a.data(); }
  const float *B() const { return m_b.data(); }

  void regenerate(uint32_t seed = 42) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    for (auto &v : m_a) {
      v = dist(rng);
    }

    for (auto &v : m_b) {
      v = dist(rng);
    }
  }

private:
  BufferGen()
      : m_a(MatMulConfig::M * MatMulConfig::K),
        m_b(MatMulConfig::K * MatMulConfig::N) {
    regenerate();
  }

  BufferGen(const BufferGen &) = delete;
  BufferGen &operator=(const BufferGen &) = delete;
  BufferGen(BufferGen &&) = delete;
  BufferGen &operator=(BufferGen &&) = delete;

private:
  std::vector<float> m_a;
  std::vector<float> m_b;
};
