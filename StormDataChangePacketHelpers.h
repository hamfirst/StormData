#pragma once

#include <StormRefl/StormReflMetaFuncs.h>

#include <hash/Hash.h>

#include "StormDataJson.h"
#include "StormDataPath.h"
#include "StormDataJsonUtil.h"

template <typename T> class RSparseList;
template <typename T> class RMergeList;
template <typename K, typename T> class RMap;

namespace StormDataChangePacketHelpers
{
  bool ParseNotifyChangeType(ReflectionNotifyChangeType & type, const char * str, const char *& result);
  bool ParseIndex(uint64_t & val, const char * str, const char *& result);
  bool ParseIndex(uint32_t & val, const char * str, const char *& result);

  template <typename T, bool IsDataRefl>
  struct StormDataApplySetDataRefl
  {
    static void Process(T & t) { }
  };

  template <typename T>
  struct StormDataApplySetDataRefl<T, true>
  {
    static void Process(T & t) 
    {
      ReflectionNotifySetObject(t.m_ReflectionInfo, StormReflEncodeJson(t));
    }
  };

  template <typename T, bool IsClass>
  struct StormDataApplySetClass
  {
    static void Process(T & t) { }
  };

  template <typename T>
  struct StormDataApplySetClass<T, true>
  {
    static void Process(T & t)
    {
      StormDataApplySetDataRefl<T, StormDataCheckReflectable<T>::value>::Process(t);
    }
  };

  template <typename T>
  struct StormDataApplySet
  {
    static bool Process(T & t, const char * result)
    {
      if (StormDataJson<T>::ParseRaw(t, result, result, false) == false)
      {
        return false;
      }

      StormDataApplySetClass<T, std::is_class<T>::value>::Process(t);
      return true;
    }
  };

  template <typename T>
  struct StormDataApplyClear
  {
    static bool Process(T & t)
    {
      return false;
    }
  };

  template <typename T>
  struct StormDataApplyClear<RSparseList<T>>
  {
    static bool Process(RSparseList<T> & t)
    {
      t.Clear();
      return true;
    }
  };

  template <typename T>
  struct StormDataApplyClear<RMergeList<T>>
  {
    static bool Process(RMergeList<T> & t)
    {
      t.Clear();
      return true;
    }
  };

  template <typename K, typename T>
  struct StormDataApplyClear<RMap<K, T>>
  {
    static bool Process(RMap<K, T> & t)
    {
      t.Clear();
      return true;
    }
  };

  template <typename T>
  struct StormDataApplyCompress
  {
    static bool Process(T & t)
    {
      return false;
    }
  };

  template <typename T>
  struct StormDataApplyCompress<RSparseList<T>>
  {
    static bool Process(RSparseList<T> & t)
    {
      t.Compress();
      return true;
    }
  };

  template <typename T>
  struct StormDataApplyCompress<RMergeList<T>>
  {
    static bool Process(RMergeList<T> & t)
    {
      t.Compress();
      return true;
    }
  };

  template <typename T>
  struct StormDataApplyInsert
  {
    static bool Process(T & t, uint64_t index, const char * data_str)
    {
      return false;
    }

    const char * m_Data;
  };

  template <typename T>
  struct StormDataApplyInsert<RSparseList<T>>
  {
    static bool Process(RSparseList<T> & t, uint64_t index, const char * data_str)
    {
      T val;
      if(StormReflJson<T>::Parse(val, data_str, data_str, false) == false)
      {
        return false;
      }

      t.EmplaceAt((std::size_t)index, std::move(val));
      return true;
    }

    const char * m_Data;
  };

  template <typename T>
  struct StormDataApplyInsert<RMergeList<T>>
  {
    static bool Process(RMergeList<T> & t, uint64_t index, const char * data_str)
    {
      T val;
      if (StormReflJson<T>::Parse(val, data_str, data_str, false) == false)
      {
        return false;
      }

      t.EmplaceAt((std::size_t)index, std::move(val));
      return true;
    }

    const char * m_Data;
  };

