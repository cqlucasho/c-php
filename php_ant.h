#ifndef PHP_ANT_H
#define PHP_ANT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#define PHP_ANT_VERSION "0.1.0"

#if defined(__GNUC__) && __GNUC__ >= 4
#define PHP_ANT_API __attribute__ ((visibility("default")))
#else
#define PHP_ANT_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#define STRS(s) #s
#define PROPERTY_STRINGL(s) s,strlen(s)
#define PROPERTY_STRINGAL(s) s,strlen(s)+1

#define ANT_SWITCH_VAL_PP(result) \
    char *_v_str; double _v_double; long _v_long; int _v_bool; int _v_type; \
    switch(Z_TYPE_PP(result)) { \
        case IS_STRING: \
            _v_str = Z_STRVAL_PP(result); _v_type = 1; break; \
        case IS_DOUBLE: \
            _v_double = Z_DVAL_PP(result); _v_type = 2; break; \
        case IS_LONG:   \
            _v_long = Z_LVAL_PP(result); _v_type = 3; break; \
        case IS_BOOL:   \
            _v_bool = Z_BVAL_PP(result); _v_type = 4; break; \
        default: \
            _v_str = NULL; _v_type = 5; \
    }   \

#define ANT_SWITCH_ZVAL_PP(zv, v) switch(Z_TYPE_PP(v)) \
    { \
        case IS_ARRAY: \
            ZVAL_ZVAL(zv, *v, 1, 1); break; \
        case IS_STRING: \
            ZVAL_STRINGL(zv, Z_STRVAL_PP(v), strlen(Z_STRVAL_PP(v)), 1); break; \
        case IS_DOUBLE: \
            ZVAL_DOUBLE(zv, Z_DVAL_PP(v)); break; \
        case IS_LONG:   \
            ZVAL_LONG(zv, Z_LVAL_PP(v)); break; \
        case IS_BOOL:   \
            ZVAL_BOOL(zv, Z_BVAL_PP(v)); break; \
        default: \
            ZVAL_NULL(zv); \
    }

#define ANT_SWITCH_ZVAL_P(zv, v) switch(Z_TYPE_P(v)) \
    { \
        case IS_ARRAY: \
            ZVAL_ZVAL(zv, v, 1, 1); break; \
        case IS_STRING: \
            ZVAL_STRINGL(zv, Z_STRVAL_P(v), strlen(Z_STRVAL_P(v)), 1); break; \
        case IS_DOUBLE: \
            ZVAL_DOUBLE(zv, Z_DVAL_P(v)); break; \
        case IS_LONG:   \
            ZVAL_LONG(zv, Z_LVAL_P(v)); break; \
        case IS_BOOL:   \
            ZVAL_BOOL(zv, Z_BVAL_P(v)); break; \
        default: \
            ZVAL_NULL(zv); \
    }

#define ANT_SWITCH_RETURN_VAL_P(result) switch(Z_TYPE_P(result)) { \
            case IS_ARRAY:  \
                RETURN_ZVAL(result, 1, 1); break;  \
            case IS_STRING: \
                RETURN_STRING(Z_STRVAL_P(result), 1); break;   \
            case IS_DOUBLE: \
                RETURN_DOUBLE(Z_DVAL_P(result)); break;    \
            case IS_LONG:   \
                RETURN_LONG(Z_LVAL_P(result)); break;  \
            case IS_BOOL:   \
                RETURN_BOOL(Z_BVAL_P(result)); break;  \
            default:    \
                RETURN_NULL()   \
        }   \

#define ANT_SWITCH_RETURN_VAL_PP(result) switch(Z_TYPE_PP(result)) { \
            case IS_ARRAY:  \
                RETURN_ZVAL(*result, 1, 1); break;  \
            case IS_STRING: \
                RETURN_STRING(Z_STRVAL_PP(result), 1); break;   \
            case IS_DOUBLE: \
                RETURN_DOUBLE(Z_DVAL_PP(result)); break;    \
            case IS_LONG:   \
                RETURN_LONG(Z_LVAL_PP(result)); break;  \
            case IS_BOOL:   \
                RETURN_BOOL(Z_BVAL_PP(result)); break;  \
            default:    \
                RETURN_NULL()   \
        }   \

#define ANT_SWITCH_ADD_ARRAY_PP(zv, result) zval **data; char *sKey; ulong iKey; HashTable* resultHt; \
    resultHt = Z_ARRVAL_PP(zv); \
    int count = zend_hash_num_elements(resultHt); \
    for(int i=0 ; i<count; i++) { \
        zend_hash_get_current_data(resultHt, (void**)&data); \
        zend_hash_get_current_key(resultHt, &sKey, &iKey, 0); \
        switch(Z_TYPE_PP(data)) { \
            case IS_ARRAY: \
                add_assoc_zval(result, sKey, data); break; \
            case IS_STRING: \
                add_assoc_stringl(result, sKey, Z_STRVAL_PP(data), Z_STRLEN_PP(data), 1); break;\
            case IS_DOUBLE: \
                add_assoc_double(result, sKey, Z_BVAL_PP(data)); break; \
            case IS_LONG:   \
                add_assoc_long(result, sKey, Z_LVAL_PP(data)); break; \
            case IS_BOOL:   \
                add_assoc_bool(result, sKey, Z_BVAL_PP(data)); break;  \
            default:    \
                break;   \
        }   \
    } \
    zend_hash_move_forward(zv);\

