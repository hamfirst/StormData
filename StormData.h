#pragma once

#include <sb\cstr.h>

#include <StormRefl\StormRefl.h>

struct StormReflectionParentInfo
{
  static const uint64_t kInvalidParentIndex = ~0;

  const char * m_MemberName = nullptr;
  uint64_t m_ParentIndex = kInvalidParentIndex;
  StormReflectionParentInfo * m_ParentInfo = nullptr;
};

#define STORM_CHANGE_NOTIFIER
#define STORM_CHANGE_NOTIFIER_INFO \
  public: \
  STORM_REFL_IGNORE StormReflectionParentInfo m_ReflectionInfo
  




