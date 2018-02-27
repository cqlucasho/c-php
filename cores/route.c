#include "php_ant.h"
#include "route.h"
#include "cores/core.h"

zend_class_entry *ant_route_interface_ptr, *ant_route_abstract_class_ptr, *ant_route_default_class_ptr, *ant_route_pathinfo_class_ptr;

ZEND_BEGIN_ARG_INFO_EX(arginfo_route_interface_url, 0, 0, 1)
    ZEND_ARG_INFO(0, url)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_route_construct, 0, 0, 1)
    ZEND_ARG_INFO(0, application)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_route_generate, 0, 0, 2)
    ZEND_ARG_INFO(0, urls)
    ZEND_ARG_INFO(0, params)
ZEND_END_ARG_INFO()

zval *route_instance(TSRMLS_D) {
    zval *instance;
    if(ANT_G(route_style) == 1) {
        ANT_MAKE_ZVAL_OBJ_INIT(instance, ant_route_default_class_ptr)
    } else {
        ANT_MAKE_ZVAL_OBJ_INIT(instance, ant_route_pathinfo_class_ptr)
    }

    return instance;
}

/* {{{ ant_route_abstract_class methods */
ZEND_METHOD(ant_route_abstract_class, url) {
    zval *urls;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &urls) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    zend_update_property(ant_route_abstract_class_ptr, getThis(), PROPERTY_STRINGL(ANT_ROUTE_PROPERTY_URL_PARAMS), urls TSRMLS_CC);
}

ZEND_METHOD(ant_route_abstract_class, parse) {
    array_init(return_value);

    zval *urlParams = zend_read_property(ant_route_abstract_class_ptr, getThis(), PROPERTY_STRINGL(ANT_ROUTE_PROPERTY_URL_PARAMS), 1 TSRMLS_CC);
    if(Z_TYPE_P(urlParams) == IS_STRING) {
        add_assoc_string(return_value, "module", ANT_G(module), 0);
        add_assoc_string(return_value, "controller", ANT_G(controller), 0);
        add_assoc_string(return_value, "action", Z_STRVAL_P(urlParams), 0);
    }
    else {
        zval *retData;
        if(zend_hash_find(Z_ARRVAL_P(urlParams), PROPERTY_STRINGAL("module"), (void *)&retData) == SUCCESS) {
            add_assoc_string(return_value, "module", Z_STRVAL_P(retData), 0);
        } else {
            add_assoc_string(return_value, "module", ANT_G(module), 0);
        }
        if(zend_hash_find(Z_ARRVAL_P(urlParams), PROPERTY_STRINGAL("controller"), (void *)&retData) == SUCCESS) {
            add_assoc_string(return_value, "controller", Z_STRVAL_P(retData), 0);
        } else {
            add_assoc_string(return_value, "controller", ANT_G(controller), 0);
        }
        if(zend_hash_find(Z_ARRVAL_P(urlParams), PROPERTY_STRINGAL("action"), (void *)&retData) == SUCCESS) {
            add_assoc_string(return_value, "action", Z_STRVAL_P(retData), 0);
        } else {
            add_assoc_string(return_value, "action", ANT_G(action), 0);
        }
    }

    return;
}
/* }}} */