  template <typename K, typename T>
  struct StormDataApplyInsert<RMap<K, T>>
  {
    static bool Process(RMap<K, T> & t, uint64_t index, const char * data_str)
    {
      T val;
      if (StormReflJson<T>::Parse(val, data_str, data_str, false) == false)
      {
        return false;
      }

      t.Set((K)index, val);
      return true;
    }
  };

  template <typename T>
  struct StormDataApplyRemove
  {
    static bool Process(T & t, uint64_t index)
    {
      return false;
    }
  };

  template <typename T>
  struct StormDataApplyRemove<RSparseList<T>>
  {
    static bool Process(RSparseList<T> & t, uint64_t index)
    {
      t.RemoveAt((std::size_t)index);
      return true;
    }
  };

  template <typename T>
  struct StormDataApplyRemove<RMergeList<T>>
  {
    static bool Process(RMergeList<T> & t, uint64_t index)
    {
      t.RemoveAt((std::size_t)index);
      return true;
    }
  };

  template <typename K, typename T>
  struct StormDataApplyRemove<RMap<K, T>>
  {
    static bool Process(RMap<K, T> & t, uint64_t index)
    {
      t.Remove((K)index);
      return true;
    }
  };

  template <typename T>
  struct StormDataApplyRearrange
  {
    static bool Process(T & t)
    {
      return false;
    }
  };

  struct StormDataApplyRemovePacket
  {
    template <typename T>
    bool operator()(T & t, const char * result)
    {
      uint64_t index;
      if (ParseIndex(index, result, result) == false)
      {
        return false;
      }

      if (*result != ' ')
      {
        return false;
      }

      result++;
      return StormDataApplyRemove<T>::Remove(t, index);
    }
  };

  template <class T>
  struct StormDataApplyChangePacketSet
  {
    static bool Process(T & t, const char * path, const char * data)
    {
      if (*path == 0)
      {
        return StormDataApplySet<T>::Process(t, data);
      }
      else if (*path == ' ')
      {
        path++;
        return StormDataApplySet<T>::Process(t, path);
      }

      auto visitor = [&](auto & new_t, auto * field_data, const char * new_str)
      {
        using MemberType = typename std::decay_t<decltype(new_t)>;
        return StormDataApplyChangePacketSet<MemberType>::Process(new_t, new_str, data);
      };

      return StormDataVisitPathElement(t, visitor, path);
    }
  };

  template <typename Base, typename TypeDatabase, typename TypeInfo, bool DefaultFirstNonBase>
  struct StormDataApplyChangePacketSet<RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase>>
  {
    static bool Process(RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase> & t, const char * path, const char * data)
    {
      if (*path == 0)
      {
        return StormDataApplySet<RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase>>::Process(t, data);
      }
      else if (*path == ' ')
      {
        path++;
        return StormDataApplySet<RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase>>::Process(t, path);
      }

      if (*path == '.' && *(path + 1) == 'T')
      {
        path += 2;
        if (*path != ' ' || *path != 0)
        {
          return false;
        }

        uint32_t type_name_hash;
        if (StormReflParseJson(type_name_hash, *path == 0 ? data : path + 1) == false)
        {
          return false;
        }

        t.SetTypeFromNameHash(type_name_hash);
        return true;
      }
      else if(*path == '.' && *(path + 1) == 'D')
      {
        path += 2;
        if (*path != ' ' && *path != 0 && *path != '[' && *path != '.')
        {
          return false;
        }

        return t.GetTypeInfo() ? t.GetTypeInfo()->ApplySet(t.GetValue(), path, data) : false;
      }

      return false;
    }
  };

  template <class T>
  struct StormDataApplyChangePacketClear
  {
    static bool Process(T & t, const char * path)
    {
      if (*path == 0 || *path == ' ')
      {
        return StormDataApplyClear<T>::Process(t);
      }

      auto visitor = [&](auto & new_t, auto * field_data, const char * new_str)
      {
        using MemberType = typename std::decay_t<decltype(new_t)>;
        return StormDataApplyChangePacketClear<MemberType>::Process(new_t, new_str);
      };

      return StormDataVisitPathElement(t, visitor, path);
    }
  };

