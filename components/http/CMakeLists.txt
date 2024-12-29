set(EMBEDDED
    assets/favicon.ico
    css/app.min.css
    html/brightness.min.html
    html/manager.min.html
    js/brightness.min.js
    js/manager.min.js
    js/jquery.min.js
)

set(SOURCES managerserver.cpp)

set(COMPONENTS esp_http_server)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS "." REQUIRES ${COMPONENTS} EMBED_FILES ${EMBEDDED})
