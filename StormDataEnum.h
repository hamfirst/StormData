#pragma once

#include "StormData.h"
#include "StormDataChangeNotifier.h"

#include <StormRefl/StormReflMetaEnum.h>

#include <hash/Hash.h>

template <class EnumType>
class REnum
{
public:

  REnum() :
    m_Value((EnumType)0)
  {
    
  }

  REnum(const REnum<EnumType> & rhs) :
    m_Value(rhs.m_Value)
  {
  }

  REnum(REnum<EnumType> && rhs) :
    m_Value(rhs.m_Value)
  {
#ifdef STORM_CHANGE_NOTIFIER
    m_ReflectionInfo = rhs.m_ReflectionInfo;
    rhs.m_ReflectionInfo = {};
#endif
  }

#ifdef STORM_CHANGE_NOTIFIER
  REnum(REnum<EnumType> && rhs, StormReflectionParentInfo * new_parent) :
    m_Value(rhs.m_Value)
  {
    m_ReflectionInfo = rhs.m_ReflectionInfo;
    m_ReflectionInfo.m_ParentInfo = new_parent;
  }
#endif

  REnum(EnumType val) :
    m_Value(val)
  {
  }
  
  const EnumType & operator = (EnumType val)
  {
    Set(val);
    return m_Value;
  }

  const EnumType & operator = (const char * val)
  {
    auto hash = crc32(val);
    EnumType out;

    if (StormReflGetEnumFromHash(out, hash))
    {
      Set(out);
    }

    return m_Value;
  }

  const EnumType & operator = (const REnum<EnumType> & rhs)
  {
    Set(rhs.m_Value);
    return m_Value;
  }

  const EnumType & operator = (REnum<EnumType> && rhs)
  {
    Set(rhs.m_Value);
    return m_Value;
  }

  void SetRaw(EnumType val)
  {
    m_Value = val;
  }

#ifdef STORM_CHANGE_NOTIFIER
  void Relocate(REnum<EnumType> && rhs, StormReflectionParentInfo * new_parent)
  {
    m_Value = std::move(rhs.m_Value);
    m_ReflectionInfo = rhs.m_ReflectionInfo;
    m_ReflectionInfo.m_ParentInfo = new_parent;
  }
#endif

  operator int() const
  {
    return (int)m_Value;
  }

  operator EnumType() const
  {
    return m_Value;
  }

  czstr ToString() const
  {
    return StormReflGetEnumAsString(m_Value);
  }

  bool operator == (EnumType val) const
  {
    return m_Value == val;
  }

  bool operator == (const REnum<EnumType> & val) const
  {
    return m_Value == val.m_Value;
  }

  bool operator != (EnumType val) const
  {
    return m_Value != val;
  }

  bool operator < (EnumType val) const
  {
    return m_Value < val;
  }

  bool operator <= (EnumType val) const
  {
    return m_Value <= val;
  }

  bool operator > (EnumType val) const
  {
    return m_Value > val;
  }

  bool operator >= (EnumType val) const
  {
    return m_Value >= val;
  }

private:
  void Set(EnumType val)
  {
    m_Value = val;

#ifdef STORM_CHANGE_NOTIFIER
    if (DoNotifyCallback(m_ReflectionInfo) == false)
    {
      return;
    }

    ReflectionNotifySet(m_ReflectionInfo, std::string(ToString()));
#endif
  }

  template <typename T, typename Enable>
  friend struct StormDataJson;

  EnumType m_Value;
  STORM_CHANGE_NOTIFIER_INFO;
};
