
#include <memory>

#include "StormData.h"
#include "StormDataChangeNotifier.h"
#include "StormDataParent.h"
#include "StormDataUtil.h"

template <class T>
class RSparseList
{
public:
  using ContainerType = T;

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

    std::pair<std::size_t, T &> operator *() const
    {
      return std::pair<std::size_t, T &>((std::size_t)m_PhysicalIndex, m_List->m_Values[m_PhysicalIndex].m_Value);
    }

    StormDataHelpers::RTempPair<std::size_t, T &> operator ->() const
    {
      return StormDataHelpers::RTempPair<std::size_t, T &>((std::size_t)m_PhysicalIndex, m_List->m_Values[m_PhysicalIndex].m_Value);
    }

    void operator++()
    {
      do
      {
        m_PhysicalIndex++;
      } while (m_PhysicalIndex <= m_List->m_HighestIndex && m_List->m_Values[m_PhysicalIndex].m_Valid == false);
    }

  private:

    RSparseListIterator(RSparseList<T> * list, int physical_index) : m_List(list), m_PhysicalIndex(physical_index) { }

    int m_PhysicalIndex;
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

    std::pair<std::size_t, const T &> operator *() const
    {
      return std::pair<std::size_t, const T &>((std::size_t)m_PhysicalIndex, m_List->m_Values[m_PhysicalIndex].m_Value);
    }

    StormDataHelpers::RTempPair<std::size_t, const T &> operator ->() const
    {
      return StormDataHelpers::RTempPair<std::size_t, const T &>((std::size_t)m_PhysicalIndex, m_List->m_Values[m_PhysicalIndex].m_Value);
    }

    void operator++()
    {
      do
      {
        m_PhysicalIndex++;
      } while (m_PhysicalIndex <= m_List->m_HighestIndex && m_List->m_Values[m_PhysicalIndex].m_Valid == false);
    }

  private:

    RSparseListIteratorConst(const RSparseList<T> * list, int physical_index) : m_List(list), m_PhysicalIndex(physical_index) { }

    int m_PhysicalIndex;
    const RSparseList<T> * m_List;

    friend class RSparseList<T>;
  };

  RSparseList() :
    m_HighestIndex(-1),
    m_Capacity(0),
    m_Values(nullptr)
  {

  }

  RSparseList(const RSparseList<T> & rhs) :
    m_HighestIndex(rhs.m_HighestIndex),
    m_Capacity(rhs.m_Capacity)
  {
    if (rhs.m_Capacity > 0)
    {
      m_Values = Allocate<ContainerData>(rhs.m_Capacity);

      for (std::size_t index = 0; index < rhs.m_Capacity; index++)
      {
        m_Values[index].m_Valid = rhs.m_Values[index].m_Valid;
        if (rhs.m_Values[index].m_Valid)
        {
          new(&m_Values[index].m_Value) T(rhs.m_Values[index].m_Value);
        }
      }

      InitAllElements();
    }
    else
    {
      m_Values = nullptr;
    }
  }

  RSparseList(RSparseList<T> && rhs) :
    m_HighestIndex(rhs.m_HighestIndex),
    m_Capacity(rhs.m_Capacity),
    m_Values(rhs.m_Values)
  {
    rhs.m_HighestIndex = -1;
    rhs.m_Capacity = 0;
    rhs.m_Values = nullptr;

#ifdef STORM_CHANGE_NOTIFIER
    m_ReflectionInfo = rhs.m_ReflectionInfo;
    rhs.m_ReflectionInfo = {};
#endif

    UpdateAllElements();
  }

#ifdef STORM_CHANGE_NOTIFIER
  RSparseList(RSparseList<T> && rhs, StormReflectionParentInfo * new_parent) :
    m_HighestIndex(rhs.m_HighestIndex),
    m_Capacity(rhs.m_Capacity),
    m_Values(rhs.m_Values)
  {
    rhs.m_HighestIndex = -1;
    rhs.m_Capacity = 0;
    rhs.m_Values = nullptr;

    m_ReflectionInfo = rhs.m_ReflectionInfo;
    m_ReflectionInfo.m_ParentInfo = new_parent;

    UpdateAllElements();
  }
