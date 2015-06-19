#configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/../config.h" @ONLY)

add_library(
    fastcgi3-logging 
    MODULE
    	logger.cpp
)
target_link_libraries(fastcgi3-logging fastcgi3-container)
set_target_properties(fastcgi3-logging PROPERTIES
	LINK_FLAGS "-llog4cxx"
)

install(
	TARGETS fastcgi3-logging
	EXPORT FastcgiContainerTargets
	LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
)