/* {{{ ant_route_default_class methods */
char *_default_full_url(zval *params TSRMLS_DC) {
    char *sql;
    HashTable *ht = HASH_OF(params);
    int paramNums = zend_hash_num_elements(ht);
    if(paramNums > 0) {
        char *key, *tempUrlStr = NULL, urlStr[1024] = {0};
        zval **pData;
        zend_hash_internal_pointer_reset(ht);
        for(int i = 0; i < paramNums; i++) {
            ulong numindex;
            zend_hash_get_current_key(ht, &key, &numindex, 0);
            if((strncmp(key, "module", 6) == 0) || (strncmp(key, "controller", 10) == 0) || (strncmp(key, "action", 6) == 0)) {
                continue;
            }

            zend_hash_get_current_data(ht, (void **)&pData);
            ANT_SWITCH_VAL_PP(pData)
            if(_v_type == 1) {
                spprintf(&tempUrlStr, 0, "&%s=%s", key, _v_str);
                _v_str = NULL;
            }
            else if(_v_type == 2) {
                spprintf(&tempUrlStr, 0, "&%s=%f", key, _v_double);
                _v_double = 0;
            }
            else if(_v_type == 3) {
                spprintf(&tempUrlStr, 0, "&%s=%ld", key, _v_long);
                _v_long = 0;
            }
            else if(_v_type == 4) {
                spprintf(&tempUrlStr, 0, "&%s=%d", key, _v_bool);
                _v_bool = 0;
            }

            if(!urlStr) {
                strncpy(urlStr, tempUrlStr, strlen(tempUrlStr));
            } else {
                strncat(urlStr, tempUrlStr, strlen(tempUrlStr));
            }

            efree(tempUrlStr);
            tempUrlStr = NULL; key = NULL;
            if(zend_hash_move_forward(ht) == FAILURE) {
                break;
            }
        }

        spprintf(&sql, 0, "%s", urlStr);
        return sql;
    }

    return NULL;
}

char *_route_default_class_url(zval *urls, zval *getParams TSRMLS_DC) {
    zval **module, **controller, **action;
    char *retUrl = NULL, *fullUrlParams = NULL;

    zend_hash_find(Z_ARRVAL_P(urls), PROPERTY_STRINGAL("module"), (void **)&module);
    zend_hash_find(Z_ARRVAL_P(urls), PROPERTY_STRINGAL("controller"), (void **)&controller);
    zend_hash_find(Z_ARRVAL_P(urls), PROPERTY_STRINGAL("action"), (void **)&action);

    fullUrlParams = _default_full_url(getParams TSRMLS_CC);
    if(fullUrlParams) {
        spprintf(&retUrl, 0, "?module=%s&controller=%s&action=%s%s", Z_STRVAL_PP(module), Z_STRVAL_PP(controller), Z_STRVAL_PP(action), fullUrlParams);
    } else {
        spprintf(&retUrl, 0, "?module=%s&controller=%s&action=%s", Z_STRVAL_PP(module), Z_STRVAL_PP(controller), Z_STRVAL_PP(action));
    }

    return retUrl;
}

char *route_default_generate(char *module, char *controller, char *action, zval *params TSRMLS_DC) {
    char *fullUrlParams = NULL, *retUrlStr;

    fullUrlParams = _default_full_url(params TSRMLS_CC);
    if(fullUrlParams) {
        spprintf(&retUrlStr, 0, "?module=%s&controller=%s&action=%s%s", module, controller, action, fullUrlParams);
    } else {
        spprintf(&retUrlStr, 0, "?module=%s&controller=%s&action=%s", module, controller, action);
    }

    return retUrlStr;
}

ZEND_METHOD(ant_route_default_class, __construct) {
    zval *appObj;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &appObj)) {
        WRONG_PARAM_COUNT
    }

    zend_update_property(ant_route_default_class_ptr, getThis(), PROPERTY_STRINGL(ANT_ROUTE_PROPERTY_APP_OBJ), appObj TSRMLS_CC);
}

ZEND_METHOD(ant_route_default_class, generate) {
    zval *urls, *getParams, *params[1];
    ANT_MAKE_ZVAL_ARRAY(getParams)
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|a", &urls, &getParams) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    ANT_CALL_USER_FUNC_NAME_C(url, 0)
    ANT_CALL_USER_FUNC_NAME_C(parse, 0)
    params[0] = urls;
    call_user_function(NULL, &getThis(), ANT_CALL_USER_FUNC_NAME(url, 0), 1, params TSRMLS_CC);
    call_user_function(NULL, &getThis(), ANT_CALL_USER_FUNC_NAME(parse, 0), 0, NULL TSRMLS_CC);

    char *url = _route_default_class_url(ANT_CALL_USER_FUNC_RET(parse, 0), getParams TSRMLS_CC);

    ANT_CALL_USER_FUNC_NAME_D(url, 0)
    ANT_CALL_USER_FUNC_NAME_D(parse, 0)
    zval_ptr_dtor(&getParams);
    RETURN_STRINGL(url, strlen(url), 1);
}
/* }}} */

