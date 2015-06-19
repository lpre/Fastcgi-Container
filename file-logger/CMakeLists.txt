#configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/../config.h" @ONLY)

add_library(
    fastcgi3-filelogger 
    MODULE
    	file_logger.cpp
)
target_link_libraries(fastcgi3-filelogger fastcgi3-container)

install(
	TARGETS fastcgi3-filelogger
	EXPORT FastcgiContainerTargets
	LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
)
