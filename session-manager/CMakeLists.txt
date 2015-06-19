#configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/../config.h" @ONLY)

add_library(
    fastcgi3-session-manager 
    MODULE
    	simple_session_manager.cpp
)
target_link_libraries(fastcgi3-session-manager fastcgi3-container)

install(
	TARGETS fastcgi3-session-manager
	EXPORT FastcgiContainerTargets
	LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
)
