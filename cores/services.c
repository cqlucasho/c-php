#include "php_ant.h"
#include "services.h"

zend_class_entry *ant_services_class_ptr;

ZEND_BEGIN_ARG_INFO_EX(arginfo_services_set_db, 0, 0, 1)
    ZEND_ARG_INFO(0, db)
ZEND_END_ARG_INFO()

ZEND_METHOD(ant_services_class, db) {
    zval *db = zend_read_static_property(ant_services_class_ptr, PROPERTY_STRINGL(ANT_SERVICE_PROPERTY_DB), 1 TSRMLS_CC);
    if(Z_TYPE_P(db) == IS_OBJECT){
        RETURN_ZVAL(db, 1, 0)
    }

    RETURN_NULL()
}

ZEND_METHOD(ant_services_class, setDB) {
    zval *db;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &db)) {
        WRONG_PARAM_COUNT
    }

    zend_update_static_property(ant_services_class_ptr, PROPERTY_STRINGL(ANT_SERVICE_PROPERTY_DB), db TSRMLS_CC);
}

zend_function_entry ant_services_methods[] = {
    ZEND_ME(ant_services_class, db, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    ZEND_ME(ant_services_class, setDB, arginfo_services_set_db, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

ANT_STARTUP_FUNCTION(services) {
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "AntAService", ant_services_methods);
    ant_services_class_ptr = zend_register_internal_class(&ce TSRMLS_CC);
    zend_register_class_alias("Ant\\AService", ant_services_class_ptr);

    zend_declare_property_null(ant_services_class_ptr, PROPERTY_STRINGL(ANT_SERVICE_PROPERTY_DB), ZEND_ACC_PROTECTED|ZEND_ACC_STATIC TSRMLS_CC);

    return SUCCESS;
}