/* {{{ ant_route_pathinfo_class methods */
char *_pathinfo_full_url(zval *params TSRMLS_DC) {
    char *sql = NULL;
    int paramNums = zend_hash_num_elements(Z_ARRVAL_P(params));
    if(paramNums > 0) {
        char *key, *tempUrlStr = NULL, urlStr[1024] = {0};
        zval **pData;
        zend_hash_internal_pointer_reset(Z_ARRVAL_P(params));
        for(int i = 0; i < paramNums; i++) {
            ulong numindex;
            zend_hash_get_current_key(Z_ARRVAL_P(params), &key, &numindex, 0);
            if((strncmp(key, "module", 6) == 0) || (strncmp(key, "controller", 10) == 0) || (strncmp(key, "action", 6)) == 0) {
                continue;
            }

            zend_hash_get_current_data(Z_ARRVAL_P(params), (void **)&pData);
            ANT_SWITCH_VAL_PP(pData)
            if(_v_type == 1) {
                spprintf(&tempUrlStr, 0, "/%s:%s", key, _v_str);
                _v_str = NULL;
            }
            else if(_v_type == 2) {
                spprintf(&tempUrlStr, 0, "/%s:%f", key, _v_double);
                _v_double = 0;
            }
            else if(_v_type == 3) {
                spprintf(&tempUrlStr, 0, "/%s:%ld", key, _v_long);
                _v_long = 0;
            }
            else if(_v_type == 4) {
                spprintf(&tempUrlStr, 0, "/%s:%d", key, _v_bool);
                _v_bool = 0;
            }

            if(!urlStr) {
                strncpy(urlStr, tempUrlStr, strlen(tempUrlStr));
            } else {
                strncat(urlStr, tempUrlStr, strlen(tempUrlStr));
            }

            efree(tempUrlStr);
            tempUrlStr = NULL; key = NULL;
            if(zend_hash_move_forward(Z_ARRVAL_P(params)) == FAILURE) {
                break;
            }
        }

        spprintf(&sql, 0, "%s", urlStr);
        return sql;
    }

    return "";
}

char *_route_pathinfo_class_url(zval *urls, zval *getParams TSRMLS_DC) {
    zval **module, **controller, **action;
    char *retUrl = NULL, *fullUrlParams = NULL;

    zend_hash_find(Z_ARRVAL_P(urls), PROPERTY_STRINGAL("module"), (void **)&module);
    zend_hash_find(Z_ARRVAL_P(urls), PROPERTY_STRINGAL("controller"), (void **)&controller);
    zend_hash_find(Z_ARRVAL_P(urls), PROPERTY_STRINGAL("action"), (void **)&action);

    fullUrlParams = _pathinfo_full_url(getParams TSRMLS_CC);
    if(fullUrlParams) {
        spprintf(&retUrl, 0, "/%s/%s/%s%s", Z_STRVAL_PP(module), Z_STRVAL_PP(controller), Z_STRVAL_PP(action), fullUrlParams);
    } else {
        spprintf(&retUrl, 0, "/%s/%s/%s", Z_STRVAL_PP(module), Z_STRVAL_PP(controller), Z_STRVAL_PP(action));
    }

    return retUrl;
}

char *route_pathinfo_generate(char *module, char *controller, char *action, zval *params TSRMLS_DC) {
    char *fullUrlParams = NULL, *retUrlStr;

    fullUrlParams = _pathinfo_full_url(params TSRMLS_CC);
    if(fullUrlParams) {
        spprintf(&retUrlStr, 0, "/%s/%s/%s%s", module, controller, action, fullUrlParams);
    } else {
        spprintf(&retUrlStr, 0, "/%s/%s/%s", module, controller, action);
    }

    return retUrlStr;
}