  template <typename Base, typename TypeDatabase, typename TypeInfo, bool DefaultFirstNonBase>
  struct StormDataApplyChangePacketClear<RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase>>
  {
    static bool Process(RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase> & t, const char * path)
    {
      if (*path == 0 || *path == ' ')
      {
        return false;
      }

      if (*path == '.' && *(path + 1) + 'D')
      {
        if (*path != ' ' && *path != 0 && *path != '[' && *path != '.')
        {
          return false;
        }

        path += 2;
        return t.GetTypeInfo() ? t.GetTypeInfo()->ApplyClear(t.GetValue(), path) : false;
      }

      return false;
    }
  };

  template <class T>
  struct StormDataApplyChangePacketCompress
  {
    static bool Process(T & t, const char * path)
    {
      if (*path == 0 || *path == ' ')
      {
        return StormDataApplyCompress<T>::Process(t);
      }

      auto visitor = [&](auto & new_t, auto * field_data, const char * new_str)
      {
        using MemberType = typename std::decay_t<decltype(new_t)>;
        return StormDataApplyChangePacketCompress<MemberType>::Process(new_t, new_str);
      };

      return StormDataVisitPathElement(t, visitor, path);
    }
  };

  template <typename Base, typename TypeDatabase, typename TypeInfo, bool DefaultFirstNonBase>
  struct StormDataApplyChangePacketCompress<RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase>>
  {
    static bool Process(RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase> & t, const char * path)
    {
      if (*path == 0 || *path == ' ')
      {
        return false;
      }

      if (*path == '.' && *(path + 1) + 'D')
      {
        if (*path != ' ' && *path != 0 && *path != '[' && *path != '.')
        {
          return false;
        }

        path += 2;
        return t.GetTypeInfo() ? t.GetTypeInfo()->ApplyCompress(t.GetValue(), path) : false;
      }

      return false;
    }
  };

  template <class T>
  struct StormDataApplyChangePacketInsert
  {
    static bool Process(T & t, const char * path, uint64_t index, const char * data)
    {
      if (*path == 0)
      {
        return StormDataApplyInsert<T>::Process(t, index, data);
      }
      else if (*path == ' ')
      {
        path++;
        if (ParseIndex(index, path, path) == false)
        {
          return false;
        }

        if (*path != ' ')
        {
          return false;
        }

        path++;
        return StormDataApplyInsert<T>::Process(t, index, path);
      }

      auto visitor = [&](auto & new_t, auto * field_data, const char * new_str)
      {
        using MemberType = typename std::decay_t<decltype(new_t)>;
        return StormDataApplyChangePacketInsert<MemberType>::Process(new_t, new_str, index, data);
      };

      return StormDataVisitPathElement(t, visitor, path);
    }
  };

  template <typename Base, typename TypeDatabase, typename TypeInfo, bool DefaultFirstNonBase>
  struct StormDataApplyChangePacketInsert<RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase>>
  {
    static bool Process(RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase> & t, const char * path, uint64_t index, const char * data)
    {
      if (*path == 0 || *path == ' ')
      {
        return false;
      }

      if (*path == '.' && *(path + 1) + 'D')
      {
        if (*path != ' ' && *path != 0 && *path != '[' && *path != '.')
        {
          return false;
        }

        path += 2;
        return t.GetTypeInfo() ? t.GetTypeInfo()->ApplyInsert(t.GetValue(), path, index, data) : false;
      }

      return false;
    }
  };

  template <class T>
  struct StormDataApplyChangePacketRemove
  {
    static bool Process(T & t, const char * path, uint64_t index)
    {
      if (*path == 0)
      {
        return StormDataApplyRemove<T>::Process(t, index);
      }
      else if (*path == ' ')
      {
        path++;
        if (ParseIndex(index, path, path) == false)
        {
          return false;
        }

        return StormDataApplyRemove<T>::Process(t, index);
      }

      auto visitor = [&](auto & new_t, auto * field_data, const char * new_str)
      {
        using MemberType = typename std::decay_t<decltype(new_t)>;
        return StormDataApplyChangePacketRemove<MemberType>::Process(new_t, new_str, index);
      };

      return StormDataVisitPathElement(t, visitor, path);
    }
  };

