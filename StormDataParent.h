#pragma once

#include "StormData.h"

#ifdef STORM_CHANGE_NOTIFIER

template <typename T> class RNumber;
template <typename T> class REnum;
template <typename T> class RSparseList;
template <typename T> class RMergeList;
template <typename K, typename T> class RMap;

static void SetParentInfo(RBool & value, const StormReflectionParentInfo & info)
{
  value.m_ReflectionInfo = info;
}

template <typename NumericType>
void SetParentInfo(RNumber<NumericType> & value, const StormReflectionParentInfo & info)
{
  value.m_ReflectionInfo = info;
}

static void SetParentInfo(RString & value, const StormReflectionParentInfo & info)
{
  value.m_ReflectionInfo = info;
}

template <typename T, typename std::enable_if_t<StormReflCheckReflectable<T>::value> * Enable = nullptr>
void SetParentInfo(T & value, const StormReflectionParentInfo & info)
{
  value.m_ReflectionInfo = info;

  StormReflectionParentInfo new_info;
  new_info.m_ParentInfo = &value.m_ReflectionInfo;

  auto parent_setter = [&](auto f)
  {
    using member_type = decltype(f)::member_type;
    member_type & elem = f.Get();

    new_info.m_MemberName = f.GetName();
    SetParentInfo(elem, new_info);
  };

  StormReflVisitEach(value, parent_setter);
}

template <typename T, int i>
void SetParentInfo(T (&value)[i], const StormReflectionParentInfo & info)
{
  StormReflectionParentInfo new_info;
  new_info.m_ParentInfo = info.m_ParentInfo;
  new_info.m_MemberName = info.m_MemberName;

  for (int index = 0; index < i; index++)
  {
    new_info.m_ParentIndex = index;
    SetParentInfo(value[index], new_info);
  }
}

//template <class T>
//void SetParentInfo(RPolymorphic<T> & value, const StormReflectionParentInfo & info)
//{
//  value.m_ReflectionInfo = info;
//
//  StormReflectionParentInfo new_info;
//  new_info.m_ParentInfo = &value.m_ReflectionInfo;
//  new_info.m_ParentIndex = 0;
//  new_info.m_ParentType = 0xFFFFFFFF;
//
//  SetParentInfo(value.GetValue(), value.GetTypeNameHash(), new_info);
//}

template <class EnumType>
void SetParentInfo(REnum<EnumType> & value, const StormReflectionParentInfo & info)
{
  value.m_ReflectionInfo = info;
}

template <class T>
void SetParentInfo(RSparseList<T> & value, const StormReflectionParentInfo & info)
{
  value.m_ReflectionInfo = info;

  StormReflectionParentInfo new_info;
  new_info.m_ParentInfo = &value.m_ReflectionInfo;

  for (auto t : value)
  {
    new_info.m_ParentIndex = t.first;
    SetParentInfo(t.second, new_info);
  }
}

template <class T>
void SetParentInfo(RMergeList<T> & value, const StormReflectionParentInfo & info)
{
  value.m_ReflectionInfo = info;

  StormReflectionParentInfo new_info;
  new_info.m_ParentInfo = &value.m_ReflectionInfo;

  for (auto t : value)
  {
    new_info.m_ParentIndex = t.first;
    SetParentInfo(t.second, new_info);
  }
}

template <class K, class T>
void SetParentInfo(RMap<K, T> & value, const StormReflectionParentInfo & info)
{
  value.m_ReflectionInfo = info;

  StormReflectionParentInfo new_info;
  new_info.m_ParentInfo = &value.m_ReflectionInfo;

  for (auto t : value)
  {
    new_info.m_ParentIndex = t.first;
    SetParentInfo(t.second, new_info);
  }
}

// For base level structs, this sets the default info
template <typename T, typename std::enable_if<
  std::is_class<T>::value && T::is_reflectable
>::type * = nullptr>
void InitializeParentInfo(T & value)
{
  StormReflectionParentInfo default_info;
  SetParentInfo(value, default_info);
}


#endif
