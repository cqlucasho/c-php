#include "php_ant.h"
#include "Zend/zend_interfaces.h"
#include "SAPI.h"
#include "php_globals.h"
#include "ant_application.h"
#include "app_controller.h"
#include "cores/core.h"
#include "cores/route.h"
#include "cores/view.h"
#include "cores/database.h"
#include "cores/services.h"
#include "session/php_session.h"

zend_class_entry *ant_application_class_ptr;

ZEND_BEGIN_ARG_INFO_EX(arginfo_application_get_session, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

zval *_request_query(uint type, char * name, uint len TSRMLS_DC) {
    zval **carrier = NULL, **ret;
    zend_bool jit_initialization = PG(auto_globals_jit);

    switch (type) {
        case TRACK_VARS_GET:
        case TRACK_VARS_POST:
        case TRACK_VARS_FILES:
        case TRACK_VARS_COOKIE:
            carrier = &PG(http_globals)[type]; break;
        case TRACK_VARS_ENV:
            if(jit_initialization) {
                zend_is_auto_global(ZEND_STRL("_ENV") TSRMLS_CC);
            }
            carrier = &PG(http_globals)[type];
            break;
        case TRACK_VARS_SERVER:
            if (jit_initialization) {
                zend_is_auto_global(ZEND_STRL("_SERVER") TSRMLS_CC);
            }
            carrier = &PG(http_globals)[type];
            break;
        case TRACK_VARS_REQUEST:
            if (jit_initialization) {
                zend_is_auto_global(ZEND_STRL("_REQUEST") TSRMLS_CC);
            }
            (void)zend_hash_find(&EG(symbol_table), ZEND_STRS("_REQUEST"), (void **)&carrier);
            break;
        default:
            break;
    }

    if (!carrier || !(*carrier)) {
        zval *empty;
        ANT_MAKE_ZVAL_NULL(empty)
        return empty;
    }

    if (!len) {
        Z_ADDREF_P(*carrier);
        return *carrier;
    }

    if (zend_hash_find(Z_ARRVAL_PP(carrier), name, len + 1, (void **)&ret) == FAILURE) {
        zval *empty;
        ANT_MAKE_ZVAL_NULL(empty)
        return empty;
    }

    Z_ADDREF_P(*ret);
    return *ret;
}

void _database_init(zval *this TSRMLS_DC) {
    zval *host, *user, *password, *dbname, *charset, *dbObj, *serviceObj, *dbParams[5], *serviceParams[1], *appParams[1];
    host     = read_db_config("host" TSRMLS_CC);
    user     = read_db_config("user" TSRMLS_CC);
    password = read_db_config("password" TSRMLS_CC);
    dbname   = read_db_config("select_db" TSRMLS_CC);
    charset  = read_db_config("charset" TSRMLS_CC);
    ANT_MAKE_ZVAL_OBJ_INIT(dbObj, ant_database_class_ptr)
    ANT_MAKE_ZVAL_OBJ_INIT(serviceObj, ant_services_class_ptr)
    ANT_CALL_USER_FUNC_NAME_C(__construct, 0)
    ANT_CALL_USER_FUNC_NAME_C(setDB, 0)
    ANT_CALL_USER_FUNC_NAME_C(setDb, application)

    dbParams[0] = host;
    dbParams[1] = user;
    dbParams[2] = password;
    dbParams[3] = dbname;
    dbParams[4] = charset;
    call_user_function(NULL, &dbObj, ANT_CALL_USER_FUNC_NAME(__construct, 0), 5, dbParams TSRMLS_CC);
    serviceParams[0] = ANT_CALL_USER_FUNC_RET(__construct, 0);
    call_user_function(NULL, &serviceObj, ANT_CALL_USER_FUNC_NAME(setDB, 0), 1, serviceParams TSRMLS_CC);
    appParams[0] = ANT_CALL_USER_FUNC_RET(__construct, 0);
    call_user_function(NULL, &this, ANT_CALL_USER_FUNC_NAME(setDb, application), 1, appParams TSRMLS_CC);

    zval_ptr_dtor(&dbObj);
    zval_ptr_dtor(&serviceObj);
    ANT_CALL_USER_FUNC_NAME_D(__construct, 0)
    ANT_CALL_USER_FUNC_NAME_D(setDB, 0)
    ANT_CALL_USER_FUNC_NAME_D(setDb, application)
}

void _application_invoke(zval *this TSRMLS_DC) {
    char *fileName, *controllerStr, *actionStr;
    zend_class_entry **controllerClass;
    zval *contrlObj, *viewObj;

    /* initialize globals that include the module and controller, action */
    zval *moduleName    = _request_query(TRACK_VARS_GET, PROPERTY_STRINGL("module") TSRMLS_CC);
    zval *contrlName    = _request_query(TRACK_VARS_GET, PROPERTY_STRINGL("controller") TSRMLS_CC);
    zval *actionName    = _request_query(TRACK_VARS_GET, PROPERTY_STRINGL("action") TSRMLS_CC);
    ANT_G(module)       = Z_STRVAL_P(moduleName);
    ANT_G(controller)   = Z_STRVAL_P(contrlName);
    ANT_G(action)       = Z_STRVAL_P(actionName);

    if((Z_TYPE_P(moduleName) == IS_NULL) || (Z_TYPE_P(contrlName) == IS_NULL) || (Z_TYPE_P(actionName) == IS_NULL)) {
        zend_error(E_ERROR, "the module name or controller name, action name is null");
        return;
    }

    /* loading the file */
    spprintf(&fileName, 0, "%s/controllers/%s_controller", ANT_G(module), ANT_G(controller));
    core_class_import(fileName, "path_apps" TSRMLS_CC);
    /* set class name */
    char *transController   = ant_pascal_func(ANT_G(controller), 0);
    char *transAction       = ant_pascal_func(ANT_G(action), 0);
    spprintf(&controllerStr, 0, "%scontroller", ant_upper_to_lower(transController));
    spprintf(&actionStr, 0, "%saction", ant_upper_to_lower(transAction));

    if(zend_hash_find(EG(class_table), PROPERTY_STRINGAL(controllerStr), (void **)&controllerClass) == FAILURE) {
        Z_DELREF_P(moduleName); Z_DELREF_P(contrlName); Z_DELREF_P(actionName);
        efree(controllerStr); efree(actionStr);
        zend_error(E_ERROR, "the %s haven't loaded.", controllerStr);
    }

    ANT_MAKE_ZVAL_OBJ_INIT(viewObj, ant_view_class_ptr)
    ANT_MAKE_ZVAL_OBJ_INIT(contrlObj, *controllerClass)
    // initialize AppController's property
    zend_call_method_with_2_params(&contrlObj, *controllerClass, NULL, "__construct", NULL, this, viewObj);
    // call the action
    zend_call_method(&contrlObj, *controllerClass, NULL, PROPERTY_STRINGL(actionStr), NULL, 0, NULL, NULL TSRMLS_CC);

    Z_DELREF_P(moduleName); Z_DELREF_P(contrlName); Z_DELREF_P(actionName);
    efree(controllerStr); efree(actionStr);
    zval_ptr_dtor(&contrlObj);
    zval_ptr_dtor(&viewObj);
    return;
}

/* the query method is major used to fetching the `GET` arguments */
zval *application_query(char *argName TSRMLS_DC) {
    zval *zv = _request_query(TRACK_VARS_GET, PROPERTY_STRINGL(argName) TSRMLS_CC);
    return zv;
}

/* the query method is major used to fetching the `POST` arguments */
zval *application_form(char *argName TSRMLS_DC) {
    zval *zv = _request_query(TRACK_VARS_POST, PROPERTY_STRINGL(argName) TSRMLS_CC);
    return zv;
}

/* set status of http header */
int application_set_header(zval *this_ptr, char *key, zval *value TSRMLS_DC) {
    return 1;
}

/* set content of http response */
int application_set_content(zval *this_ptr, char *value TSRMLS_DC) {
    return 1;
}

zval *get_db(zval *this TSRMLS_DC) {
    zval *db = zend_read_property(ant_application_class_ptr, this, PROPERTY_STRINGL(ANT_APPLICATION_PROPERTY_DB), 1 TSRMLS_CC);
    return db;
}

void _display() {
    //php_printf("display");
}

ZEND_METHOD(ant_application_class, __construct) {
    zval *routeParam[1], *routeDefaultObj, *routePathinfoObj;
    ANT_MAKE_ZVAL_OBJ_INIT(routeDefaultObj, ant_route_default_class_ptr);
    ANT_MAKE_ZVAL_OBJ_INIT(routePathinfoObj, ant_route_pathinfo_class_ptr);
    ANT_CALL_USER_FUNC_NAME_C(__construct, droute)
    ANT_CALL_USER_FUNC_NAME_C(__construct, proute)

    routeParam[0] = getThis();
    call_user_function(NULL, &routeDefaultObj, ANT_CALL_USER_FUNC_NAME(__construct, droute), 1, routeParam TSRMLS_CC);
    call_user_function(NULL, &routePathinfoObj, ANT_CALL_USER_FUNC_NAME(__construct, proute), 1, routeParam TSRMLS_CC);
    zend_update_property(ant_application_class_ptr, getThis(), PROPERTY_STRINGL(ANT_APPLICATION_PROPERTY_ROUTE_DEFAULT_OBJ), routeDefaultObj TSRMLS_CC);
    zend_update_property(ant_application_class_ptr, getThis(), PROPERTY_STRINGL(ANT_APPLICATION_PROPERTY_ROUTE_PATHINFO_OBJ), routePathinfoObj TSRMLS_CC);

    // initialize ANT_G(route_style)
    zval *routeStyle = read_config("route_style" TSRMLS_CC);
    if(strncmp(Z_STRVAL_P(routeStyle), "default", 7) == 0) {
        ANT_G(route_style) = 1;
    } else {
        ANT_G(route_style) = 2;
    }

    zval_ptr_dtor(&routeDefaultObj);
    zval_ptr_dtor(&routePathinfoObj);
    ANT_CALL_USER_FUNC_NAME_D(__construct, droute)
    ANT_CALL_USER_FUNC_NAME_D(__construct, proute)
    return;
}

/* get route object */
ZEND_METHOD(ant_application_class, getRoute) {
    zval *routeObj = route_instance(TSRMLS_C);
    if(Z_TYPE_P(routeObj) == IS_OBJECT) {
        RETURN_ZVAL(routeObj, 1, 1)
    }

    RETURN_NULL()
}

/* get all the session */
ZEND_METHOD(ant_application_class, getAllSession) {
    if(Z_TYPE_P(PS(http_session_vars)) == IS_ARRAY) {
        RETURN_ZVAL(PS(http_session_vars), 1, 1)
    }

    RETURN_NULL()
}

/* {{{ get a specified session value by key */
ZEND_METHOD(ant_application_class, getSession) {
    zval **result, *httpSession;
    char *key;
    int keyLen;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &keyLen) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    httpSession = PS(http_session_vars);
    if((Z_TYPE_P(httpSession) == IS_ARRAY) &&
        (zend_hash_find(HASH_OF(httpSession), PROPERTY_STRINGAL(key), (void **)&result) == SUCCESS)) {

        if(Z_TYPE_PP(result) == IS_ARRAY) {
            RETURN_ZVAL(*result, 1, 0)
        }

        ANT_SWITCH_RETURN_VAL_PP(result)
    }

    RETURN_NULL()
}
/* }}} */

