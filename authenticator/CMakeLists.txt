#configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/../config.h" @ONLY)

add_library(
    fastcgi3-authenticator 
    MODULE
    	abstract_authenticator.h  
    	basic_authenticator.h  
    	delegated_authenticator.h  
    	form_authenticator.h
    	abstract_authenticator.cpp  
    	basic_authenticator.cpp  
    	delegated_authenticator.cpp  
    	factory.cpp  
    	form_authenticator.cpp
)
target_link_libraries(fastcgi3-authenticator fastcgi3-container)

install(
	TARGETS fastcgi3-authenticator
	EXPORT FastcgiContainerTargets
	LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
)
