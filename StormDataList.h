#pragma once

#include <vector>

#include <optional/optional.hpp>

#include "StormData.h"
#include "StormDataChangeNotifier.h"
#include "StormDataParent.h"

template <class T>
class RSparseList
{
public:

  using ContainerData = std::experimental::optional<T>;

  struct RSparseListIterator
  {
    RSparseListIterator(const RSparseListIterator & rhs) : m_List(rhs.m_List), m_PhysicalIndex(rhs.m_PhysicalIndex) { }

    bool operator != (const RSparseListIterator & rhs) const
    {
      if (m_List != rhs.m_List)
      {
        return true;
      }

      return m_PhysicalIndex != rhs.m_PhysicalIndex;
    }

    bool operator == (const RSparseListIterator & rhs) const
    {
      if (m_List != rhs.m_List)
      {
        return false;
      }

      return m_PhysicalIndex == rhs.m_PhysicalIndex;
    }

    const std::pair<std::size_t, T &> operator *() const
    {
      std::pair<std::size_t, T &> val(m_PhysicalIndex, *m_List->m_Values[m_PhysicalIndex]);
      return val;
    }

    const std::pair<std::size_t, T &> operator ->() const
    {
      std::pair<std::size_t, T &> val(m_PhysicalIndex, *m_List->m_Values[m_PhysicalIndex]);
      return val;
    }

    void operator++()
    {
      do
      {
        m_PhysicalIndex++;
      } while (m_PhysicalIndex < m_List->m_Values.size() && ((bool)m_List->m_Values[m_PhysicalIndex]) == false);
    }

  private:

    RSparseListIterator(RSparseList<T> * list, std::size_t physical_index) : m_List(list), m_PhysicalIndex(physical_index) { }

    std::size_t m_PhysicalIndex = 0;
    RSparseList<T> * m_List;

    friend class RSparseList<T>;
  };

  struct RSparseListIteratorConst
  {
    RSparseListIteratorConst(const RSparseListIteratorConst & rhs) : m_List(rhs.m_List), m_PhysicalIndex(rhs.m_PhysicalIndex) { }

    bool operator != (const RSparseListIteratorConst & rhs) const
    {
      if (m_List != rhs.m_List)
      {
        return true;
      }

      return m_PhysicalIndex != rhs.m_PhysicalIndex;
    }

    bool operator == (const RSparseListIteratorConst & rhs) const
    {
      if (m_List != rhs.m_List)
      {
        return false;
      }

      return m_PhysicalIndex == rhs.m_PhysicalIndex;
    }

    const std::pair<std::size_t, const T &> operator *() const
    {
      std::pair<std::size_t, const T &> val(m_PhysicalIndex, *m_List->m_Values[m_PhysicalIndex]);
      return val;
    }

    const std::pair<std::size_t, T &> operator ->() const
    {
      std::pair<std::size_t, T &> val(m_PhysicalIndex, *m_List->m_Values[m_PhysicalIndex].m_T);
      return val;
    }

    void operator++()
    {
      do
      {
        m_PhysicalIndex++;
      } while (m_PhysicalIndex < m_List->m_Values.size() && ((bool)m_List->m_Values[m_PhysicalIndex]) == false);
    }

  private:

    RSparseListIteratorConst(const RSparseList<T> * list, std::size_t physical_index) : m_List(list), m_PhysicalIndex(physical_index) { }

    std::size_t m_PhysicalIndex = 0;
    const RSparseList<T> * m_List;

    friend class RSparseList<T>;
  };

  T & operator[](std::size_t index)
  {
    return *m_Values[index];
  }

  void Clear()
  {
    m_Values.clear();
    m_HighestIndex = 0;
    Cleared();
  }

  void Reserve(std::size_t size)
  {
    m_Values.reserve(size);
  }

  void PushBack(const T & val)
  {
    m_Values.push_back(ContainerData(val));
    m_HighestIndex = m_Values.size();

    Inserted(m_Values.size() - 1);
  }

  T & InsertAt(const T & val, std::size_t logical_index)
  {
    m_Values.reserve(logical_index + 1);

    for (std::size_t index = m_Values.size(); index <= logical_index; index++)
    {
      m_Values.push_back(ContainerData());
    }

    m_Values[logical_index] = ContainerData(val);
    m_HighestIndex = std::max(m_HighestIndex, logical_index);

    Inserted(logical_index);
    return *m_Values[logical_index];
  }

  void RemoveAt(std::size_t logical_index)
  {
    m_Values[logical_index] = ContainerData{};

    if (m_HighestIndex == logical_index)
    {
      for (std::size_t index = m_Values.size() - 1; index >= 0 && ((bool)m_Values[index]) == false; index--)
      {
        m_HighestIndex--;
      }
    }

    Removed(logical_index);
  }

  std::size_t HighestIndex() const
  {
    return m_HighestIndex;
  }

  std::size_t Size() const
  {
    return m_Values.size();
  }

