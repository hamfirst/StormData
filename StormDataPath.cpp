
#include "StormData.h"
#include "StormDataPath.h"
#include "StormDataParent.h"

#include <cstring>

bool StormDataMatchPathExact(const char * path, const char * test)
{
  while (*test != 0)
  {
    if (*test != *path)
    {
      return false;
    }

    test++;
    path++;
  }

  return *path == 0;
}

bool StormDataMatchPathPartial(const char * path, const char * partial)
{
  while (*partial != 0)
  {
    if (*partial != *path)
    {
      return false;
    }

    partial++;
    path++;
  }

  return true;
}

bool StormDataMatchPathWildcard(const char * path, const char * test)
{
  while (*test != 0)
  {
    if (*path == 0)
    {
      return true;
    }

    if (*test == '*')
    {
      while (*path != 0 && *path != '.' && *path != '[' && *path != ']')
      {
        path++;
      }

      test++;
    }
    else
    {
      if (*test != *path)
      {
        return false;
      }

      test++;
      path++;
    }
  }

  return true;
}

bool StormDataParsePathComponents(const char * path, std::vector<StormDataPathComponent> & comps)
{
  while (true)
  {
    if (*path == 0)
    {
      return true;
    }
    else if (*path == '.')
    {
      path++;
      if (*path == 0 || *path == '.' || *path == '[' || *path == ']')
      {
        return false;
      }

      std::string val;
      val += *path;
     
      path++;
      while (true)
      {
        if (*path == 0 || *path == '.' || *path == '[' || *path == ']')
        {
          break;
        }

        val += *path;
        path++;
      }

      comps.emplace_back(StormDataPathComponent{ StormDataPathComponentType::kFieldName, std::move(val) });
    }
    else if (*path == '[')
    {
      path++;
      if (*path < '0' || *path > '9')
      {
        return false;
      }

      std::string val;
      val += *path;

      path++;
      while (true)
      {
        if (*path < '0' || *path > '9')
        {
          return false;
        }

        val += *path;
        path++;
      }

      comps.emplace_back(StormDataPathComponent{ StormDataPathComponentType::kIndex, std::move(val) });
    }
    else
    {
      return false;
    }
  }
}

std::string StormDataConvertPathComponentList(const std::vector<StormDataPathComponent> & comps)
{
  std::string val;
  for (auto & comp : comps)
  {
    if (comp.m_Type == StormDataPathComponentType::kFieldName)
    {
      val += '.';
      val += comp.m_Value;
    }
    else if (comp.m_Type == StormDataPathComponentType::kIndex)
    {
      val += '[';
      val += comp.m_Value;
      val += ']';
    }
  }

  return val;
}

