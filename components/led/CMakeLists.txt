set(SOURCES ledesp32.cpp)

set(COMPONENTS driver)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS "." REQUIRES ${COMPONENTS})