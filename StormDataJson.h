#pragma once

#include <StormRefl/StormReflJson.h>
#include <StormRefl/StormReflJsonStd.h>

#include "StormDataNumber.h"
#include "StormDataString.h"
#include "StormDataEnum.h"
#include "StormDataList.h"
#include "StormDataMap.h"

template <>
struct StormReflJson<RBool, void>
{
  template <class StringBuilder>
  static void Encode(const RBool & t, StringBuilder & sb)
  {
    sb += t ? "true" : "false";
  }

  template <class StringBuilder>
  static void EncodePretty(const RBool & t, StringBuilder & sb, int indent)
  {
    Encode(t, sb);
  }

  static bool Parse(RBool & t, const char * str, const char *& result)
  {
    if (StormReflJsonMatchStr(str, result, "true"))
    {
      t = true;
      return true;
    }
    else if (StormReflJsonMatchStr(str, result, "false"))
    {
      t = false;
      return true;
    }
    else
    {
      return false;
    }
  }
};

template <typename T>
struct StormReflJson<RNumber<T>, void>
{
  template <class StringBuilder>
  static void Encode(const RNumber<T> & t, StringBuilder & sb)
  {
    StormReflJson<T>::Encode(t, sb);
  }

  template <class StringBuilder>
  static void EncodePretty(const RNumber<T> & t, StringBuilder & sb, int indent)
  {
    StormReflJson<T>::Encode(t, sb);
  }

  static bool Parse(RNumber<T> & t, const char * str, const char *& result)
  {
    T val;
    if (!StormReflJson<T>::Parse(val, str, result))
    {
      return false;
    }

    t = val;
    return true;
  }
};

template <>
struct StormReflJson<RString, void>
{
  template <class StringBuilder>
  static void Encode(const RString & t, StringBuilder & sb)
  {
    StormReflJson<std::string>::Encode(t, sb);
  }

  template <class StringBuilder>
  static void EncodePretty(const RString & t, StringBuilder & sb, int indent)
  {
    StormReflJson<std::string>::EncodePretty(t, sb, indent);
  }

  static bool Parse(RString & t, const char * str, const char *& result)
  {
    std::string val;
    if (!StormReflJson<std::string>::Parse(val, str, result))
    {
      return false;
    }

    t = val;
    return true;
  }
};

template <typename T>
struct StormReflJson<REnum<T>, void>
{
  template <class StringBuilder>
  static void Encode(const REnum<T> & t, StringBuilder & sb)
  {
    StormReflJson<T>::Encode(t, sb);
  }

  template <class StringBuilder>
  static void EncodePretty(const REnum<T> & t, StringBuilder & sb, int indent)
  {
    StormReflJson<T>::EncodePretty(t, sb, indent);
  }

  static bool Parse(REnum<T> & t, const char * str, const char *& result)
  {
    T val;
    if (StormReflJson<T>::Parse(val, str, result))
    {
      t = val;
      return true;
    }

    return false;
  }
};

template <typename T>
struct StormReflJson<RSparseList<T>, void>
{
  template <class StringBuilder>
  static void Encode(const RSparseList<T> & t, StringBuilder & sb)
  {
    auto itr = t.begin();
    if (itr == t.end())
    {
      sb += "{}";
      return;
    }

    auto & elem = *itr;
    sb += "{\"";
    sb += std::to_string(elem.first);
    sb += "\":";
    StormReflJson<T>::Encode(elem.second, sb);
    ++itr;

    while(itr != t.end())
    {
      auto & elem = *itr;
      sb += ",\"";
      sb += std::to_string(elem.first);
      sb += "\":";
      StormReflJson<T>::Encode(elem.second, sb);
      ++itr;
    }

    sb += '}';
  }

