project (ccls)

set (MAJOR_VERSION "0")
set (MINOR_VERSION "8")
set (PATCH_LEVEL "2")
set (VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_LEVEL}")

file(GLOB  SRC_FILES	ccl.c
			client.c 
			data.c
			employee.c 
			logging.c
			member.c
			tarif.c
			util.c )

add_library(${PROJECT_NAME}	SHARED	 ${SRC_FILES})


set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${VERSION} SOVERSION 0)

# Find the dependencies first
INCLUDE(FindPkgConfig)
pkg_check_modules(GLIB2	    REQUIRED	glib-2.0)
if(GLIB2_FOUND)
    include_directories(${GLIB2_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${GLIB2_LIBRARIES})
endif(GLIB2_FOUND)


pkg_check_modules(SQLITE3 REQUIRED sqlite3)
if(SQLITE3_FOUND)
    include_directories(${SQLITE3_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME}	sqlite3)
endif(SQLITE3_FOUND)


find_package(OpenSSL)
if(OPENSSL_FOUND)
    #	 libssl-dev header files are already included in source code
    #    include_directories(${OPENSSL_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} ${OPENSSL_LIBRARIES})
endif(OPENSSL_FOUND)


install (TARGETS  ${PROJECT_NAME} DESTINATION lib)
install (FILES ccls.h DESTINATION include)
