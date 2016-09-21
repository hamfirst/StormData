
#include "StormDataChangePacketHelpers.h"
#include "StormDataChangeNotifier.h"

namespace StormDataChangePacketHelpers
{
  bool ParseNotifyChangeType(ReflectionNotifyChangeType & type, const char * str, const char *& result)
  {
    if (*str == 0)
    {
      return false;
    }

    int index;

    char type_str[128];
    for (index = 0; index < sizeof(type_str) - 1; index++)
    {
      if (*str == 0)
      {
        return false;
      }

      if (*str == ' ')
      {
        break;
      }

      type_str[index] = *str;
      str++;
    }

    if (index >= sizeof(type_str) - 1)
    {
      return false;
    }

    type_str[index] = 0;
    auto val = ReflectionNotifyChangeType::_from_string_nothrow(type_str);

    if (val)
    {
      type = *val;
      str++;
      result = str;
      return true;
    }

    return false;
  }

  bool ParseIndex(uint32_t & val, const char * str, const char *& result)
  {
    if (*str < '0' || *str > '9')
    {
      return false;
    }

    uint32_t index = *str - '0';
    while (true)
    {
      str++;

      if (*str < '0' || *str > '9')
      {
        break;
      }

      index *= 10;
      index += *str - '0';
    }

    result = str;
    val = index;
    return true;
  }
}
