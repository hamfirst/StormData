#pragma once

#include <memory>

#include "StormData.h"
#include "StormDataChangeNotifier.h"

template <class K, class T>
class RMap
{
  struct ContainerList;
  struct ContainerData;

public:

  using ContainerType = T;
  static const std::size_t kBucketCount = 256;

  template <typename MapType, typename ContainerType, typename BucketType, typename ValueType>
  struct RMapIteratorBase
  {
    RMapIteratorBase(const RMapIteratorBase & rhs) : m_Map((MapType)rhs.m_List), m_List((BucketType)rhs.m_List), m_BucketIndex(rhs.m_BucketIndex), m_ListIndex(rhs.m_ListIndex) { }

    bool operator != (const RMapIteratorBase & rhs) const
    {
      if (m_Map != rhs.m_Map || m_List != rhs.m_List || m_BucketIndex != rhs.m_BucketIndex)
      {
        return true;
      }

      return m_ListIndex != rhs.m_ListIndex;
    }

    bool operator == (const RMapIteratorBase & rhs) const
    {
      if (m_Map != rhs.m_Map || m_List != rhs.m_List || m_BucketIndex != rhs.m_BucketIndex)
      {
        return false;
      }

      return m_ListIndex == rhs.m_ListIndex;
    }

    std::pair<K, ValueType> operator *() const
    {
      ContainerType data = m_Map->m_Buckets[m_BucketIndex].m_Values[m_ListIndex];
      return std::pair<K, ValueType>(data.m_Key, data.m_Value);
    }

    StormDataHelpers::RTempPair<K, ValueType> operator ->() const
    {
      ContainerType data = m_Map->m_Buckets[m_BucketIndex].m_Values[m_ListIndex];
      return StormDataHelpers::RTempPair<K, ValueType>(data.m_Key, data.m_Value);
    }

    void operator++()
    {
      Advance();
    }

  private:

    RMapIteratorBase(MapType map) :
      m_Map(map),
      m_List(map->m_Buckets ? &map->m_Buckets[0] : nullptr),
      m_BucketIndex(map->m_Buckets ? 0 : kBucketCount),
      m_ListIndex(-1)
    {
      Advance();
    }

    RMapIteratorBase(MapType map, bool end) :
      m_Map(map),
      m_List(nullptr),
      m_BucketIndex(kBucketCount),
      m_ListIndex(-1)
    {

    }

    void Advance()
    {
      if (m_BucketIndex >= kBucketCount)
      {
        return;
      }

      while (true)
      {
        m_ListIndex++;
        if (m_ListIndex >= (int)m_Map->m_Buckets[m_BucketIndex].m_Size)
        {
          m_BucketIndex++;
          m_List = &m_Map->m_Buckets[m_BucketIndex];
          m_ListIndex = -1;
          if (m_BucketIndex >= kBucketCount)
          {
            m_List = nullptr;
            break;
          }

          continue;
        }

        break;
      }
    }

    MapType m_Map;
    BucketType m_List;
    int m_BucketIndex;
    int m_ListIndex;

    friend class RMap<K, T>;
  };

  using RMapIterator = RMapIteratorBase<RMap<K, T> *, ContainerData &, ContainerList *, T &>;
  using RMapConstIterator = RMapIteratorBase<const RMap<K, T> *, const ContainerData &, const ContainerList *, const T &>;

  template <typename MapType, typename ContainerType, typename BucketType, typename ValueType>
  friend struct RMapIteratorBase;

  RMap() :
    m_Size(0)
  {

  }

  RMap(const RMap<K, T> & rhs) :
    m_Size(rhs.m_Size)
  {
    if(rhs.m_Size)
    {
      m_Buckets = std::make_unique<ContainerList[]>(kBucketCount);
      for (std::size_t index = 0; index < kBucketCount; index++)
      {
        m_Buckets[index].CopyFrom(rhs.m_Buckets[index], GetParentInfo());
      }
    }
  }

