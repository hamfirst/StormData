#pragma once

#include <cstdint>
#include <type_traits>

#include <StormRefl/StormRefl.h>
#include <StormRefl/StormReflMetaFuncs.h>

#include "StormDataParentInfo.h"

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

template <typename T, bool IsDataRefl>
struct StormDataSetRaw
{
  static void Copy(const T & src, T & dst)
  {
    StormReflCopy(dst, src);
  }

  static void Move(T && src, T & dst)
  {
    StormReflMove(dst, std::move(src));
  }
};

template <typename T>
struct StormDataSetRaw<T, true>
{
  static void Copy(const T & src, T & dst)
  {
    dst.SetRaw(src);
  }

  static void Move(T && src, T & dst)
  {
    dst.SetRaw(std::move(src));
  }
};

template <typename T>
void StormDataSetRawCopy(const T & src, T & dst)
{
  StormDataSetRaw<T, StormDataCheckReflectable<T>::value>::Copy(src, dst);
}

template <typename T>
void StormDataSetRawMove(T && src, T & dst)
{
  StormDataSetRaw<T, StormDataCheckReflectable<T>::value>::Move(std::move(src), dst);
}

#define STORM_CHANGE_NOTIFIER
#define STORM_CHANGE_MINIMAL

#define STORM_CHANGE_NOTIFIER_INFO \
  public: \
  STORM_REFL_IGNORE StormReflectionParentInfo m_ReflectionInfo

#define STORM_DATA_DEFAULT_CONSTRUCTION(TypeName) \
  STORM_REFL; \
  STORM_CHANGE_NOTIFIER_INFO; \
  TypeName() noexcept; \
  TypeName(const TypeName & rhs) noexcept; \
  TypeName(TypeName && rhs) noexcept; \
  TypeName(TypeName && rhs, StormReflectionParentInfo * new_parent) noexcept; \
  TypeName & operator = (const TypeName & rhs) noexcept; \
  TypeName & operator = (TypeName && rhs) noexcept; \
  void SetRaw(const TypeName & rhs) noexcept; \
  void SetRaw(TypeName && rhs) noexcept; \
  void Relocate(TypeName && rhs, StormReflectionParentInfo * new_parent) noexcept; \

#define STORM_DATA_DEFAULT_CONSTRUCTION_DERIVED(TypeName) \
  STORM_REFL; \
  TypeName() noexcept; \
  TypeName(const TypeName & rhs) noexcept; \
  TypeName(TypeName && rhs) noexcept; \
  TypeName(TypeName && rhs, StormReflectionParentInfo * new_parent) noexcept; \
  TypeName & operator = (const TypeName & rhs) noexcept; \
  TypeName & operator = (TypeName && rhs) noexcept; \
  void SetRaw(const TypeName & rhs) noexcept; \
  void SetRaw(TypeName && rhs) noexcept; \
  void Relocate(TypeName && rhs, StormReflectionParentInfo * new_parent) noexcept; \

#define STORM_DATA_DEFAULT_CONSTRUCTION_IMPL(TypeName) \
  TypeName::TypeName() noexcept \
  { \
    InitializeParentInfo(*this); \
  } \
  TypeName::TypeName(const TypeName & rhs) noexcept \
  { \
    InitializeParentInfo(*this); \
    StormReflVisitEach(rhs, *this, [](auto src, auto dst) { StormReflCopy(dst.Get(), src.Get()); }); \
  } \
  TypeName::TypeName(TypeName && rhs) noexcept \
  { \
    InitializeParentInfo(*this); \
    StormReflVisitEach(static_cast<TypeName &>(rhs), *this, [](auto src, auto dst) { StormReflElementwiseMove(dst.Get(), src.Get()); }); \
    MoveParentInfo(rhs, *this); \
    StormReflVisitEach(*this, [&](auto f) { SetParentInfoPointer(f.Get(), &m_ReflectionInfo); }); \
  } \
  TypeName::TypeName(TypeName && rhs, StormReflectionParentInfo * new_parent) noexcept \
  { \
    m_ReflectionInfo = rhs.m_ReflectionInfo; \
    m_ReflectionInfo.m_ParentInfo = new_parent; \
    StormReflVisitEach(static_cast<TypeName &>(rhs), *this, [&](auto src, auto dst) { StormDataRelocate(std::move(src.Get()), dst.Get(), &m_ReflectionInfo); }); \
  } \
  TypeName & TypeName::operator = (const TypeName & rhs) noexcept \
  { \
    StormReflVisitEach(static_cast<const TypeName &>(rhs), *this, [&](auto src, auto dst) { StormDataSetRawCopy(src.Get(), dst.Get()); }); \
    ReflectionNotifySetObject(m_ReflectionInfo, StormReflEncodeJson(*this)); \
    return *this; \
  } \
  TypeName & TypeName::operator = (TypeName && rhs) noexcept \
  { \
    StormReflVisitEach(static_cast<TypeName &>(rhs), *this, [&](auto src, auto dst) { StormDataSetRawMove(std::move(src.Get()), dst.Get()); }); \
    ReflectionNotifySetObject(m_ReflectionInfo, StormReflEncodeJson(*this)); \
    return *this; \
  } \
  void TypeName::SetRaw(const TypeName & rhs) noexcept \
  { \
    StormReflVisitEach(static_cast<const TypeName &>(rhs), *this, [&](auto src, auto dst) { StormDataSetRawCopy(src.Get(), dst.Get()); }); \
  } \
  void TypeName::SetRaw(TypeName && rhs) noexcept \
  { \
    StormReflVisitEach(static_cast<TypeName &>(rhs), *this, [&](auto src, auto dst) { StormDataSetRawMove(std::move(src.Get()), dst.Get()); }); \
  } \
  void TypeName::Relocate(TypeName && rhs, StormReflectionParentInfo * new_parent) noexcept \
  { \
    m_ReflectionInfo = rhs.m_ReflectionInfo; \
    m_ReflectionInfo.m_ParentInfo = new_parent; \
    StormReflVisitEach(static_cast<TypeName &>(rhs), *this, [&](auto src, auto dst) { StormDataRelocate(std::move(src.Get()), dst.Get(), &m_ReflectionInfo); }); \
  } \



