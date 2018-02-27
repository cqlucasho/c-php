cmake_minimum_required(VERSION 3.7)
project(ant_compile)

set(CMAKE_CXX_STANDARD 11)
set(CURRENT_DIR /opt/vendor/php/php-5.6.31/ext/ant_compile)
set(SOURCE_FILES
        cores/route.c
        cores/route.h
        ant_application.c
        ant_application.h
        cores/controller.c
        cores/controller.h
        cores/core.c
        cores/core.h
        cores/view.c
        cores/view.h
        cores/database.c
        cores/database.h
        cores/services.c
        cores/services.h
        authorize/authorize.h
        authorize/authorize.c
        ant.c
        ant_config.c
        ant_config.h
        app_controller.c
        app_controller.h
        php_ant.h
        ant_mmap.c ant_mmap.h)

add_executable(ant_compile ${SOURCE_FILES})
include_directories(
        "/opt/downloads/php-5.6.31"
        "/opt/downloads/php-5.6.31/ext"
        "/opt/downloads/php-5.6.31/main"
        "/opt/downloads/php-5.6.31/TSRM"
        "/opt/downloads/php-5.6.31/Zend"
        "/opt/downloads/yaf-php5"
        "${CURRENT_DIR}"
        "${CURRENT_DIR}/cores"
)