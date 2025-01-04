set(SOURCES wififactory.cpp wifiap.cpp wifista.cpp)

set(COMPONENTS mdns esp_event esp_wifi esp_netif lwip)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS "." REQUIRES ${COMPONENTS})