  template <class T>
  struct StormDataApplyChangePacketRearrange
  {
    static bool Process(T & t, const char * path, uint64_t index)
    {
      if (*path == 0)
      {
        return StormDataApplyRearrange<T>::Process(t, index);
      }
      else if (*path == ' ')
      {
        path++;
        if (ParseIndex(index, path, path) == false)
        {
          return false;
        }

        return StormDataApplyRearrange<T>::Process(t, index);
      }

      auto visitor = [&](auto & new_t, auto * field_data, const char * new_str)
      {
        using MemberType = typename std::decay_t<decltype(new_t)>;
        return StormDataApplyChangePacketRearrange<MemberType>::Process(new_t, new_str, index);
      };

      return StormDataVisitPathElement(t, visitor, path);
    }
  };

  template <typename Base, typename TypeDatabase, typename TypeInfo, bool DefaultFirstNonBase>
  struct StormDataApplyChangePacketRemove<RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase>>
  {
    static bool Process(RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase> & t, const char * path, uint64_t index)
    {
      if (*path == 0 || *path == ' ')
      {
        return false;
      }

      if (*path == '.' && *(path + 1) + 'D')
      {
        if (*path != ' ' && *path != 0 && *path != '[' && *path != '.')
        {
          return false;
        }

        path += 2;
        return t.GetTypeInfo() ? t.GetTypeInfo()->ApplyRemove(t.GetValue(), path, index) : false;
      }

      return false;
    }
  };

  template <class T, class FieldData>
  struct StormDataApplyChangePacketRevert
  {
    static bool Process(T & t, FieldData * field_data, const char * path)
    {
      if (*path == 0 || *path == ' ')
      {
        field_data->SetDefault();
        return true;
      }

      auto visitor = [&](auto & new_t, auto * field_data, const char * new_str)
      {
        using MemberType = typename std::decay_t<decltype(new_t)>;
        using FieldDataType = typename std::decay_t<decltype(*field_data)>;
        return StormDataApplyChangePacketRevert<MemberType, FieldDataType>::Process(new_t, field_data, new_str);
      };

      return StormDataVisitPathElement(t, visitor, path);
    }
  };

  template <class T>
  struct StormDataApplyChangePacketRevertDefault
  {
    static bool Process(T & t, const char * path)
    {
      if (*path == 0 || *path == ' ')
      {
        t = {};
        return true;
      }

      auto visitor = [&](auto & new_t, auto * field_data, const char * new_str)
      {
        using MemberType = typename std::decay_t<decltype(new_t)>;
        using FieldType = typename std::decay_t<decltype(*field_data)>;
        return StormDataApplyChangePacketRevert<MemberType, FieldType>::Process(new_t, field_data, new_str);
      };

      return StormDataVisitPathElement(t, visitor, path);
    }
  };

  template <class Base, class TypeDatabase, class TypeInfo, bool DefaultFirstNonBase>
  struct StormDataApplyChangePacketRevertDefault<RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase>>
  {
    static bool Process(RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase> & t, const char * path)
    {
      if (*path == 0 || *path == ' ')
      {
        t = {};
        return true;
      }

      if (*path == '.' && *(path + 1) == 'T')
      {
        path += 2;
        if (*path != ' ' || *path != 0)
        {
          return false;
        }

        t.SetTypeFromNameHash(0);
        return true;
      }
      else if (*path == '.' && *(path + 1) == 'D')
      {
        path += 2;
        if (*path != ' ' && *path != 0 && *path != '[' && *path != '.')
        {
          return false;
        }

        return t.GetTypeInfo() ? t.GetTypeInfo()->ApplyRevertDefault(t.GetValue(), path) : false;
      }

      return false;
    }
  };
}
