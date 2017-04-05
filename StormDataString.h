#pragma once

#include <string>

#include "StormData.h"
#include "StormDataChangeNotifier.h"

class RString
{
public:

  RString() noexcept
  {

  }

  RString(const RString & rhs) noexcept :
    m_Value(rhs.m_Value)
  {
  }

  RString(RString && rhs) noexcept :
    m_Value(rhs.m_Value)
  {
#ifdef STORM_CHANGE_NOTIFIER
    m_ReflectionInfo = rhs.m_ReflectionInfo;
    rhs.m_ReflectionInfo = {};
#endif
  }

#ifdef STORM_CHANGE_NOTIFIER
  RString(RString && rhs, StormReflectionParentInfo * new_parent) noexcept :
    m_Value(rhs.m_Value)
  {
    m_ReflectionInfo = rhs.m_ReflectionInfo;
    m_ReflectionInfo.m_ParentInfo = new_parent;
  }
#endif

  RString(const std::string & val) noexcept :
    m_Value(val)
  {
  }

  RString(std::string && val) noexcept :
    m_Value(std::move(val))
  {
  }

  RString(czstr val) noexcept :
    m_Value(val)
  {
  }

  operator const std::string & () const
  {
    return m_Value;
  }

  const std::string & ToString() const
  {
    return m_Value;
  }

  const std::string & operator = (const std::string & val)
  {
    m_Value = val;
    Modified();
    return m_Value;
  }

  const std::string & operator = (czstr val)
  {
    m_Value = val;
    Modified();
    return m_Value;
  }

  const std::string & operator = (std::initializer_list<char> val)
  {
    m_Value = val;
    Modified();
    return m_Value;
  }

  const std::string & operator = (std::string && val)
  {
    m_Value = val;
    Modified();
    return m_Value;
  }

  RString & operator = (const RString & val)
  {
    m_Value = val.m_Value;
    Modified();
    return *this;
  }

  RString & operator = (RString && val)
  {
    m_Value = std::move(val.m_Value);
    Modified();
    return *this;
  }

  void SetRaw(const std::string & rhs)
  {
    m_Value = rhs;
  }

  void SetRaw(std::string && rhs)
  {
    m_Value = std::move(rhs);
  }

#ifdef STORM_CHANGE_NOTIFIER
  void Relocate(RString && rhs, StormReflectionParentInfo * new_parent) noexcept
  {
    m_Value = std::move(rhs.m_Value);
    m_ReflectionInfo = rhs.m_ReflectionInfo;
    m_ReflectionInfo.m_ParentInfo = new_parent;
  }
#endif

  bool operator == (const RString & val) const
  {
    return m_Value == val.m_Value;
  }

  void SetIfDifferent(const std::string & val)
  {
    if (m_Value != val)
    {
      m_Value = val;
      Modified();
    }
  }

  void SetIfDifferent(const char * val)
  {
    if (m_Value != val)
    {
      m_Value = val;
      Modified();
    }
  }

  void SetIfDifferent(const RString & val)
  {
    if (m_Value != val.m_Value)
    {
      m_Value = val.m_Value;
      Modified();
    }
  }

  auto begin()
  {
    return m_Value.begin();
  }

  auto end()
  {
    return m_Value.end();
  }

  auto begin() const
  {
    return m_Value.begin();
  }

  auto end() const
  {
    return m_Value.end();
  }

  auto rbegin()
  {
    return m_Value.rbegin();
  }

  auto rend()
  {
    return m_Value.rend();
  }

  auto rbegin() const
  {
    return m_Value.rbegin();
  }

  auto rend() const
  {
    return m_Value.rend();
  }

  auto cbegin() const
  {
    return m_Value.cbegin();
  }

  auto cend() const
  {
    return m_Value.cend();
  }

  auto crbegin() const
  {
    return m_Value.crbegin();
  }

  auto crend() const
  {
    return m_Value.crend();
  }

  auto size() const
  {
    return m_Value.size();
  }

  auto length() const
  {
    return m_Value.length();
  }

  auto max_size() const
  {
    return m_Value.max_size();
  }

  auto capacity() const
  {
    return m_Value.capacity();
  }

  void reserve()
  {
    m_Value.reserve();
  }

  auto emtpy() const
  {
    return m_Value.empty();
  }

