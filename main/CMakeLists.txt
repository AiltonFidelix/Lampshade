set(SOURCES main.cpp)

set(COMPONENTS led wifi nvs_flash)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS "." REQUIRES ${COMPONENTS})
