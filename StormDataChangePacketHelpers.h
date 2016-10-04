#pragma once

#include <StormRefl\StormReflMetaFuncs.h>

#include <hash\Hash.h>

#include "StormDataJson.h"
#include "StormDataPath.h"

template <typename T> class RSparseList;
template <typename T> class RMergeList;
template <typename K, typename T> class RMap;

namespace StormDataChangePacketHelpers
{
  bool ParseNotifyChangeType(ReflectionNotifyChangeType & type, const char * str, const char *& result);
  bool ParseIndex(uint32_t & val, const char * str, const char *& result);

  struct StormDataApplySetPacket
  {
    template <typename T>
    bool operator()(T & t, const char * result)
    {
      return StormReflJson<T>::Parse(t, result, result);
    }

    template <typename T>
    static bool Process(T & t, const char * str)
    {
      StormDataApplySetPacket visitor;
      return StormDataVisitPath(t, visitor, str);
    }
  };

  template <typename T>
  struct StormDataApplyClear
  {
    static bool Clear(T & t)
    {
      return false;
    }
  };

  template <typename T>
  struct StormDataApplyClear<RSparseList<T>>
  {
    static bool Clear(RSparseList<T> & t)
    {
      t.Clear();
      return true;
    }
  };

  template <typename T>
  struct StormDataApplyClear<RMergeList<T>>
  {
    static bool Clear(RMergeList<T> & t)
    {
      t.Clear();
      return true;
    }
  };

  template <typename K, typename T>
  struct StormDataApplyClear<RMap<K, T>>
  {
    static bool Clear(RMap<K, T> & t)
    {
      t.Clear();
      return true;
    }
  };

  struct StormDataApplyClearPacket
  {
    template <typename T>
    bool operator()(T & t, const char * result)
    {
      return StormDataApplyClear<T>::Clear(t);
    }

    template <typename T>
    static bool Process(T & t, const char * str)
    {
      StormDataApplyClearPacket visitor;
      return StormDataVisitPath(t, visitor, str);
    }
  };

  template <typename T>
  struct StormDataApplyCompress
  {
    static bool Compress(T & t)
    {
      return false;
    }
  };

  template <typename T>
  struct StormDataApplyCompress<RSparseList<T>>
  {
    static bool Compress(RSparseList<T> & t)
    {
      t.Compress();
      return true;
    }
  };

  template <typename T>
  struct StormDataApplyCompress<RMergeList<T>>
  {
    static bool Compress(RMergeList<T> & t)
    {
      t.Compress();
      return true;
    }
  };

  struct StormDataApplyCompressPacket
  {
    template <typename T>
    bool operator()(T & t, const char * result)
    {
      return StormDataApplyCompress<T>::Compress(t);
    }

    template <typename T>
    static bool Process(T & t, const char * str)
    {
      StormDataApplyCompressPacket visitor;
      return StormDataVisitPath(t, visitor, str);
    }
  };

  template <typename T>
  struct StormDataApplyInsert
  {
    static bool Insert(T & t, uint32_t index, const char * data_str)
    {
      return false;
    }
  };

  template <typename T>
  struct StormDataApplyInsert<RSparseList<T>>
  {
    static bool Insert(RSparseList<T> & t, uint32_t index, const char * data_str)
    {
      auto & elem = t.InsertAt(T{}, index);
      return StormReflJson<T>::Parse(elem, data_str, data_str);
    }
  };

  template <typename T>
  struct StormDataApplyInsert<RMergeList<T>>
  {
    static bool Insert(RMergeList<T> & t, uint32_t index, const char * data_str)
    {
      auto & elem = t.InsertAt(T{}, index);
      return StormReflJson<T>::Parse(elem, data_str, data_str);
    }
  };

  template <typename K, typename T>
  struct StormDataApplyInsert<RMap<K, T>>
  {
    static bool Insert(RMap<K, T> & t, uint32_t index, const char * data_str)
    {
      auto & elem = t.Set(index, T{});
      return StormReflJson<T>::Parse(elem, data_str, data_str);
    }
  };

  struct StormDataApplyInsertPacket
  {
    template <typename T>
    bool operator()(T & t, const char * result)
    {
      uint32_t index;
      if (ParseIndex(index, result, result) == false)
      {
        return false;
      }

      if (*result != ' ')
      {
        return false;
      }

      result++;
      return StormDataApplyInsert<T>::Insert(t, index, result);
    }

    template <typename T>
    static bool Process(T & t, const char * str)
    {
      StormDataApplyInsertPacket visitor;
      return StormDataVisitPath(t, visitor, str);
    }
  };

  template <typename T>
  struct StormDataApplyRemove
  {
    static bool Remove(T & t, uint32_t index)
    {
      return false;
    }
  };

  template <typename T>
  struct StormDataApplyRemove<RSparseList<T>>
  {
    static bool Remove(RSparseList<T> & t, uint32_t index)
    {
      t.RemoveAt(index);
      return true;
    }
  };

  template <typename T>
  struct StormDataApplyRemove<RMergeList<T>>
  {
    static bool Remove(RMergeList<T> & t, uint32_t index)
    {
      t.RemoveAt(index);
      return true;
    }
  };

  template <typename K, typename T>
  struct StormDataApplyRemove<RMap<K, T>>
  {
    static bool Remove(RMap<K, T> & t, uint32_t index)
    {
      t.Remove(index);
      return true;
    }
  };

  struct StormDataApplyRemovePacket
  {
    template <typename T>
    bool operator()(T & t, const char * result)
    {
      uint32_t index;
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

    template <typename T>
    static bool Process(T & t, const char * str)
    {
      StormDataApplyRemovePacket visitor;
      return StormDataVisitPath(t, visitor, str);
    }
  };
}
