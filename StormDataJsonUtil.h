#pragma once

template<typename T, typename Enable = void> struct StormDataCheckReflectable {
  struct Fallback { bool m_ReflectionInfo; };
  struct Derived : T, Fallback { };

  template<typename C, C> struct ChT;

  template<typename C> static char(&f(ChT<bool Fallback::*, &C::m_ReflectionInfo>*))[1];
  template<typename C> static char(&f(...))[2];

  static const bool value = sizeof(f<Derived>(0)) == 2;
};

template <typename T> struct StormDataCheckReflectable<T, typename std::enable_if<std::is_enum<T>::value>::type> { static const bool value = false; };
template <typename T, int i> struct StormDataCheckReflectable<T[i], void> { static const bool value = false; };
template <typename T> struct StormDataCheckReflectable<T *, void> { static const bool value = false; };
template <> struct StormDataCheckReflectable<bool, void> { static const bool value = false; };
template <> struct StormDataCheckReflectable<int8_t, void> { static const bool value = false; };
template <> struct StormDataCheckReflectable<int16_t, void> { static const bool value = false; };
template <> struct StormDataCheckReflectable<int32_t, void> { static const bool value = false; };
template <> struct StormDataCheckReflectable<long, void> { static const bool value = false; };
template <> struct StormDataCheckReflectable<long long, void> { static const bool value = false; };
template <> struct StormDataCheckReflectable<uint8_t, void> { static const bool value = false; };
template <> struct StormDataCheckReflectable<uint16_t, void> { static const bool value = false; };
template <> struct StormDataCheckReflectable<uint32_t, void> { static const bool value = false; };
template <> struct StormDataCheckReflectable<unsigned long, void> { static const bool value = false; };
template <> struct StormDataCheckReflectable<unsigned long long, void> { static const bool value = false; };
template <> struct StormDataCheckReflectable<float, void> { static const bool value = false; };
template <> struct StormDataCheckReflectable<double, void> { static const bool value = false; };


const char * StormDataFindJsonStartByPath(const char * path, const char * document);



