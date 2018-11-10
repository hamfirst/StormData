#pragma once

#include <cstdint>
#include <cassert>

#include <string>
#include <vector>
#include <unordered_map>
#include <type_traits>

struct StormDataTypeInfo
{
  std::string m_Name;
  std::vector<std::pair<uint32_t, void * (*)(void *)>> m_BaseTypes;
  uint32_t m_TypeNameHash;
  std::size_t m_TypeIdHash;

  void * (*HeapCreate)();
  void(*HeapFree)(void * data);
  void(*CopyRaw)(const void * src, void * dest);

  void(*EncodeJson)(const void * poly_ptr, std::string & sb);
  void(*EncodePrettyJson)(const void * poly_ptr, std::string & sb, int indent);
  bool(*ParseJson)(void * poly_ptr, const char * data_start, bool additive);

  bool(*ApplySet)(void * poly_ptr, const char * path, const char * data);
  bool(*ApplyClear)(void * poly_ptr, const char * path);
  bool(*ApplyCompress)(void * poly_ptr, const char * path);
  bool(*ApplyInsert)(void * poly_ptr, const char * path, uint64_t index, const char * data);
  bool(*ApplyRemove)(void * poly_ptr, const char * path, uint64_t index);
  bool(*ApplyRevertDefault)(void * poly_ptr, const char * path);

  void(*SetParentInfo)(void * poly_ptr, const StormReflectionParentInfo & parent_info);
  void(*SetParentInfoFlag)(void * poly_ptr, StormDataParentInfoFlags flags);
  void(*ClearParentInfoFlag)(void * poly_ptr, StormDataParentInfoFlags flags);
  void(*ClearParentInfoCallback)(void * poly_ptr);
  void(*MoveParentInfo)(void * src, void * dest);

  void(*DeltaCopy)(const void * src, void * dest);
  void(*Sync)(const void * src, void * dest, const char * path);

  void *(*Cast)(std::size_t type_id, void * obj);
  const void *(*ConstCast)(std::size_t type_id, const void * obj);

  void * CastTo(void * ptr, uint32_t type_name_hash) const
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

template <typename Base, typename TypeInfo>
class StormDataTypeDatabase;

template <typename Base, typename TypeInfo, typename Visitor>
void StormDataTypeDatabaseVisitTypes(const StormDataTypeDatabase<Base, TypeInfo> & db_info, Visitor && visitor);

template <typename Base, typename TypeInfo>
class StormDataTypeDatabase
{
public:

  void FinalizeTypes();

  TypeInfo * GetTypeInfo(uint32_t type_name_hash);
  TypeInfo * GetFirstNonBaseType();

  template <typename Visitor>
  void VisitTypes(Visitor && visitor)
  {
    for(auto & elem : m_TypeList)
    {
      visitor(elem.second);
    }
  }

protected:

  template <typename BaseDef, typename TypeInfoDef, typename Visitor>
  friend void StormDataTypeDatabaseVisitTypes(const StormDataTypeDatabase<BaseDef, TypeInfoDef> & db_info, Visitor && visitor);

  void AddBaseTypesForType(TypeInfo & type_info, uint32_t base_type_hash, void * (*CastFunc)(void *));

protected:

  std::unordered_map<uint32_t, TypeInfo> m_TypeList;
};

template <typename DerivedTypeDatabase>
class StormDataTypeDatabaseSingleton
{
public:

  static DerivedTypeDatabase & Get()
  {
    static DerivedTypeDatabase db;
    return db;
  }
};

template <typename Base, typename TypeInfo, typename Visitor>
void StormDataTypeDatabaseVisitTypes(const StormDataTypeDatabase<Base, TypeInfo> & db_info, Visitor && visitor)
{
  for (auto & elem : db_info.m_TypeList)
  {
    visitor(elem.first, elem.second);
  }
}


