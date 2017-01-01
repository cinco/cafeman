project (cclc)

set (MAJOR_VERSION "0")
set (MINOR_VERSION "8")
set (PATCH_LEVEL "0")
set (VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_LEVEL}")


file(GLOB SRC_FILES cclc.c
	  util.c )


add_library(${PROJECT_NAME}	SHARED	 ${SRC_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${VERSION} SOVERSION 0)

# Find the dependencies first

find_package(OpenSSL)
if(OPENSSL_FOUND)
    #	 libssl-dev header files are already included in source code
    #    include_directories(${OPENSSL_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} ${OPENSSL_LIBRARIES})
endif(OPENSSL_FOUND)


install (TARGETS  ${PROJECT_NAME} DESTINATION lib)
install (FILES cclc.h DESTINATION include)

