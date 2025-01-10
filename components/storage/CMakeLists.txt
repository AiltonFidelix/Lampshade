set(SOURCES wifistorage.cpp)

set(COMPONENTS nvs_flash)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS "." REQUIRES ${COMPONENTS})