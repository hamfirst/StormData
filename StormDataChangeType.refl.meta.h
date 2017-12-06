#pragma once

#include <StormRefl/StormReflMetaInfoBase.h>

#include "StormDataChangeType.refl.h"


template <>
struct StormReflEnumInfo<ReflectionNotifyChangeType>
{
  static constexpr int elems_n = 7;
  static constexpr auto GetName() { return "ReflectionNotifyChangeType"; }
  static constexpr auto GetNameHash() { return 0xD4869875; }
  template <int N> struct elems { };
};

template <>
struct StormReflEnumInfo<ReflectionNotifyChangeType>::elems<0>
{
  static constexpr auto GetName() { return "kSet"; }
  static constexpr auto GetNameHash() { return 0xEC7B1530; }
  static constexpr auto GetValue() { return ReflectionNotifyChangeType::kSet; }
};

template <>
struct StormReflEnumInfo<ReflectionNotifyChangeType>::elems<1>
{
  static constexpr auto GetName() { return "kClear"; }
  static constexpr auto GetNameHash() { return 0x31A37725; }
  static constexpr auto GetValue() { return ReflectionNotifyChangeType::kClear; }
};

template <>
struct StormReflEnumInfo<ReflectionNotifyChangeType>::elems<2>
{
  static constexpr auto GetName() { return "kCompress"; }
  static constexpr auto GetNameHash() { return 0xED5F65F1; }
  static constexpr auto GetValue() { return ReflectionNotifyChangeType::kCompress; }
};

template <>
struct StormReflEnumInfo<ReflectionNotifyChangeType>::elems<3>
{
  static constexpr auto GetName() { return "kInsert"; }
  static constexpr auto GetNameHash() { return 0x677FFD01; }
  static constexpr auto GetValue() { return ReflectionNotifyChangeType::kInsert; }
};

template <>
struct StormReflEnumInfo<ReflectionNotifyChangeType>::elems<4>
{
  static constexpr auto GetName() { return "kRemove"; }
  static constexpr auto GetNameHash() { return 0xCA337EC4; }
  static constexpr auto GetValue() { return ReflectionNotifyChangeType::kRemove; }
};

template <>
struct StormReflEnumInfo<ReflectionNotifyChangeType>::elems<5>
{
  static constexpr auto GetName() { return "kRearrange"; }
  static constexpr auto GetNameHash() { return 0x30AC1611; }
  static constexpr auto GetValue() { return ReflectionNotifyChangeType::kRearrange; }
};

template <>
struct StormReflEnumInfo<ReflectionNotifyChangeType>::elems<6>
{
  static constexpr auto GetName() { return "kRevert"; }
  static constexpr auto GetNameHash() { return 0x4E60CE7A; }
  static constexpr auto GetValue() { return ReflectionNotifyChangeType::kRevert; }
};

namespace StormReflFileInfo
{
  struct StormDataChangeType
  {
    static const int types_n = 0;
    template <int i> struct type_info { using type = void; };
  };

}

