cmake_minimum_required (VERSION 2.8.5)
if(NOT CMAKE_VERSION VERSION_LESS "3.0")
	cmake_policy(SET CMP0048 OLD)
endif()
if(NOT CMAKE_VERSION VERSION_LESS "3.1")
	cmake_policy(SET CMP0054 OLD)
endif()
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
cmake_dependent_option(ZW_BUILD_TESTS "Build Tests" ON "NOT CMAKE_TOOLCHAIN_FILE" OFF)

if(NOT MSVC)
	if(NOT CMAKE_VERSION VERSION_LESS "3.1")
		set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
		set(CMAKE_CXX_STANDARD "11")
	else()
		check_cxx_compiler_flag("-std=gnu++11" _COMPILER_HAS_STD_GNUPP11)
		if(_COMPILER_HAS_STD_GNUPP11)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
		else()
			check_cxx_compiler_flag("-std=c++11" _COMPILER_HAS_STD_GNUPP0X)
			if(_COMPILER_HAS_STD_GNUPP0X)
				set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
			else()
				check_cxx_compiler_flag("-std=c++11" _COMPILER_HAS_STD_CPP11)
				if(_COMPILER_HAS_STD_CPP11)
					set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
				endif()
			endif()
		endif()
	endif()
endif()

cmake_dependent_option(NOWIDE_USE_MSVC_RUNTIME "Use MSVC Runtime Library DLL" ON MSVC OFF)
if(MSVC AND NOT NOWIDE_USE_MSVC_RUNTIME)
	foreach(flag CMAKE_C_FLAGS
			CMAKE_C_FLAGS_DEBUG
			CMAKE_C_FLAGS_RELEASE
			CMAKE_C_FLAGS_MINSIZEREL
			CMAKE_C_FLAGS_RELWITHDEBINFO)
		string(REGEX REPLACE "/MDd" "/MTd" ${flag} "${${flag}}")
		string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
	endforeach()
endif()

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
else()
	check_cxx_compiler_flag("-fvisibility=hidden" _COMPILER_HAS_VISIBILITY_HIDDEN)
	check_cxx_compiler_flag("-Wall" _COMPILER_HAS_WARN_ALL)
	check_cxx_compiler_flag("-Wextra" _COMPILER_HAS_WARN_EXTRA)
	if(_COMPILER_HAS_WARN_ALL)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
	endif()
	if(_COMPILER_HAS_WARN_EXTRA)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
	endif()
endif()

set(SUBDIVIDE_HEADERS
	"${PROJECT_SOURCE_DIR}/basedata.hpp"
	"${PROJECT_SOURCE_DIR}/config.hpp"
	"${PROJECT_SOURCE_DIR}/serialize.hpp"
	"${PROJECT_SOURCE_DIR}/vector.hpp")
set(SUBDIVIDE_SOURCE
	"${PROJECT_SOURCE_DIR}/basedata.cpp"
	"${PROJECT_SOURCE_DIR}/subdivide.cpp")
add_executable(subdivide ${SUBDIVIDE_SOURCE} ${SUBDIVIDE_HEADERS})
if(HONOR_VISILIBITY)
	set_target_properties("subdivide" PROPERTIES C_VISIBILITY_PRESET hidden)
elseif(_COMPILER_HAS_VISIBILITY_HIDDEN)
	set_target_properties("subdivide" PROPERTIES COMPILE_FLAGS "-fvisibility=hidden")
endif()

install(TARGETS subdivide
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
if(MSVC)
	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/Debug/subdivide.pdb"
		DESTINATION ${CMAKE_INSTALL_BINDIR} CONFIGURATIONS Debug)
	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/subdivide.pdb"
		DESTINATION ${CMAKE_INSTALL_BINDIR} CONFIGURATIONS RelWithDebInfo)
endif()
install(FILES "${PROJECT_SOURCE_DIR}/LICENSE.md" DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES "${PROJECT_SOURCE_DIR}/README.md" DESTINATION ${CMAKE_INSTALL_DOCDIR})

