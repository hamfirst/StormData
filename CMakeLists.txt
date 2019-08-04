cmake_minimum_required(VERSION 3.1.0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}
                    ${PROJECT_SOURCE_DIR}/External
                    ${PROJECT_SOURCE_DIR}/StormTech
                    ${PROJECT_SOURCE_DIR}/Tools
                    ${PROJECT_SOURCE_DIR}/Shared
                    ${PROJECT_SOURCE_DIR}/Project
                    ${PROJECT_SOURCE_DIR}/Lobby)

set(CMAKE_CXX_STANDARD 17)

set(GENERIC_SRC_StormData 
            StormDataChangeNotifier.cpp
            StormDataChangePacket.cpp
            StormDataChangePacketHelpers.cpp
            StormDataJsonUtil.cpp
            StormDataPath.cpp
            )

set(GENERIC_HEADER_StormData 
            StormData.h
            StormDataChangeNotifier.h
            StormDataChangePacket.h
            StormDataChangePacketHelpers.h
            StormDataChangeType.refl.h
            StormDataChangeType.refl.meta.h
            StormDataDelta.h
            StormDataEnum.h
            StormDataJson.h
            StormDataJsonUtil.h
            StormDataList.h
            StormDataListUtil.h
            StormDataMap.h
            StormDataMergeList.h
            StormDataNumber.h
            StormDataOpaque.h
            StormDataOptional.h
            StormDataParent.h
            StormDataParentInfo.h
            StormDataPath.h
            StormDataPathHelpers.h
            StormDataPolymorphic.h
            StormDataSparseList.h
            StormDataString.h
            StormDataSync.h
            StormDataTypeDatabase.h
            StormDataTypeDatabaseRegister.h
            StormDataTypes.h
            StormDataUtil.h
            )

set(GENERIC_REFL_StormData 
            StormDataChangeType.refl.h
            )

if(GENERATE_REFL)
  foreach(REFL_FILE ${GENERIC_REFL_StormData})
    string(REPLACE ".refl.h" ".refl.meta.h" META_FILE ${REFL_FILE})
    add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${META_FILE}
                       COMMAND stormrefl ${CMAKE_CURRENT_SOURCE_DIR}/${REFL_FILE} -- -DSTORM_REFL_PARSE -D_CRT_SECURE_NO_WARNINGS -std=c++17 -x c++ -Wno-pragma-once-outside-header -I${CMAKE_CURRENT_SOURCE_DIR} -I${PROJECT_SOURCE_DIR} -I${PROJECT_SOURCE_DIR}/StormTech -I${PROJECT_SOURCE_DIR}/Tools -I${PROJECT_SOURCE_DIR}/Shared -I${PROJECT_SOURCE_DIR}/Project -I${PROJECT_SOURCE_DIR}/Lobby -I${PROJECT_SOURCE_DIR}/External -I${CLANG_HEADER_PATH} -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
                       MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/${REFL_FILE}
                       IMPLICIT_DEPENDS CXX ${CMAKE_CURRENT_SOURCE_DIR}/${REFL_FILE})
  endforeach()
endif()

add_library(StormData STATIC ${GENERIC_SRC_StormData} ${GENERIC_HEADER_StormData})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${GENERIC_SRC_StormData} ${GENERIC_HEADER_StormData})