  void shrink_to_fit()
  {
    m_Value.shrink_to_fit();
  }

  void set(size_t index, char c)
  {
    m_Value[index] = c;
    Modified();
  }

  char operator[](size_t pos) const
  {
    return m_Value[pos];
  }

  const std::string & operator += (const std::string & val)
  {
    m_Value += val;
    Modified();
    return m_Value;
  }

  const std::string & operator += (czstr val)
  {
    m_Value += val;
    Modified();
    return m_Value;
  }

  const std::string & operator += (char val)
  {
    m_Value += val;
    Modified();
    return m_Value;
  }

  const std::string & operator += (std::initializer_list<char> val)
  {
    m_Value += val;
    Modified();
    return m_Value;
  }

  const std::string & append(const std::string & val)
  {
    m_Value.append(val);
    Modified();
    return m_Value;
  }

  const std::string & append(const std::string & val, size_t subpos, size_t sublen = std::string::npos)
  {
    m_Value.append(val, subpos, sublen);
    Modified();
    return m_Value;
  }

  const std::string & append(czstr val)
  {
    m_Value.append(val);
    Modified();
    return m_Value;
  }

  const std::string & append(czstr val, size_t n)
  {
    m_Value.append(val, n);
    Modified();
    return m_Value;
  }

  const std::string & append(size_t n, char val)
  {
    m_Value.append(n, val);
    Modified();
    return m_Value;
  }

  template <class InputIterator>
  const std::string & append(InputIterator first, InputIterator last)
  {
    m_Value.append(first, last);
    Modified();
    return m_Value;
  }

  const std::string & append(std::initializer_list<char> val)
  {
    m_Value.append(val);
    Modified();
    return m_Value;
  }

  void push_back(char val)
  {
    m_Value.push_back(val);
    Modified();
  }

  const std::string & assign(const std::string & val)
  {
    m_Value.assign(val);
    Modified();
    return m_Value;
  }

  const std::string & assign(const std::string & val, size_t subpos, size_t sublen = std::string::npos)
  {
    m_Value.assign(val, subpos, sublen);
    Modified();
    return m_Value;
  }

  const std::string & assign(czstr val)
  {
    m_Value.assign(val);
    Modified();
    return m_Value;
  }

  const std::string & assign(czstr val, size_t n)
  {
    m_Value.assign(val, n);
    Modified();
    return m_Value;
  }

  const std::string & assign(size_t n, char val)
  {
    m_Value.assign(n, val);
    Modified();
    return m_Value;
  }

  template <class InputIterator>
  const std::string & assign(InputIterator first, InputIterator last)
  {
    m_Value.assign(first, last);
    Modified();
    return m_Value;
  }

  const std::string & assign(std::initializer_list<char> val)
  {
    m_Value.assign(val);
    Modified();
    return m_Value;
  }

  const std::string & assign(std::string && val) noexcept
  {
    m_Value.assign(val);
    Modified();
    return m_Value;
  }

  const std::string & insert(size_t pos, const std::string & val)
  {
    m_Value.insert(pos, val);
    Modified();
    return m_Value;
  }

  const std::string & insert(size_t pos, const std::string & val, size_t subpos, size_t sublen = std::string::npos)
  {
    m_Value.insert(pos, val, subpos, sublen);
    Modified();
    return m_Value;
  }

  const std::string & insert(size_t pos, czstr val)
  {
    m_Value.insert(pos, val);
    Modified();
    return m_Value;
  }

  const std::string & insert(size_t pos, czstr val, size_t n)
  {
    m_Value.insert(pos, val, n);
    Modified();
    return m_Value;
  }

  const std::string & insert(size_t pos, size_t n, char val)
  {
    m_Value.insert(pos, n, val);
    Modified();
    return m_Value;
  }

  const std::string & insert(std::string::const_iterator pos, size_t n, char val)
  {
    m_Value.insert(pos, n, val);
    Modified();
    return m_Value;
  }

  const std::string & insert(std::string::const_iterator pos, char val)
  {
    m_Value.insert(pos, val);
    Modified();
    return m_Value;
  }

  template <class InputIterator>
  const std::string & insert(std::string::iterator pos, InputIterator first, InputIterator last)
  {
    m_Value.insert(pos, first, last);
    Modified();
    return m_Value;
  }

