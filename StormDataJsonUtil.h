#pragma once

template<typename T, typename Enable = void> struct StormDataCheckReflectable {
  struct Fallback { bool m_ReflectionInfo; };
  struct Derived : T, Fallback { };

  template<typename C, C> struct ChT;

  template<typename C> static char(&f(ChT<bool Fallback::*, &C::m_ReflectionInfo>*))[1];
  template<typename C> static char(&f(...))[2];

  static const bool value = sizeof(f<Derived>(0)) == 2;
};


const char * StormDataFindJsonStartByPath(const char * path, const char * document);



