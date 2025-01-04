set(SOURCES storage.cpp)

set(COMPONENTS wifi nvs_flash)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS "." REQUIRES ${COMPONENTS})