  //const std::string & insert(std::string::const_iterator pos, std::initializer_list<char> val)
  //{
  //  m_Value.insert(pos, val);
  //  Modified();
  //  return m_Value;
  //}

  const std::string & erase(size_t pos = 0, size_t len = std::string::npos)
  {
    m_Value.erase(pos, len);
    Modified();
    return m_Value;
  }

  auto erase(std::string::const_iterator pos)
  {
    auto ret_val = m_Value.erase(pos);
    Modified();
    return ret_val;
  }

  auto erase(std::string::const_iterator first, std::string::const_iterator last)
  {
    auto ret_val = m_Value.erase(first, last);
    Modified();
    return ret_val;
  }

  const std::string & replace(size_t pos, size_t len, const std::string & val)
  {
    m_Value.replace(pos, len, val);
    Modified();
    return m_Value;
  }

  const std::string & replace(std::string::iterator i1, std::string::iterator i2, const std::string & val)
  {
    m_Value.replace(i1, i2, val);
    Modified();
    return m_Value;
  }

  const std::string & replace(size_t pos, size_t len, const std::string & val, size_t subpos, size_t sublen)
  {
    m_Value.replace(pos, len, val, subpos, sublen);
    Modified();
    return m_Value;
  }

  const std::string & replace(size_t pos, size_t len, const char* val)
  {
    m_Value.replace(pos, len, val);
    Modified();
    return m_Value;
  }

  const std::string & replace(std::string::iterator i1, std::string::iterator i2, const char* val)
  {
    m_Value.replace(i1, i2, val);
    Modified();
    return m_Value;
  }

  const std::string & replace(size_t pos, size_t len, const char* val, size_t n)
  {
    m_Value.replace(pos, len, val, n);
    Modified();
    return m_Value;
  }

  const std::string& replace(std::string::iterator i1, std::string::iterator i2, const char* val, size_t n)
  {
    m_Value.replace(i1, i2, val, n);
    Modified();
    return m_Value;
  }
  
  const std::string & replace(size_t pos, size_t len, size_t n, char c)
  {
    m_Value.replace(pos, len, n, c);
    Modified();
    return m_Value;
  }

  const std::string & replace(std::string::iterator i1, std::string::iterator i2, size_t n, char c)
  {
    m_Value.replace(i1, i2, n, c);
    Modified();
    return m_Value;
  }

  template <class InputIterator>
  const std::string & replace(std::string::iterator i1, std::string::iterator i2, InputIterator first, InputIterator last)
  {
    m_Value.replace(i1, i2, first, last);
    Modified();
    return m_Value;
  }

  void swap(std::string & val)
  {
    m_Value.swap(val);
    Modified();
  }

  void pop_back()
  {
    m_Value.pop_back();
    Modified();
  }

  czstr c_str() const
  {
    return m_Value.c_str();
  }

  czstr data() const
  {
    return m_Value.data();
  }

  size_t find(const std::string & str, size_t pos = 0) const noexcept
  {
    return m_Value.find(str, pos);
  }

  size_t find(const char* s, size_t pos = 0) const
  {
    return m_Value.find(s, pos);
  }

  size_t find(const char* s, size_t pos, size_t n) const
  {
    return m_Value.find(s, pos, n);
  }

  size_t find(char c, size_t pos = 0) const noexcept
  {
    return m_Value.find(c, pos);
  }

  size_t rfind(const std::string & str, size_t pos = std::string::npos) const noexcept
  {
    return m_Value.rfind(str, pos);
  }

  size_t rfind(czstr s, size_t pos = std::string::npos) const
  {
    return m_Value.rfind(s, pos);
  }

  size_t rfind(czstr s, size_t pos, size_t n) const
  {
    return m_Value.rfind(s, pos, n);
  }

  size_t rfind(char c, size_t pos = std::string::npos) const noexcept
  {
    return m_Value.rfind(c, pos);
  }

  size_t find_first_of(const std::string & str, size_t pos = 0) const noexcept
  {
    return m_Value.find_first_of(str, pos);
  }

  size_t find_first_of(const char* s, size_t pos = 0) const
  {
    return m_Value.find_first_of(s, pos);
  }

  size_t find_first_of(const char* s, size_t pos, size_t n) const
  {
    return m_Value.find_first_of(s, pos, n);
  }

