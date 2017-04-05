#pragma once

#include "StormData.h"
#include "StormDataChangeNotifier.h"

template <class Base, class TypeDatabase, class TypeInfo>
class RPolymorphic
{
public:

  RPolymorphic() :
    m_TypeInfo(nullptr),
    m_TypeNameHash(0),
    m_Data(nullptr)
  {

  }

  RPolymorphic(const RPolymorphic<Base, TypeDatabase, TypeInfo> & rhs)
  {
    if (rhs.m_TypeInfo == 0)
    {
      m_TypeInfo = nullptr;
      m_TypeNameHash = 0;
      m_Data = nullptr;
      return;
    }

    m_TypeInfo = rhs.m_TypeInfo;
    m_TypeNameHash = rhs.m_TypeNameHash;
    m_Data = t.GetTypeInfo()->HeapCreate();

    t.GetTypeInfo()->CopyRaw(rhs.GetValue(), GetValue());
    SetParentInfo();
  }

  RPolymorphic(RPolymorphic<Base, TypeDatabase, TypeInfo> && rhs) noexcept
  {
#ifdef STORM_CHANGE_NOTIFIER
    m_ReflectionInfo = rhs.m_ReflectionInfo;
    rhs.m_ReflectionInfo = {};
#endif

    if (rhs.m_TypeInfo == 0)
    {
      m_TypeInfo = nullptr;
      m_TypeNameHash = 0;
      m_Data = nullptr;
      return;
    }

    m_TypeInfo = rhs.m_TypeInfo;
    m_TypeNameHash = rhs.m_TypeNameHash;
    m_Data = rhs.m_Data;
    SetParentInfo();

    rhs.m_TypeInfo = nullptr;
    rhs.m_TypeNameHash = 0;
    rhs.m_Data = nullptr;
  }

#ifdef STORM_CHANGE_NOTIFIER
  RPolymorphic(RPolymorphic<Base, TypeDatabase, TypeInfo> && rhs, StormReflectionParentInfo * new_parent) noexcept
  {
    m_ReflectionInfo = rhs.m_ReflectionInfo;
    m_ReflectionInfo.m_ParentInfo = new_parent;

    if (rhs.m_TypeInfo == 0)
    {
      m_TypeInfo = nullptr;
      m_TypeNameHash = 0;
      m_Data = nullptr;
      return;
    }

    m_TypeInfo = rhs.m_TypeInfo;
    m_TypeNameHash = rhs.m_TypeNameHash;
    m_Data = rhs.m_Data;
    SetParentInfo();

    rhs.m_TypeInfo = nullptr;
    rhs.m_TypeNameHash = 0;
    rhs.m_Data = nullptr;
  }
#endif

  ~RPolymorphic()
  {
    if (m_Data)
    {
      t.GetTypeInfo()->HeapFree(m_Data);
    }
  }

  RPolymorphic<Base, TypeDatabase, TypeInfo> & operator = (const RPolymorphic<Base, TypeDatabase, TypeInfo> & rhs)
  {
    SetRaw(rhs);
    Modified();
  }

  RPolymorphic<Base, TypeDatabase, TypeInfo> & operator = (RPolymorphic<Base, TypeDatabase, TypeInfo> && rhs)
  {
    SetRaw(std::move(rhs));
    Modified();
  }

  void SetRaw(const RPolymorphic<Base, TypeDatabase, TypeInfo> & rhs)
  {
    if (rhs.m_TypeInfo == 0)
    {
      if (m_TypeInfo == 0)
      {
        return;
      }

      if (m_Data != nullptr)
      {
        t.GetTypeInfo()->HeapFree(m_Data);
      }

      m_TypeInfo = nullptr;
      m_TypeNameHash = 0;
      m_Data = nullptr;
      return;
    }

    if (m_TypeInfo != rhs.m_TypeInfo)
    {
      if (m_Data != nullptr)
      {
        t.GetTypeInfo()->HeapFree(m_Data);
      }

      m_TypeInfo = rhs.m_TypeInfo;
      m_TypeNameHash = rhs.m_TypeNameHash;
      m_Data = t.GetTypeInfo()->HeapCreate();
      SetParentInfo();
    }

    t.GetTypeInfo()->CopyRaw(rhs.GetValue(), GetValue());
  }

  void SetRaw(RPolymorphic<Base, TypeDatabase, TypeInfo> && rhs)
  {
    if (m_Data != nullptr)
    {
      t.GetTypeInfo()->HeapFree(m_Data);
    }

    if (rhs.m_TypeInfo == 0)
    {
      if (m_TypeInfo == 0)
      {
        return;
      }

      m_TypeInfo = nullptr;
      m_TypeNameHash = 0;
      m_Data = nullptr;

      Modified();
      return;
    }

    m_TypeInfo = rhs.m_TypeInfo;
    m_TypeNameHash = rhs.m_TypeNameHash;
    m_Data = rhs.m_Data;
    SetParentInfo();

    rhs.m_TypeInfo = nullptr;
    rhs.m_TypeNameHash = 0;
    rhs.m_Data = nullptr;
  }

#ifdef STORM_CHANGE_NOTIFIER
  void Relocate(RPolymorphic<Base, TypeDatabase, TypeInfo> && rhs, StormReflectionParentInfo * new_parent) noexcept
  {
    SetRaw(std::move(rhs));
    m_ReflectionInfo = rhs.m_ReflectionInfo;
    m_ReflectionInfo.m_ParentInfo = new_parent;
  }
#endif

  uint32_t GetTypeNameHash() const
  {
    return m_TypeNameHash;
  }

  Base * GetValue()
  {
    return m_Data;
  }

  const Base * GetValue() const
  {
    return m_Data;
  }

  const TypeInfo * GetTypeInfo()
  {
    return m_TypeInfo;
  }

  void SetTypeFromNameHash(uint32_t type_name_hash)
  {
    if (m_Data != nullptr)
    {
      t.GetTypeInfo()->HeapFree(m_Data);
    }

    auto type_info = TypeDatabase::GetTypeInfo(type_name_hash);
    if (type_info == nullptr)
    {
      m_TypeInfo = nullptr;
      m_TypeNameHash = 0;
      m_Data = nullptr;
      return;
    }

    m_TypeInfo = type_info;
    m_TypeNameHash = type_name_hash;
    m_Data = type_info->HeapCreate();
    SetParentInfo();
  }

protected:

  template <typename T>
  friend struct SetBasicParentInfo;

  void Modified()
  {
#ifdef STORM_CHANGE_NOTIFIER
    if (DoNotifyCallback(m_ReflectionInfo) == false)
    {
      return;
    }

    ReflectionNotifySetObject(m_ReflectionInfo, StormReflEncodeJson(*this));
#endif
  }

  void SetParentInfo()
  {
#ifdef STORM_CHANGE_NOTIFIER
    if (m_TypeInfo == nullptr)
    {
      return;
    }

    StormReflectionParentInfo new_info;
    new_info.m_ParentInfo = parent_info;
    new_info.m_MemberName = "D";
    new_info.m_Flags = (m_ReflectionInfo.m_Callback != nullptr || (m_ReflectionInfo.m_Flags & (uint32_t)StormDataParentInfoFlags::kParentHasCallback) != 0) ?
      (uint32_t)StormDataParentInfoFlags::kParentHasCallback : 0;

    m_TypeInfo->SetParentInfo(m_Data, new_info);
#endif
  }

private:

  const TypeInfo * m_TypeInfo;

  uint32_t m_TypeNameHash;
  Base * m_Data;

  STORM_CHANGE_NOTIFIER_INFO;
};


