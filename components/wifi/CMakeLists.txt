set(SOURCES wififactory.cpp wifiap.cpp)

set(COMPONENTS esp_wifi)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS "." REQUIRES ${COMPONENTS})