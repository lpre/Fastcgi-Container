cmake_minimum_required (VERSION 2.8)

include(CheckLibraryExists)
include(CheckIncludeFileCXX)

project(FastcgiContainer CXX)

set(FASTCGICONTAINER_MAJOR_VERSION 1)
set(FASTCGICONTAINER_MINOR_VERSION 0)
set(FASTCGICONTAINER_PATCH_VERSION 0)
set(FASTCGICONTAINER_VERSION
  ${FASTCGICONTAINER_MAJOR_VERSION}.${FASTCGICONTAINER_MINOR_VERSION}.${FASTCGICONTAINER_PATCH_VERSION}
)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (NOT (GCC_VERSION VERSION_GREATER 4.9 OR GCC_VERSION VERSION_EQUAL 4.9))
        message(FATAL_ERROR "${PROJECT_NAME} requires g++ 4.9 or greater.")
    endif ()
#elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
else ()
    message(FATAL_ERROR "Your C++ compiler does not support C++11")
endif ()

check_include_file_cxx(fcgiapp.h HAVE_FCGI_H)
check_include_file_cxx(dlfcn.h HAVE_DLFCN_H)
check_include_file_cxx(uuid/uuid.h HAVE_UUID_H)
check_include_file_cxx("libxml/tree.h" HAVE_XML_TREE_H)
check_include_file_cxx("libxml/parser.h" HAVE_XML_PARSER_H)
check_include_file_cxx("libxml/xpath.h" HAVE_XML_XPATH_H)
check_include_file_cxx("openssl/md5.h" HAVE_OPENSSL_MD5_H)
check_include_file_cxx(core/any.hpp HAVE_MNMLSTC_H "-std=c++11")

if(NOT HAVE_FCGI_H)
	message(FATAL_ERROR "fcgiapp.h not found")
endif(NOT HAVE_FCGI_H)
if(NOT HAVE_DLFCN_H)
	message(FATAL_ERROR "dlfn.h not found")
endif(NOT HAVE_DLFCN_H)
if(NOT HAVE_UUID_H)
	message(FATAL_ERROR "uuid/uuid.h not found")
endif(NOT HAVE_UUID_H)
if(NOT HAVE_XML_TREE_H)
	message(FATAL_ERROR "libxml header files not found")
endif(NOT HAVE_XML_TREE_H)
if(NOT HAVE_XML_PARSER_H)
	message(FATAL_ERROR "libxml header files not found")
endif(NOT HAVE_XML_PARSER_H)
if(NOT HAVE_XML_XPATH_H)
	message(FATAL_ERROR "libxml header files not found")
endif(NOT HAVE_XML_XPATH_H)
if(NOT HAVE_OPENSSL_MD5_H)
	message(FATAL_ERROR "libcrypto header files not found")
endif(NOT HAVE_OPENSSL_MD5_H)
if(NOT HAVE_MNMLSTC_H)
	message(FATAL_ERROR "MNMLSTC headers not found")
endif(NOT HAVE_MNMLSTC_H)


if(NOT CMAKE_BUILD_TYPE)
#  set(CMAKE_BUILD_TYPE Release)
   set(CMAKE_BUILD_TYPE Release CACHE STRING
			"Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
			FORCE)
endif(NOT CMAKE_BUILD_TYPE)
message("-- ${CMAKE_BUILD_TYPE} mode")

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/CMake/FastcgiContainer)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

# set up include-directories
include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")
set(LIBS "${LIBS} -lpthread")

if(NOT CMAKE_BUILD_TYPE MATCHES DEBUG)
    if(CMAKE_COMPILER_IS_GNUCXX)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s") 	# Strip binary
	endif(CMAKE_COMPILER_IS_GNUCXX)	
endif(NOT CMAKE_BUILD_TYPE MATCHES DEBUG)


# Add sub-directories
add_subdirectory(main)
add_subdirectory(library)
add_subdirectory(request-cache)
add_subdirectory(file-logger)
add_subdirectory(syslog)
add_subdirectory(statistics)
add_subdirectory(session-manager)
add_subdirectory(authenticator)
add_subdirectory(page-compiler)
###add_subdirectory(example)
###add_subdirectory(logging)


# Add all targets to the build-tree export set
export(
	TARGETS 
		fastcgi3-container
		fastcgi3-syslog
		fastcgi3-filelogger
		fastcgi3-statistics
		fastcgi3-request-cache
		fastcgi3-session-manager
		fastcgi3-authenticator
		fastcgi3-page-compiler
		fastcgi3-daemon
  	FILE "${PROJECT_BINARY_DIR}/FastcgiContainerTargets.cmake"
)

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE FastcgiContainer)

# Create the FastcgiContainerConfig.cmake and FastcgiContainerConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${INSTALL_INCLUDE_DIR}")
# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(FastcgiContainerConfig.cmake.in "${PROJECT_BINARY_DIR}/FastcgiContainerConfig.cmake" @ONLY)
# ... for the install tree
set(CONF_INCLUDE_DIRS "\${FASTCGICONTAINER_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(FastcgiContainerConfig.cmake.in "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/FastcgiContainerConfig.cmake" @ONLY)
# ... for both
configure_file(FastcgiContainerConfigVersion.cmake.in "${PROJECT_BINARY_DIR}/FastcgiContainerConfigVersion.cmake" @ONLY)

# Install the FastcgiContainerConfig.cmake and FastcgiContainerConfigVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/FastcgiContainerConfig.cmake"
  "${PROJECT_BINARY_DIR}/FastcgiContainerConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT FastcgiContainerTargets DESTINATION
  "${INSTALL_CMAKE_DIR}" COMPONENT dev)