/* the current request whether is `get` */
ZEND_METHOD(ant_application_class, isGet) {
    const char *requestMethod = SG(request_info).request_method;
    if(strncmp(requestMethod, PROPERTY_STRINGL("GET")) == 0) {
        RETURN_TRUE
    }

    RETURN_FALSE
}

/* the current request whether is `post` */
ZEND_METHOD(ant_application_class, isPost) {
    const char *requestMethod = SG(request_info).request_method;
    if(strncmp(requestMethod, PROPERTY_STRINGL("POST")) == 0) {
        RETURN_TRUE
    }

    RETURN_FALSE
}

/* fetch the module name by GET argument */
ZEND_METHOD(ant_application_class, module) {
    ANT_APPLICATION_FETCH_GET(module, ANT_APPLICATION_PROPERTY_MODULE)
}

/* fetch the controller name by GET argument */
ZEND_METHOD(ant_application_class, controller) {
    ANT_APPLICATION_FETCH_GET(controller, ANT_APPLICATION_PROPERTY_CONTROLLER)
}

/* fetch the action name by GET argument */
ZEND_METHOD(ant_application_class, action) {
    ANT_APPLICATION_FETCH_GET(action, ANT_APPLICATION_PROPERTY_ACTION)
}

ZEND_METHOD(ant_application_class, setDb) {
    zval *db;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &db) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    zend_update_property(ant_application_class_ptr, getThis(), PROPERTY_STRINGL(ANT_APPLICATION_PROPERTY_DB), db TSRMLS_CC);
}

