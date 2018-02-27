#include "php_ant.h"
#include "cores/core.h"
#include "cores/controller.h"
#include "cores/database.h"
#include "cores/services.h"
#include "ant_config.h"
#include "ant_application.h"

ZEND_DECLARE_MODULE_GLOBALS(ant)

/**
 * @see core.h/ant_pascal_func
 */
ZEND_FUNCTION(ant_pascal) {
    char *name;
    int name_len, flag = 1, flagLen;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &name, &name_len, &flag, &flagLen) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    char *result = ant_pascal_func(name, flag);
    RETVAL_STRING(result, 1);
    efree(result);
    return;
}
/**
 * @see core.h/ant_camel_func
 */
ZEND_FUNCTION(ant_camel) {
    char *name;
    int name_len;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    char *result = ant_camel_func(name);
    RETVAL_STRING(result, 1);
    efree(result);
    return;
}

zend_function_entry ant_functions[] = {
    ZEND_FE(ant_pascal, NULL)
    ZEND_FE(ant_camel, NULL)
    {NULL,NULL,NULL}
};

PHP_GINIT_FUNCTION(ant) {
    ant_globals->response_headers   = NULL;
    ant_globals->response_content	= NULL;
    ant_globals->request_module_path = NULL;
    ant_globals->module		        = NULL;
    ant_globals->controller			= NULL;
    ant_globals->action			    = NULL;
    ant_globals->view_vars          = NULL;
    ant_globals->show_message       = NULL;
    ant_globals->auto_render        = 1;
    ant_globals->route_style        = 0;
}

PHP_MINIT_FUNCTION(ant) {
    ANT_STARTUP(core);
    ANT_STARTUP(config);
    ANT_STARTUP(controller);
    ANT_STARTUP(application);
    ANT_STARTUP(database);
    ANT_STARTUP(services);

    REGISTER_STRINGL_CONSTANT("ALERT_INFO", "alert-info", strlen("alert-info"), CONST_CS | CONST_PERSISTENT);
    REGISTER_STRINGL_CONSTANT("ALERT_SUCCESS", "alert-success", strlen("alert-success"), CONST_CS | CONST_PERSISTENT);
    REGISTER_STRINGL_CONSTANT("ALERT_WARNING", "alert-warning", strlen("alert-warning"), CONST_CS | CONST_PERSISTENT);
    REGISTER_STRINGL_CONSTANT("ALERT_DANGER", "alert-danger", strlen("alert-danger"), CONST_CS | CONST_PERSISTENT);

    return SUCCESS;
};

PHP_MSHUTDOWN_FUNCTION(ant) {
    //auth_free();

	return SUCCESS;
};

PHP_RINIT_FUNCTION(ant) {
    ANT_G(response_headers)     = NULL;
    ANT_G(response_content)     = NULL;
    ANT_G(request_module_path)  = NULL;
    ANT_G(module)               = NULL;
    ANT_G(controller)           = NULL;
    ANT_G(action)               = NULL;
    ANT_G(view_vars)            = NULL;
    ANT_G(show_message)         = NULL;
    ANT_G(auto_render)          = 1;
    ANT_G(route_style)          = 0;

	return SUCCESS;
};

PHP_RSHUTDOWN_FUNCTION(ant) {
	return SUCCESS;
};

PHP_MINFO_FUNCTION(ant) {
	php_info_print_table_start();
	php_info_print_table_header(2, "ant support", "enabled");
	php_info_print_table_row(2, "version", PHP_ANT_VERSION);
	php_info_print_table_end();
};

#if ZEND_MODULE_API_NO >= 20050922
zend_module_dep ant_deps[] = {
    ZEND_MOD_REQUIRED("mysqli")
    {NULL, NULL, NULL}
};
#endif

zend_module_entry ant_module_entry = {
#if ZEND_MODULE_API_NO >= 20050922
    STANDARD_MODULE_HEADER_EX, NULL,
    ant_deps,
#else
    STANDARD_MODULE_HEADER,
#endif
	"ant",
	ant_functions,
	PHP_MINIT(ant),
	PHP_MSHUTDOWN(ant),
	PHP_RINIT(ant),
	PHP_RSHUTDOWN(ant),
	PHP_MINFO(ant),
	PHP_ANT_VERSION,
    PHP_MODULE_GLOBALS(ant),
    PHP_GINIT(ant),
    NULL, NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_ANT
ZEND_GET_MODULE(ant)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
