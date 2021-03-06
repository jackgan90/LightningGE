project(UnitTest)
message(STATUS "Configuring project UnitTest")

set(HEADERS catch.hpp)
set(SOURCES Main.cpp
			MemoryTest.cpp
			MathTest.cpp
			HelperStubTest.cpp
			ECSTest.cpp)

if (WIN32)
	add_definitions(-DLIGHTNING_WIN32)
endif()
if(LIGHTNING_USE_D3D12)
	add_definitions(-DLIGHTNING_USE_D3D12)
endif()

add_definitions(-DBOOST_FILESYSTEM_NO_DEPRECATED)

include_directories( ${CMAKE_SOURCE_DIR}/Foundation
					${CMAKE_SOURCE_DIR}/Render/Types
					${CMAKE_SOURCE_DIR}/Foundation/Memory
					${CMAKE_SOURCE_DIR}/PluginSystem
					${LIGHTNING_DEPENDENCIES_DIR}/eigen
					${Boost_INCLUDE_DIR}
					${TBB_INCLUDE_DIR}
					${RTTR_INCLUDE_DIR})
if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /wd4250 /wd4251 /wd4275 /EHsc /MDd /MP")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_DEBUG} /wd4250 /wd4251 /wd4275 /EHsc /MD /MP")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4250 /wd4251 /wd4275 /EHsc /MP")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /wd4250 /wd4251 /wd4275 /EHsc /MP")
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(${PROJECT_NAME} WIN32 ${HEADERS} ${SOURCES})
add_dependencies(${PROJECT_NAME} Foundation)
target_link_libraries(${PROJECT_NAME} ${TBB_LIBRARIES})
if(MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
endif()
