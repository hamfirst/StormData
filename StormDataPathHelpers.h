#pragma once

#include <hash/Hash.h>
#include <StormRefl/StormReflMetaFuncs.h>

template <typename T> class RSparseList;
template <typename T> class RMergeList;
template <typename K, typename T> class RMap;

#include "StormDataChangeType.refl.h"

namespace StormDataChangePacketHelpers
{
  bool ParseNotifyChangeType(ReflectionNotifyChangeType & type, const char * str, const char *& result);
  bool ParseIndex(uint64_t & val, const char * str, const char *& result);
}

struct StormDataNullField
{
  void SetDefault()
  {

  }

  static constexpr bool HasDefault()
  {
    return true;
  }
};

namespace StormDataPathHelpers
{
  template <typename T, typename Enable = void>
  struct StormDataPath
  {
    template <typename Callable>
    static bool VisitPath(T & t, Callable & callable, const char * str)
    {
      return false;
    }

    bool HasAtPath(const char * path)
    {
      return false;
    }
  };

  template <typename T, int i>
  struct StormDataPath<T[i], void>
  {
    template <typename Callable>
    static bool VisitPath(T(&t)[i], Callable & callable, const char * str)
    {
      if (*str != '[')
      {
        return false;
      }

      str++;
      uint64_t index;
      if (StormDataChangePacketHelpers::ParseIndex(index, str, str) == false)
      {
        return false;
      }

      if (*str != ']')
      {
        return false;
      }

      str++;
      return callable(t[index], (StormDataNullField *)nullptr, str);
    }

    bool HasAtPath(const char * path)
    {
      return *path == 0;
    }
  };


  template <typename T>
  struct StormDataPath<T, typename std::enable_if_t<StormReflCheckReflectable<T>::value>>
  {
    template <typename Callable>
    static bool VisitPath(T & t, Callable & callable, const char * str)
    {
      if (*str != '.')
      {
        return false;
      }

      str++;
      if (*str == '[' || *str == ']' || *str == '.')
      {
        return false;
      }

      Hash field_name_hash = crc32begin();

      do
      {
        field_name_hash = crc32additive(field_name_hash, *str);
        str++;
      } while (*str != '[' && *str != ']' && *str != '.' && *str != 0 && *str != ' ');

      field_name_hash = crc32end(field_name_hash);

      bool result = false;
      auto field_visitor = [&](auto f)
      {
        result = callable(f.Get(), &f, str);
      };

      StormReflVisitField(t, field_visitor, field_name_hash);
      return result;
    }
  };

  template <typename T>
  struct StormDataPath<RSparseList<T>, void>
  {
    template <typename Callable>
    static bool VisitPath(RSparseList<T> & t, Callable & callable, const char * str)
    {
      if (*str != '[')
      {
        return false;
      }

      str++;
      uint64_t index;
      if (StormDataChangePacketHelpers::ParseIndex(index, str, str) == false)
      {
        return false;
      }

      if (t.HasAt((std::size_t)index) == false)
      {
        return false;
      }

      if (*str != ']')
      {
        return false;
      }

      str++;
      return callable(t[(int)index], (StormDataNullField *)nullptr, str);
    }
  };

  template <typename T>
  struct StormDataPath<RMergeList<T>, void>
  {
    template <typename Callable>
    static bool VisitPath(RMergeList<T> & t, Callable & callable, const char * str)
    {
      if (*str != '[')
      {
        return false;
      }

      str++;
      uint64_t index;
      if (StormDataChangePacketHelpers::ParseIndex(index, str, str) == false)
      {
        return false;
      }

      if (t.HasAt((int)index) == false)
      {
        return false;
      }

      if (*str != ']')
      {
        return false;
      }

      str++;

      return callable(t[(int)index], (StormDataNullField *)nullptr, str);
    }
  };

  template <typename K, typename T>
  struct StormDataPath<RMap<K, T>, void>
  {
    template <typename Callable>
    static bool VisitPath(RMap<K, T> & t, Callable & callable, const char * str)
    {
      if (*str != '[')
      {
        return false;
      }

      str++;
      uint64_t index;
      if (StormDataChangePacketHelpers::ParseIndex(index, str, str) == false)
      {
        return false;
      }

      auto val = t.TryGet(static_cast<K>(index));
      if (val == nullptr)
      {
        return false;
      }

      if (*str != ']')
      {
        return false;
      }

      str++;
      return callable(*val, (StormDataNullField *)nullptr, str);
    }
  };

  template <typename T, typename Enable = void>
  struct StormDataHasPath
  {
    static bool Process(T & t, const char * path)
    {
      return *path == 0;
    }
  };

  template <typename T>
  struct StormDataHasPath<T, typename std::enable_if<StormReflCheckReflectable<T>::value>::type>
  {
    static bool Process(T & t, const char * path)
    {
      if (*path == 0)
      {
        return true;
      }

      if (*path != '.')
      {
        return false;
      }

      uint32_t hash = crc32begin();
      while (*path != '[' && *path != '.' && *path != 0)
      {
        hash = crc32additive(hash, *path);
        path++;
      }

      hash = crc32end(hash);
      bool has_path = false;

      auto visitor = [&](auto f)
      {
        using MemberType = typename decltype(f)::member_type;
        has_path = StormDataHasPath<MemberType>::Process(f.Get(), path);
      };

      StormReflVisitField(t, visitor, hash);
      return has_path;
    }
  };

  template <typename ListType, typename T>
  struct StormDataHasPathList
  {
    static bool Process(ListType & t, const char * path)
    {
      if (*path == 0)
      {
        return true;
      }

      if (*path != '[')
      {
        return false;
      }

      std::size_t index = 0;
      while (*path != ']')
      {
        if (*path != 0)
        {
          return false;
        }

        index *= 10;
        index += *path - '0';
      }

      auto val = t.TryGet(index);
      if (val == nullptr)
      {
        return false;
      }

      path++;
      return StormDataPath<T>::Process(*val, path);
    }
  };

  template <typename T>
  struct StormDataHasPath<RMergeList<T>, void> : StormDataHasPathList<RMergeList<T>, T>
  {
  };

  template <typename T>
  struct StormDataHasPath<RSparseList<T>, void> : StormDataHasPathList<RSparseList<T>, T>
  {
  };

  template <typename K, typename T>
  struct StormDataHasPath<RMap<K, T>, void> : StormDataHasPathList<RSparseList<T>, T>
  {
  };
}
