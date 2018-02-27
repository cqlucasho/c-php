#include "php_ant.h"
#include "ant_config.h"

zend_class_entry *ant_config_class_ptr;

zval *config_instance(TSRMLS_D) {
    zval *instance = zend_read_static_property(ant_config_class_ptr, PROPERTY_STRINGL(ANT_CONFIG_PROPERTY_INSTANCE), 1 TSRMLS_CC);
    if(Z_TYPE_P(instance) != IS_NULL) {
        return instance;
    }

    ANT_MAKE_ZVAL_OBJ_INIT(instance, ant_config_class_ptr);
    zend_update_static_property(ant_config_class_ptr, PROPERTY_STRINGL(ANT_CONFIG_PROPERTY_INSTANCE), instance TSRMLS_CC);

    return instance;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_config_class_config, 0, 0, 1)
    ZEND_ARG_INFO(0, configs)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_config_class_config_get, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_config_class_config_set, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_config_class_db_config, 0, 0, 1)
    ZEND_ARG_INFO(0, dbs)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_config_class_db_get, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_config_class_db_set, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

/* {{{ initial config of directory */
ZEND_METHOD(ant_config_class, initConfig) {
    zval *args;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &args) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    zval *configs = zend_read_static_property(ConfigClass, PROPERTY_STRINGL(ANT_CONFIG_PROPERTY_CONFIGS), 1 TSRMLS_CC);
    if(Z_TYPE_P(configs) != IS_NULL) {
        return;
    }

    if(Z_TYPE_P(args) == IS_ARRAY) {
        zend_update_static_property(ConfigClass, PROPERTY_STRINGL(ANT_CONFIG_PROPERTY_CONFIGS), args TSRMLS_CC);
        RETURN_TRUE
    }

    RETURN_FALSE
}

ZEND_METHOD(ant_config_class, getConfig) {
    char *argName;
    int argLen;
    zval **retString;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &argName, &argLen) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    zval *ret = zend_read_static_property(ant_config_class_ptr, PROPERTY_STRINGL(ANT_CONFIG_PROPERTY_CONFIGS), 1 TSRMLS_CC);
    if((Z_TYPE_P(ret) != IS_NULL) &&
        (zend_hash_find(Z_ARRVAL_P(ret), argName, strlen(argName)+1, (void **)&retString) == SUCCESS)) {

        ANT_SWITCH_RETURN_VAL_PP(retString)
    }

    RETURN_NULL()
}

ZEND_METHOD(ant_config_class, setConfig) {
    char *argName;
    char *argValue;
    int argNLen, argVLen;
    zval *val;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &argName, &argNLen, &argValue, &argVLen) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    MAKE_STD_ZVAL(val);
    ZVAL_STRINGL(val, argValue, strlen(argValue), 1);
    zval *configs = zend_read_static_property(ant_config_class_ptr, PROPERTY_STRINGL(ANT_CONFIG_PROPERTY_CONFIGS), 1 TSRMLS_CC);
    if(zend_hash_update(Z_ARRVAL_P(configs), argName, strlen(argName)+1, (void*)&val, sizeof(zval*), NULL) == SUCCESS) {
        RETURN_TRUE
    }

    RETURN_FALSE
}
/* }}} */

/* {{{ initial config of db */
ZEND_METHOD(ant_config_class, initDb) {
    zval *args;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &args) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    zval *_configs = zend_read_static_property(ConfigClass, PROPERTY_STRINGL(ANT_CONFIG_PROPERTY_DBS), 1 TSRMLS_CC);
    if(Z_TYPE_P(_configs) != IS_NULL) {
        return;
    }

    if(Z_TYPE_P(args) == IS_ARRAY) {
        zend_update_static_property(ConfigClass, PROPERTY_STRINGL(ANT_CONFIG_PROPERTY_DBS), args TSRMLS_CC);
        RETURN_TRUE
    }

    RETURN_FALSE
}

ZEND_METHOD(ant_config_class, getDb) {
    char *argName;
    int argLen;
    zval **retString;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &argName, &argLen) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    zval *ret = zend_read_static_property(ConfigClass, PROPERTY_STRINGL(ANT_CONFIG_PROPERTY_DBS), 1 TSRMLS_CC);
    if((Z_TYPE_P(ret) != IS_NULL) &&
       (zend_hash_find(Z_ARRVAL_P(ret), argName, strlen(argName)+1, (void **)&retString) == SUCCESS)) {

        ANT_SWITCH_RETURN_VAL_PP(retString)
    }

    RETURN_NULL()
}

