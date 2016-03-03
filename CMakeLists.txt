cmake_minimum_required (VERSION 3.2)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0054 NEW)
if(NOT CMAKE_VERSION VERSION_LESS "3.3")
	cmake_policy(SET CMP0063 NEW)
	set(HONOR_VISIBILITY TRUE)
else()
	set(HONOR_VISIBILITY FALSE)
endif()

project (zawarudo CXX)
include(CheckCXXCompilerFlag)
include(CMakeDependentOption)
include(GNUInstallDirs)
cmake_dependent_option(BUILD_TESTS "Build Tests" ON "NOT CMAKE_TOOLCHAIN_FILE" OFF)

if(NOT MSVC)
	if(NOT CMAKE_VERSION VERSION_LESS "3.1")
		set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
		set(CMAKE_CXX_STANDARD "11")
	else()
		check_cxx_compiler_flag("-std=gnu++11" _COMPILER_HAS_STD_GNUPP11)
		if(_COMPILER_HAS_STD_GNUPP11)
			list(APPEND CUSTOM_CFLAGS "-std=gnu++11")
		else()
			check_cxx_compiler_flag("-std=c++11" _COMPILER_HAS_STD_CPP11)
			if(_COMPILER_HAS_STD_CPP11)
				list(APPEND CUSTOM_CFLAGS "-std=c++11")
			endif()
		endif()
	endif()
endif()

cmake_dependent_option(USE_MSVC_RUNTIME "Use MSVC Runtime Library DLL" ON MSVC OFF)
if(MSVC AND NOT USE_MSVC_RUNTIME)
	foreach(flag CMAKE_C_FLAGS
			CMAKE_CXX_FLAGS_DEBUG
			CMAKE_CXX_FLAGS_RELEASE
			CMAKE_CXX_FLAGS_MINSIZEREL
			CMAKE_CXX_FLAGS_RELWITHDEBINFO)
		string(REGEX REPLACE "/MDd" "/MTd" ${flag} "${${flag}}")
		string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
	endforeach()
endif()

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	list(APPEND CUSTOM_CFLAGS "/W3")
else()
	check_cxx_compiler_flag("-fvisibility=hidden" _COMPILER_HAS_VISIBILITY_HIDDEN)
	check_cxx_compiler_flag("-Wall" _COMPILER_HAS_WARN_ALL)
	check_cxx_compiler_flag("-Wextra" _COMPILER_HAS_WARN_EXTRA)
    if(_COMPILER_HAS_VISIBILITY_HIDDEN)
		list(APPEND CUSTOM_CFLAGS "-fvisibility=hidden")
	endif()
	if(_COMPILER_HAS_WARN_ALL)
		list(APPEND CUSTOM_CFLAGS "-Wall")
	endif()
	if(_COMPILER_HAS_WARN_EXTRA)
		list(APPEND CUSTOM_CFLAGS "-Wextra")
	endif()
endif()

if(NOT MSVC)
	set(OLD_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})

	set(CMAKE_REQUIRED_FLAGS "-Wl,--as-needed")
	check_cxx_compiler_flag("" _LINKER_HAS_AS_NEEDED)
	if (_LINKER_HAS_AS_NEEDED)
		list(APPEND CUSTOM_LDFLAGS "-Wl,--as-needed")
	endif()

	set(CMAKE_REQUIRED_FLAGS "-Wl,--no-undefined")
	check_cxx_compiler_flag("" _LINKER_HAS_NO_UNDEFINED)
	if (_LINKER_HAS_NO_UNDEFINED)
		list(APPEND CUSTOM_LDFLAGS "-Wl,--no-undefined")
	endif()

	set(CMAKE_REQUIRED_FLAGS "-Wl,--no-allow-shlib-undefined")
	check_cxx_compiler_flag("" _LINKER_HAS_NO_SHLIB_UNDEFINED)
	if (_LINKER_HAS_NO_SHLIB_UNDEFINED)
		list(APPEND CUSTOM_LDFLAGS "-Wl,--no-allow-shlib-undefined")
	endif()

	if (MINGW)
		set(CMAKE_REQUIRED_FLAGS "-Wl,--nxcompat")
		check_cxx_compiler_flag("" _LINKER_HAS_DEP)
		if (_LINKER_HAS_DEP)
			list(APPEND CUSTOM_LDFLAGS "-Wl,--nxcompat")
		endif()

		set(CMAKE_REQUIRED_FLAGS "-Wl,--dynamicbase")
		check_cxx_compiler_flag("" _LINKER_HAS_ASLR)
		if (_LINKER_HAS_ASLR)
			list(APPEND CUSTOM_LDFLAGS "-Wl,--dynamicbase")
		endif()

		set(CMAKE_REQUIRED_FLAGS "-Wl,--high-entropy-va")
		check_cxx_compiler_flag("" _LINKER_HAS_64ASLR)
		if (_LINKER_HAS_64ASLR)
			list(APPEND CUSTOM_LDFLAGS "-Wl,--high-entropy-va")
		endif()
	endif()

	set(CMAKE_REQUIRED_FLAGS ${OLD_CMAKE_REQUIRED_FLAGS})
endif()

set(ZAWARUDO_HEADERS
	"${PROJECT_SOURCE_DIR}/config.hpp"
	"${PROJECT_SOURCE_DIR}/lib/ezOptionParser.hpp"
	"${PROJECT_SOURCE_DIR}/lib/stb_image_write.h"
	"${PROJECT_SOURCE_DIR}/geodesic.hpp"
	"${PROJECT_SOURCE_DIR}/image.hpp"
	"${PROJECT_SOURCE_DIR}/map.hpp"
	"${PROJECT_SOURCE_DIR}/serialize.hpp"
	"${PROJECT_SOURCE_DIR}/vector.hpp")
set(ZAWARUDO_SOURCE
	"${PROJECT_SOURCE_DIR}/geodesic.cpp"
	"${PROJECT_SOURCE_DIR}/image.cpp"
	"${PROJECT_SOURCE_DIR}/zawarudo.cpp")
add_executable(zawarudo ${ZAWARUDO_SOURCE} ${ZAWARUDO_HEADERS})

if(HONOR_VISILIBITY)
	set_target_properties(zawarudo PROPERTIES C_VISIBILITY_PRESET hidden)
endif()
set_target_properties(zawarudo PROPERTIES
	COMPILE_OPTIONS ${CUSTOM_CFLAGS}
	LINK_FLAGS ${CUSTOM_LDFLAGS}
	POSITION_INDEPENDENT_CODE ON)

if(BUILD_TESTS)
	enable_testing()
	add_test(subdivide_force zawarudo -f -i 2)
	add_test(subdivide_reuse zawarudo -i 2)
endif()

install(TARGETS zawarudo
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
if(MSVC)
	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/Debug/zawarudo.pdb"
		DESTINATION ${CMAKE_INSTALL_BINDIR} CONFIGURATIONS Debug)
	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/zawarudo.pdb"
		DESTINATION ${CMAKE_INSTALL_BINDIR} CONFIGURATIONS RelWithDebInfo)
endif()
install(FILES "${PROJECT_SOURCE_DIR}/LICENSE.md" DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES "${PROJECT_SOURCE_DIR}/README.md" DESTINATION ${CMAKE_INSTALL_DOCDIR})

