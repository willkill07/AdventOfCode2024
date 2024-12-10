#pragma once

#include <bit>
#include <type_traits>

#include <ankerl/unordered_dense.h>

struct Hash {
  using is_avalanching = void;

  template <typename T>
    requires(std::has_single_bit(sizeof(T))) and std::has_unique_object_representations_v<T>
  [[nodiscard]] auto operator()(T const& data) const noexcept -> uint64_t {
    return ankerl::unordered_dense::detail::wyhash::hash(std::addressof(data), sizeof(T));
  }
};