ZEND_METHOD(ant_config_class, setDb) {
    char *argName;
    char *argValue;
    int argNLen, argVLen;
    zval *val;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &argName, &argNLen, &argValue, &argVLen) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    MAKE_STD_ZVAL(val);
    ZVAL_STRINGL(val, argValue, strlen(argValue), 1);
    zval *configs = zend_read_static_property(ConfigClass, PROPERTY_STRINGL(ANT_CONFIG_PROPERTY_DBS), 1 TSRMLS_CC);
    if(zend_hash_update(Z_ARRVAL_P(configs), argName, strlen(argName)+1, (void*)&val, sizeof(zval*), NULL) == SUCCESS) {
        RETURN_TRUE
    }

    RETURN_FALSE
}
/* }}} */

/* {{{ initialize config of authes*/
ZEND_METHOD(ant_config_class, initAuth) {
    zval *args;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &args) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    zval *authes = zend_read_static_property(ConfigClass, PROPERTY_STRINGL(ANT_CONFIG_PROPERTY_AUTHES), 1 TSRMLS_CC);
    if(Z_TYPE_P(authes) != IS_NULL) {
        return;
    }

    if(Z_TYPE_P(args) == IS_ARRAY) {
        zend_update_static_property(ConfigClass, PROPERTY_STRINGL(ANT_CONFIG_PROPERTY_AUTHES), args TSRMLS_CC);
        RETURN_TRUE
    }

    RETURN_FALSE
}

ZEND_METHOD(ant_config_class, getAuth) {
    char *argName;
    int argLen;
    zval **retString;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &argName, &argLen) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    zval *ret = zend_read_static_property(ConfigClass, PROPERTY_STRINGL(ANT_CONFIG_PROPERTY_AUTHES), 1 TSRMLS_CC);
    if((Z_TYPE_P(ret) != IS_NULL) &&
       (zend_hash_find(Z_ARRVAL_P(ret), argName, strlen(argName)+1, (void **)&retString) == SUCCESS)) {
        ANT_SWITCH_RETURN_VAL_PP(retString)
    }

    RETURN_NULL()
}
/* }}} */

zend_function_entry ant_config_methods[] = {
    ZEND_ME(ant_config_class, initConfig, arginfo_config_class_config, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    ZEND_ME(ant_config_class, getConfig, arginfo_config_class_config_get, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    ZEND_ME(ant_config_class, setConfig, arginfo_config_class_config_set, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    ZEND_ME(ant_config_class, initDb, arginfo_config_class_db_config, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    ZEND_ME(ant_config_class, setDb, arginfo_config_class_db_set, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    ZEND_ME(ant_config_class, getDb, arginfo_config_class_db_get, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    ZEND_ME(ant_config_class, initAuth, arginfo_config_class_config, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    ZEND_ME(ant_config_class, getAuth, arginfo_config_class_config_get, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    PHP_FE_END
};

ANT_STARTUP_FUNCTION(config) {
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "AntConfig", ant_config_methods);
    ant_config_class_ptr = zend_register_internal_class(&ce TSRMLS_CC);
    zend_register_class_alias("Ant\\Config", ant_config_class_ptr);

    zend_declare_property_null(ant_config_class_ptr, PROPERTY_STRINGL(ANT_CONFIG_PROPERTY_CONFIGS), ZEND_ACC_PRIVATE|ZEND_ACC_STATIC TSRMLS_CC);
    zend_declare_property_null(ant_config_class_ptr, PROPERTY_STRINGL(ANT_CONFIG_PROPERTY_DBS), ZEND_ACC_PRIVATE|ZEND_ACC_STATIC TSRMLS_CC);
    zend_declare_property_null(ant_config_class_ptr, PROPERTY_STRINGL(ANT_CONFIG_PROPERTY_AUTHES), ZEND_ACC_PRIVATE|ZEND_ACC_STATIC TSRMLS_CC);
    zend_declare_property_null(ant_config_class_ptr, PROPERTY_STRINGL(ANT_CONFIG_PROPERTY_INSTANCE), ZEND_ACC_PRIVATE|ZEND_ACC_STATIC TSRMLS_CC);

    return SUCCESS;
}
