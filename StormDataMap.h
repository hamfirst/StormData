#pragma once

#include <unordered_map>

#include <optional/optional.hpp>

#include "StormData.h"
#include "StormDataChangeNotifier.h"

template <class K, class T>
class RMap
{
public:
  template <class T>
  using Optional = std::experimental::optional<T>;

  bool operator == (const RMap<K, T> & rhs) const
  {
    if (rhs.Size() != this->Size())
    {
      return false;
    }

    auto it1 = rhs.begin();
    auto it2 = this->begin();

    auto last = this->end();
    while (it2 != last)
    {
      if (it1->first == it2->first && it1->second == it2->second)
      {
        ++it1;
        ++it2;
        continue;
      }

      return false;
    }

    return true;
  }

  void Clear()
  {
    m_Values.clear();
    Cleared();
  }

  void Reserve(size_t size)
  {
    m_Values.reserve(size);
  }

  T & Set(const K & k, T && t)
  {
    auto & result = m_Values.insert_or_assign(k, t);
    if (result.second)
    {
      Inserted(k, result.first->second);
    }

    return result.first->second;
  }

  void Remove(const K & k)
  {
    auto itr = m_Values.find(k);
    if (itr == m_Values.end())
    {
      return;
    }

    m_Values.erase(itr);
    Removed(k);
  }

  Optional<T> Get(const K & k)
  {
    auto itr = m_Values.find(k);
    if (itr == m_Values.end())
    {
      return Optional<T>();
    }

    return itr->second;
  }

  T & operator [] (const K & k)
  {
    return m_Values[k];
  }

  const T & operator [] (const K & k) const
  {
    return m_Values[k];
  }

  std::size_t Size() const
  {
    return m_Values.size();
  }

  auto begin()
  {
    return m_Values.begin();
  }

  auto end()
  {
    return m_Values.end();
  }

  auto begin() const
  {
    return m_Values.begin();
  }

  auto end() const
  {
    return m_Values.end();
  }

private:

  void Cleared()
  {
#ifdef STORM_CHANGE_NOTIFIER
    if (DoReflectionCallback() == false)
    {
      return;
    }

    ReflectionNotifyClearObject(m_ReflectionInfo);
#endif
  }

  void Inserted(std::size_t key, T & value)
  {
#ifdef STORM_CHANGE_NOTIFIER

    StormReflectionParentInfo new_info;
    new_info.m_ParentInfo = &m_ReflectionInfo;
    new_info.m_MemberName = nullptr;
    new_info.m_ParentIndex = key;
    SetParentInfo(value, new_info);

    if (DoReflectionCallback() == false)
    {
      return;
    }

    std::string data;
    StormReflEncodeJson(value, data);

    ReflectionNotifyInsertObject(m_ReflectionInfo, key, data);
#endif
  }

  void Removed(std::size_t logical_index)
  {
#ifdef STORM_CHANGE_NOTIFIER
    if (DoReflectionCallback() == false)
    {
      return;
    }

    ReflectionNotifyRemoveObject(m_ReflectionInfo, logical_index);
#endif
  }

  std::unordered_map<K, T> m_Values;
  STORM_CHANGE_NOTIFIER_INFO;
};
