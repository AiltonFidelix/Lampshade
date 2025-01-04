set(SOURCES main.cpp)

set(COMPONENTS led wifi http storage nvs_flash)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS "." REQUIRES ${COMPONENTS})
