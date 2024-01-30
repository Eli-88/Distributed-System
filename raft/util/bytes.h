#pragma once

#include <cassert>
#include <span>

#include "concepts.h"

namespace util {

template <typename T>
std::span<std::byte> ConvertToBytes(T* buffer, std::size_t length) {
  return std::span<std::byte>(reinterpret_cast<std::byte*>(buffer),
                              sizeof(T) * length);
}

template <ContainerConcept Container>
std::span<std::byte> ConvertToBytes(Container& buffer) {
  using T = typename Container::value_type;

  return std::span<std::byte>(reinterpret_cast<std::byte*>(buffer.data()),
                              sizeof(T) * buffer.size());
}

template <typename T>
std::span<const std::byte> ConvertToBytesConst(const T* buffer,
                                               std::size_t length) {
  return std::span<const std::byte>(reinterpret_cast<const std::byte*>(buffer),
                                    sizeof(T) * length);
}

template <ContainerConcept Container>
std::span<const std::byte> ConvertToBytesConst(const Container& buffer) {
  using T = Container::value_type;

  return std::span<const std::byte>(
      reinterpret_cast<const std::byte*>(buffer.data()),
      sizeof(T) * buffer.size());
}

// remainder byte that less than size of T shall be ignored
template <typename T>
std::span<T> ConvertFromBytes(std::span<std::byte> buf) {
  return std::span<T>(reinterpret_cast<T*>(buf.data()), buf.size() / sizeof(T));
}

// remainder byte that less than size of T shall be ignored
template <typename T>
std::span<const T> ConvertFromBytesConst(std::span<const std::byte> buf) {
  return std::span<const T>(reinterpret_cast<const T*>(buf.data()),
                            buf.size() / sizeof(T));
}

template <typename T>
std::span<std::byte> CastToBytes(T value) {
  return std::span<std::byte>(reinterpret_cast<std::byte*>(&value, sizeof(T)));
}

template <typename T>
T CastFromBytes(std::span<const std::byte> buf) {
  assert(buf.size() >= sizeof(T));

  auto subspan = buf.subspan(0, sizeof(T));
  return *reinterpret_cast<const T*>(subspan.data());
}

}  // namespace util
