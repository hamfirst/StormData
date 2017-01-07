#pragma once

#include <cstdint>

#include <StormRefl/StormRefl.h>

struct ReflectionChangeNotification;

typedef void(*StormDataNotifyCallback)(void * user_ptr, const ReflectionChangeNotification & change);

enum class StormDataParentInfoFlags : uint32_t
{
  kNone = 0,
  kParentHasCallback = 0x01,
};

struct StormReflectionParentInfo
{
  static const uint64_t kInvalidParentIndex = ~0;

  uint32_t m_Flags = (uint32_t)StormDataParentInfoFlags::kNone;
  uint64_t m_ParentIndex = kInvalidParentIndex;
  const char * m_MemberName = nullptr;
  StormReflectionParentInfo * m_ParentInfo = nullptr;
  StormDataNotifyCallback m_Callback = nullptr;
  void * m_CallbackUserPtr = nullptr;
};

#define STORM_CHANGE_NOTIFIER
#define STORM_CHANGE_NOTIFIER_INFO \
  public: \
  STORM_REFL_IGNORE StormReflectionParentInfo m_ReflectionInfo

#define STORM_DATA_DEFAULT_CONSTRUCTION(TypeName) \
  STORM_REFL; \
  STORM_CHANGE_NOTIFIER_INFO; \
  TypeName(); \
  TypeName(const TypeName & rhs); \
  TypeName(TypeName && rhs); \
  TypeName & operator = (const TypeName & rhs); \
  TypeName & operator = (TypeName && rhs);

#define STORM_DATA_DEFAULT_CONSTRUCTION_IMPL(TypeName) \
  TypeName::TypeName() \
  { \
    InitializeParentInfo(*this); \
  } \
  TypeName::TypeName(const TypeName & rhs) \
  { \
    InitializeParentInfo(*this); \
    StormReflVisitEach(rhs, *this, [](auto src, auto dst) { StormReflElementwiseCopy(dst.Get(), src.Get()); }); \
  } \
  TypeName::TypeName(TypeName && rhs) \
  { \
    InitializeParentInfo(*this); \
    StormReflVisitEach(static_cast<TypeName &>(rhs), *this, [](auto src, auto dst) { StormReflElementwiseMove(dst.Get(), src.Get()); }); \
    MoveParentInfo(rhs, *this); \
    StormReflVisitEach(*this, [&](auto f) { SetParentInfoPointer(f.Get(), &m_ReflectionInfo); }); \
  } \
  TypeName & TypeName::operator = (const TypeName & rhs) \
  { \
    InitializeParentInfo(*this); \
    StormReflVisitEach(rhs, *this, [](auto src, auto dst) { StormReflElementwiseCopy(dst.Get(), src.Get()); }); \
    StormReflVisitEach(*this, [&](auto f) { SetParentInfoPointer(f.Get(), &m_ReflectionInfo); }); \
    ReflectionNotifySetObject(m_ReflectionInfo, StormReflEncodeJson(*this)); \
    return *this; \
  } \
  TypeName & TypeName::operator = (TypeName && rhs) \
  { \
    InitializeParentInfo(*this); \
    StormReflVisitEach(static_cast<TypeName &>(rhs), *this, [](auto src, auto dst) { StormReflElementwiseMove(dst.Get(), src.Get()); }); \
    StormReflVisitEach(*this, [&](auto f) { SetParentInfoPointer(f.Get(), &m_ReflectionInfo); }); \
    ReflectionNotifySetObject(m_ReflectionInfo, StormReflEncodeJson(*this)); \
    return *this; \
  } \



