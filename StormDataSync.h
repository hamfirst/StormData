#pragma once

#include "StormDataTypes.h"
#include "StormDataDelta.h"

#include <StormRefl/StormReflMetaFuncs.h>

template <typename T>
bool StormDataSync(const T & src, T & dest, const char * path);

namespace StormDataSyncHelpers
{
  template <typename T>
  struct StormDataSyncBase
  {
    static bool Process(const T & src, T & dest, const char * path)
    {
      if (*path != 0)
      {
        return false;
      }

      StormDataDeltaCopy(src, dest);
      return true;
    }
  };

  template <typename T>
  struct StormDataSyncStruct
  {
    static bool Process(const T & src, T & dest, const char * path)
    {
      if (*path == 0)
      {
        StormDataDeltaCopy(src, dest);
        return true;
      }

      if (*path == '.')
      {
        path++;

        auto hash = crc32begin();
        while (*path != 0 && *path != '[' && *path != '.')
        {
          hash = crc32additive(hash, *path);
          path++;
        }

        hash = crc32end(hash);
        bool success = false;

        auto visitor = [&](auto f1, auto f2)
        {
          success = ::StormDataSync(f1.Get(), f2.Get(), path);
        };

        StormReflVisitField(src, dest, visitor, hash);
      }

      return false;
    }
  };

  template <typename T>
  struct StormDataSync : public std::conditional_t<StormReflCheckReflectable<T>::value && StormDataCheckReflectable<T>::value, StormDataSyncStruct<T>, StormDataSyncBase<T>>
  {

  };

  template <typename T>
  struct StormDataSyncMap
  {
    static bool Process(const T & src, T & dest, const char * path)
    {
      if (*path == 0)
      {
        StormDataDeltaCopy(src, dest);
        return true;
      }

      if (*path == '[')
      {
        path++;

        if (*path < '0' || *path > '9')
        {
          return false;
        }

        std::size_t index = *path - '0';      
        path++;

        while (*path != ']')
        {
          if (*path < '0' || *path > '9')
          {
            return false;
          }

          index *= 10;
          index += *path - '0';
          path++;
        }

        path++;

        if (src.HasAt(index) == false)
        {
          if (*path == 0)
          {
            dest.RemoveAt(index);
            return true;
          }

          return false;
        }

        if(dest.HasAt(index) == false)
        {
          dest.InsertAt(index, {});
        }

        return ::StormDataSync(src[index], dest[index], path);
      }

      return false;
    }
  };

  template <typename T>
  struct StormDataSync<RMergeList<T>> : public StormDataSyncMap<RMergeList<T>>
  {

  };

  template <typename T>
  struct StormDataSync<RSparseList<T>> : public StormDataSyncMap<RSparseList<T>>
  {

  };

  template <typename K, typename T>
  struct StormDataSync<RMap<K, T>> : public StormDataSyncMap<RMap<K, T>>
  {

  };

  template <class Base, class TypeDatabase, class TypeInfo, bool DefaultFirstNonBase>
  struct StormDataSync<RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase>>
  {
    static bool Process(const RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase> & src,
                              RPolymorphicBase<Base, TypeDatabase, TypeInfo, DefaultFirstNonBase> & dest, const char * path)
    {
      if (*path == 0)
      {
        StormDataDeltaCopy(src, dest);
        return true;
      }

      if (*path == '.')
      {
        path++;

        if (*path == 'T')
        {
          StormDataDeltaCopy(src, dest);
          return true;
        }
        else if (*path == 'D')
        {
          path++;
          if (*path == 0)
          {
            StormDataDeltaCopy(src, dest);
            return true;
          }
          else
          {
            if (*path != '.' && *path != '[')
            {
              return false;
            }

            if (src.GetTypeNameHash() != dest.GetTypeNameHash())
            {
              StormDataDeltaCopy(src, dest);
              return true;
            }
            else if(dest.GetTypeInfo() != nullptr)
            {
              dest.GetTypeInfo()->Sync(src.GetValue(), dest.GetValue(), path);
            }
          }
        }
        else
        {
          return false;
        }
      }

      return false;
    }
  };
}

template <typename T>
bool StormDataSync(const T & src, T & dest, const char * path)
{
  return StormDataSyncHelpers::StormDataSync<T>::Process(src, dest, path);
}
