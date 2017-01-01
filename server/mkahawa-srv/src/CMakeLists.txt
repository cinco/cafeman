project (mkahawa-srv)
set (MAJOR_VERSION "0")
set (MINOR_VERSION "0")
set (PATCH_LEVEL "4")
set (VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_LEVEL}")

add_definitions(-DVERSION)


file(GLOB   SRC_FILES	CCLIconItem.cpp
			CCLWin.cpp
			CashingFrame.cpp
			EmployeesFrame.cpp
			LogFrame.cpp
			MSGWin.cpp
			MembersFrame.cpp
			NotpaidFrame.cpp
			ProductsFrame.cpp
			QTicketsBox.cpp
			ReportFrame.cpp
			SettingsBox.cpp
			TarifFrame.cpp
			TicketsBox.cpp
			icons.cpp
			main.cpp
			verifiers.cpp )

add_executable (${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} ccls)


INCLUDE(FindPkgConfig)
pkg_check_modules(FOX-1.6   REQUIRED	fox)
if(FOX-1.6_FOUND)
    include_directories(${FOX-1.6_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${FOX-1.6_LIBRARIES})
endif(FOX-1.6_FOUND) 

find_package(Threads REQUIRED)
if(Threads_FOUND)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
endif(Threads_FOUND)

install (TARGETS  ${PROJECT_NAME} DESTINATION bin)
