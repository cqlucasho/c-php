/**
 * @author lucasho
 * @created 2017-8-4
 * @modified 2017-8-4
 * @version 1.0
 * @link github.com/cqlucasho
 */
#ifndef PHP_ANT_APPLICATION_H
#define PHP_ANT_APPLICATION_H

extern zend_class_entry *ant_application_class_ptr;

#define ANT_APPLICATION_PROPERTY_DB "_db"
#define ANT_APPLICATION_PROPERTY_HEADER "_header"
#define ANT_APPLICATION_PROPERTY_CONTENT "_content"
#define ANT_APPLICATION_PROPERTY_ROUTE_DEFAULT_OBJ "_route_default_obj"
#define ANT_APPLICATION_PROPERTY_ROUTE_PATHINFO_OBJ "_route_pathinfo_obj"
#define ANT_APPLICATION_PROPERTY_MODULE "_module"
#define ANT_APPLICATION_PROPERTY_CONTROLLER "_controller"
#define ANT_APPLICATION_PROPERTY_ACTION "_action"

#define ANT_APPLICATION_FETCH_GET(name, property) \
    zval *vars, **ret; \
    vars = PG(http_globals)[TRACK_VARS_GET];    \
    if(zend_hash_find(HASH_OF(vars), ZEND_STRS(STRS(name)), (void **)&ret) == SUCCESS) {   \
        char *retName = Z_STRVAL_PP(ret);  \
        zend_update_property_stringl(ant_application_class_ptr, getThis(), PROPERTY_STRINGL(property), PROPERTY_STRINGL(retName) TSRMLS_CC);\
        RETURN_STRING(retName, 1)   \
    }   \
    RETURN_NULL()   \

ANT_STARTUP_FUNCTION(application);
ANT_SHUTDOWN_FUNCTION(application);

zval *_request_query(uint type, char * name, uint len TSRMLS_DC);
void _application_invoke(zval *this TSRMLS_DC);
extern zval *application_query(char *argName TSRMLS_DC);
extern zval *application_form(char *argName TSRMLS_DC);
extern int application_set_header(zval *this_ptr, char *key, zval *value TSRMLS_DC);
extern int application_set_content(zval *this_ptr, char *value TSRMLS_DC);
extern zval *get_db(zval *this TSRMLS_DC);

#endif
