
#include <StormRefl/StormReflJson.h>

#include "StormDataJsonUtil.h"


const char * StormDataFindJsonStartByPath(const char * path, const char * document)
{
  if (*path == '.')
  {
    path++;
    if (*path == '.' || *path == '[' || *path == ']' || *path == ' ' || *path == 0)
    {
      return nullptr;
    }

    StormReflJsonAdvanceWhiteSpace(document);
    if (*document != '{')
    {
      return nullptr;
    }

    document++;
    auto path_hash = crc32begin();

    while (true)
    {
      path_hash = crc32additive(path_hash, *path);
      path++;

      if (*path == '.' || *path == '[' || *path == ']' || *path == ' ' || *path == 0)
      {
        break;
      }
    }

    path_hash = crc32end(path_hash);

    while (true)
    {
      StormReflJsonAdvanceWhiteSpace(document);
      if (*document != '\"')
      {
        return nullptr;
      }

      document++;
      auto document_hash = crc32begin();
      while (*document != '\"')
      {
        if (*document == 0)
        {
          return nullptr;
        }

        document_hash = crc32additive(document_hash, *document);
        document++;
      }

      document++;
      document_hash = crc32end(document_hash);

      StormReflJsonAdvanceWhiteSpace(document);
      if (*document != ':')
      {
        return nullptr;
      }
      document++;

      if (document_hash == path_hash)
      {
        return StormDataFindJsonStartByPath(path, document);
      }

      StormReflJsonAdvanceWhiteSpace(document);
      if (StormReflJsonParseOverValue(document, document) == false)
      {
        return nullptr;
      }

      StormReflJsonAdvanceWhiteSpace(document);
      if (*document != ',')
      {
        return nullptr;
      }

      document++;
    }
  }
  else if (*path == '[')
  {
    path++;
    if (*path < '0' && *path > '9')
    {
      return nullptr;
    }

    int index = *path - '0';
    path++;

    while (*path >= '0' && *path <= '9')
    {
      index *= 10;
      index += *path - '0';
      path++;
    }

    if (*path != ']')
    {
      return nullptr;
    }

    path++;

    StormReflJsonAdvanceWhiteSpace(document);
    if (*document == '[')
    {
      while (index > 0)
      {
        document++;
        StormReflJsonAdvanceWhiteSpace(document);
        if (StormReflJsonParseOverValue(document, document) == false)
        {
          return nullptr;
        }

        StormReflJsonAdvanceWhiteSpace(document);
        if (*document != ',')
        {
          return nullptr;
        }

        index--;
      }

      document++;
      return StormDataFindJsonStartByPath(path, document);
    }
    else if (*document == '{')
    {
      document++;
      StormReflJsonAdvanceWhiteSpace(document);

      while (true)
      {
        if (*document != '\"')
        {
          return nullptr;
        }

        document++;
        if (*document < '0' && *document > '9')
        {
          return nullptr;
        }

        int document_index = *document - '0';
        document++;

        while (*document >= '0' && *document <= '9')
        {
          document_index *= 10;
          document_index += *document - '0';
          document++;
        }

        if (*document != '\"')
        {
          return nullptr;
        }

        document++;
        StormReflJsonAdvanceWhiteSpace(document);

        if (*document != ':')
        {
          return nullptr;
        }

        document++;
        StormReflJsonAdvanceWhiteSpace(document);
        if (document_index == index)
        {
          return StormDataFindJsonStartByPath(path, document);
        }
      }
    }
    else
    {
      return nullptr;
    }
  }
  else if (*path == 0 || *path == ' ')
  {
    return document;
  }
  else
  {
    return nullptr;
  }
}