  template <class StringBuilder>
  static void EncodePretty(const RSparseList<T> & t, StringBuilder & sb, int indent)
  {
    auto itr = t.begin();
    if (itr == t.end())
    {
      sb += "{}";
      return;
    }

    auto & elem = *itr;
    sb += "{\n";
    StormReflJsonHelpers::StormReflEncodeIndent(indent + 1, sb);

    sb += '\"';
    sb += std::to_string(elem.first);
    sb += "\": ";
    StormReflJson<T>::EncodePretty(elem.second, sb, indent + 1);
    ++itr;

    while (itr != t.end())
    {
      auto & elem = *itr;
      sb += ",\n";
      StormReflJsonHelpers::StormReflEncodeIndent(indent + 1, sb);
      sb += '\"';
      sb += std::to_string(elem.first);
      sb += "\": ";
      StormReflJson<T>::EncodePretty(elem.second, sb, indent + 1);
      ++itr;
    }

    sb += '\n';
    StormReflJsonHelpers::StormReflEncodeIndent(indent, sb);
    sb += "}";
  }

  static bool Parse(RSparseList<T> & t, const char * str, const char *& result)
  {
    if (*str != '{')
    {
      return false;
    }

    str++;
    while (true)
    {
      if (*str == '}')
      {
        str++;
        result = str;
        return true;
      }

      std::size_t index;

      if (*str != '\"')
      {
        return false;
      }

      str++;
      if (StormReflJson<std::size_t>::Parse(index, str, str) == false)
      {
        return false;
      }

      if (*str != '\"')
      {
        return false;
      }
      str++;

      StormReflJsonAdvanceWhiteSpace(str);
      if (*str != ':')
      {
        return false;
      }

      str++;
      StormReflJsonAdvanceWhiteSpace(str);

      t.InsertAt(T{}, index);

      if (StormReflJson<T>::Parse(t[index], str, str) == false)
      {
        if (StormReflJsonParseOverValue(str, str) == false)
        {
          return false;
        }
      }

      StormReflJsonAdvanceWhiteSpace(str);
      if (*str != '}')
      {
        if (*str != ',')
        {
          return false;
        }
        else
        {
          str++;
        }
      }
    }
  }
};

template <typename T>
struct StormReflJson<RMergeList<T>, void>
{
  template <class StringBuilder>
  static void Encode(const RMergeList<T> & t, StringBuilder & sb)
  {
    auto itr = t.begin();
    if (itr == t.end())
    {
      sb += "{}";
      return;
    }

    auto elem = *itr;
    sb += "{\"";
    sb += std::to_string(elem.first);
    sb += "\":";
    StormReflJson<T>::Encode(elem.second, sb);
    ++itr;

    while (itr != t.end())
    {
      auto elem = *itr;
      sb += ",\"";
      sb += std::to_string(elem.first);
      sb += "\":";
      StormReflJson<T>::Encode(elem.second, sb);
      ++itr;
    }

    sb += '}';
  }

  template <class StringBuilder>
  static void EncodePretty(const RMergeList<T> & t, StringBuilder & sb, int indent)
  {
    auto itr = t.begin();
    if (itr == t.end())
    {
      sb += "{}";
      return;
    }

    auto elem = *itr;
    sb += "{\n";
    StormReflJsonHelpers::StormReflEncodeIndent(indent + 1, sb);

    sb += '\"';
    sb += std::to_string(elem.first);
    sb += "\": ";
    StormReflJson<T>::EncodePretty(elem.second, sb, indent + 1);
    ++itr;

    while (itr != t.end())
    {
      auto elem = *itr;
      sb += ",\n";
      StormReflJsonHelpers::StormReflEncodeIndent(indent + 1, sb);
      sb += '\"';
      sb += std::to_string(elem.first);
      sb += "\": ";
      StormReflJson<T>::EncodePretty(elem.second, sb, indent + 1);
      ++itr;
    }

    sb += '\n';
    StormReflJsonHelpers::StormReflEncodeIndent(indent, sb);
    sb += "}";
  }

