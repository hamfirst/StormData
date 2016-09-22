
#include <StormRefl\StormReflJson.h>

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

      if (StormReflJsonParseOverValue(document, document) == false)
      {
        return nullptr;
      }
    }
  }
  else if (*path == '[')
  {

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

