#configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/../config.h" @ONLY)

add_library(
    example 
    MODULE
    	example.cpp
)
target_link_libraries(example fastcgi3-container)

add_custom_command(
    OUTPUT Login.cpp factory.cpp
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../page-compiler/fastcgi3-page-compiler -updateFactory=true -updateMakefile=false *.cpsp
)
add_library(
    example-cpsp 
    MODULE
    	factory.cpp
	   	Login.cpp
)
target_link_libraries(example-cpsp fastcgi3-container)
add_dependencies(example-cpsp fastcgi3-page-compiler)

