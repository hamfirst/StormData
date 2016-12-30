#pragma once

#include <StormRefl/StormReflMetaInfoBase.h>

#include "StormDataChangeType.refl.h"


template <>
struct StormReflEnumInfo<ReflectionNotifyChangeType>
{
  static constexpr int elems_n = 5;
  static constexpr auto GetName() { return "ReflectionNotifyChangeType"; }
  static constexpr auto GetNameHash() { return 0x93A22823; }
  template <int N> struct elems { };
};

template <>
struct StormReflEnumInfo<ReflectionNotifyChangeType>::elems<0>
{
  static constexpr auto GetName() { return "kSet"; }
  static constexpr auto GetNameHash() { return 0x22ED732D; }
  static constexpr auto GetValue() { return ReflectionNotifyChangeType::kSet; }
};

template <>
struct StormReflEnumInfo<ReflectionNotifyChangeType>::elems<1>
{
  static constexpr auto GetName() { return "kClear"; }
  static constexpr auto GetNameHash() { return 0xC9DE8DD1; }
  static constexpr auto GetValue() { return ReflectionNotifyChangeType::kClear; }
};

template <>
struct StormReflEnumInfo<ReflectionNotifyChangeType>::elems<2>
{
  static constexpr auto GetName() { return "kCompress"; }
  static constexpr auto GetNameHash() { return 0x012CE362; }
  static constexpr auto GetValue() { return ReflectionNotifyChangeType::kCompress; }
};

template <>
struct StormReflEnumInfo<ReflectionNotifyChangeType>::elems<3>
{
  static constexpr auto GetName() { return "kInsert"; }
  static constexpr auto GetNameHash() { return 0xF71F77EB; }
  static constexpr auto GetValue() { return ReflectionNotifyChangeType::kInsert; }
};

template <>
struct StormReflEnumInfo<ReflectionNotifyChangeType>::elems<4>
{
  static constexpr auto GetName() { return "kRemove"; }
  static constexpr auto GetNameHash() { return 0xE8BE8E47; }
  static constexpr auto GetValue() { return ReflectionNotifyChangeType::kRemove; }
};

namespace StormReflFileInfo
{
  struct StormDataChangeType
  {
    static const int types_n = 0;
    template <int i> struct type_info { using type = void; };
  };

}