#endif

  ~RSparseList()
  {
    DestroyAllElements();

    if (m_Capacity > 0)
    {
      Deallocate(m_Values);
    }
  }

  RSparseList<T> & operator = (const RSparseList<T> & rhs)
  {
    Copy(rhs);
    Set();

    return *this;
  }

  RSparseList<T> & operator = (RSparseList<T> && rhs)
  {
    Move(std::move(rhs));
    Set();

    return *this;
  }

  void SetRaw(const RSparseList<T> & rhs)
  {
    Copy(rhs);
  }

  void SetRaw(RSparseList<T> && rhs)
  {
    Move(std::move(rhs));
  }

#ifdef STORM_CHANGE_NOTIFIER
  void Relocate(RSparseList<T> && rhs, StormReflectionParentInfo * new_parent)
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
    DestroyAllElementsAndInvalidate();

    m_HighestIndex = -1;
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
    if (m_HighestIndex + 1 == m_Capacity)
    {
      Grow();
    }

    m_HighestIndex++;

    m_Values[m_HighestIndex].m_Valid = true;
    new(&m_Values[m_HighestIndex].m_Value) T(val);

    Inserted(m_HighestIndex, false);
    return m_Values[m_HighestIndex].m_Value;
  }

  template <typename ... InitArgs>
  T & EmplaceBack(InitArgs && ... args)
  {
    if (m_HighestIndex + 1 == m_Capacity)
    {
      Grow();
    }

    m_HighestIndex++;

    m_Values[m_HighestIndex].m_Valid = true;
    new(&m_Values[m_HighestIndex].m_Value) T(std::forward<InitArgs>(args)...);

    Inserted(m_HighestIndex, sizeof...(InitArgs) == 0);
    return m_Values[m_HighestIndex].m_Value;
  }

  T & InsertAt(std::size_t logical_index, const T & val)
  {
    GrowToFit(logical_index);

    m_HighestIndex = m_HighestIndex == -1 || logical_index > (std::size_t)m_HighestIndex ? (int)logical_index : m_HighestIndex;

    if (m_Values[logical_index].m_Valid)
    {
      m_Values[logical_index].m_Value = val;
    }
    else
    {
      m_Values[logical_index].m_Valid = true;
      new (&m_Values[logical_index].m_Value) T(val);
      Inserted(logical_index, false);
    }

    return m_Values[logical_index].m_Value;
  }

  template <typename ... InitArgs>
  T & EmplaceAt(std::size_t logical_index, InitArgs && ... args)
  {
    GrowToFit(logical_index);

    m_HighestIndex = m_HighestIndex == -1 || logical_index > (std::size_t)m_HighestIndex ? (int)logical_index : m_HighestIndex;

    if (m_Values[logical_index].m_Valid)
    {
      m_Values[logical_index].m_Value = T(std::forward<InitArgs>(args)...);
    }
    else
    {
      m_Values[logical_index].m_Valid = true;
      new (&m_Values[logical_index].m_Value) T(std::forward<InitArgs>(args)...);
      Inserted(logical_index, sizeof...(InitArgs) == 0);
    }

    return m_Values[logical_index].m_Value;
  }

  void Remove(const RSparseListIterator & itr)
  {
    RemoveAt(itr.m_PhysicalIndex);
  }

  void Remove(const RSparseListIteratorConst & itr)
  {
    RemoveAt(itr.m_PhysicalIndex);
  }

  void RemoveAt(std::size_t logical_index)
  {
    if (logical_index >= m_Capacity)
    {
      return;
    }

    if (m_Values[logical_index].m_Valid)
    {
      m_Values[logical_index].m_Value.~T();
      m_Values[logical_index].m_Valid = false;
      while (m_HighestIndex >= 0 && m_Values[m_HighestIndex].m_Valid == false)
      {
        m_HighestIndex--;
      }

      Removed(logical_index);
    }
  }

  int HighestIndex() const
  {
    return m_HighestIndex;
  }

  std::size_t Size() const
  {
    return m_Capacity;
  }

  void Compress()
  {
    int empty_index = -1;
    for (std::size_t index = 0; index < m_Capacity; index++)
    {
      if (m_Values[index].m_Valid)
      {
        if (empty_index != -1)
        {
          new (&m_Values[empty_index].m_Value) T(std::move(m_Values[index].m_Value));
          m_Values[index].m_Value.~T();
          m_Values[empty_index].m_Valid = true;
          m_Values[index].m_Valid = false;

          empty_index++;
        }
      }
      else
      {
        if (empty_index == -1)
        {
          empty_index = (int)index;
        }
      }
    }

    Compressed();
  }

  bool HasAt(std::size_t index) const
  {
    if (m_HighestIndex == -1)
    {
      return false;
    }

    if (index > (std::size_t)m_HighestIndex)
    {
      return false;
    }

    if (m_Values[index].m_Valid == false)
    {
      return false;
    }

    return true;
  }

  T & operator[](std::size_t index)
  {
    if (m_HighestIndex == -1)
    {
      throw std::out_of_range("Invalid index");
    }

    if (index > (std::size_t)m_HighestIndex)
    {
      throw std::out_of_range("Invalid index");
    }

    if (m_Values[index].m_Valid == false)
    {
      throw std::out_of_range("Invalid index");
    }

    return m_Values[index].m_Value;
  }

  const T & operator[](std::size_t index) const
  {
    if (m_HighestIndex == -1)
    {
      throw std::out_of_range("Invalid index");
    }

    if (index > (std::size_t)m_HighestIndex)
    {
      throw std::out_of_range("Invalid index");
    }

    if (m_Values[index].m_Valid == false)
    {
      throw std::out_of_range("Invalid index");
    }

    return m_Values[index].m_Value;
  }

  T * TryGet(std::size_t index)
  {
    if (m_HighestIndex == -1)
    {
      return nullptr;
    }

    if (index > (std::size_t)m_HighestIndex)
    {
      return nullptr;
    }

    if (m_Values[index].m_Valid == false)
    {
      return nullptr;
    }

    return &m_Values[index].m_Value;
  }

  const T * TryGet(std::size_t index) const
  {
    if (m_HighestIndex == -1)
    {
      return nullptr;
    }

    if (index > (std::size_t)m_HighestIndex)
    {
      return nullptr;
    }

    if (m_Values[index].m_Valid == false)
    {
      return nullptr;
    }

    return &m_Values[index].m_Value;
  }

  RSparseListIterator begin()
  {
    int start_index = 0;
    while (start_index <= m_HighestIndex && m_Values[start_index].m_Valid == false)
    {
      start_index++;
    }

    RSparseListIterator itr(this, start_index);
    return itr;
  }

  RSparseListIterator end()
  {
    RSparseListIterator itr(this, m_HighestIndex + 1);
    return itr;
  }

  RSparseListIteratorConst begin() const
  {
    int start_index = 0;
    while (start_index <= m_HighestIndex && m_Values[start_index].m_Valid == false)
    {
      start_index++;
    }

    RSparseListIteratorConst itr(this, start_index);
    return itr;
  }

  RSparseListIteratorConst end() const
  {
    RSparseListIteratorConst itr(this, m_HighestIndex + 1);
    return itr;
  }

  bool operator == (const RSparseList<T> & rhs) const
  {
    if (m_HighestIndex != rhs.m_HighestIndex)
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

  void GrowToFit(std::size_t logical_index)
  {
    if (logical_index >= m_Capacity)
    {
      if (m_Capacity == 0)
      {
        Grow(logical_index + 1);
      }
      else
      {
        std::size_t new_capacity = m_Capacity;
        do
        {
          new_capacity *= 2;
        } while (new_capacity <= logical_index);

        Grow(new_capacity);
      }
    }
  }

  void Grow(std::size_t requested_size)
  {
    auto values = Allocate<ContainerData>(requested_size);

    if (m_Capacity > 0)
    {
      for (int index = 0; index <= m_HighestIndex; index++)
      {
        values[index].m_Valid = m_Values[index].m_Valid;
        if (m_Values[index].m_Valid)
        {
#ifdef STORM_CHANGE_NOTIFIER
          StormDataRelocateConstruct(std::move(m_Values[index].m_Value), &values[index].m_Value, &m_ReflectionInfo);
#else
          new (&values[index].m_Value) T(std::move(m_Values[index].m_Value));
#endif

          m_Values[index].m_Value.~T();
        }
      }

      Deallocate(m_Values);
    }

    for (int index = m_HighestIndex + 1; index < (int)requested_size; index++)
    {
      values[index].m_Valid = false;
    }

    m_Values = values;
    m_Capacity = requested_size;

    UpdateAllElements();
  }

  void DestroyAllElements()
  {
    for (int index = 0; index <= m_HighestIndex; index++)
    {
      if (m_Values[index].m_Valid)
      {
        m_Values[index].m_Value.~T();        
      }
    }
  }

  void DestroyAllElementsAndInvalidate()
  {
    for (int index = 0; index <= m_HighestIndex; index++)
    {
      if (m_Values[index].m_Valid)
      {
        m_Values[index].m_Value.~T();
        m_Values[index].m_Valid = false;
      }
    }
  }

  void InitAllElements()
  {
#ifdef STORM_CHANGE_NOTIFIER
    StormReflectionParentInfo new_info;
    new_info.m_ParentInfo = &m_ReflectionInfo;
    new_info.m_MemberName = nullptr;
    new_info.m_Flags = (m_ReflectionInfo.m_Callback != nullptr || (m_ReflectionInfo.m_Flags & (uint32_t)StormDataParentInfoFlags::kParentHasCallback) != 0) ?
                       (uint32_t)StormDataParentInfoFlags::kParentHasCallback : 0;

    for (int index = 0; index <= m_HighestIndex; index++)
    {
      if (m_Values[index].m_Valid)
      {
        new_info.m_ParentIndex = index;
        SetParentInfo(m_Values[index].m_Value, new_info);
      }
    }
#endif
  }

  void UpdateAllElements()
  {
#ifdef STORM_CHANGE_NOTIFIER
    bool has_callback = m_ReflectionInfo.m_Callback != nullptr || (m_ReflectionInfo.m_Flags & (uint32_t)StormDataParentInfoFlags::kParentHasCallback) != 0;

    for (int index = 0; index <= m_HighestIndex; index++)
    {
      if (m_Values[index].m_Valid)
      {
        m_Values[index].m_Value.m_ReflectionInfo.m_ParentInfo = &m_ReflectionInfo;
        bool child_has_flag = (m_Values[index].m_Value.m_ReflectionInfo.m_Flags & (uint32_t)StormDataParentInfoFlags::kParentHasCallback) != 0;

        if (child_has_flag)
        {
          if (has_callback == false)
          {
            SetParentInfoStruct<T>::ClearParentCallback(m_Values[index].m_Value);
          }
        }
        else
        {
          if (has_callback == true)
          {
            SetParentInfoStruct<T>::SetFlag(m_Values[index].m_Value, StormDataParentInfoFlags::kParentHasCallback);
          }
        }
      }
    }
#endif
  }

  void ShiftUp(std::size_t start_index, std::size_t end_index)
  {
    auto end = std::min((int)end_index, m_HighestIndex);
    GrowToFit(end);

    for (int pos = end; pos > start_index; --pos)
    {
#ifdef STORM_CHANGE_NOTIFIER
      StormDataRelocateConstruct(std::move(m_Values[pos].m_Value), &m_Values[pos + 1].m_Value, &m_ReflectionInfo);
#else
      new (&values[pos + 1].m_Value) T(std::move(m_Values[pos].m_Value));
#endif
    }

    m_Values[start_index].~T();
  }

  void ShiftDown(std::size_t start_index, std::size_t end_index)
  {
    auto end = std::min((int)end_index, m_HighestIndex);
    for (int pos = (int)start_index; pos <= end; ++pos)
    {
#ifdef STORM_CHANGE_NOTIFIER
      StormDataRelocateConstruct(std::move(m_Values[pos].m_Value), &m_Values[pos - 1].m_Value, &m_ReflectionInfo);
#else
      new (&values[pos - 1].m_Value) T(std::move(m_Values[pos].m_Value));
#endif
    }

    m_Values[end].~T();
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

  void Copy(const RSparseList<T> & rhs)
  {
    DestroyAllElements();

    if (rhs.m_HighestIndex >= (int)m_Capacity)
    {
      Deallocate(m_Values);

      m_Values = Allocate<ContainerData>(rhs.m_Capacity);
      m_Capacity = rhs.m_Capacity;
    }

    for (int index = 0; index <= rhs.m_HighestIndex; index++)
    {
      m_Values[index].m_Valid = rhs.m_Values[index].m_Valid;
      if (rhs.m_Values[index].m_Valid)
      {
        new(&m_Values[index].m_Value) T(rhs.m_Values[index].m_Value);
      }
    }

    for (int index = rhs.m_HighestIndex + 1; index < (int)m_Capacity; index++)
    {
      m_Values[index].m_Valid = false;
    }

    m_HighestIndex = rhs.m_HighestIndex;

    InitAllElements();
  }

  void Move(RSparseList<T> && rhs)
  {
    DestroyAllElements();

    if (m_Capacity > 0)
    {
      Deallocate(m_Values);
    }

    m_HighestIndex = rhs.m_HighestIndex;
    m_Capacity = rhs.m_Capacity;
    m_Values = rhs.m_Values;

    rhs.m_HighestIndex = -1;
    rhs.m_Capacity = 0;
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

    for (int index = 0; index <= m_HighestIndex; index++)
    {
      if (m_Values[index].m_Valid)
      {
        m_Values[index].m_Value.m_ReflectionInfo.m_ParentIndex = index;
      }
    }

    if (DoNotifyCallback(m_ReflectionInfo) == false)
    {
      return;
    }

    ReflectionNotifyCompress(m_ReflectionInfo);
#endif
  }

  void Inserted(std::size_t logical_index, bool minimal)
  {
#ifdef STORM_CHANGE_NOTIFIER

    StormReflectionParentInfo new_info;
    new_info.m_ParentInfo = &m_ReflectionInfo;
    new_info.m_MemberName = nullptr;
    new_info.m_ParentIndex = logical_index;
    new_info.m_Flags = (m_ReflectionInfo.m_Callback != nullptr || (m_ReflectionInfo.m_Flags & (uint32_t)StormDataParentInfoFlags::kParentHasCallback) != 0) ?
      (uint32_t)StormDataParentInfoFlags::kParentHasCallback : 0;
    SetParentInfo(m_Values[logical_index].m_Value, new_info);

    if (DoNotifyCallback(m_ReflectionInfo) == false)
    {
      return;
    }

    std::string data;

#ifdef STORM_CHANGE_MINIMAL
    if (minimal)
    {
      StormReflSerializeDefaultJson(m_Values[logical_index].m_Value, data);
    }
    else
    {
      StormReflEncodeJson(m_Values[logical_index].m_Value, data);
    }
#else
    StormReflEncodeJson(m_Values[logical_index].m_Value, data);
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

  struct ContainerData
  {
    bool m_Valid;
    T m_Value;
  };

  template <typename Type, typename Enable>
  friend struct StormDataJson;

  int m_HighestIndex;
  std::size_t m_Capacity;
  ContainerData * m_Values;

  STORM_CHANGE_NOTIFIER_INFO;
};
