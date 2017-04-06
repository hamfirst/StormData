#pragma once

#include "StormDataTypeDatabaseRegister.h"

template <typename Base, typename TypeInfo> template <typename Class>
void StormDataTypeDatabase<Base, TypeInfo>::InitTypeInfo<Class>(TypeInfo & type_info)
{
  type_info.m_Name = StormReflTypeInfo<Class>::GetName();
  type_info.HeapCreate = []() -> void * { return new Class; };
  type_info.HeapFree = [](void * data) { delete (Class *)data; };
  type_info.CopyRaw = [](const void * src, void * dest) { ((Class *)dest)->SetRaw(*(Class *)src); };

  type_info.EncodeJson = [](const void * poly_ptr, std::string & sb) { StormReflEncodeJson(*(const Class *)poly_ptr, sb); };
  type_info.EncodePrettyJson = [](const void * poly_ptr, std::string & sb, int indent) { StormReflEncodePrettyJson(*(const Class *)poly_ptr, sb, indent); };
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
  type_info.MoveParentInfo = [](void * src, void * dest) {SetParentInfoStruct<Class>::MoveParentInfo(*(Class *)src, *(Class *)dest); };

  type_info.DeltaCopy = [](const void * src, void * dest) { StormDataDeltaCopy(*(const Class *)src, *(Class *)dest); };
  type_info.Sync = [](const void * src, void * dest, const char * path) { StormDataSync(*(const Class *)src, *(Class *)dest, path); };
}

