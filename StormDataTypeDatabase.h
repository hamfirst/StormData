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

template <typename Base, typename TypeInfo>
class StormDataTypeDatabaseVistorInfo
{

};

template <typename Base, typename TypeInfo, typename Visitor>
void StormDataTypeDatabaseVisitTypes(const StormDataTypeDatabaseVistorInfo<Base, TypeInfo> & db_info, Visitor && visitor);

template <typename Base, typename TypeInfo>
class StormDataTypeDatabase
{
public:

  using VisitorInfo = StormDataTypeDatabaseVistorInfo<Base, TypeInfo>;

  template <typename Class>
  static void InitTypeInfo(TypeInfo & type_info);

  template <typename Class, typename ... BaseTypes>
  static void RegisterType();

  static void FinalizeTypes();

  static TypeInfo * GetTypeInfo(uint32_t type_name_hash);
  static TypeInfo * GetFirstNonBaseType();

protected:

  template <typename BaseDef, typename TypeInfoDef, typename Visitor>
  friend void StormDataTypeDatabaseVisitTypes(const StormDataTypeDatabaseVistorInfo<BaseDef, TypeInfoDef> & db_info, Visitor && visitor);

  static void AddBaseTypesForType(TypeInfo & type_info, uint32_t base_type_hash, void * (*CastFunc)(void *));

protected:

  static std::unordered_map<uint32_t, TypeInfo> m_TypeList;
};

template <typename Base, typename TypeInfo>
std::unordered_map<uint32_t, TypeInfo> StormDataTypeDatabase<Base, TypeInfo>::m_TypeList;

template <typename Base, typename TypeInfo, typename Visitor>
void StormDataTypeDatabaseVisitTypes(const StormDataTypeDatabaseVistorInfo<Base, TypeInfo> & db_info, Visitor && visitor)
{
  for (auto & elem : StormDataTypeDatabase<Base, TypeInfo>::m_TypeList)
  {
    visitor(elem.first, elem.second);
  }
}
