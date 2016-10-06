#pragma once

#include <type_traits>

#include "StormData.h"
#include "StormDataChangeNotifier.h"

class RBool
{
public:

  RBool()
  {
    m_Value = false;
  }

  RBool(bool val)
  {
    m_Value = val;
  }

  RBool(const RBool & val) = default;
  RBool(RBool && val) = default;

  bool operator = (bool val)
  {
    Set(val);
    return m_Value;
  }

  RBool & operator = (const RBool & rhs)
  {
    Set(rhs.m_Value);
    return *this;
  }

  RBool & operator = (RBool && rhs) = default;

  operator bool() const
  {
    return m_Value;
  }

  bool operator !() const
  {
    return !m_Value;
  }

  bool operator ==(bool val) const
  {
    return m_Value == val;
  }

  bool operator !=(bool val) const
  {
    return m_Value != val;
  }

  bool operator == (const RBool & val) const
  {
    return m_Value != val.m_Value;
  }

private:
  void Set(bool val)
  {
    m_Value = val;

#ifdef STORM_CHANGE_NOTIFIER
    if (DoReflectionCallback() == false)
    {
      return;
    }

    ReflectionNotifySet(m_ReflectionInfo, m_Value);
#endif
  }

  bool m_Value;
  STORM_CHANGE_NOTIFIER_INFO;
};

template <class NumericType>
class RNumber
{
public:

  RNumber()
  {
    m_Value = 0;
  }

  RNumber(NumericType val)
  {
    m_Value = val;
  }

  RNumber(const RNumber & val) = default;
  RNumber(RNumber && val) = default;

  NumericType operator = (NumericType val)
  {
    Set(val);
    return m_Value;
  }

  RNumber<NumericType> & operator = (const RNumber<NumericType> & rhs)
  {
    Set(rhs.m_Value);
    return *this;
  }

  RNumber<NumericType> & operator = (RNumber<NumericType> && rhs) = default;

  operator NumericType() const
  {
    return m_Value;
  }

  operator bool () const
  {
    return m_Value != 0;
  }

  float AsFloat() const
  {
    return (float)m_Value;
  }

  NumericType operator ++()
  {
    Set(m_Value + 1);
    return m_Value;
  }

  NumericType operator --()
  {
    Set(m_Value - 1);
    return m_Value;
  }

  NumericType operator !() const
  {
    return !m_Value;
  }

  NumericType operator + (NumericType val) const
  {
    return m_Value + val;
  }

  NumericType operator += (NumericType val)
  {
    Set(m_Value + val);
    return m_Value;
  }

  NumericType operator - (NumericType val) const
  {
    return m_Value - val;
  }

  NumericType operator -= (NumericType val)
  {
    Set(m_Value - val);
    return m_Value;
  }

  NumericType operator * (NumericType val) const
  {
    return m_Value * val;
  }

  NumericType operator *= (NumericType val)
  {
    Set(m_Value * val);
    return m_Value;
  }

  NumericType operator / (NumericType val) const
  {
    return m_Value / val;
  }

  NumericType operator /= (NumericType val)
  {
    Set(m_Value / val);
    return m_Value;
  }

  template <class T = NumericType>
  typename std::enable_if<std::is_integral<T>::value, T>::type operator % (NumericType val) const
  {
    return m_Value % val;
  }

  template <class T = NumericType>
  typename std::enable_if<std::is_integral<T>::value, T>::type operator %= (NumericType val)
  {
    Set(m_Value % val);
    return m_Value;
  }

  template <class T = NumericType>
  typename std::enable_if<std::is_integral<T>::value, T>::type operator & (NumericType val) const
  {
    return m_Value & val;
  }

  template <class T = NumericType>
  typename std::enable_if<std::is_integral<T>::value, T>::type operator &= (NumericType val)
  {
    Set(m_Value & val);
    return m_Value;
  }

  template <class T = NumericType>
  typename std::enable_if<std::is_integral<T>::value, T>::type operator | (NumericType val) const
  {
    return m_Value | val;
  }

  template <class T = NumericType>
  typename std::enable_if<std::is_integral<T>::value, T>::type operator |= (NumericType val)
  {
    Set(m_Value | val);
    return m_Value;
  }

  template <class T = NumericType>
  typename std::enable_if<std::is_integral<T>::value, T>::type operator ^ (NumericType val) const
  {
    return m_Value ^ val;
  }

  template <class T = NumericType>
  typename std::enable_if<std::is_integral<T>::value, T>::type operator ^= (NumericType val)
  {
    Set(m_Value ^ val);
    return m_Value;
  }

  template <class T = NumericType>
  typename std::enable_if<std::is_integral<T>::value, T>::type operator ~ () const
  {
    return ~m_Value;
  }

  template <class T = NumericType>
  typename std::enable_if<std::is_integral<T>::value, T>::type operator >> (NumericType val) const
  {
    return m_Value >> val;
  }

  template <class T = NumericType>
  typename std::enable_if<std::is_integral<T>::value, T>::type operator >>= (NumericType val)
  {
    Set(m_Value >> val);
    return m_Value;
  }

  template <class T = NumericType>
  typename std::enable_if<std::is_integral<T>::value, T>::type operator << (NumericType val) const
  {
    return m_Value << val;
  }

  template <class T = NumericType>
  typename std::enable_if<std::is_integral<T>::value, T>::type operator <<= (NumericType val)
  {
    Set(m_Value << val);
    return m_Value;
  }

  bool operator == (NumericType val) const
  {
    return m_Value == val;
  }

  bool operator != (NumericType val) const
  {
    return m_Value != val;
  }

  bool operator > (NumericType val) const
  {
    return m_Value > val;
  }

  bool operator >= (NumericType val) const
  {
    return m_Value >= val;
  }

  bool operator < (NumericType val) const
  {
    return m_Value < val;
  }

  bool operator <= (NumericType val) const
  {
    return m_Value <= val;
  }

private:
  void Set(NumericType val)
  {
    m_Value = val;

#ifdef STORM_CHANGE_NOTIFIER
    if (DoReflectionCallback() == false)
    {
      return;
    }

    ReflectionNotifySet(m_ReflectionInfo, m_Value);
#endif
  }

  NumericType m_Value;
  STORM_CHANGE_NOTIFIER_INFO;
};
