#pragma once

#include <cstdlib>

#include "StormData.h"
#include "StormDataChangeNotifier.h"
#include "StormDataParent.h"
#include "StormDataUtil.h"

template <class T>
class RMergeList
{
public:

  using ContainerType = T;

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

    std::pair<std::size_t, T &> operator *() const
    {
      return std::pair<std::size_t, T &>(m_List->m_Indices[m_PhysicalIndex], m_List->m_Values[m_PhysicalIndex]);
    }

    StormDataHelpers::RTempPair<std::size_t, T &> operator ->() const
    {
      return StormDataHelpers::RTempPair<std::size_t, T &>(m_List->m_Indices[m_PhysicalIndex], m_List->m_Values[m_PhysicalIndex]);
    }

    RMergeListIterator & operator++()
    {
      m_PhysicalIndex++;
      return *this;
    }

  private:

    RMergeListIterator(RMergeList<T> * list, std::size_t physical_index) : m_List(list), m_PhysicalIndex(physical_index) { }

    std::size_t m_PhysicalIndex;
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

    std::pair<std::size_t, const T &> operator *() const
    {
      return std::pair<std::size_t, const T &>(m_List->m_Indices[m_PhysicalIndex], m_List->m_Values[m_PhysicalIndex]);
    }

    StormDataHelpers::RTempPair<std::size_t, const T &> operator ->() const
    {
      return StormDataHelpers::RTempPair<std::size_t, const T &>(m_List->m_Indices[m_PhysicalIndex], m_List->m_Values[m_PhysicalIndex]);
    }

    RMergeListIteratorConst & operator++()
    {
      m_PhysicalIndex++;
      return *this;
    }

  private:

    RMergeListIteratorConst(const RMergeList<T> * list, std::size_t physical_index) : m_List(list), m_PhysicalIndex((int)physical_index) { }

    std::size_t m_PhysicalIndex;
    const RMergeList<T> * m_List;

    friend class RMergeList<T>;
  };

  RMergeList() :
    m_HighestIndex(-1),
    m_Capacity(0),
    m_Size(0),
    m_Indices(nullptr),
    m_Values(nullptr)
  {

  }

  RMergeList(const RMergeList<T> & rhs) :
    m_HighestIndex(rhs.m_HighestIndex),
    m_Capacity(rhs.m_Capacity),
    m_Size(rhs.m_Size)
  {
    if (rhs.m_Size > 0)
    {
      m_Indices = Allocate<uint32_t>(rhs.m_Capacity);
      m_Values = Allocate<T>(rhs.m_Capacity);

      for (std::size_t index = 0; index < rhs.m_Size; index++)
      {
        m_Indices[index] = rhs.m_Indices[index];
        new(&m_Values[index]) T(rhs.m_Values[index]);
      }

      InitAllElements();
    }
    else
    {
      m_Indices = nullptr;
      m_Values = nullptr;
    }
  }

  RMergeList(RMergeList<T> && rhs) :
    m_HighestIndex(rhs.m_HighestIndex),
    m_Capacity(rhs.m_Capacity),
    m_Size(rhs.m_Size),
    m_Indices(rhs.m_Indices),
    m_Values(rhs.m_Values)
  {
    rhs.m_HighestIndex = -1;
    rhs.m_Capacity = 0;
    rhs.m_Size = 0;
    rhs.m_Indices = nullptr;
    rhs.m_Values = nullptr;

#ifdef STORM_CHANGE_NOTIFIER
    m_ReflectionInfo = rhs.m_ReflectionInfo;
    rhs.m_ReflectionInfo = {};
#endif

    UpdateAllElements();
  }

#ifdef STORM_CHANGE_NOTIFIER
  RMergeList(RMergeList<T> && rhs, StormReflectionParentInfo * new_parent) :
    m_HighestIndex(rhs.m_HighestIndex),
    m_Capacity(rhs.m_Capacity),
    m_Size(rhs.m_Size),
    m_Indices(rhs.m_Indices),
    m_Values(rhs.m_Values)
  {
    rhs.m_HighestIndex = -1;
    rhs.m_Capacity = 0;
    rhs.m_Size = 0;
    rhs.m_Indices = nullptr;
    rhs.m_Values = nullptr;

    m_ReflectionInfo = rhs.m_ReflectionInfo;
    m_ReflectionInfo.m_ParentInfo = new_parent;

    UpdateAllElements();
  }
