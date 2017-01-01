project (mkahawa-client)
set (MAJOR_VERSION "0")
set (MINOR_VERSION "0")
set (PATCH_LEVEL "5")
set (VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_LEVEL}")

add_definitions(-DVERSION)

file(GLOB   SRC_FILES	cclc.c
			util.c
			cclcfox.cpp
			gui.cpp
			locker.cpp
			lockpix.cpp
			main.cpp )

add_executable (${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME}	cclc
					dl )

INCLUDE(FindPkgConfig)
pkg_check_modules(FOX-1.6   REQUIRED	fox)
if(FOX-1.6_FOUND)
    include_directories(${FOX-1.6_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${FOX-1.6_LIBRARIES})
endif(FOX-1.6_FOUND) 

pkg_check_modules(X11 REQUIRED x11)
if(X11_FOUND)
    include_directories(${X11_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES})
endif(X11_FOUND)

find_package(Threads REQUIRED)
if(Threads_FOUND)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
endif(Threads_FOUND)



find_library(GTK2 REQUIRED)
if(GTK2_FOUND)
    include_directories(${GTK2_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${GTK2_LIBRARIES})
endif(GTK2_FOUND) 

find_library(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
    target_link_libraries(${PROJECT_NAME} ${OPENSSL_LIBRARIES})
endif(OPENSSL_FOUND) 

target_link_libraries(${PROJECT_NAME} ${OPENSSL_LIBRARIES})


install (TARGETS  ${PROJECT_NAME} DESTINATION bin)

#make LIBS= -ldl -lFOX-1.6 -lX11 -lpthread -lcrypto -lssl -lnotify