ZEND_METHOD(ant_route_pathinfo_class, __construct) {
    zval *appObj;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &appObj)) {
        WRONG_PARAM_COUNT
    }

    zend_update_property(ant_route_pathinfo_class_ptr, getThis(), PROPERTY_STRINGL(ANT_ROUTE_PROPERTY_APP_OBJ), appObj TSRMLS_CC);
}

ZEND_METHOD(ant_route_pathinfo_class, generate) {
    zval *urls, *getParams, *params[1];
    ANT_MAKE_ZVAL_ARRAY(getParams)
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|a", &urls, &getParams) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    ANT_CALL_USER_FUNC_NAME_C(url, 0)
    ANT_CALL_USER_FUNC_NAME_C(parse, 0)

    params[0] = urls;
    call_user_function(NULL, &getThis(), ANT_CALL_USER_FUNC_NAME(url, 0), 1, params TSRMLS_CC);
    call_user_function(NULL, &getThis(), ANT_CALL_USER_FUNC_NAME(parse, 0), 0, NULL TSRMLS_CC);

    char *url = _route_pathinfo_class_url(ANT_CALL_USER_FUNC_RET(parse, 0), getParams TSRMLS_CC);

    ANT_CALL_USER_FUNC_NAME_D(url, 0)
    ANT_CALL_USER_FUNC_NAME_D(parse, 0)
    zval_ptr_dtor(&getParams);
    RETURN_STRINGL(url, strlen(url), 1);
}
/* }}} */

zend_function_entry ant_route_interface_methods[] = {
    ZEND_ABSTRACT_ME(ant_route_interface, url, arginfo_route_interface_url)
    ZEND_ABSTRACT_ME(ant_route_interface, parse, NULL)
    PHP_FE_END
};

zend_function_entry ant_route_abstract_methods[] = {
    ZEND_ME(ant_route_abstract_class, url, arginfo_route_interface_url, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_route_abstract_class, parse, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

zend_function_entry ant_route_default_methods[] = {
    ZEND_ME(ant_route_default_class, __construct, arginfo_route_construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    ZEND_ME(ant_route_default_class, generate, arginfo_route_generate, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

zend_function_entry ant_route_pathinfo_methods[] = {
    ZEND_ME(ant_route_pathinfo_class, __construct, arginfo_route_construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    ZEND_ME(ant_route_pathinfo_class, generate, arginfo_route_generate, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

ANT_STARTUP_FUNCTION(route) {
    zend_class_entry ce_route_interface, ce_route_abstract, ce_route_default, ce_route_pathinfo;

    INIT_CLASS_ENTRY(ce_route_interface, "AntIRoute", ant_route_interface_methods);
    ant_route_interface_ptr = zend_register_internal_class(&ce_route_interface TSRMLS_CC);

    INIT_CLASS_ENTRY(ce_route_abstract, "AntARoute", ant_route_abstract_methods);
    ant_route_abstract_class_ptr = zend_register_internal_class(&ce_route_abstract TSRMLS_CC);
    zend_class_implements(ant_route_abstract_class_ptr TSRMLS_CC, 1, ant_route_interface_ptr);
    zend_declare_property_null(ant_route_abstract_class_ptr, PROPERTY_STRINGL(ANT_ROUTE_PROPERTY_URL_PARAMS), ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(ant_route_abstract_class_ptr, PROPERTY_STRINGL(ANT_ROUTE_PROPERTY_APP_OBJ), ZEND_ACC_PROTECTED TSRMLS_CC);

    INIT_CLASS_ENTRY(ce_route_default, "AntRouteDefault", ant_route_default_methods);
    ant_route_default_class_ptr = zend_register_internal_class_ex(&ce_route_default, ant_route_abstract_class_ptr, "AntARoute" TSRMLS_CC);

    INIT_CLASS_ENTRY(ce_route_pathinfo, "AntRoutePathinfo", ant_route_pathinfo_methods);
    ant_route_pathinfo_class_ptr = zend_register_internal_class_ex(&ce_route_pathinfo, ant_route_abstract_class_ptr, "AntARoute" TSRMLS_CC);

    return SUCCESS;
}

ANT_SHUTDOWN_FUNCTION(route) {
    return SUCCESS;
}