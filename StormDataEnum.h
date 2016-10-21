#pragma once

#include "StormData.h"
#include "StormDataChangeNotifier.h"

#include <StormRefl\StormReflMetaEnum.h>

template <class EnumType>
class REnum
{
public:

  REnum()
  {
    
  }

  REnum(const REnum & rhs) = default;
  REnum(REnum && rhs) = default;

  REnum(EnumType val)
  {
    m_Value = val;
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
    if (DoReflectionCallback() == false)
    {
      return;
    }

    ReflectionNotifySet(m_ReflectionInfo, std::string(ToString()));
#endif
  }

  EnumType m_Value;
  STORM_CHANGE_NOTIFIER_INFO;
};