  static bool Parse(RMergeList<T> & t, const char * str, const char *& result)
  {
    if (*str != '{')
    {
      return false;
    }

    str++;
    while (true)
    {
      if (*str == '}')
      {
        str++;
        result = str;
        return true;
      }

      std::size_t index;

      if (*str != '\"')
      {
        return false;
      }

      str++;
      if (StormReflJson<std::size_t>::Parse(index, str, str) == false)
      {
        return false;
      }

      if (*str != '\"')
      {
        return false;
      }
      str++;

      StormReflJsonAdvanceWhiteSpace(str);
      if (*str != ':')
      {
        return false;
      }

      str++;
      StormReflJsonAdvanceWhiteSpace(str);

      auto & val = t.InsertAt(T{}, index);
      if (StormReflJson<T>::Parse(val, str, str) == false)
      {
        if (StormReflJsonParseOverValue(str, str) == false)
        {
          return false;
        }
      }

      StormReflJsonAdvanceWhiteSpace(str);
      if (*str != '}')
      {
        if (*str != ',')
        {
          return false;
        }
        else
        {
          str++;
        }
      }
    }
  }
};

template <typename K, typename T>
struct StormReflJson<RMap<K, T>, void>
{
  template <class StringBuilder>
  static void Encode(const RMap<K, T> & t, StringBuilder & sb)
  {
    auto itr = t.begin();
    if (itr == t.end())
    {
      sb += "{}";
      return;
    }

    auto & elem = *itr;
    sb += "{\"";
    sb += std::to_string(elem.first);
    sb += "\":";
    StormReflJson<T>::Encode(elem.second, sb);
    ++itr;

    while (itr != t.end())
    {
      auto & elem = *itr;
      sb += ",\"";
      sb += std::to_string(elem.first);
      sb += "\":";
      StormReflJson<T>::Encode(elem.second, sb);
      ++itr;
    }

    sb += '}';
  }

  template <class StringBuilder>
  static void EncodePretty(const RMap<K, T> & t, StringBuilder & sb, int indent)
  {
    auto itr = t.begin();
    if (itr == t.end())
    {
      sb += "{}";
      return;
    }

    auto & elem = *itr;
    sb += "{\n";
    StormReflJsonHelpers::StormReflEncodeIndent(indent + 1, sb);

    sb += '\"';
    sb += std::to_string(elem.first);
    sb += "\": ";
    StormReflJson<T>::EncodePretty(elem.second, sb, indent + 1);
    ++itr;

    while (itr != t.end())
    {
      auto & elem = *itr;
      sb += ",\n";
      StormReflJsonHelpers::StormReflEncodeIndent(indent + 1, sb);
      sb += '\"';
      sb += std::to_string(elem.first);
      sb += "\": ";
      StormReflJson<T>::EncodePretty(elem.second, sb, indent + 1);
      ++itr;
    }

    sb += '\n';
    StormReflJsonHelpers::StormReflEncodeIndent(indent, sb);
    sb += "}";
  }

  static bool Parse(RMap<K, T> & t, const char * str, const char *& result)
  {
    if (*str != '{')
    {
      return false;
    }

    str++;
    while (true)
    {
      if (*str == '}')
      {
        str++;
        result = str;
        return true;
      }

      std::size_t index;

      if (*str != '\"')
      {
        return false;
      }

      str++;
      if (StormReflJson<std::size_t>::Parse(index, str, str) == false)
      {
        return false;
      }

      if (*str != '\"')
      {
        return false;
      }
      str++;

      StormReflJsonAdvanceWhiteSpace(str);
      if (*str != ':')
      {
        return false;
      }

      str++;
      StormReflJsonAdvanceWhiteSpace(str);

      auto & val = t.Set(index, T{});
      if (StormReflJson<T>::Parse(val, str, str) == false)
      {
        if (StormReflJsonParseOverValue(str, str) == false)
        {
          return false;
        }
      }

      StormReflJsonAdvanceWhiteSpace(str);
      if (*str != '}')
      {
        if (*str != ',')
        {
          return false;
        }
        else
        {
          str++;
        }
      }
    }
  }
};