  size_t find_first_of(char c, size_t pos = 0) const noexcept
  {
    return m_Value.find_first_of(c, pos);
  }

  size_t find_last_of(const std::string & str, size_t pos = std::string::npos) const noexcept
  {
    return m_Value.find_last_of(str, pos);
  }

  size_t find_last_of(const char* s, size_t pos = std::string::npos) const
  {
    return m_Value.find_last_of(s, pos);
  }

  size_t find_last_of(const char* s, size_t pos, size_t n) const
  {
    return m_Value.find_last_of(s, pos, n);
  }

  size_t find_last_of(char c, size_t pos = std::string::npos) const noexcept
  {
    return m_Value.find_last_of(c, pos);
  }

  size_t find_first_not_of(const std::string & str, size_t pos = 0) const noexcept
  {
    return m_Value.find_first_not_of(str, pos);
  }

  size_t find_first_not_of(const char* s, size_t pos = 0) const
  {
    return m_Value.find_first_not_of(s, pos);
  }

  size_t find_first_not_of(const char* s, size_t pos, size_t n) const
  {
    return m_Value.find_first_not_of(s, pos, n);
  }

  size_t find_first_not_of(char c, size_t pos = 0) const noexcept
  {
    return m_Value.find_first_not_of(c, pos);
  }

  size_t find_last_not_of(const std::string & str, size_t pos = std::string::npos) const noexcept
  {
    return m_Value.find_last_not_of(str, pos);
  }

  size_t find_last_not_of(const char* s, size_t pos = std::string::npos) const
  {
    return m_Value.find_last_not_of(s, pos);
  }

  size_t find_last_not_of(const char* s, size_t pos, size_t n) const
  {
    return m_Value.find_last_not_of(s, pos, n);
  }

  size_t find_last_not_of(char c, size_t pos = std::string::npos) const noexcept
  {
    return m_Value.find_last_not_of(c, pos);
  }

  std::string substr(size_t pos = 0, size_t len = std::string::npos) const
  {
    return m_Value.substr(pos, len);
  }

  int compare(const std::string & str) const noexcept
  {
    return m_Value.compare(str);
  }

  int compare(size_t pos, size_t len, const std::string & str) const
  {
    return m_Value.compare(pos, len, str);
  }

  int compare(size_t pos, size_t len, const std::string & str, size_t subpos, size_t sublen = std::string::npos) const
  {
    return m_Value.compare(pos, len, str, subpos, sublen);
  }

  int compare(const char* s) const
  {
    return m_Value.compare(s);
  }

  int compare(size_t pos, size_t len, const char* s) const
  {
    return m_Value.compare(pos, len, s);
  }

  int compare(size_t pos, size_t len, const char* s, size_t n) const
  {
    return m_Value.compare(pos, len, s, n);
  }

  size_t hash() const
  {
    return std::hash<std::string>()(m_Value);
  }

  bool BasicCaseInsensitiveCompare(const RString & rhs)
  {
    auto my_size = size();
    if (rhs.size() != my_size)
    {
      return false;
    }


    for(std::size_t index = 0; index < my_size; index++)
    {
      if (tolower(m_Value[index]) != tolower(rhs.m_Value[index]))
      {
        return false;
      }
    }

    return true;
  }

  bool BasicCaseInsensitiveCompare(const std::string & rhs)
  {
    auto my_size = size();
    if (rhs.size() != my_size)
    {
      return false;
    }

    for (std::size_t index = 0; index < my_size; index++)
    {
      if (tolower(m_Value[index]) != tolower(rhs[index]))
      {
        return false;
      }
    }

    return true;
  }

  bool BasicCaseInsensitiveCompare(const char * rhs)
  {
    auto my_size = size();
    auto other_size = strlen(rhs);
    if (other_size != my_size)
    {
      return false;
    }

    for (std::size_t index = 0; index < my_size; index++)
    {
      if (tolower(m_Value[index]) != tolower(rhs[index]))
      {
        return false;
      }
    }

    return true;
  }


private:
  void Modified()
  {
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

  std::string m_Value;
  STORM_CHANGE_NOTIFIER_INFO;
};

namespace std
{
  template <>
  struct hash<RString>
  {
    size_t operator()(const RString & val)
    {
      return val.hash();
    }
  };
}
