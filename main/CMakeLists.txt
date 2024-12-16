set(SOURCES main.cpp)

set(COMPONENTS led driver esp_wifi esp_http_server)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS "." REQUIRES ${COMPONENTS})
