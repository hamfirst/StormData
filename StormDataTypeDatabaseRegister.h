#pragma once

#include "StormDataTypeDatabase.h"
#include "StormDataTypeDatabaseRegister.h"

#include "StormDataChangeNotifier.h"
#include "StormDataJson.h"
#include "StormDataChangePacket.h"
#include "StormDataParent.h"
#include "StormDataDelta.h"
#include "StormDataSync.h"

template <typename TypeInfo, typename Class, typename ... BaseTypes>
struct StormDataInitBaseClass
{
  static void Process(TypeInfo & type_info)
  {

  }
};

template <typename TypeInfo, typename Class, typename BaseType, typename ... BaseTypes>
struct StormDataInitBaseClass<TypeInfo, Class, BaseType, BaseTypes...>
{
  static void Process(TypeInfo & type_info)
  {
    static_assert(std::is_base_of<BaseType, Class>::value, "Registering type that is not of the right base class");
    type_info.m_BaseTypes.emplace_back(std::make_pair(StormReflTypeInfo<BaseType>::GetNameHash(), [](void * c) -> void * { auto ptr = (Class *)c; return (BaseType *)ptr; }));

    StormDataInitBaseClass<TypeInfo, Class, BaseTypes...>::Process(type_info);
  }
};

template <typename Class, typename TypeInfo>
void StormDataInitTypeInfo(TypeInfo & type_info)
{
  type_info.m_Name = StormReflTypeInfo<Class>::GetName();
  type_info.m_TypeNameHash = crc32(type_info.m_Name);
  type_info.m_TypeIdHash = typeid(Class).hash_code();
  type_info.HeapCreate = []() -> void * { return new Class; };
  type_info.HeapFree = [](void * data) { delete (Class *)data; };
  type_info.CopyRaw = [](const void * src, void * dest) { ((Class *)dest)->SetRaw(*(Class *)src); };

  type_info.EncodeJson = [](const void * poly_ptr, std::string & sb) { StormReflEncodeJson(*(const Class *)poly_ptr, sb); };
  type_info.EncodePrettyJson = [](const void * poly_ptr, std::string & sb, int indent) { StormReflEncodePrettyJson(*(const Class *)poly_ptr, sb, indent); };
  type_info.ParseJson = [](void * poly_ptr, const char * data_start, bool additive) { return StormReflParseJson(*(Class *)poly_ptr, data_start, additive); };

  type_info.ApplySet = [](void * poly_ptr, const char * path, const char * data) { return StormDataChangePacketHelpers::StormDataApplyChangePacketSet<Class>::Process(*(Class *)poly_ptr, path, data); };
  type_info.ApplyClear = [](void * poly_ptr, const char * path) { return StormDataChangePacketHelpers::StormDataApplyChangePacketClear<Class>::Process(*(Class *)poly_ptr, path); };
  type_info.ApplyCompress = [](void * poly_ptr, const char * path) { return StormDataChangePacketHelpers::StormDataApplyChangePacketCompress<Class>::Process(*(Class *)poly_ptr, path); };
  type_info.ApplyInsert = [](void * poly_ptr, const char * path, uint64_t index, const char * data) { return StormDataChangePacketHelpers::StormDataApplyChangePacketInsert<Class>::Process(*(Class *)poly_ptr, path, index, data); };
  type_info.ApplyRemove = [](void * poly_ptr, const char * path, uint64_t index) { return StormDataChangePacketHelpers::StormDataApplyChangePacketRemove<Class>::Process(*(Class *)poly_ptr, path, index); };
  type_info.ApplyRevertDefault = [](void * poly_ptr, const char * path) { return StormDataChangePacketHelpers::StormDataApplyChangePacketRevertDefault<Class>::Process(*(Class *)poly_ptr, path); };

  type_info.SetParentInfo = [](void * poly_ptr, const StormReflectionParentInfo & parent_info) { SetParentInfo(*(Class *)poly_ptr, parent_info); };
  type_info.SetParentInfoFlag = [](void * poly_ptr, StormDataParentInfoFlags flags) { SetParentInfoStruct<Class>::SetFlag(*(Class *)poly_ptr, flags); };
  type_info.ClearParentInfoFlag = [](void * poly_ptr, StormDataParentInfoFlags flags) { SetParentInfoStruct<Class>::ClearFlag(*(Class *)poly_ptr, flags); };
  type_info.ClearParentInfoCallback = [](void * poly_ptr) { SetParentInfoStruct<Class>::ClearParentCallback(*(Class *)poly_ptr); };
  type_info.MoveParentInfo = [](void * src, void * dest) {SetParentInfoStruct<Class>::MoveParentInfo(*(Class *)src, *(Class *)dest); };

  type_info.DeltaCopy = [](const void * src, void * dest) { StormDataDeltaCopy(*(const Class *)src, *(Class *)dest); };
  type_info.Sync = [](const void * src, void * dest, const char * path) { StormDataSync(*(const Class *)src, *(Class *)dest, path); };

  type_info.Cast = static_cast<void * (*)(std::size_t, void *)>(&StormReflTypeInfo<Class>::CastFromTypeIdHash);
  type_info.ConstCast = static_cast<const void * (*)(std::size_t, const void *)>(&StormReflTypeInfo<Class>::CastFromTypeIdHash);
}

template <typename Base, typename TypeInfo>
void StormDataTypeDatabase<Base, TypeInfo>::FinalizeTypes()
{
  for (auto & elem : m_TypeList)
  {
    for (auto & base : elem.second.m_BaseTypes)
    {
      auto itr = m_TypeList.find(base.first);
      if (itr == m_TypeList.end())
      {
        assert(false);
      }

      for (auto & base_base : itr->second.m_BaseTypes)
      {
        AddBaseTypesForType(elem.second, base_base.first, base_base.second);
      }
    }
  }
}

template <typename Base, typename TypeInfo>
void StormDataTypeDatabase<Base, TypeInfo>::AddBaseTypesForType(TypeInfo & type_info, uint32_t base_type_hash, void * (*CastFunc)(void *))
{
  for (auto & base : type_info.m_BaseTypes)
  {
    if (base.first == base_type_hash)
    {
      return;
    }
  }

  type_info.m_BaseTypes.emplace_back(std::make_pair(base_type_hash, CastFunc));

  auto itr = m_TypeList.find(base_type_hash);
  if (itr == m_TypeList.end())
  {
    assert(false);
  }

  for (auto & base_base : itr->second.m_BaseTypes)
  {
    AddBaseTypesForType(type_info, base_base.first, base_base.second);
  }
}

template <typename Base, typename TypeInfo>
TypeInfo * StormDataTypeDatabase<Base, TypeInfo>::GetTypeInfo(uint32_t type_name_hash)
{
  auto itr = m_TypeList.find(type_name_hash);
  if (itr == m_TypeList.end())
  {
    return nullptr;
  }

  return &itr->second;
}

template <typename Base, typename TypeInfo>
TypeInfo * StormDataTypeDatabase<Base, TypeInfo>::GetFirstNonBaseType()
{
  for (auto && elem : m_TypeList)
  {
    if (elem.second.m_BaseTypes.size() == 0)
    {
      continue;
    }

    return &elem.second;
  }

  return nullptr;
}
