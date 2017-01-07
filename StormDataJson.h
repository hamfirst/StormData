#pragma once

#include <StormRefl/StormReflJson.h>
#include <StormRefl/StormReflJsonStd.h>

#include "StormDataNumber.h"
#include "StormDataString.h"
#include "StormDataEnum.h"
#include "StormDataList.h"
#include "StormDataMap.h"
#include "StormDataJsonUtil.h"


template <typename T, bool IsDataRefl>
struct StormDataJsonParseRawDataRefl
{
  static bool ParseRaw(T & t, const char * str, const char *& result)
  {
    return StormReflJson<T>::Parse(t, str, result);
  }
};

template <typename T>
struct StormDataJsonParseRawDataRefl<T, true>
{
  static bool ParseRaw(T & t, const char * str, const char *& result)
  {
    StormReflJsonAdvanceWhiteSpace(str);
    if (*str != '{')
    {
      return false;
    }

    str++;
    while (true)
    {
      StormReflJsonAdvanceWhiteSpace(str);

      if (*str == '}')
      {
        str++;
        result = str;
        return true;
      }

      Hash field_name_hash;
      if (StormReflJsonParseStringHash(field_name_hash, str, str) == false)
      {
        return false;
      }

      StormReflJsonAdvanceWhiteSpace(str);
      if (*str != ':')
      {
        return false;
      }

      str++;

      StormReflJsonAdvanceWhiteSpace(str);
      bool parsed_field = false;
      const char * result_str = str;

      auto field_visitor = [&](auto f)
      {
        using member_type = typename decltype(f)::member_type;
        member_type & member = f.Get();
        parsed_field = StormDataJson<member_type>::ParseRaw(member, str, result_str);
      };

      StormReflVisitField(t, field_visitor, field_name_hash);
      if (parsed_field)
      {
        str = result_str;
      }
      else
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

template <typename T, bool IsClass>
struct StormDataJsonParseRawClass
{
  static bool ParseRaw(T & t, const char * str, const char *& result)
  {
    return StormReflJson<T>::Parse(t, str, result);
  }
};

template <typename T>
struct StormDataJsonParseRawClass<T, true>
{
  static bool ParseRaw(T & t, const char * str, const char *& result)
  {
    return StormDataJsonParseRawDataRefl<T, StormDataCheckReflectable<T>::value>::ParseRaw(t, str, result);
  }
};

template <typename T, typename Enable = void>
struct StormDataJson
{
  static bool ParseRaw(T & t, const char * str, const char *& result)
  {
    return StormDataJsonParseRawClass<T, std::is_class<T>::value>::ParseRaw(t, str, result);
  }
};


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

template <>
struct StormDataJson<RBool, void>
{
  static bool ParseRaw(RBool & t, const char * str, const char *& result)
  {
    if (StormReflJsonMatchStr(str, result, "true"))
    {
      t.SetRaw(true);
      return true;
    }
    else if (StormReflJsonMatchStr(str, result, "false"))
    {
      t.SetRaw(false);
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

template <typename T>
struct StormDataJson<RNumber<T>, void>
{
  static bool ParseRaw(RNumber<T> & t, const char * str, const char *& result)
  {
    T val;
    if (!StormReflJson<T>::Parse(val, str, result))
    {
      return false;
    }

    t.SetRaw(val);
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

template <>
struct StormDataJson<RString, void>
{
  static bool ParseRaw(RString & t, const char * str, const char *& result)
  {
    std::string val;
    if (!StormReflJson<std::string>::Parse(val, str, result))
    {
      return false;
    }

    t.SetRaw(val);
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
struct StormDataJson<REnum<T>, void>
{
  static bool ParseRaw(REnum<T> & t, const char * str, const char *& result)
  {
    T val;
    if (StormReflJson<T>::Parse(val, str, result))
    {
      t.SetRaw(val);
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

  template <typename ItemParser>
  static bool ParseList(RSparseList<T> & t, const char * str, const char *& result, ItemParser && parser)
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

      if (parser(str, index) == false)
      {
        return false;
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

  static bool Parse(RSparseList<T> & t, const char * str, const char *& result)
  {
    auto default_parse = [&](const char *& str, std::size_t index)
    {
      t.EmplaceAt(index);

      if (StormReflJson<T>::Parse(t[index], str, str) == false)
      {
        if (StormReflJsonParseOverValue(str, str) == false)
        {
          return false;
        }
      }

      return true;
    };

    return ParseList(t, str, result, default_parse);
  }
};

template <typename T>
struct StormDataJson<RSparseList<T>, void>
{
  static bool ParseRaw(RSparseList<T> & t, const char * str, const char *& result)
  {
    auto raw_parse = [&](const char *& str, std::size_t index)
    {
      T val;

      if (StormReflJson<T>::Parse(val, str, str) == false)
      {
        if (StormReflJsonParseOverValue(str, str) == false)
        {
          return false;
        }
      }

      t.EmplaceAt(index, std::move(val));
      return true;
    };

    return StormReflJson<RSparseList<T>>::ParseList(t, str, result, raw_parse);
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

  template <typename ItemParser>
  static bool ParseList(RMergeList<T> & t, const char * str, const char *& result, ItemParser && item_parser)
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

      if (item_parser(str, index) == false)
      {
        return false;
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

  static bool Parse(RMergeList<T> & t, const char * str, const char *& result)
  {
    auto default_parse = [&](const char *& str, std::size_t index)
    {
      auto & val = t.EmplaceAt(index);
      if (StormReflJson<T>::Parse(val, str, str) == false)
      {
        if (StormReflJsonParseOverValue(str, str) == false)
        {
          return false;
        }
      }

      return true;
    };

    return ParseList(t, str, result, default_parse);
  }
};


template <typename T>
struct StormDataJson<RMergeList<T>, void>
{
  static bool ParseRaw(RMergeList<T> & t, const char * str, const char *& result)
  {
    auto raw_parse = [&](const char *& str, std::size_t index)
    {
      T val;

      if (StormReflJson<T>::Parse(val, str, str) == false)
      {
        if (StormReflJsonParseOverValue(str, str) == false)
        {
          return false;
        }
      }

      t.EmplaceAt(index, std::move(val));
      return true;
    };

    return StormReflJson<RMergeList<T>>::ParseList(t, str, result, raw_parse);
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

  template <typename ItemParser>
  static bool ParseList(RMap<K, T> & t, const char * str, const char *& result, ItemParser && item_parser)
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

      if (item_parser(str, index) == false)
      {
        return false;
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

  static bool Parse(RMap<K, T> & t, const char * str, const char *& result)
  {
    auto default_parse = [&](const char *& str, K index)
    {
      auto & val = t.Set(index, T{});
      if (StormReflJson<T>::Parse(val, str, str) == false)
      {
        if (StormReflJsonParseOverValue(str, str) == false)
        {
          return false;
        }
      }

      return true;
    };

    return ParseList(t, str, result, default_parse);
  }
};

template <typename K, typename T>
struct StormDataJson<RMap<K, T>, void>
{
  static bool ParseRaw(RMap<K, T> & t, const char * str, const char *& result)
  {
    auto raw_parse = [&](const char *& str, K index)
    {
      T val;
      if (StormReflJson<T>::Parse(val, str, str) == false)
      {
        if (StormReflJsonParseOverValue(str, str) == false)
        {
          return false;
        }
      }

      t.Set(index, std::move(val));
      return true;
    };

    return StormReflJson<RMap<K, T>>::ParseList(t, str, result, raw_parse);
  }
};
