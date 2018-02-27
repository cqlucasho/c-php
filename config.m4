PHP_ARG_ENABLE(ant,
    whether to enable ant support,
    [  --enable-ant          Enable ant support])

if test -z "$PHP_DEBUG"; then
   AC_ARG_ENABLE(debug,
       [--enable-debug compile with debugging system],
       [PHP_DEBUG=$enableval],[PHP_DEBUG=no])
fi

if test "$PHP_ANT" != "no"; then
    PHP_ADD_EXTENSION_DEP(ant, mysqli mysqlnd)
    PHP_ADD_BUILD_DIR([$ext_builddir/cores])
    PHP_NEW_EXTENSION(ant,
        ant.c \
        ant_config.c \
        ant_application.c \
        app_controller.c \
        cores/core.c \
        cores/controller.c \
        cores/route.c \
        cores/view.c \
        cores/database.c \
        cores/services.c \
        authorize/authorize.c,
        $ext_shared)
    PHP_SUBST(ANT_SHARED_LIBADD)
fi
