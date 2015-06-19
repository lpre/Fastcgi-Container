#configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/../config.h" @ONLY)

add_library(
    fastcgi3-request-cache 
    MODULE
    	file_cache.cpp
)
target_link_libraries(fastcgi3-request-cache fastcgi3-container)

install(
	TARGETS fastcgi3-request-cache
	EXPORT FastcgiContainerTargets
	LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
)

