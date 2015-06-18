
cmake_minimum_required (VERSION 2.8)

project(FastcgiContainer CXX)

set(FASTCGICONTAINER_MAJOR_VERSION 1)
set(FASTCGICONTAINER_MINOR_VERSION 0)
set(FASTCGICONTAINER_PATCH_VERSION 0)
set(FASTCGICONTAINER_VERSION
  ${FASTCGICONTAINER_MAJOR_VERSION}.${FASTCGICONTAINER_MINOR_VERSION}.${FASTCGICONTAINER_PATCH_VERSION}
)

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

set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -pthread -std=c++11")
set(LIBS "${LIBS} -lpthread")


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
add_subdirectory(example)
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



