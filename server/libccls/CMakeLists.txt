project (ccls)


set (LIBCCLS_MAJOR_VERSION "0")
set (LIBCCLS_MINOR_VERSION "8")
set (LIBCCLS_PATCH_LEVEL "2")
set (VERSION "${LIBCCLS_MAJOR_VERSION}.${LIBCCLS_MINOR_VERSION}.${LIBCCLS_PATCH_LEVEL}")


add_library(${PROJECT_NAME} SHARED src/ccl.c
						src/client.c 
						src/data.c
						src/employee.c 
						src/logging.c
						src/member.c
						src/tarif.c
						src/util.c)

#add_library(${PROJECT_NAME} STATIC src/ccl.c
#						src/client.c 
#						src/data.c
#						src/employee.c 
#						src/logging.c
#						src/member.c
#						src/tarif.c
#						src/util.c)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${VERSION} SOVERSION 0)
include_directories("./src")
include_directories("/usr/include/glib-2.0")
include_directories("/usr/lib/x86_64-linux-gnu/glib-2.0/include")
target_link_libraries(${PROJECT_NAME} sqlite3)
target_link_libraries(${PROJECT_NAME} ssl)
target_link_libraries(${PROJECT_NAME} glib-2.0)

install (TARGETS  ${PROJECT_NAME} DESTINATION lib)
install (FILES src/ccls.h DESTINATION include)
