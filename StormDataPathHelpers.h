#pragma once

#include <hash\Hash.h>
#include <StormRefl\StormReflMetaFuncs.h>

template <typename T> class RSparseList;
template <typename T> class RMergeList;
template <typename K, typename T> class RMap;

enum ReflectionNotifyChangeType;

namespace StormDataChangePacketHelpers
{
  bool ParseNotifyChangeType(ReflectionNotifyChangeType & type, const char * str, const char *& result);
  bool ParseIndex(uint32_t & val, const char * str, const char *& result);
}

namespace StormDataPathHelpers
{
  template <typename T, typename Enable = void>
  struct StormDataPath
  {
    template <typename Callable>
    static bool VisitPath(T & t, Callable & callable, const char * str)
    {
      if (*str == ' ' || *str == 0)
      {
        str++;
        return callable(t, str);
      }

      return false;
    }
  };

  template <typename T, int i>
  struct StormDataPath<T[i], void>
  {
    template <typename Callable>
    static bool VisitPath(T(&t)[i], Callable & callable, const char * str)
    {
      if (*str == ' ' || *str == 0)
      {
        str++;
        return callable(t, str);
      }

      if (*str != '[')
      {
        return false;
      }

      str++;
      uint32_t index;
      if (StormDataChangePacketHelpers::ParseIndex(index, str, str) == false)
      {
        return false;
      }

      if (*str != ']')
      {
        return false;
      }

      str++;
      return StormDataPath<T>::VisitPath(t[index], callable, str);
    }
  };


  template <typename T>
  struct StormDataPath<T, typename std::enable_if_t<StormReflCheckReflectable<T>::value>>
  {
    template <typename Callable>
    static bool VisitPath(T & t, Callable & callable, const char * str)
    {
      if (*str == ' ' || *str == 0)
      {
        str++;
        return callable(t, str);
      }

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
        using member_type = typename decltype(f)::member_type;
        result = StormDataPath<member_type>::VisitPath(f.Get(), callable, str);
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
      if (*str == ' ' || *str == 0)
      {
        str++;
        return callable(t, str);
      }

      if (*str != '[')
      {
        return false;
      }

      str++;
      uint32_t index;
      if (StormDataChangePacketHelpers::ParseIndex(index, str, str) == false)
      {
        return false;
      }

      if (*str != ']')
      {
        return false;
      }

      str++;
      return StormDataPath<T>::VisitPath(t[index], callable, str);
    }
  };

  template <typename T>
  struct StormDataPath<RMergeList<T>, void>
  {
    template <typename Callable>
    static bool VisitPath(RMergeList<T> & t, Callable & callable, const char * str)
    {
      if (*str == ' ' || *str == 0)
      {
        str++;
        return callable(t, str);
      }

      if (*str != '[')
      {
        return false;
      }

      str++;
      uint32_t index;
      if (StormDataChangePacketHelpers::ParseIndex(index, str, str) == false)
      {
        return false;
      }

      if (*str != ']')
      {
        return false;
      }

      str++;
      return StormDataPath<T>::VisitPath(t[index], callable, str);
    }
  };

  template <typename K, typename T>
  struct StormDataPath<RMap<K, T>, void>
  {
    template <typename Callable>
    static bool VisitPath(RMap<K, T> & t, Callable & callable, const char * str)
    {
      if (*str == ' ' || *str == 0)
      {
        str++;
        return callable(t, str);
      }

      if (*str != '[')
      {
        return false;
      }

      str++;
      uint32_t index;
      if (StormDataChangePacketHelpers::ParseIndex(index, str, str) == false)
      {
        return false;
      }

      if (*str != ']')
      {
        return false;
      }

      str++;
      return StormDataPath<T>::VisitPath(t[index], callable, str);
    }
  };
}
