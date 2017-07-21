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

class RDeterministicFloat
{
public:
  static const int kMaxLength = 32;

  RDeterministicFloat(const char * str)
  {
    ParseStr(str);
  }

  RDeterministicFloat()
  {
    m_Value[0] = '0';
    m_Value[1] = 0;
  }

  RDeterministicFloat(const RDeterministicFloat & val)
  {
    CopyFrom(val.m_Value);
  }

  RDeterministicFloat(RDeterministicFloat && val) noexcept
  {
    CopyFrom(val.m_Value);
#ifdef STORM_CHANGE_NOTIFIER
    m_ReflectionInfo = val.m_ReflectionInfo;
    val.m_ReflectionInfo = {};
#endif
  }

#ifdef STORM_CHANGE_NOTIFIER
  RDeterministicFloat(RDeterministicFloat && val, StormReflectionParentInfo * new_parent) noexcept
  {
    CopyFrom(val.m_Value);
    m_ReflectionInfo = val.m_ReflectionInfo;
    m_ReflectionInfo.m_ParentInfo = new_parent;
  }
#endif

  RDeterministicFloat & operator = (const RDeterministicFloat & rhs) noexcept
  {
    CopyFrom(rhs.m_Value);
    Set();
    return *this;
  }

  RDeterministicFloat & operator = (RDeterministicFloat && rhs) noexcept
  {
    CopyFrom(rhs.m_Value);
    Set();
    return *this;
  }

  RDeterministicFloat & operator = (czstr val) noexcept
  {
    ParseStr(val);
    Set();
    return *this;
  }

  float ToFloat()
  {
    return (float)atof(m_Value);
  }

  void SetRaw(const RDeterministicFloat & rhs)
  {
    CopyFrom(rhs.m_Value);
  }

  void SetRaw(RDeterministicFloat && rhs)
  {
    CopyFrom(rhs.m_Value);
  }

  void SetRaw(czstr val)
  {
    ParseStr(val);
  }

  czstr GetStr() const
  {
    return m_Value;
  }

  void SetZero()
  {
    m_Value[0] = '0';
    m_Value[1] = 0;
  }

#ifdef STORM_CHANGE_NOTIFIER
  void Relocate(RDeterministicFloat && val, StormReflectionParentInfo * new_parent) noexcept
  {
    CopyFrom(val.m_Value);
    m_ReflectionInfo = val.m_ReflectionInfo;
    m_ReflectionInfo.m_ParentInfo = new_parent;
  }
#endif

  bool operator == (const RDeterministicFloat & val) const
  {
    return !strcmp(val.GetStr(), GetStr());
  }

  bool operator != (const RDeterministicFloat & val) const
  {
    return strcmp(val.GetStr(), GetStr()) != 0;
  }

private:

  void CopyFrom(const char(&val)[kMaxLength])
  {
    memcpy(m_Value, val, kMaxLength);
  }

  void ParseStr(czstr str)
  {
    auto start_str = str;

    bool negative = false;
    if (*str == '-')
    {
      negative = true;
      str++;
    }

    if (*str == '+')
    {
      str++;
    }

    bool has_mantessa = false;
    bool has_numbers = false;
    bool has_exponent = false;
    bool has_negative_exponent = false;
    bool has_exponent_numbers = false;
    while (true)
    {
      if (*str >= '0' && *str <= '9')
      {
        if (has_exponent)
        {
          has_exponent_numbers = true;
        }
        else
        {
          has_numbers = true;
        }
        str++;
      }
      else if (*str == '.')
      {
        if (has_mantessa || has_exponent)
        {
          SetZero();
          return;
        }
        else
        {
          has_mantessa = true;
        }
        str++;
      }
      else if (*str == 'e' || *str == 'E')
      {
        if (has_exponent)
        {
          SetZero();
          return;
        }
        else
        {
          str++;

          if (*str == '-')
          {
            has_negative_exponent = true;
            str++;
          }
          else if (*str == '+')
          {
            str++;
          }
        }
      }
      else
      {
        break;
      }
    }

    if (has_numbers == false)
    {
      SetZero();
      return;
    }

    if (has_exponent && has_exponent_numbers == false)
    {
      SetZero();
      return;
    }

    auto length = str - start_str;
    if (length > kMaxLength - 1)
    {
      // Deal with this later
      SetZero();
    }
    else
    {
      memcpy(m_Value, start_str, length);
      m_Value[length] = 0;
    }
  }

  void Set()
  {
#ifdef STORM_CHANGE_NOTIFIER
    if (DoNotifyCallback(m_ReflectionInfo) == false)
    {
      return;
    }

    ReflectionNotifySet(m_ReflectionInfo, std::string(m_Value));
#endif
  }

  char m_Value[kMaxLength];
  STORM_CHANGE_NOTIFIER_INFO;
};

using RInt = RNumber<int>;
using RFloat = RNumber<float>;
