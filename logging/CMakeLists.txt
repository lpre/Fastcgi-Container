# CC=gcc-4.9 CXX=g++-4.9 cmake ./
# rm -fr CMakeFiles && rm -f CMakeCache.txt && CC=gcc-4.9 CXX=g++-4.9 cmake ./

cmake_minimum_required (VERSION 2.8)

project(fastcgi3 CXX C)

set(FASTCGI3_MAJOR_VERSION 1)
set(FASTCGI3_MINOR_VERSION 0)
set(FASTCGI3_PATCH_VERSION 0)
set(FASTCGI3_VERSION
  ${FASTCGI3_MAJOR_VERSION}.${FASTCGI3_MINOR_VERSION}.${FASTCGI3_PATCH_VERSION}
)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
check_cxx_compiler_flag("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
	message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -pthread")
set(LIBS "${LIBS} -lxml -lpthread")
# set(LIBS "${LIBS} -lxml -lpthread -lfcgi -lfcgi++ -lssl -lcrypto")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

#--------------------------------

file(
    GLOB 
    container_files
    "library/*.cpp"
)
add_library(
    fastcgi3-container 
    SHARED
    ${container_files}
)
#set_target_properties(fastcgi3-container PROPERTIES
#  COMPILE_FLAGS "-pthread"
#)
set_target_properties(fastcgi3-container PROPERTIES
  LINK_FLAGS "-ldl -lfcgi -lfcgi++ -lssl -lcrypto"
)
set_target_properties(fastcgi3-container PROPERTIES
  PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/fastcgi3/*.h"
)

#--------------------------------

#file(
#    GLOB 
#    logging_files
#    "logging/*.cpp"
#)
#add_library(
#    fastcgi3-logging 
#    MODULE
#    ${logging_files}
#)
#SET_TARGET_PROPERTIES(fastcgi3-logging PROPERTIES
#  LINK_FLAGS "-llog4cxx"
#)
#target_link_libraries(fastcgi3-logging fastcgi3-container)

#--------------------------------

file(
    GLOB 
    syslog_files
    "syslog/*.cpp"
)
add_library(
    fastcgi3-syslog 
    MODULE
    ${syslog_files}
)
target_link_libraries(fastcgi3-syslog fastcgi3-container)

#--------------------------------

file(
    GLOB 
    filelogger_files
    "file-logger/*.cpp"
)
add_library(
    fastcgi3-filelogger 
    MODULE
    ${filelogger_files}
)
target_link_libraries(fastcgi3-filelogger fastcgi3-container)

#--------------------------------

file(
    GLOB 
    statistics_files
    "statistics/*.cpp"
)
add_library(
    fastcgi3-statistics 
    MODULE
    ${statistics_files}
)
target_link_libraries(fastcgi3-statistics fastcgi3-container)

#--------------------------------

file(
    GLOB 
    request-cache_files
    "request-cache/*.cpp"
)
add_library(
    fastcgi3-request-cache 
    MODULE
    ${request-cache_files}
)
target_link_libraries(fastcgi3-request-cache fastcgi3-container)

#--------------------------------

file(
    GLOB 
    session-manager_files
    "session-manager/*.cpp"
)
add_library(
    fastcgi3-session-manager 
    MODULE
    ${session-manager_files}
)
target_link_libraries(fastcgi3-session-manager fastcgi3-container)

#--------------------------------

file(
    GLOB 
    authenticator_files
    "authenticator/*.cpp"
)
add_library(
    fastcgi3-authenticator 
    MODULE
    ${authenticator_files}
)
target_link_libraries(fastcgi3-authenticator fastcgi3-container)

#--------------------------------

file(
    GLOB 
    page-compiler_files
    "page-compiler/*.cpp"
)
add_executable(
    fastcgi3-page-compiler 
    ${page-compiler_files}
)
target_link_libraries(fastcgi3-page-compiler fastcgi3-container)
SET_TARGET_PROPERTIES(fastcgi3-page-compiler PROPERTIES
  LINK_FLAGS "-luuid -lxml2"
)

#--------------------------------

file(
    GLOB 
    main_files
    "main/*.cpp"
)
add_executable(
    fastcgi3-daemon 
    ${main_files}
)
target_link_libraries(fastcgi3-daemon fastcgi3-container)
SET_TARGET_PROPERTIES(fastcgi3-daemon PROPERTIES
  LINK_FLAGS "-luuid -lxml2 -lfcgi -lfcgi++"
)

#--------------------------------

install(
	TARGETS 
		fastcgi3-daemon
		fastcgi3-page-compiler 
		fastcgi3-container
		fastcgi3-request-cache
		fastcgi3-filelogger
		fastcgi3-syslog
		fastcgi3-statistics
		fastcgi3-session-manager
		fastcgi3-authenticator
    EXPORT fastcgi3-container
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
    PUBLIC_HEADER DESTINATION include
)


