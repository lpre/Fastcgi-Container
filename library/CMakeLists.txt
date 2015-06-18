#configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/../config.h" @ONLY)

add_library(
    fastcgi3-container 
    SHARED
	attributes_holder.cpp  
	componentset.cpp  
	except.cpp       
	handlerset.cpp     
	loader.cpp     
	request.cpp                   
	security_authenticator.cpp  
	server.cpp           
	string_buffer.cpp
	component_context.cpp  
	config.cpp        
	file_buffer.cpp  
	http_request.cpp   
	logger.cpp     
	request_filter.cpp            
	security_principal.cpp      
	session.cpp          
	util.cpp
	component.cpp          
	cookie.cpp        
	globals.cpp      
	http_response.cpp  
	mmap_file.cpp  
	request_thread_pool.cpp       
	security_realm.cpp          
	session_manager.cpp  xml.cpp
	component_factory.cpp  
	data_buffer.cpp   
	handler.cpp      
	http_servlet.cpp   
	parser.cpp     
	response_time_statistics.cpp  
	security_subject.cpp        
	stream.cpp
)

set_target_properties(fastcgi3-container PROPERTIES
	LINK_FLAGS "-ldl -lfcgi -lfcgi++ -lssl -lcrypto"
)

file(
	GLOB 
    fastcgi3-container_include_files
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/fastcgi3/*.h"
)
set_target_properties(fastcgi3-container PROPERTIES
	PUBLIC_HEADER "${fastcgi3-container_include_files}"
)

install(
	TARGETS fastcgi3-container
	EXPORT FastcgiContainerTargets
	RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
	LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
	PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/fastcgi3"
	COMPONENT dev
)

