cmake_minimum_required(VERSION 3.20) # JackSDK

project(vpDiscordRPC
	VERSION 1.0.0
	LANGUAGES C CXX
	DESCRIPTION "Discord Rich Presence plugin for J.A.C.K"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/external/JackSDK)

if(NOT EXISTS ${SRCDIR}/sdk/JackPluginSDK.cmake)
	message(FATAL_ERROR "JackSDK is missing. Did you forget submodules? Run: git submodule update --init --recursive")
endif()

include(${SRCDIR}/cmake/srcgrp.cmake)
include(${SRCDIR}/sdk/JackPluginSDK.cmake)

add_subdirectory(external/discord-rpc)

# Sources
set(VPDISCORD_SOURCES
	src/vp_main.cpp
	src/discord_integration.cpp
	src/settings.cpp
	src/log.cpp
	src/ini.c
	src/includes.h
	src/log.h
	src/discord_integration.h
	src/settings.h
	src/ini.h
)

if(NOT WIN32)
	list(APPEND VPDISCORD_SOURCES src/x11.cpp src/x11.h)
endif()

# Output naming: x64 -> vpDiscordRPCx64, x86 -> vpDiscordRPCx86
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(VPDISCORD_OUTPUT_NAME "vpDiscordRPCx64")
else()
	set(VPDISCORD_OUTPUT_NAME "vpDiscordRPCx86")
endif()

function(add_vpdiscordrpc_plugin targetName apiVersion outputDir)
	add_library(${targetName} SHARED ${VPDISCORD_SOURCES})

	target_use_jackpluginsdk(${targetName})

	target_include_directories(${targetName} PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
	)

	target_link_libraries(${targetName} PRIVATE discord-rpc)

	target_compile_definitions(${targetName} PRIVATE JACK_API_VERSION=${apiVersion})

	# $<CONFIG> keeps CMake from appending the config subdirectory itself
	set_target_properties(${targetName} PROPERTIES
		OUTPUT_NAME ${VPDISCORD_OUTPUT_NAME}
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/${outputDir}
		LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/${outputDir}
		ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/${outputDir}
	)

	if(WIN32)
		target_compile_definitions(${targetName} PRIVATE WIN32 _CRT_SECURE_NO_WARNINGS)
		target_link_libraries(${targetName} PRIVATE psapi)
	else()
		find_package(Threads REQUIRED)
		target_link_libraries(${targetName} PRIVATE Threads::Threads ${CMAKE_DL_LIBS})

		set_target_properties(${targetName} PROPERTIES
			PREFIX ""
			CXX_VISIBILITY_PRESET hidden
			VISIBILITY_INLINES_HIDDEN ON
		)
	endif()

	add_custom_command(TARGET ${targetName} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_CURRENT_SOURCE_DIR}/vpDiscordRPC.ini
			$<TARGET_FILE_DIR:${targetName}>/vpDiscordRPC.ini
	)
endfunction()

# See the head of JackSDK/sdk/public/BaseTypes.h for API version nums
add_vpdiscordrpc_plugin(vpDiscordRPC_freeware	10	"freeware")
add_vpdiscordrpc_plugin(vpDiscordRPC_steam		100	"steam")
add_vpdiscordrpc_plugin(vpDiscordRPC_steambeta	121	"steambeta")
