cmake_minimum_required(VERSION 2.6)

project(BingoCoreC)

file(GLOB BingoCoreC_src src/*.cpp)
file(GLOB BingoCoreC_headers src/*.h)

set(BingoCoreC_HEADERS_DIR ${BingoCoreC_SOURCE_DIR}/src CACHE INTERNAL "include directories" FORCE)

include(ConfigureCommon)

include_directories(../src ../src/core ${Common_SOURCE_DIR} ${Common_SOURCE_DIR}/.. ${ZLib_HEADERS_DIR} ${BingoCore_HEADERS_DIR}
	${Common_SOURCE_DIR}/../api/plugins/inchi/src ${InChI_SOURCE_DIR}/inchi_dll)

add_library(bingo-core-c STATIC ${BingoCoreC_src} ${BingoCoreC_headers} ${Common_SOURCE_DIR}/../api/plugins/inchi/src/indigo_inchi_core.cpp)
add_library(bingo-core-c-shared SHARED ${BingoCoreC_src} ${BingoCoreC_headers} ${Common_SOURCE_DIR}/../api/plugins/inchi/src/indigo_inchi_core.cpp ${Common_SOURCE_DIR}/hacks/memcpy.c)
message(STATUS ${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS})
target_link_libraries(bingo-core-c bingo-core inchi)
target_link_libraries(bingo-core-c-shared molecule reaction layout bingo-core inchi)

if (UNIX AND NOT APPLE)
	message(STATUS "UNIX AND NOT APPLE")
	if(${SUBSYSTEM_NAME} MATCHES "x64")
		message(STATUS "SUBSYSTEM_NAME EQUALS x64")
		set_target_properties(bingo-core-c-shared PROPERTIES LINK_FLAGS "${LINK_FLAGS} -Wl,--wrap=memcpy")
	endif()
endif()

set_target_properties(bingo-core-c PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS}")
set_property(TARGET bingo-core-c PROPERTY FOLDER "bingo-core-c")

set_target_properties(bingo-core-c-shared PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS}")

set_property(TARGET bingo-core-c PROPERTY OUTPUT_NAME "bingo-core-c-static")
set_property(TARGET bingo-core-c-shared PROPERTY FOLDER "bingo-core-c-shared")
set_property(TARGET bingo-core-c-shared PROPERTY OUTPUT_NAME "bingo-core-c")
