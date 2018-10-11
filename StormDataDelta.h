#pragma once

#include "StormDataTypes.h"
#include "StormRefl/StormReflMetaFuncs.h"

template <typename T>
void StormDataDeltaCopy(const T & src, T & dest);

namespace StormDataDeltaHelpers
{
  template <typename T>
  struct StormDataDeltaBase
  {
    static void Process(const T & src, T & dest)
    {
      if (src == dest)
      {
        return;
      }
      else
      {
        dest = src;
      }
    }
  };

  template <typename T>
  struct StormDataDeltaStruct
  {
    static void Process(const T & src, T & dest)
    {
      auto visitor = [](auto src_f, auto dst_f)
      {
        StormDataDeltaCopy(src_f.Get(), dst_f.Get());
      };

      StormReflVisitEach(src, dest, visitor);
    }
  };

  template <typename T>
  struct StormDataDelta : public std::conditional_t<StormReflCheckReflectable<T>::value && StormDataCheckReflectable<T>::value, StormDataDeltaStruct<T>, StormDataDeltaBase<T>>
  {

  };

  template <typename T>
  struct StormDataDeltaMap
  {
    static void Process(const T & src, T & dest)
    {
      auto src_itr = src.begin();
      auto dest_itr = dest.begin();

      std::vector<std::size_t> copy_indices;
      std::vector<std::size_t> dead_indices;

      while (src_itr != src.end())
      {
        if (dest_itr == dest.end())
        {
          copy_indices.emplace_back(src_itr->first);
        }
        else
        {
          while (dest_itr != dest.end() && dest_itr->first < src_itr->first)
          {
            dead_indices.emplace_back(dest_itr->first);
            ++dest_itr;
          }

          if (dest_itr != dest.end() && dest_itr->first == src_itr->first)
          {
            StormDataDeltaCopy(src_itr->second, dest_itr->second);
            ++dest_itr;
          }
          else
          {
            copy_indices.emplace_back(src_itr->first);
          }
        }

        ++src_itr;
      }

      while (dest_itr != dest.end())
      {
        dead_indices.emplace_back(dest_itr->first);
        ++dest_itr;
      }

      for (auto & dead_idx : dead_indices)
      {
        dest.RemoveAt(dead_idx);
      }

      for (auto & copy_idx : copy_indices)
      {
        dest.EmplaceAt(copy_idx, src[copy_idx]);
      }
    }
  };

  template <typename T>
  struct StormDataDelta<RMergeList<T>> : public StormDataDeltaMap<RMergeList<T>>
  {

  };

  template <typename T>
  struct StormDataDelta<RSparseList<T>> : public StormDataDeltaMap<RSparseList<T>>
  {

  };

  template <typename K, typename T>
  struct StormDataDelta<RMap<K, T>> : public StormDataDeltaMap<RMap<K, T>>
  {

  };

  template <class Base, class TypeDatabase, class TypeInfo, bool DefaultFirstNonBase>
  struct StormDataDelta<RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase>>
  {
    static void Process(const RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase> & src, RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase> & dest)
    {
      if (src.GetTypeNameHash() != dest.GetTypeNameHash())
      {
        dest = src;
        return;
      }

      if (dest.GetTypeInfo())
      {
        dest.GetTypeInfo()->DeltaCopy(src.GetValue(), dest.GetValue());
      }
    }
  };
}

template <typename T>
void StormDataDeltaCopy(const T & src, T & dest)
{
  StormDataDeltaHelpers::StormDataDelta<T>::Process(src, dest);
}