#define ANT_ARRAY_ADD_ASSOC_P(key, v, result) \
    switch(Z_TYPE_P(v)) { \
        case IS_ARRAY: \
            add_assoc_zval(result, key, v); break; \
        case IS_STRING: \
            add_assoc_stringl(result, key, Z_STRVAL_P(v), Z_STRLEN_P(v), 1); break;\
        case IS_DOUBLE: \
            add_assoc_double(result, key, Z_BVAL_P(v)); break; \
        case IS_LONG:   \
            add_assoc_long(result, key, Z_LVAL_P(v)); break; \
        case IS_BOOL:   \
            add_assoc_bool(result, key, Z_BVAL_P(v)); break;  \
        default:    \
            break;   \
    }   \

#define ANT_CALL_USER_FUNC_NAME_C(name, prefix) \
    zval *zv_call_##prefix##_##name, *zv_call_##prefix##_##name##_ret; \
    ANT_MAKE_ZVAL_STRINGL(zv_call_##prefix##_##name, name) \
    ANT_MAKE_ZVAL_NULL(zv_call_##prefix##_##name##_ret)
#define ANT_CALL_USER_FUNC_NAME(name, prefix)   zv_call_##prefix##_##name, zv_call_##prefix##_##name##_ret
#define ANT_CALL_USER_FUNC_NAME_D(name, prefix) zval_ptr_dtor(&zv_call_##prefix##_##name);zval_ptr_dtor(&zv_call_##prefix##_##name##_ret);
#define ANT_CALL_USER_FUNC_RET(name, prefix)    zv_call_##prefix##_##name##_ret

#define ANT_MAKE_ZVAL_STRINGL(zv, v) MAKE_STD_ZVAL(zv);ZVAL_STRINGL(zv, STRS(v), strlen(STRS(v)), 1);
#define ANT_MAKE_ZVAL_STRINGL_D(zv, v, dup) MAKE_STD_ZVAL(zv);ZVAL_STRINGL(zv, v, strlen(v), dup);
#define ANT_MAKE_ZVAL_ZVAL(zv, v) MAKE_STD_ZVAL(zv);ZVAL_ZVAL(zv, v, 1, 1);
#define ANT_MAKE_ZVAL_OBJCE_P(zv, v) MAKE_STD_ZVAL(zv);Z_OBJCE_P(zv, v, 1, 1);
#define ANT_MAKE_ZVAL_OBJCE_PP(zv, v) MAKE_STD_ZVAL(zv);Z_OBJCE_PP(zv, v, 1, 1);
#define ANT_MAKE_ZVAL_OBJ_INIT(zv, class_ptr) MAKE_STD_ZVAL(zv);object_init_ex(zv, class_ptr);
#define ANT_MAKE_ZVAL_ARRAY(zv) MAKE_STD_ZVAL(zv);array_init(zv);
#define ANT_MAKE_ZVAL_BOOL(zv, b) MAKE_STD_ZVAL(zv);ZVAL_BOOL(zv, b);
#define ANT_MAKE_ZVAL_LONG(zv, v) MAKE_STD_ZVAL(zv);ZVAL_LONG(zv, v);
#define ANT_MAKE_ZVAL_NULL(zv) MAKE_STD_ZVAL(zv);ZVAL_NULL(zv);

#define ANT_STARTUP_FUNCTION(module)   	ZEND_MINIT_FUNCTION(ant_##module)
#define ANT_STARTUP(module)	 		  	ZEND_MODULE_STARTUP_N(ant_##module)(INIT_FUNC_ARGS_PASSTHRU)
#define ANT_SHUTDOWN_FUNCTION(module)   	ZEND_MSHUTDOWN_FUNCTION(ant_##module)
#define ANT_SHUTDOWN(module)	 		  	ZEND_MODULE_SHUTDOWN_N(ant_##module)(SHUTDOWN_FUNC_ARGS_PASSTHRU)
#define ANT_ACTIVATE_FUNCTION(module)   	ZEND_MODULE_ACTIVATE_D(ant_##module)
#define ANT_ACTIVATE(module)	 		  	ZEND_MODULE_ACTIVATE_N(ant_##module)(INIT_FUNC_ARGS_PASSTHRU)
#define ANT_DEACTIVATE_FUNCTION(module)  ZEND_MODULE_DEACTIVATE_D(ant_##module)
#define ANT_DEACTIVATE(module)           ZEND_MODULE_DEACTIVATE_N(ant_##module)(SHUTDOWN_FUNC_ARGS_PASSTHRU)

extern zend_module_entry ant_module_entry;
#define phpext_ant_ptr &ant_module_entry

ZEND_BEGIN_MODULE_GLOBALS(ant)
    zval    *response_headers;
    char    *response_content;
    char    *request_module_path;
    char    *module;
    char    *controller;
    char    *action;
    char    *show_message;
    zval    *view_vars;
    int     auto_render;
    int     route_style;
ZEND_END_MODULE_GLOBALS(ant)
ZEND_EXTERN_MODULE_GLOBALS(ant);

PHP_MINIT_FUNCTION(ant);
PHP_MSHUTDOWN_FUNCTION(ant);
PHP_RINIT_FUNCTION(ant);
PHP_RSHUTDOWN_FUNCTION(ant);
PHP_MINFO_FUNCTION(ant);

#ifdef ZTS
#define ANT_G(v) TSRMG(ant_globals_id, zend_ant_globals *, v)
#else
#define ANT_G(v) (ant_globals.v)
#endif

/**
 * define functions
 */
ZEND_FUNCTION(ant_pascal);
ZEND_FUNCTION(ant_camel);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
