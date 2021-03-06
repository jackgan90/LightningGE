project(AssetPipeline)
message(STATUS "Configuring project AssetPipeline")

set(CONVERTER_HEADERS	Converters/AssimpConverter.h)
set(CONVERTER_SOURCES	Converters/AssimpConverter.cpp)

set(HEADERS )
set(SOURCES AssetPipeline.cpp)

list(APPEND HEADERS ${CONVERTER_HEADERS})
list(APPEND SOURCES ${CONVERTER_SOURCES})

source_group("Converters" FILES ${CONVERTER_HEADERS} ${CONVERTER_SOURCES})

set(ASSIMP_ROOT_DIR ${LIGHTNING_DEPENDENCIES_DIR}/assimp)
find_package(assimp REQUIRED)

include_directories(${ASSIMP_ROOT_DIR}/include
					${CMAKE_SOURCE_DIR}/World
					${CMAKE_SOURCE_DIR}/Render
					${CMAKE_SOURCE_DIR}/Foundation
					${CMAKE_SOURCE_DIR}/Foundation/Math
					${TBB_INCLUDE_DIR})


if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /wd4250 /wd4251 /wd4275 /EHsc /MDd /MP")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_DEBUG} /wd4250 /wd4251 /wd4275 /EHsc /MD /MP")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4250 /wd4251 /wd4275 /EHsc /MP")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4250 /wd4251 /wd4275 /EHsc /MP")
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(${PROJECT_NAME} WIN32 ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${ASSIMP_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${TBB_LIBRARIES})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ASSIMP_DLL_DEBUG} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ASSIMP_DLL_RELEASE} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)

if(MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
endif()