  RMap(RMap<K, T> && rhs) :
    m_Size(rhs.m_Size)
  {
    if (rhs.m_Size)
    {
      m_Buckets = std::make_unique<ContainerList[]>(kBucketCount);
      for (std::size_t index = 0; index < kBucketCount; index++)
      {
        m_Buckets[index].MoveFrom(rhs.m_Buckets[index], GetParentInfo());
      }
    }

#ifdef STORM_CHANGE_NOTIFIER
    m_ReflectionInfo = rhs.m_ReflectionInfo;
    rhs.m_ReflectionInfo = {};
#endif
  }

#ifdef STORM_CHANGE_NOTIFIER
  RMap(RMap<K, T> && rhs, StormReflectionParentInfo * new_parent) :
    m_Size(rhs.m_Size)
  {
    m_ReflectionInfo = rhs.m_ReflectionInfo;
    m_ReflectionInfo.m_ParentInfo = new_parent;

    if (rhs.m_Size)
    {
      m_Buckets = std::make_unique<ContainerList[]>(kBucketCount);
      for (std::size_t index = 0; index < kBucketCount; index++)
      {
        m_Buckets[index].MoveFrom(rhs.m_Buckets[index], GetParentInfo());
      }
    }
  }
#endif

  ~RMap()
  {

  }

  RMap<K, T> & operator = (const RMap<K, T> & rhs)
  {
    Copy(rhs);
    Set();
    return *this;
  }

  RMap<K, T> & operator = (RMap<K, T> && rhs)
  {
    Move(std::move(rhs));
    Set();
    return *this;
  }

  void SetRaw(const RMap<K, T> & rhs)
  {
    Copy(rhs);
  }

  void SetRaw(RMap<K, T> && rhs)
  {
    Move(std::move(rhs));
  }

#ifdef STORM_CHANGE_NOTIFIER
  void Relocate(RMap<K, T> && rhs, StormReflectionParentInfo * new_parent)
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
    DestroyAllElements();
  }

  T & Set(const K & k, const T & t)
  {
    CreateBuckets();

    ContainerList * bucket = GetBucket(k);
    T * val = bucket->Find(k);

    if (val == nullptr)
    {
      T & ret_val = bucket->Emplace(k, &m_ReflectionInfo, t);
      m_Size++;
      Inserted(k, ret_val, false);
      return ret_val;
    }
    else
    {
      *val = t;
      return *val;
    }
  }

  T & InsertAt(const K & k, const T & t)
  {
    return Set(k, t);
  }

  template <typename ... InitArgs>
  T & Emplace(const K & k, InitArgs && ... args)
  {
    CreateBuckets();

    ContainerList * bucket = GetBucket(k);
    T * val = bucket->Find(k);

    if (val == nullptr)
    {
      T & ret_val = bucket->Emplace(k, &m_ReflectionInfo, std::forward<InitArgs>(args)...);
      m_Size++;
      Inserted(k, ret_val, sizeof...(InitArgs) == 0);
      return ret_val;
    }
    else
    {
      *val = T(std::forward<InitArgs>(args)...);
      return *val;
    }
  }

  template <typename ... InitArgs>
  T & EmplaceAt(const K & k, InitArgs && ... args)
  {
    return Emplace(k, std::forward<InitArgs>(args)...);
  }

  bool Remove(const K & k)
  {
    if (!m_Buckets)
    {
      return false;
    }

    ContainerList * bucket = GetBucket(k);
    if (bucket->Remove(k))
    {
      m_Size--;
      Removed(k);
      return true;
    }

    return false;
  }

  bool RemoveAt(const K & k)
  {
    return Remove(k);
  }

  T & Get(const K & k)
  {
    if (!m_Buckets)
    {
      throw std::out_of_range("Invalid key");
    }

    ContainerList * bucket = GetBucket(k);
    T * val = bucket->Find(k);

    if (val == nullptr)
    {
      throw std::out_of_range("Invalid key");
    }

    return *val;
  }

  const T & Get(const K & k) const
  {
    if (!m_Buckets)
    {
      throw std::out_of_range("Invalid key");
    }

    ContainerList * bucket = GetBucket(k);
    T * val = bucket->Find(k);

    if (val == nullptr)
    {
      throw std::out_of_range("Invalid key");
    }

    return *val;
  }

  T * TryGet(const K & k)
  {
    if (!m_Buckets)
    {
      return nullptr;
    }

    ContainerList * bucket = GetBucket(k);
    return bucket->Find(k);
  }

  const T * TryGet(const K & k) const
  {
    if (!m_Buckets)
    {
      return nullptr;
    }

    ContainerList * bucket = GetBucket(k);
    return bucket->Find(k);
  }

  bool HasAt(const K & k) const
  {
    return TryGet(k) != nullptr;
  }

  T & operator [] (const K & k)
  {
    return Get(k);
  }

  const T & operator [] (const K & k) const
  {
    return Get(k);
  }

  std::size_t Size() const
  {
    return m_Size;
  }

  auto begin()
  {
    return RMapIterator(this);
  }

  auto end()
  {
    return RMapIterator(this, true);
  }

  auto begin() const
  {
    return RMapConstIterator(this);
  }

  auto end() const
  {
    return RMapConstIterator(this, true);
  }

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

