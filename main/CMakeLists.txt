#configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/../config.h" @ONLY)

add_executable(
    fastcgi3-daemon 
    	endpoint.cpp  
    	fcgi_request.cpp  
    	fcgi_server.cpp
    	main.cpp
)
target_link_libraries(fastcgi3-daemon fastcgi3-container)
SET_TARGET_PROPERTIES(fastcgi3-daemon PROPERTIES
  LINK_FLAGS "-luuid -lxml2 -lfcgi -lfcgi++"
)

install(
	TARGETS fastcgi3-daemon
	EXPORT FastcgiContainerTargets
	RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
)
