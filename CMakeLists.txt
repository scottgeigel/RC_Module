set(PROJECT_NAME rc_init)

set(SOURCE_FILES
    rc_init.c
)

add_library(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} pthread)

add_executable(tester main.c)
target_link_libraries(tester ${PROJECT_NAME})