#endif

  RMergeList(std::initializer_list<T> l) :
    RMergeList()
  {
    m_Indices = Allocate<uint32_t>(l.size());
    m_Values = Allocate<T>(l.size());
    m_Capacity = l.size();
    m_Size = l.size();
    m_HighestIndex = (int)l.size() - 1;

    auto itr = l.begin();
    for (std::size_t index = 0; index < l.size(); index++)
    {
      m_Indices[index] = (int)index;
      new(&m_Values[index]) T(*itr);

      ++itr;
    }

    InitAllElements();
  }

  ~RMergeList()
  {
    if (m_Capacity > 0)
    {
      for (std::size_t index = 0; index < m_Size; index++)
      {
        m_Values[index].~T();
      }

      Deallocate(m_Values);
      Deallocate(m_Indices);
    }
  }

  RMergeList<T> & operator = (const RMergeList<T> & rhs)
  {
    Copy(rhs);
    Set();

    return *this;
  }

  RMergeList<T> & operator = (RMergeList<T> && rhs)
  {
    Move(std::move(rhs));
    Set();

    return *this;
  }

  void SetRaw(const RMergeList<T> & rhs)
  {
    Copy(rhs);
  }

  void SetRaw(RMergeList<T> && rhs)
  {
    Move(std::move(rhs));
  }

#ifdef STORM_CHANGE_NOTIFIER
  void Relocate(RMergeList<T> && rhs, StormReflectionParentInfo * new_parent)
  {
    m_ReflectionInfo = rhs.m_ReflectionInfo;
    m_ReflectionInfo.m_ParentInfo = new_parent;
    Move(std::move(rhs));
  }
