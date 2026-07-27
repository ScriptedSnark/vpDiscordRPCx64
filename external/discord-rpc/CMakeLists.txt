cmake_minimum_required(VERSION 3.15)

project(discord-rpc
	LANGUAGES CXX
	DESCRIPTION "Discord Rich Presence library (HLGSX fork)"
)

set(RAPIDJSON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../rapidjson)

if(NOT EXISTS ${RAPIDJSON_DIR}/include/rapidjson/document.h)
	message(FATAL_ERROR "rapidjson is missing. Run: git submodule update --init --recursive")
endif()

add_library(${PROJECT_NAME} STATIC
	src/discord_rpc.cpp
	src/rpc_connection.cpp
	src/serialization.cpp
	src/backoff.h
	src/connection.h
	src/msg_queue.h
	src/rpc_connection.h
	src/serialization.h
	include/discord_rpc.h
	include/discord_register.h
)

if(WIN32)
	target_sources(${PROJECT_NAME} PRIVATE
		src/connection_win.cpp
		src/discord_register_win.cpp
	)
	target_link_libraries(${PROJECT_NAME} PRIVATE psapi advapi32)
else()
	target_sources(${PROJECT_NAME} PRIVATE
		src/connection_unix.cpp
		src/discord_register_linux.cpp
	)

	find_package(Threads REQUIRED)
	target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
endif()

target_include_directories(${PROJECT_NAME}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${RAPIDJSON_DIR}/include
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)

set_target_properties(${PROJECT_NAME} PROPERTIES
	POSITION_INDEPENDENT_CODE ON
	CXX_VISIBILITY_PRESET hidden
	VISIBILITY_INLINES_HIDDEN ON
)
