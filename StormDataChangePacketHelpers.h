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
  bool ParseIndex(uint64_t & val, const char * str, const char *& result);

  template <typename T>
  struct StormDataApplySet
  {
    static bool Process(T & t, const char * result)
    {
      return StormReflJson<T>::Parse(t, result, result);
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
      auto & elem = t.InsertAt(T{}, index);
      return StormReflJson<T>::Parse(elem, data_str, data_str);
    }

    const char * m_Data;
  };

  template <typename T>
  struct StormDataApplyInsert<RMergeList<T>>
  {
    static bool Process(RMergeList<T> & t, uint64_t index, const char * data_str)
    {
      auto & elem = t.InsertAt(T{}, index);
      return StormReflJson<T>::Parse(elem, data_str, data_str);
    }

    const char * m_Data;
  };

  template <typename K, typename T>
  struct StormDataApplyInsert<RMap<K, T>>
  {
    static bool Process(RMap<K, T> & t, uint64_t index, const char * data_str)
    {
      auto & elem = t.Set(index, T{});
      return StormReflJson<T>::Parse(elem, data_str, data_str);
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
      t.RemoveAt(index);
      return true;
    }
  };

  template <typename T>
  struct StormDataApplyRemove<RMergeList<T>>
  {
    static bool Process(RMergeList<T> & t, uint64_t index)
    {
      t.RemoveAt(index);
      return true;
    }
  };

  template <typename K, typename T>
  struct StormDataApplyRemove<RMap<K, T>>
  {
    static bool Process(RMap<K, T> & t, uint64_t index)
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
  };
}