ZEND_METHOD(ant_application_class, display) {
    _database_init(getThis() TSRMLS_CC);
    _application_invoke(getThis() TSRMLS_CC);
    _display();
}

zend_function_entry ant_application_methods[] = {
    ZEND_ME(ant_application_class, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    ZEND_ME(ant_application_class, getAllSession, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_application_class, getSession, arginfo_application_get_session, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_application_class, isGet, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_application_class, isPost, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_application_class, module, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_application_class, controller, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_application_class, action, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_application_class, getRoute, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_application_class, display, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_application_class, setDb, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

ANT_STARTUP_FUNCTION(application) {
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "AntApplication", ant_application_methods);
    ant_application_class_ptr = zend_register_internal_class(&ce TSRMLS_CC);
    zend_register_class_alias("Ant\\Application", ant_application_class_ptr);

    zend_declare_property_null(ant_application_class_ptr, PROPERTY_STRINGL(ANT_APPLICATION_PROPERTY_DB), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(ant_application_class_ptr, PROPERTY_STRINGL(ANT_APPLICATION_PROPERTY_ROUTE_DEFAULT_OBJ), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(ant_application_class_ptr, PROPERTY_STRINGL(ANT_APPLICATION_PROPERTY_ROUTE_PATHINFO_OBJ), ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(ant_application_class_ptr, PROPERTY_STRINGL(ANT_APPLICATION_PROPERTY_HEADER), ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_string(ant_application_class_ptr, PROPERTY_STRINGL(ANT_APPLICATION_PROPERTY_CONTENT), "", ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_string(ant_application_class_ptr, PROPERTY_STRINGL(ANT_APPLICATION_PROPERTY_MODULE), "", ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_string(ant_application_class_ptr, PROPERTY_STRINGL(ANT_APPLICATION_PROPERTY_CONTROLLER), "", ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_string(ant_application_class_ptr, PROPERTY_STRINGL(ANT_APPLICATION_PROPERTY_ACTION), "", ZEND_ACC_PROTECTED TSRMLS_CC);

    ANT_STARTUP(route);
    ANT_STARTUP(view);
    ANT_STARTUP(app_controller);
    return SUCCESS;
}

ANT_SHUTDOWN_FUNCTION(application) {
    return SUCCESS;
}