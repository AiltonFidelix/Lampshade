set(EMBEDDED
    assets/favicon.ico
    css/app.min.css
    html/lampshade.min.html
    html/manager.min.html
    js/lampshade.min.js
    js/manager.min.js
    js/jquery.min.js
)

set(SOURCES managerserver.cpp lampshadeserver.cpp)

set(COMPONENTS esp_http_server esp_wifi)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS "." REQUIRES ${COMPONENTS} EMBED_FILES ${EMBEDDED})
