cmake_minimum_required(VERSION 3.1.0)

include_directories(. ../. .././External)
set(CMAKE_CXX_STANDARD 14)

set(GENERIC_SRC_StormData 
            ./StormDataPath.cpp
            ./StormDataChangePacket.cpp
            ./StormDataJsonUtil.cpp
            ./StormDataChangeNotifier.cpp
            ./StormDataChangePacketHelpers.cpp
            #CPP_PLACEHOLDER
            )
set(GENERIC_HEADER_StormData 
            ./StormDataUtil.h
            ./StormDataChangeType.refl.meta.h
            ./StormDataTypeDatabase.h
            ./StormDataChangeType.refl.h
            ./StormDataMap.h
            ./StormDataTypeDatabaseRegister.h
            ./StormDataNumber.h
            ./StormDataChangePacketHelpers.h
            ./StormDataSync.h
            ./StormDataEnum.h
            ./StormDataPathHelpers.h
            ./StormDataChangeNotifier.h
            ./StormDataParentInfo.h
            ./StormDataParent.h
            ./StormDataPolymorphic.h
            ./StormDataMergeList.h
            ./StormDataOpaque.h
            ./StormDataJsonUtil.h
            ./StormDataPath.h
            ./StormDataTypes.h
            ./StormDataListUtil.h
            ./StormData.h
            ./StormDataSparseList.h
            ./StormDataJson.h
            ./StormDataList.h
            ./StormDataChangePacket.h
            ./StormDataDelta.h
            ./StormDataString.h
            #HEADER_PLACEHOLDER
            )
set(GENERIC_REFL_StormData 
            ./StormDataChangeType.refl.h
            #REFL_PLACEHOLDER
            )

if (MSVC)
  add_definitions(-D_WINDOWS)
  include_directories(.././External/Windows)
  set(PLATFORM_SRC_StormData 
            )

set(PLATFORM_HEADER_StormData 
            )
endif()
if (WEB)
  add_definitions(-D_WEB)
  include_directories(.././External/Web)
  set(PLATFORM_SRC_StormData 
            )

set(PLATFORM_HEADER_StormData 
            )
endif()
if (IOS)
  add_definitions(-D_IOS)
  include_directories(.././External/IOS)
  set(PLATFORM_SRC_StormData 
            )

set(PLATFORM_HEADER_StormData 
            )
endif()
if (APPLE AND NOT IOS)
  add_definitions(-D_MACOS)
  include_directories(.././External/MacOS)
  include_directories(/usr/include/freetype2)
  set(PLATFORM_SRC_StormData 
            )

set(PLATFORM_HEADER_StormData 
            )
endif()
if (ANDROID)
  add_definitions(-D_ANDROID)
  include_directories(.././External/Android)
  set(PLATFORM_SRC_StormData 
            )

set(PLATFORM_HEADER_StormData 
            )
endif()
if (UNIX AND NOT APPLE)
  add_definitions(-D_LINUX)
  include_directories(.././External/Linux)
  include_directories(/usr/include/freetype2)
  set(PLATFORM_SRC_StormData 
            )

set(PLATFORM_HEADER_StormData 
            )
endif()

foreach(REFL_FILE ${GENERIC_REFL_StormData})
  string(REPLACE ".refl.h" ".refl.meta.h" META_FILE ${REFL_FILE})
  add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${META_FILE}
                     COMMAND stormrefl ${CMAKE_CURRENT_SOURCE_DIR}/${REFL_FILE} -- -DSTORM_REFL_PARSE -D_CRT_SECURE_NO_WARNINGS -x c++ -Wno-pragma-once-outside-header -I${CMAKE_CURRENT_SOURCE_DIR} -I${CMAKE_CURRENT_SOURCE_DIR}/../. -I${CMAKE_CURRENT_SOURCE_DIR}/.././External -I${CLANG_HEADER_PATH}
                     MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/${REFL_FILE}
                     IMPLICIT_DEPENDS CXX ${CMAKE_CURRENT_SOURCE_DIR}/${REFL_FILE})
endforeach()

add_library(StormData ${GENERIC_SRC_StormData} ${PLATFORM_SRC_StormData}
            ${GENERIC_HEADER_StormData} ${PLATFORM_HEADER_StormData})