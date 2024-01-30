#pragma once

#include <random>

template <typename T>
T GetRandom(T lower, T upper) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<T> distribution(lower, upper);
  return distribution(gen);
}
