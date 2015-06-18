#configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/../config.h" @ONLY)

add_executable(
    fastcgi3-page-compiler 
	    code_writer.cpp  
	    main.cpp  
	    page.cpp  
	    page_reader.cpp  
	    utils.cpp
	    code_writer.h  
	    page.h  
	    page_reader.h  
	    utils.h
)
target_link_libraries(fastcgi3-page-compiler fastcgi3-container)
SET_TARGET_PROPERTIES(fastcgi3-page-compiler PROPERTIES
  LINK_FLAGS "-luuid -lxml2"
)

install(
	TARGETS fastcgi3-page-compiler
	EXPORT FastcgiContainerTargets
	RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
)

