#pragma once

#include <type_traits>

namespace util {

template <typename T>
concept ContainerConcept = requires(T container) {
  typename T::value_type;
  { container.data() } -> std::convertible_to<const typename T::value_type*>;
  { container.size() } -> std::same_as<std::size_t>;
};

}  // namespace util