#endif

  void Clear()
  {
    ClearRaw();
    Cleared();
  }

  void ClearRaw()
  {
    for (std::size_t index = 0; index < m_Size; index++)
    {
      m_Values[index].~T();
    }

    m_HighestIndex = -1;
    m_Size = 0;
  }

  void Reserve(std::size_t size)
  {
    if (size > m_Capacity)
    {
      Grow(size);
    }
  }

  T & PushBack(const T & val)
  {
    bool existing_elem;
    std::size_t physical_index;
    T * elem = InsertInternal(m_HighestIndex + 1, existing_elem, physical_index);
    new(elem) T(val);
    Inserted(m_HighestIndex, physical_index, false);

    return *elem;
  }

  template <typename ... InitArgs>
  T & EmplaceBack(InitArgs && ... args)
  {
    bool existing_elem;
    std::size_t physical_index;
    T * elem = InsertInternal(m_HighestIndex + 1, existing_elem, physical_index);
    new(elem) T(std::forward<InitArgs>(args)...);
    Inserted(m_HighestIndex, physical_index, sizeof...(InitArgs) == 0);

    return *elem;
  }

  T & InsertAt(std::size_t logical_index, const T & val)
  {
    bool existing_elem;
    std::size_t physical_index;
    T * elem = InsertInternal(logical_index, existing_elem, physical_index);
    if (existing_elem)
    {
      *elem = val;
    }
    else
    {
      new(elem) T(val);
      Inserted(logical_index, physical_index, false);
    }

    return *elem;
  }

  template <typename ... InitArgs>
  T & EmplaceAt(std::size_t logical_index, InitArgs && ... args)
  {
    bool existing_elem;
    std::size_t physical_index;
    T * elem = InsertInternal(logical_index, existing_elem, physical_index);
    if (existing_elem)
    {
      *elem = T(std::forward<InitArgs>(args)...);
    }
    else
    {
      new(elem) T(std::forward<InitArgs>(args)...);
      Inserted(logical_index, physical_index, sizeof...(InitArgs) == 0);
    }

    return *elem;
  }

  void Remove(const RMergeListIterator & itr)
  {
    RemoveInternal(itr.m_PhysicalIndex);
  }

  void Remove(const RMergeListIteratorConst & itr)
  {
    RemoveInternal(itr.m_PhysicalIndex);
  }

  bool RemoveAt(std::size_t logical_index)
  {
    for (size_t test = 0; test < m_Size; test++)
    {
      if (m_Indices[test] == logical_index)
      {
        RemoveInternal(test);
        return true;
      }
    }

    return false;
  }

  int HighestIndex() const
  {
    return m_HighestIndex;
  }

  std::size_t NumElements()
  {
    return m_Size;
  }

  void Compress()
  {
    for (std::size_t index = 0; index < m_Size; index++)
    {
      m_Indices[index] = (uint32_t)index;
    }

    UpdateAllElements();
    Compressed();
  }

  bool HasAt(std::size_t logical_index) const
  {
    for (size_t test = 0; test < m_Size; test++)
    {
      if (m_Indices[test] == logical_index)
      {
        return true;
      }

      if (m_Indices[test] > (uint32_t)logical_index)
      {
        return false;
      }
    }

    return false;
  }

  T * TryGet(int logical_index)
  {
    for (size_t test = 0; test < m_Size; test++)
    {
      if (m_Indices[test] == logical_index)
      {
        return &m_Values[test];
      }

      if (m_Indices[test] > (uint32_t)logical_index)
      {
        return nullptr;
      }
    }

    return nullptr;
  }

  const T * TryGet(int logical_index) const
  {
    for (size_t test = 0; test < m_Size; test++)
    {
      if (m_Indices[test] == logical_index)
      {
        return &m_Values[test];
      }

      if (m_Indices[test] >(uint32_t)logical_index)
      {
        return nullptr;
      }
    }

    return nullptr;
  }

  T & operator[](std::size_t logical_index)
  {
    for (size_t test = 0; test < m_Size; test++)
    {
      if (m_Indices[test] == logical_index)
      {
        return m_Values[test];
      }
    }

    throw std::out_of_range("No element at index");
  }

  const T & operator[](std::size_t logical_index) const
  {
    for (size_t test = 0; test < m_Size; test++)
    {
      if (m_Indices[test] == logical_index)
      {
        return m_Values[test];
      }
    }

    throw std::out_of_range("No element at index");
  }

  T & operator[](int logical_index)
  {
    for (size_t test = 0; test < m_Size; test++)
    {
      if (m_Indices[test] == logical_index)
      {
        return m_Values[test];
      }
    }

    throw std::out_of_range("No element at index");
  }

  const T & operator[](int logical_index) const
  {
    for (size_t test = 0; test < m_Size; test++)
    {
      if (m_Indices[test] == logical_index)
      {
        return m_Values[test];
      }
    }

    throw std::out_of_range("No element at index");
  }

  RMergeListIterator begin()
  {
    RMergeListIterator itr(this, 0);
    return itr;
  }

  RMergeListIterator end()
  {
    RMergeListIterator itr(this, m_Size);
    return itr;
  }

  RMergeListIteratorConst begin() const
  {
    RMergeListIteratorConst itr(this, 0);
    return itr;
  }

  RMergeListIteratorConst end() const
  {
    RMergeListIteratorConst itr(this, m_Size);
    return itr;
  }

  bool operator == (const RMergeList<T> & rhs) const
  {
    if (m_Size != rhs.m_Size)
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

  template <typename Elem>
  Elem * Allocate(std::size_t count)
  {
    auto ptr = (Elem *)malloc(sizeof(Elem) * count);
    if (ptr == nullptr)
    {
      throw std::bad_alloc();
    }

    return ptr;
  }

  void Deallocate(void * ptr)
  {
    free(ptr);
  }

  void Grow()
  {
    if (m_Capacity == 0)
    {
      Grow(1);
      return;
    }

    Grow(m_Capacity * 2);
  }

  void Grow(std::size_t requested_size)
  {
    auto indices = Allocate<uint32_t>(requested_size);
    auto values = Allocate<T>(requested_size);

    if (m_Capacity > 0)
    {
      for (std::size_t index = 0; index < m_Size; index++)
      {
        indices[index] = m_Indices[index];

#ifdef STORM_CHANGE_NOTIFIER
        StormDataRelocateConstruct(std::move(m_Values[index]), &values[index], &m_ReflectionInfo);
#else
        new (&values[index]) T(std::move(m_Values[index]));
#endif

        m_Values[index].~T();
      }
    }

    Deallocate(m_Indices);
    Deallocate(m_Values);

    m_Indices = indices;
    m_Values = values;
    m_Capacity = requested_size;

    UpdateAllElements();
  }

  void MoveForward(std::size_t start_index)
  {
    for (std::size_t index = m_Size; index > start_index; index--)
    {
      m_Indices[index] = m_Indices[index - 1];
#ifdef STORM_CHANGE_NOTIFIER
      StormDataRelocateConstruct(std::move(m_Values[index - 1]), &m_Values[index], &m_ReflectionInfo);
#else
      new (&m_Values[index]) T(std::move(m_Values[index - 1]));
#endif

      m_Values[index - 1].~T();
    }
  }

  void MoveBackward(std::size_t start_index)
  {
    m_Values[start_index].~T();
    for (std::size_t index = start_index; index < m_Size - 1; index++)
    {
      m_Indices[index] = m_Indices[index + 1];

#ifdef STORM_CHANGE_NOTIFIER
      StormDataRelocateConstruct(std::move(m_Values[index + 1]), &m_Values[index], &m_ReflectionInfo);
#else
      new (&m_Values[index]) T(std::move(m_Values[index + 1]));
#endif


      m_Values[index + 1].~T();
    }
  }

  void ShiftUp(std::size_t start_index, std::size_t end_index)
  {
    for (std::size_t pos = 0; pos < m_Size; ++pos)
    {
      if (m_Indices[pos] >= start_index)
      {
        while (m_Indices[pos] < end_index)
        {
          m_Indices[pos]++;
          pos++;
        }

        return;
      }
    }
  }

  void ShiftDown(std::size_t start_index, std::size_t end_index)
  {
    for (std::size_t pos = 0; pos < m_Size; ++pos)
    {
      if (m_Indices[pos] >= start_index)
      {
        while (m_Indices[pos] < end_index)
        {
          m_Indices[pos]--;
          pos++;
        }

        return;
      }
    }
  }

  T * InsertInternal(std::size_t logical_index, bool & exists, std::size_t & physical_index)
  {
    if (m_Capacity == m_Size)
    {
      Grow();
    }

    exists = false;

    if (m_HighestIndex < (int)logical_index)
    {
      m_Indices[m_Size] = (int)logical_index;
      m_HighestIndex = (int)logical_index;

      physical_index = m_Size;
      return &m_Values[m_Size++];
    }

    for (size_t test = 0; test < m_Size; test++)
    {
      if (m_Indices[test] == logical_index)
      {
        exists = true;
        return &m_Values[test];
      }

      if (m_Indices[test] > logical_index)
      {
        MoveForward(test);
        m_Size++;

        m_Indices[test] = (int)logical_index;
        physical_index = test;
        return &m_Values[test];
      }
    }

    throw std::logic_error("?");
  }

  void RemoveInternal(std::size_t physical_index)
  {
    size_t logical_index = m_Indices[physical_index];

    if (physical_index >= m_Size)
    {
      throw std::out_of_range("Removing invalid iterator");
    }

    MoveBackward(physical_index);
    m_Size--;

    m_HighestIndex = m_Size > 0 ? m_Indices[m_Size - 1] : -1;
    Removed(logical_index);
  }

  void InitAllElements()
  {
#ifdef STORM_CHANGE_NOTIFIER
    StormReflectionParentInfo new_info;
    new_info.m_ParentInfo = &m_ReflectionInfo;
    new_info.m_MemberName = nullptr;
    new_info.m_Flags = (m_ReflectionInfo.m_Callback != nullptr || (m_ReflectionInfo.m_Flags & (uint32_t)StormDataParentInfoFlags::kParentHasCallback) != 0) ?
                       (uint32_t)StormDataParentInfoFlags::kParentHasCallback : 0;

    for (std::size_t index = 0; index < m_Size; index++)
    {
      new_info.m_ParentIndex = m_Indices[index];
      SetParentInfo(m_Values[index], new_info);
    }
#endif
  }

  void UpdateAllElements()
  {
#ifdef STORM_CHANGE_NOTIFIER
    bool has_callback = m_ReflectionInfo.m_Callback != nullptr || (m_ReflectionInfo.m_Flags & (uint32_t)StormDataParentInfoFlags::kParentHasCallback) != 0;

    for (std::size_t index = 0; index < m_Size; index++)
    {
      m_Values[index].m_ReflectionInfo.m_ParentInfo = &m_ReflectionInfo;
      bool child_has_flag = (m_Values[index].m_ReflectionInfo.m_Flags & (uint32_t)StormDataParentInfoFlags::kParentHasCallback) != 0;

      if (child_has_flag)
      {
        if (has_callback == false)
        {
          SetParentInfoStruct<T>::ClearParentCallback(m_Values[index]);
        }
      }
      else
      {
        if (has_callback == true)
        {
          SetParentInfoStruct<T>::SetFlag(m_Values[index], StormDataParentInfoFlags::kParentHasCallback);
        }
      }
    }
#endif
  }

  void Set()
  {
#ifdef STORM_CHANGE_NOTIFIER
    if (DoNotifyCallback(m_ReflectionInfo) == false)
    {
      return;
    }

    ReflectionNotifySetObject(m_ReflectionInfo, StormReflEncodeJson(*this));
#endif
  }

  void Copy(const RMergeList<T> & rhs)
  {
    for (std::size_t index = 0; index < m_Size; index++)
    {
      m_Values[index].~T();
    }

    if (rhs.m_Size > m_Capacity)
    {
      Deallocate(m_Values);
      Deallocate(m_Indices);

      m_Indices = Allocate<uint32_t>(rhs.m_Capacity);
      m_Values = Allocate<T>(rhs.m_Capacity);

      m_Capacity = rhs.m_Capacity;
    }

    for (std::size_t index = 0; index < rhs.m_Size; index++)
    {
      m_Indices[index] = rhs.m_Indices[index];
      new(&m_Values[index]) T(rhs.m_Values[index]);
    }

    m_Size = rhs.m_Size;
    m_HighestIndex = rhs.m_HighestIndex;

    InitAllElements();
  }

  void Move(RMergeList<T> && rhs)
  {
    for (std::size_t index = 0; index < m_Size; index++)
    {
      m_Values[index].~T();
    }

    if (m_Capacity > 0)
    {
      Deallocate(m_Values);
      Deallocate(m_Indices);
    }

    m_HighestIndex = rhs.m_HighestIndex;
    m_Indices = rhs.m_Indices;
    m_Values = rhs.m_Values;
    m_Size = rhs.m_Size;
    m_Capacity = rhs.m_Capacity;

    rhs.m_HighestIndex = -1;
    rhs.m_Size = 0;
    rhs.m_Capacity = 0;
    rhs.m_Indices = nullptr;
    rhs.m_Values = nullptr;

    UpdateAllElements();
  }

  void Cleared()
  {
#ifdef STORM_CHANGE_NOTIFIER
    if (DoNotifyCallback(m_ReflectionInfo) == false)
    {
      return;
    }

    ReflectionNotifyClearObject(m_ReflectionInfo);
#endif
  }

  void Compressed()
  {
#ifdef STORM_CHANGE_NOTIFIER
    if (DoNotifyCallback(m_ReflectionInfo) == false)
    {
      return;
    }

    ReflectionNotifyCompress(m_ReflectionInfo);
#endif
  }

  void Inserted(std::size_t logical_index, std::size_t physical_index, bool minimal)
  {
#ifdef STORM_CHANGE_NOTIFIER

    StormReflectionParentInfo new_info;
    new_info.m_ParentInfo = &m_ReflectionInfo;
    new_info.m_ParentIndex = logical_index;
    new_info.m_MemberName = nullptr;
    new_info.m_Flags = (m_ReflectionInfo.m_Callback != nullptr || (m_ReflectionInfo.m_Flags & (uint32_t)StormDataParentInfoFlags::kParentHasCallback) != 0) ?
      (uint32_t)StormDataParentInfoFlags::kParentHasCallback : 0;

    SetParentInfo(m_Values[physical_index], new_info);

    if (DoNotifyCallback(m_ReflectionInfo) == false)
    {
      return;
    }

    std::string data;

#ifdef STORM_CHANGE_MINIMAL
    if (minimal)
    {
      StormReflSerializeDefaultJson(m_Values[physical_index], data);
    }
    else
    {
      StormReflEncodeJson(m_Values[physical_index], data);
    }
#else
    StormReflEncodeJson(m_Values[physical_index], data);
#endif

    ReflectionNotifyInsertObject(m_ReflectionInfo, logical_index, data);
#endif
  }

  void Removed(std::size_t logical_index)
  {
#ifdef STORM_CHANGE_NOTIFIER
    if (DoNotifyCallback(m_ReflectionInfo) == false)
    {
      return;
    }

    ReflectionNotifyRemoveObject(m_ReflectionInfo, logical_index);
#endif
  }

  template <typename Type, typename Enable>
  friend struct StormDataJson;

  int m_HighestIndex;

  std::size_t m_Capacity;
  std::size_t m_Size;

  uint32_t * m_Indices;
  T * m_Values;

  STORM_CHANGE_NOTIFIER_INFO;
};

