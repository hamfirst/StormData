#pragma once

#include "StormData.h"
#include "StormDataChangeNotifier.h"

template <class EnumType>
class REnum
{
public:

  REnum()
  {
    
  }

  REnum(EnumType val)
  {
    m_Value = val;
  }
  
  const EnumType & operator = (EnumType val)
  {
    Set(val);
    return m_Value;
  }

  const EnumType & operator = (const std::string & val)
  {
    auto enum_val = EnumType::_from_string_nothrow(val.c_str());

    if (enum_val)
    {
      Set(*enum_val);
    }

    return m_Value;
  }

  operator int() const
  {
    return m_Value._to_integral();
  }

  operator EnumType() const
  {
    return m_Value;
  }

  int _to_integral() const
  {
    return m_Value._to_integral();
  }

  czstr _to_string() const
  {
    return m_Value._to_string();
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

    ReflectionNotifySet(m_ReflectionInfo, std::string(_to_string()));
#endif
  }

  EnumType m_Value;
  STORM_CHANGE_NOTIFIER_INFO;
};