private:

  StormReflectionParentInfo * GetParentInfo()
  {
#ifdef STORM_CHANGE_NOTIFIER
    return &m_ReflectionInfo;
#else
    return nullptr;
#endif
  }

  void CreateBuckets()
  {
    if (!m_Buckets)
    {
      m_Buckets = std::make_unique<ContainerList[]>(kBucketCount);
    }
  }

  void DestroyAllElements()
  {
    if (!m_Buckets)
    {
      return;
    }

    for (std::size_t index = 0; index < kBucketCount; index++)
    {
      m_Buckets[index].Clear();
    }

    m_Size = 0;
  }

  ContainerList * GetBucket(K key) const
  {
    return (ContainerList *)&m_Buckets[key & 0xFF];
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

  void Copy(const RMap<K, T> & rhs)
  {
    DestroyAllElements();

    if (rhs.m_Buckets)
    {
      CreateBuckets();

      for (std::size_t index = 0; index < kBucketCount; index++)
      {
        m_Buckets[index].CopyFrom(rhs.m_Buckets[index], GetParentInfo());
      }
    }

    m_Size = rhs.m_Size;
  }

  void Move(RMap<K, T> && rhs)
  {
    DestroyAllElements();
    if (rhs.m_Buckets)
    {
      CreateBuckets();

      for (std::size_t index = 0; index < kBucketCount; index++)
      {
        m_Buckets[index].MoveFrom(rhs.m_Buckets[index], GetParentInfo());
      }
    }

    m_Size = rhs.m_Size;
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

  void Inserted(K key, T & value, bool minimal)
  {
#ifdef STORM_CHANGE_NOTIFIER

    StormReflectionParentInfo new_info;
    new_info.m_ParentInfo = &m_ReflectionInfo;
    new_info.m_MemberName = nullptr;
    new_info.m_ParentIndex = (uint64_t)key;
    new_info.m_Flags = (m_ReflectionInfo.m_Callback != nullptr || (m_ReflectionInfo.m_Flags & (uint32_t)StormDataParentInfoFlags::kParentHasCallback) != 0) ?
      (uint32_t)StormDataParentInfoFlags::kParentHasCallback : 0;
    SetParentInfo(value, new_info);

    if (DoNotifyCallback(m_ReflectionInfo) == false)
    {
      return;
    }

    std::string data;

#ifdef STORM_CHANGE_MINIMAL
    if (minimal)
    {
      StormReflSerializeDefaultJson(value, data);
    }
    else
    {
      StormReflEncodeJson(value, data);
    }
#else
    StormReflEncodeJson(value, data);
#endif

    ReflectionNotifyInsertObject(m_ReflectionInfo, key, data);
#endif
  }

  void Removed(K key)
  {
#ifdef STORM_CHANGE_NOTIFIER
    if (DoNotifyCallback(m_ReflectionInfo) == false)
    {
      return;
    }

    ReflectionNotifyRemoveObject(m_ReflectionInfo, key);
#endif
  }

  struct ContainerData
  {
    K m_Key;
    T m_Value;
  };

  struct ContainerList
  {
    ContainerData * m_Values = nullptr;
    std::size_t m_Capacity = 0;
    std::size_t m_Size = 0;

    template <typename Elem>
    Elem * Allocate(std::size_t count)
    {
      return (Elem *)malloc(sizeof(Elem) * count);
    }

    void Deallocate(void * ptr)
    {
      free(ptr);
    }

    ~ContainerList()
    {
      Clear();

      if (m_Capacity > 0)
      {
        Deallocate(m_Values);
      }
    }

    void CopyFrom(const ContainerList & rhs, StormReflectionParentInfo * parent_info)
    {
      Clear();
      if (rhs.m_Size > m_Capacity)
      {
        Grow(rhs.m_Capacity, parent_info);
      }

      m_Size = rhs.m_Size;
      for (std::size_t index = 0; index < m_Size; index++)
      {
        m_Values[index].m_Key = rhs.m_Values[index].m_Key;
        new(&m_Values[index].m_Value) T(rhs.m_Values[index].m_Value);

#ifdef STORM_CHANGE_NOTIFIER
        StormReflectionParentInfo new_info;
        new_info.m_ParentInfo = parent_info;
        new_info.m_MemberName = nullptr;
        new_info.m_Flags = (parent_info->m_Callback != nullptr || (parent_info->m_Flags & (uint32_t)StormDataParentInfoFlags::kParentHasCallback) != 0) ?
          (uint32_t)StormDataParentInfoFlags::kParentHasCallback : 0;
        new_info.m_ParentIndex = m_Values[index].m_Key;

        SetParentInfo(m_Values[index].m_Value, new_info);
#endif
      }
    }

    void MoveFrom(ContainerList & rhs, StormReflectionParentInfo * parent_info)
    {
      Clear();

      if (m_Capacity > 0)
      {
        Deallocate(m_Values);
      }
      
      m_Values = rhs.m_Values;
      m_Size = rhs.m_Size;
      m_Capacity = rhs.m_Capacity;

      rhs.m_Values = nullptr;
      rhs.m_Size = 0;
      rhs.m_Capacity = 0;

#ifdef STORM_CHANGE_NOTIFIER
      bool has_callback = parent_info->m_Callback != nullptr || (parent_info->m_Flags & (uint32_t)StormDataParentInfoFlags::kParentHasCallback) != 0;
      for (std::size_t index = 0; index < m_Size; index++)
      {
        m_Values[index].m_Value.m_ReflectionInfo.m_ParentInfo = parent_info;
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
#endif
    }

    template <typename ... InitArgs>
    T & Emplace(K key, StormReflectionParentInfo * parent_info, InitArgs && ... args)
    {
      if (m_Size == m_Capacity)
      {
        Grow(parent_info);
      }

      m_Values[m_Size].m_Key = key;
      new (&m_Values[m_Size].m_Value) T(std::forward<InitArgs>(args)...);
      return m_Values[m_Size++].m_Value;
    }

    T * Find(K key)
    {
      for (std::size_t index = 0; index < m_Size; index++)
      {
        if (m_Values[index].m_Key == key)
        {
          return &m_Values[index].m_Value;
        }
      }

      return nullptr;
    }

    bool Remove(K key)
    {
      for (std::size_t index = 0; index < m_Size; index++)
      {
        if (m_Values[index].m_Key == key)
        {
          m_Values[index].m_Value.~T();
          if (index != m_Size - 1)
          {
            new (&m_Values[index].m_Value) T(std::move(m_Values[m_Size - 1].m_Value));
            m_Values[m_Size - 1].m_Value.~T();
          }

          m_Size--;
          return true;
        }
      }

      return false;
    }

    void Grow(StormReflectionParentInfo * parent_info)
    {
      std::size_t new_size = (m_Capacity == 0 ? 1 : m_Capacity * 2);
      Grow(new_size, parent_info);
    }

    void Grow(std::size_t new_size, StormReflectionParentInfo * parent_info)
    {
      ContainerData * values = Allocate<ContainerData>(new_size);

      if (m_Capacity > 0)
      {
        for (std::size_t index = 0; index < m_Size; index++)
        {
          values[index].m_Key = m_Values[index].m_Key;
#ifdef STORM_CHANGE_NOTIFIER
          StormDataRelocateConstruct(std::move(m_Values[index]), &values[index], parent_info);
#else
          new (&values[index].m_Value) T(std::move(m_Values[index].m_Value));
#endif
          m_Values[index].m_Value.~T();
        }

        Deallocate(m_Values);
      }

      m_Capacity = new_size;
      m_Values = values;
    }

    void Clear()
    {
      for (std::size_t index = 0; index < m_Size; index++)
      {
        m_Values[index].m_Value.~T();
      }

      m_Size = 0;
    }
  };


  template <typename Type, typename Enable>
  friend struct StormDataJson;

  std::unique_ptr<ContainerList[]> m_Buckets;
  std::size_t m_Size;

  STORM_CHANGE_NOTIFIER_INFO;
};
