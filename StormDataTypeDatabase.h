#pragma once

#include <cstdint>
#include <cassert>

#include <string>
#include <vector>
#include <unordered_map>
#include <type_traits>

#include "StormDataChangeNotifier.h"
#include "StormDataJson.h"
#include "StormDataChangePacket.h"
#include "StormDataParent.h"
#include "StormDataDelta.h"
#include "StormDataSync.h"

struct StormDataTypeInfo
{
  std::string m_Name;
  std::vector<std::pair<uint32_t, void * (*)(void *)>> m_BaseTypes;

  void * (*HeapCreate)();
  void(*HeapFree)(void * data);
  void(*CopyRaw)(const void * src, void * dest);

  void(*EncodeJson)(void * poly_ptr, std::string & sb);
  void(*EncodePrettyJson)(void * poly_ptr, std::string & sb, int indent);
  void(*ParseJson)(void * poly_ptr, const char * data_start);

  void(*ApplySet)(void * poly_ptr, const char * path, const char * data);
  void(*ApplyClear)(void * poly_ptr, const char * path);
  void(*ApplyCompress)(void * poly_ptr, const char * path);
  void(*ApplyInsert)(void * poly_ptr, const char * path, uint64_t index, const char * data);
  void(*ApplyRemove)(void * poly_ptr, const char * path, uint64_t index);
  void(*ApplyRevertDefault)(void * poly_ptr, const char * path);

  void(*SetParentInfo)(void * poly_ptr, const StormReflectionParentInfo & parent_info);
  void(*SetParentInfoFlag)(void * poly_ptr, StormDataParentInfoFlags flags);
  void(*ClearParentInfoCallback)(void * poly_ptr);
  void(*MoveParentInfo)(const void * src, void * dest);

  void(*DeltaCopy)(const void * src, void * dest);
  void(*Sync)(const void * src, void * dest);

  void * CastTo(void * ptr, uint32_t type_name_hash)
  {
    for (auto & base : m_BaseTypes)
    {
      if (base.first == type_name_hash)
      {
        return base.second(ptr);
      }
    }

    return nullptr;
  }
};

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
    type_info.m_BaseTypes.emplace_back(std::make_pair(StormReflTypeInfo<BaseType>::GetNameHash(), [](void * c) -> void * { auto ptr = (Class *)c; return (BaseType *)ptr; });

    StormDataInitBaseClass<TypeInfo, Class, BaseTypes...>::Process(type_info);
  }
};


template <typename Base, typename TypeInfo>
class StormDataTypeDatabase
{
public:

  template <typename Class>
  static void InitTypeInfo(TypeInfo & type_info)
  {
    type_info.m_Nmae = StormReflTypeInfo<Class>::GetName();
    type_info.HeapCreate = []() -> void * { return new Class; };
    type_info.HeapFree = [](void * data) { delete (Class *)data; };
    type_info.CopyRaw = [](const void * src, void * dest) { ((Class *)dest)->SetRaw(*(Class *)src); };

    type_info.EncodeJson = [](void * poly_ptr, std::string & sb) { StormReflEncodeJson(*(Class *)poly_ptr, sb); };
    type_info.EncodePrettyJson = [](void * poly_ptr, std::string & sb, int indent) { StormReflEncodePrettyJson(*(Class *)poly_ptr, sb, indent); };
    type_info.ParseJson = [](void * poly_ptr, const char * data_start) { return StormReflParseJson(*(Class *)poly_ptr, data_start); };

    type_info.ApplySet = [](void * poly_ptr, const char * path, const char * data) { return StormDataChangePacketHelpers::StormDataApplyChangePacketSet<Class>::Process(*(Class *)poly_ptr, path, data); };
    type_info.ApplyClear = [](void * poly_ptr, const char * path) { return StormDataChangePacketHelpers::StormDataApplyChangePacketClear<Class>::Process(*(Class *)poly_ptr, path); };
    type_info.ApplyCompress = [](void * poly_ptr, const char * path) { return StormDataChangePacketHelpers::StormDataApplyChangePacketCompress<Class>::Process(*(Class *)poly_ptr, path); };
    type_info.ApplyInsert = [](void * poly_ptr, const char * path, uint64_t index, const char * data) { return StormDataChangePacketHelpers::StormDataApplyChangePacketInsert<Class>::Process(*(Class *)poly_ptr, path, index, data); };
    type_info.ApplyRemove = [](void * poly_ptr, const char * path, uint64_t index) { return StormDataChangePacketHelpers::StormDataApplyChangePacketRemove<Class>::Process(*(Class *)poly_ptr, path, index); };
    type_info.ApplyRevertDefault = [](void * poly_ptr, const char * path) { return StormDataChangePacketHelpers::StormDataApplyChangePacketRevertDefault<Class>::Process(*(Class *)poly_ptr, path); };

    type_info.SetParentInfo = [](void * poly_ptr, const StormReflectionParentInfo & parent_info) { SetParentInfo(*(Class *)poly_ptr, parent_info); };
    type_info.SetParentInfoFlag = [](void * poly_ptr, StormDataParentInfoFlags flags) { SetParentInfoStruct<Class>::SetFlag(*(Class *)poly_ptr, flags); };
    type_info.ClearParentInfoCallback = [](void * poly_ptr) { SetParentInfoStruct<Class>::ClearParentCallback(*(Class *)poly_ptr); };
    type_info.MoveParentInfo = [](const void * src, void * dest) {SetParentInfoStruct<Class>::MoveParentInfo(*(Class *)src, *(Class *)dst); };

    type_info.DeltaCopy = [](const void * src, void * dest) { StormDataDeltaCopy(*(Class *)src, *(Class *)dst); };
    type_info.Sync = [](const void * src, void * dest) { StormDataSync(*(Class *)src, *(Class *)dst); };
  }


  template <typename Class, typename ... BaseTypes>
  static void RegisterType()
  {
    static_assert(std::is_base_of<Base, Class>::value, "Registering type that is not of the right base class");
    static_assert(std::is_same<Base, Class>::value || sizeof...(BaseTypes) > 0, "Registering type that does not have a base class specified");

    TypeInfo type_info;
    InitTypeInfo<Class>(type_info);

    StormDataInitBaseClass<TypeInfo, Class, BaseTypes...>::Process(type_info);

    auto type_name_hash = StormReflTypeInfo<Class>::GetNameHash();
    m_TypeList.emplace(std::make_pair(type_name_hash, type_info));
  }

  static void FinalizeTypes()
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

  static TypeInfo * GetTypeInfo(uint32_t type_name_hash)
  {
    auto itr = m_TypeList.find(type_name_hash);
    if (itr == m_TypeList.end())
    {
      return nullptr;
    }

    return &itr->second;
  }

private:

  static void AddBaseTypesForType(TypeInfo & type_info, uint32_t base_type_hash, void * (*CastFunc)(void *))
  {
    for (auto & base : type_info.m_BaseTypes)
    {
      if (base.first == base_type_hash)
      {
        return;
      }
    }

    type_info.m_BaseTypes.emplace_back(std::make_pair(base_type_hash, CastFunc));

    auto itr = m_TypeList.find(base.first);
    if (itr == m_TypeList.end())
    {
      assert(false);
    }

    for (auto & base_base : itr->second.m_BaseTypes)
    {
      AddBaseTypesForType(type_info, base_base.first, base_base.second);
    }
  }

private:

  static std::unordered_map<uint32_t, TypeInfo> m_TypeList;
};

