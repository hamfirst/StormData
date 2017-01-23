#pragma once

#include <type_traits>

#include "StormData.h"
#include "StormDataChangeNotifier.h"

class RBool
{
public:

  RBool() :
    m_Value(false)
  {
  }

  RBool(bool val) :
    m_Value(val)
  {
  }

  RBool(const RBool & val) :
    m_Value(val.m_Value)
  {
  }

  RBool(RBool && val) :
    m_Value(val.m_Value)
  {
#ifdef STORM_CHANGE_NOTIFIER
    m_ReflectionInfo = val.m_ReflectionInfo;
    val.m_ReflectionInfo = {};
#endif
  }

#ifdef STORM_CHANGE_NOTIFIER
  RBool(RBool && val, StormReflectionParentInfo * new_parent) :
    m_Value(val.m_Value)
  {
    m_ReflectionInfo = val.m_ReflectionInfo;
    m_ReflectionInfo.m_ParentInfo = new_parent;
  }
#endif

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

  RBool & operator = (RBool && rhs)
  {
    Set(rhs.m_Value);
    return *this;
  }

  void SetRaw(bool val)
  {
    m_Value = val;
  }

#ifdef STORM_CHANGE_NOTIFIER
  void Relocate(RBool && val, StormReflectionParentInfo * new_parent)
  {
    m_Value = val.m_Value;
    m_ReflectionInfo = val.m_ReflectionInfo;
    m_ReflectionInfo.m_ParentInfo = new_parent;
  }
#endif

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
    if (DoNotifyCallback(m_ReflectionInfo) == false)
    {
      return;
    }

    ReflectionNotifySet(m_ReflectionInfo, m_Value);
#endif
  }


  template <typename T, typename Enable>
  friend struct StormDataJson;

  bool m_Value;
  STORM_CHANGE_NOTIFIER_INFO;
};

template <class NumericType>
class RNumber
{
public:

  RNumber() noexcept :
    m_Value(0)
  {
  }

  RNumber(NumericType val) noexcept :
    m_Value(val)
  {
  }

  RNumber(const RNumber & val) noexcept :
    m_Value(val.m_Value)
  {
  }

  RNumber(RNumber && val) noexcept :
    m_Value(val.m_Value)
  {
#ifdef STORM_CHANGE_NOTIFIER
    m_ReflectionInfo = val.m_ReflectionInfo;
    val.m_ReflectionInfo = {};
#endif
  }

#ifdef STORM_CHANGE_NOTIFIER
  RNumber(RNumber && val, StormReflectionParentInfo * new_parent) noexcept :
    m_Value(val.m_Value)
  {
    m_ReflectionInfo = val.m_ReflectionInfo;
    m_ReflectionInfo.m_ParentInfo = new_parent;
  }
#endif

  NumericType operator = (NumericType val) noexcept
  {
    Set(val);
    return m_Value;
  }

  RNumber<NumericType> & operator = (const RNumber<NumericType> & rhs) noexcept
  {
    Set(rhs.m_Value);
    return *this;
  }

  RNumber<NumericType> & operator = (RNumber<NumericType> && rhs) noexcept
  {
    Set(rhs.m_Value);
    return *this;
  }

  void SetRaw(NumericType val)
  {
    m_Value = val;
  }

#ifdef STORM_CHANGE_NOTIFIER
  void Relocate(RNumber && val, StormReflectionParentInfo * new_parent) noexcept
  {
    m_Value = val.m_Value;
    m_ReflectionInfo = val.m_ReflectionInfo;
    m_ReflectionInfo.m_ParentInfo = new_parent;
  }
#endif

  operator NumericType() const
  {
    return m_Value;
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

  NumericType operator + (RNumber<NumericType> val) const
  {
    return m_Value + val.m_Value;
  }

  NumericType operator += (NumericType val)
  {
    Set(m_Value + val);
    return m_Value;
  }

  NumericType operator += (RNumber<NumericType> val)
  {
    Set(m_Value + val.m_Value);
    return m_Value;
  }

  NumericType operator - (NumericType val) const
  {
    return m_Value - val;
  }

  NumericType operator - (RNumber<NumericType> val) const
  {
    return m_Value - val.m_Value;
  }

  NumericType operator -= (NumericType val)
  {
    Set(m_Value - val);
    return m_Value;
  }

  NumericType operator -= (RNumber<NumericType> val)
  {
    Set(m_Value - val.m_Value);
    return m_Value;
  }

  NumericType operator * (NumericType val) const
  {
    return m_Value * val;
  }

  NumericType operator * (RNumber<NumericType> val) const
  {
    return m_Value * val.m_Value;
  }

  NumericType operator *= (NumericType val)
  {
    Set(m_Value * val);
    return m_Value;
  }

  NumericType operator *= (RNumber<NumericType> val)
  {
    Set(m_Value * val.m_Value);
    return m_Value;
  }

  NumericType operator / (NumericType val) const
  {
    return m_Value / val;
  }

  NumericType operator / (RNumber<NumericType> val) const
  {
    return m_Value / val.m_Value;
  }

  NumericType operator /= (NumericType val)
  {
    Set(m_Value / val);
    return m_Value;
  }

  NumericType operator /= (RNumber<NumericType> val)
  {
    Set(m_Value / val.m_Value);
    return m_Value;
  }

  template <typename RhsType, typename Enable = std::enable_if_t<!std::is_same<RhsType, NumericType>::value>>
  auto operator + (RhsType val) const
  {
    return m_Value + val;
  }

  template <typename RhsType, typename Enable = std::enable_if_t<!std::is_same<RhsType, NumericType>::value>>
  NumericType operator += (RhsType val)
  {
    Set((NumericType)(m_Value + val));
    return m_Value;
  }

  template <typename RhsType, typename Enable = std::enable_if_t<!std::is_same<RhsType, NumericType>::value>>
  auto operator - (RhsType val) const
  {
    return m_Value - val;
  }

  template <typename RhsType, typename Enable = std::enable_if_t<!std::is_same<RhsType, NumericType>::value>>
  NumericType operator -= (RhsType val)
  {
    Set((NumericType)(m_Value - val));
    return m_Value;
  }

  template <typename RhsType, typename Enable = std::enable_if_t<!std::is_same<RhsType, NumericType>::value>>
  auto operator * (RhsType val) const
  {
    return m_Value * val;
  }

  template <typename RhsType, typename Enable = std::enable_if_t<!std::is_same<RhsType, NumericType>::value>>
  NumericType operator *= (RhsType val)
  {
    Set((NumericType)(m_Value * val));
    return m_Value;
  }

  template <typename RhsType, typename Enable = std::enable_if_t<!std::is_same<RhsType, NumericType>::value>>
  auto operator / (RhsType val) const
  {
    return m_Value / val;
  }

  template <typename RhsType, typename Enable = std::enable_if_t<!std::is_same<RhsType, NumericType>::value>>
  NumericType operator /= (RhsType val)
  {
    Set((NumericType)(m_Value / val));
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
    if (DoNotifyCallback(m_ReflectionInfo) == false)
    {
      return;
    }

    ReflectionNotifySet(m_ReflectionInfo, m_Value);
#endif
  }

  template <typename T, typename Enable>
  friend struct StormDataJson;

  NumericType m_Value;
  STORM_CHANGE_NOTIFIER_INFO;
};

using RInt = RNumber<int>;
using RFloat = RNumber<float>;
