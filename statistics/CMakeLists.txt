#configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/../config.h" @ONLY)

add_library(
    fastcgi3-statistics 
    MODULE
    	response_time_handler.cpp
    	response_time_handler.h
)
target_link_libraries(fastcgi3-statistics fastcgi3-container)

install(
	TARGETS fastcgi3-statistics
	EXPORT FastcgiContainerTargets
	LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
)
