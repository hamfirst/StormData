#pragma once

#include <cstdint>
#include <type_traits>

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

template <typename T, bool IsClass>
struct StormDataCheckReflectableClass {
  struct Fallback { bool m_ReflectionInfo; };
  struct Derived : T, Fallback { };

  template<typename C, C> struct ChT;

  template<typename C> static char(&f(ChT<bool Fallback::*, &C::m_ReflectionInfo>*))[1];
  template<typename C> static char(&f(...))[2];

  static const bool value = sizeof(f<Derived>(0)) == 2;
};

template <typename T>
struct StormDataCheckReflectableClass<T, false>
{
  static const bool value = false;
};

template <typename T>
struct StormDataCheckReflectable : public StormDataCheckReflectableClass<T, std::template is_class<typename std::decay<T>::type>::value>
{

};

template <typename T, bool IsReflectable>
struct StormDataRelocateObject
{
  static void Construct(T && src, void * dst, StormReflectionParentInfo * new_parent)
  {
    new(dst) T(std::move(src));
  }

  static void Process(T && src, T & dst, StormReflectionParentInfo * new_parent)
  {
    dst = std::move(src);
  }
};

template <typename T>
struct StormDataRelocateObject<T, true>
{
  static void Construct(T && src, void * dst, StormReflectionParentInfo * new_parent)
  {
    new(dst) T(std::move(src), new_parent);
  }

  static void Process(T && src, T & dst, StormReflectionParentInfo * new_parent)
  {
    dst.Relocate(std::move(src), new_parent);
  }
};

template <typename T>
void StormDataRelocate(T && src, T & dst, StormReflectionParentInfo * new_parent)
{
  StormDataRelocateObject<T, StormDataCheckReflectable<T>::value>::Process(std::move(src), dst, new_parent);
}

template <typename T>
void StormDataRelocateConstruct(T && src, void * dst, StormReflectionParentInfo * new_parent)
{
  StormDataRelocateObject<T, StormDataCheckReflectable<T>::value>::Construct(std::move(src), dst, new_parent);
}

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
  TypeName(TypeName && rhs, StormReflectionParentInfo * new_parent); \
  TypeName & operator = (const TypeName & rhs); \
  TypeName & operator = (TypeName && rhs); \
  void Relocate(TypeName && rhs, StormReflectionParentInfo * new_parent); \

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
  TypeName::TypeName(TypeName && rhs, StormReflectionParentInfo * new_parent) \
  { \
    m_ReflectionInfo = rhs.m_ReflectionInfo; \
    m_ReflectionInfo.m_ParentInfo = new_parent; \
    StormReflVisitEach(static_cast<TypeName &>(rhs), *this, [&](auto src, auto dst) { StormDataRelocate(std::move(src.Get()), dst.Get(), &m_ReflectionInfo); }); \
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
  void TypeName::Relocate(TypeName && rhs, StormReflectionParentInfo * new_parent) \
  { \
    m_ReflectionInfo = rhs.m_ReflectionInfo; \
    m_ReflectionInfo.m_ParentInfo = new_parent; \
    StormReflVisitEach(static_cast<TypeName &>(rhs), *this, [&](auto src, auto dst) { StormDataRelocate(std::move(src.Get()), dst.Get(), &m_ReflectionInfo); }); \
  } \