  void Compress()
  {
    std::vector<ContainerData> new_vals;
    new_vals.reserve(m_Values.size());

    for (std::size_t index = 0; index < m_Values.size(); index++)
    {
      if (m_Values[index])
      {
        new_vals.push_back(ContainerData(m_Values[index]));
      }
    }

    m_Values = new_vals;
    Compressed();
  }

  RSparseListIterator begin()
  {
    std::size_t start_index = 0;
    while (start_index < m_HighestIndex && ((bool)m_Values[start_index]) == false)
    {
      start_index++;
    }

    RSparseListIterator itr(this, start_index);
    return itr;
  }

  RSparseListIterator end()
  {
    RSparseListIterator itr(this, m_HighestIndex);
    return itr;
  }

  RSparseListIteratorConst begin() const
  {
    std::size_t start_index = 0;
    while (start_index < m_HighestIndex && ((bool)m_Values[start_index]) == false)
    {
      start_index++;
    }

    RSparseListIteratorConst itr(this, start_index);
    return itr;
  }

  RSparseListIteratorConst end() const
  {
    RSparseListIteratorConst itr(this, m_HighestIndex);
    return itr;
  }

  bool operator == (const RSparseList<T> & rhs) const
  {
    if (m_Values.size() != rhs.m_Values.size())
    {
      return false;
    }

    auto itr1 = begin();
    auto itr2 = rhs.begin();

    auto last = end();

    while (itr1 != last)
    {
      if (itr1->first == itr2->first && itr1->second == itr2->second)
      {
        ++itr1;
        ++itr2;
        continue;
      }

      return false;
    }

    return true;
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

  void Compressed()
  {
#ifdef STORM_CHANGE_NOTIFIER

    StormReflectionParentInfo new_info;
    new_info.m_ParentInfo = &m_ReflectionInfo;
    new_info.m_MemberName = nullptr;

    for (uint32_t index = 0; index < m_Values.size(); index++)
    {
      new_info.m_ParentIndex = index;
      SetParentInfo(*m_Values[index], new_info);
    }

    if (DoReflectionCallback() == false)
    {
      return;
    }

    ReflectionNotifyCompress(m_ReflectionInfo);
#endif
  }

  void Inserted(std::size_t logical_index)
  {
#ifdef STORM_CHANGE_NOTIFIER

    StormReflectionParentInfo new_info;
    new_info.m_ParentInfo = &m_ReflectionInfo;
    new_info.m_MemberName = nullptr;
    new_info.m_ParentIndex = logical_index;
    SetParentInfo(*m_Values[logical_index], new_info);

    if (DoReflectionCallback() == false)
    {
      return;
    }

    std::string data;
    StormReflEncodeJson(*m_Values[logical_index], data);

    ReflectionNotifyInsertObject(m_ReflectionInfo, logical_index, data);
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

  std::size_t m_HighestIndex = 0;
  std::vector<ContainerData> m_Values;
  STORM_CHANGE_NOTIFIER_INFO;
};

template <class T>
class RMergeList
{
public:

  struct RMergeListIterator
  {
    RMergeListIterator(const RMergeListIterator & rhs) : m_List(rhs.m_List), m_PhysicalIndex(rhs.m_PhysicalIndex) { }

    bool operator == (const RMergeListIterator & rhs) const
    {
      if (m_List != rhs.m_List)
      {
        return false;
      }

      return m_PhysicalIndex == rhs.m_PhysicalIndex;
    }

    bool operator != (const RMergeListIterator & rhs) const
    {
      if (m_List != rhs.m_List)
      {
        return true;
      }

      return m_PhysicalIndex != rhs.m_PhysicalIndex;
    }

    const std::pair<int, T &> operator *() const
    {
      std::pair<int, T &> val(m_List->m_Indices[m_PhysicalIndex], m_List->m_Values[m_PhysicalIndex]);
      return val;
    }

    const std::pair<int, T &> operator ->() const
    {
      std::pair<int, T &> val(m_List->m_Indices[m_PhysicalIndex], m_List->m_Values[m_PhysicalIndex]);
      return val;
    }

    RMergeListIterator & operator++()
    {
      m_PhysicalIndex++;
      return *this;
    }

  private:

    RMergeListIterator(RMergeList<T> * list, std::size_t physical_index) : m_List(list), m_PhysicalIndex(physical_index) { }

    int m_PhysicalIndex = 0;
    RMergeList<T> * m_List;

    friend class RMergeList<T>;
  };

  struct RMergeListIteratorConst
  {
    RMergeListIteratorConst(const RMergeListIteratorConst & rhs) : m_List(rhs.m_List), m_PhysicalIndex(rhs.m_PhysicalIndex) { }

    bool operator == (const RMergeListIteratorConst & rhs) const
    {
      if (m_List != rhs.m_List)
      {
        return false;
      }

      return m_PhysicalIndex == rhs.m_PhysicalIndex;
    }

    bool operator != (const RMergeListIteratorConst & rhs) const
    {
      if (m_List != rhs.m_List)
      {
        return true;
      }

      return m_PhysicalIndex != rhs.m_PhysicalIndex;
    }

    const std::pair<int, const T &> operator *() const
    {
      std::pair<int, const T &> val(m_List->m_Indices[m_PhysicalIndex], m_List->m_Values[m_PhysicalIndex]);
      return val;
    }

    const std::pair<int, T &> operator ->() const
    {
      std::pair<int, T &> val(m_List->m_Indices[m_PhysicalIndex], m_List->m_Values[m_PhysicalIndex]);
      return val;
    }

    RMergeListIteratorConst & operator++()
    {
      m_PhysicalIndex++;
      return *this;
    }

  private:

    RMergeListIteratorConst(const RMergeList<T> * list, std::size_t physical_index) : m_List(list), m_PhysicalIndex(physical_index) { }

    int m_PhysicalIndex = 0;
    const RMergeList<T> * m_List;

    friend class RMergeList<T>;
  };

  void Clear()
  {
    m_Indices.clear();
    m_Values.clear();

    m_HighestIndex = 0;
    Cleared();
  }

  void Reserve(std::size_t size)
  {
    m_Indices.reserve(size);
    m_Values.reserve(size);
  }

  T & PushBack(const T & val)
  {
    m_Indices.push_back(m_HighestIndex);
    m_Values.push_back(val);

    m_HighestIndex++;
    Inserted(m_HighestIndex - 1, m_Indices.size() - 1);
    return m_Values.back();
  }

  T & InsertAt(const T & val, std::size_t logical_index)
  {
    if (logical_index == m_HighestIndex)
    {
      return PushBack(val);
    }

    if (logical_index > m_HighestIndex)
    {
      m_Indices.push_back(logical_index);
      m_Values.push_back(val);
      m_HighestIndex = logical_index + 1;

      Inserted(logical_index, m_Indices.size() - 1);
      return m_Values.back();
    }

    size_t array_size = m_Indices.size();
    for (size_t test = 0; test < array_size; test++)
    {
      if (m_Indices[test] == logical_index)
      {
        m_Values[test] = val;
        return m_Values[test];
      }

      if (m_Indices[test] > logical_index)
      {
        m_Indices.insert(m_Indices.begin() + test, logical_index);
        m_Values.insert(m_Values.begin() + test, val);

        Inserted(logical_index, test);
        return m_Values[test];
      }
    }

    return m_Values[0];
  }

  void RemoveAt(std::size_t logical_index)
  {
    size_t array_size = m_Indices.size();
    for (size_t test = 0; test < array_size; test++)
    {
      if (m_Indices[test] == logical_index)
      {
        m_Indices.erase(m_Indices.begin() + test);
        m_Values.erase(m_Values.begin() + test);

        Removed(logical_index);
        return;
      }
    }
  }

  std::size_t HighestIndex()
  {
    return m_HighestIndex;
  }

  void Compress()
  {
    for (std::size_t index = 0; index < m_Indices.size(); index++)
    {
      m_Indices[index] = index;
    }

    Compressed();
  }

  T & operator[](int logical_index)
  {
    size_t array_size = m_Indices.size();
    for (size_t test = 0; test < array_size; test++)
    {
      if (m_Indices[test] == logical_index)
      {
        return m_Values[test];
      }
    }

    return m_Values[-1];
  }

  RMergeListIterator begin()
  {
    RMergeListIterator itr(this, 0);
    return itr;
  }

  RMergeListIterator end()
  {
    RMergeListIterator itr(this, m_Values.size());
    return itr;
  }

  RMergeListIteratorConst begin() const
  {
    RMergeListIteratorConst itr(this, 0);
    return itr;
  }

  RMergeListIteratorConst end() const
  {
    RMergeListIteratorConst itr(this, m_Values.size());
    return itr;
  }

  bool operator == (const RMergeList<T> & rhs) const
  {
    if (m_Indices.size() != rhs.m_Indices.size())
    {
      return false;
    }

    auto itr1 = begin();
    auto itr2 = rhs.begin();

    auto last = end();

    while (itr1 != last)
    {
      if ((*itr1).first == (*itr2).first && (*itr1).second == (*itr2).second)
      {
        ++itr1;
        ++itr2;
        continue;
      }

      return false;
    }

    return true;
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

  void Compressed()
  {
#ifdef STORM_CHANGE_NOTIFIER
    if (DoReflectionCallback() == false)
    {
      return;
    }

    ReflectionNotifyCompress(m_ReflectionInfo);
#endif
  }

  void Inserted(std::size_t logical_index, std::size_t physical_index)
  {
#ifdef STORM_CHANGE_NOTIFIER

    StormReflectionParentInfo new_info;
    new_info.m_ParentInfo = &m_ReflectionInfo;
    new_info.m_ParentIndex = logical_index;
    new_info.m_MemberName = nullptr;

    SetParentInfo(m_Values[logical_index], new_info);

    if (DoReflectionCallback() == false)
    {
      return;
    }

    std::string data;
    StormReflEncodeJson(m_Values[physical_index], data);
    ReflectionNotifyInsertObject(m_ReflectionInfo, logical_index, data);
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

  std::size_t m_HighestIndex = 0;

  std::vector<uint32_t> m_Indices;
  std::vector<T> m_Values;

  STORM_CHANGE_NOTIFIER_INFO;
};
