#pragma once

#include "StormData.h"

#include <StormRefl/StormReflMetaFuncs.h>

#ifdef STORM_CHANGE_NOTIFIER

class RBool;
class RString;

template <typename T> class RNumber;
template <typename T> class REnum;
template <typename T> class RSparseList;
template <typename T> class RMergeList;
template <typename K, typename T> class RMap;

template <typename T, typename Enable = void>
struct SetParentInfoStruct
{
  void operator()(T & t, const StormReflectionParentInfo & info)
  {

  }
};

template <>
struct SetParentInfoStruct<RBool>
{
  void operator()(RBool & value, const StormReflectionParentInfo & info)
  {
    value.m_ReflectionInfo = info;
  }
};

template <typename NumericType>
struct SetParentInfoStruct<RNumber<NumericType>>
{
  void operator()(RNumber<NumericType> & value, const StormReflectionParentInfo & info)
  {
    value.m_ReflectionInfo = info;
  }
};

template <>
struct SetParentInfoStruct<RString>
{
  void operator()(RString & value, const StormReflectionParentInfo & info)
  {
    value.m_ReflectionInfo = info;
  }
};

template <typename T>
struct SetParentInfoStruct<T, typename std::enable_if_t<StormReflCheckReflectable<T>::value>>
{
  void operator()(T & value, const StormReflectionParentInfo & info)
  {
    value.m_ReflectionInfo = info;

    StormReflectionParentInfo new_info;
    new_info.m_ParentInfo = &value.m_ReflectionInfo;

    auto parent_setter = [&](auto f)
    {
      using member_type = typename decltype(f)::member_type;
      member_type & elem = f.Get();

      new_info.m_MemberName = f.GetName();
      SetParentInfo(elem, new_info);
    };

    StormReflVisitEach(value, parent_setter);
  }
};

template <typename T, int i>
struct SetParentInfoStruct<T[i]>
{
  void operator()(T(&value)[i], const StormReflectionParentInfo & info)
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
};

template <class EnumType>
struct SetParentInfoStruct<REnum<EnumType>>
{
  void operator()(REnum<EnumType> & value, const StormReflectionParentInfo & info)
  {
    value.m_ReflectionInfo = info;
  }
};

template <class T>
struct SetParentInfoStruct<RSparseList<T>>
{
  void operator()(RSparseList<T> & value, const StormReflectionParentInfo & info)
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
};

template <class T>
struct SetParentInfoStruct<RMergeList<T>>
{
  void operator()(RMergeList<T> & value, const StormReflectionParentInfo & info)
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
};

template <class K, class T>
struct SetParentInfoStruct<RMap<K, T>>
{
  void operator()(RMap<K, T> & value, const StormReflectionParentInfo & info)
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
};

template <typename T>
void SetParentInfo(T & t, const StormReflectionParentInfo & info)
{
  SetParentInfoStruct<T>{}(t, info);
}

// For base level structs, this sets the default info
template <typename T>
void InitializeParentInfo(T & value)
{
  StormReflectionParentInfo default_info;
  default_info.m_MemberName = (const char *)&value;
  SetParentInfo(value, default_info);
}


#endif
