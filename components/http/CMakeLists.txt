set(EMBEDDED
    assets/favicon.ico
    css/app.min.css
    html/lampshade.min.html
    js/app.min.js
    js/jquery-3.7.1.min.js
)

set(SOURCES )

set(COMPONENTS esp_http_server)

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS "." REQUIRES ${COMPONENTS} EMBED_FILES ${EMBEDDED})