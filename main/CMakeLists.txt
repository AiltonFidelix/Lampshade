set(SOURCES main.cpp)

set(COMPONENTS led wifi esp_http_server)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS "." REQUIRES ${COMPONENTS})
