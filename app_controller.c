#include "php_ant.h"
#include <main/SAPI.h>
#include "app_controller.h"
//#include "ant_mmap.h"
#include "cores/core.h"
#include "cores/controller.h"
#include "authorize/authorize.h"

zend_class_entry *ant_app_controller_class_ptr;

ZEND_BEGIN_ARG_INFO_EX(arginfo_app_controller_construct, 0, 0, 2)
    ZEND_ARG_INFO(0, application)
    ZEND_ARG_INFO(0, view)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_app_controller_register_privilige, 0, 0, 2)
    ZEND_ARG_INFO(0, userId)
    ZEND_ARG_INFO(0, priviliges)
ZEND_END_ARG_INFO()

ZEND_METHOD(ant_app_controller_class, __construct) {
    zval *appObj, *viewObj;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "oo", &appObj, &viewObj)) {
        WRONG_PARAM_COUNT
    }

    zval *open = read_auth_config("open" TSRMLS_CC);
    if(Z_LVAL_P(open) == 1) {
        // search the ignore list
        if(search_ignore_lists(TSRMLS_C)) {
            goto authEnd;
        }

        /*{{{ get user's session*/
        zval *param[1], *zvKey;
        ANT_MAKE_ZVAL_STRINGL(zvKey, current_user)
        ANT_CALL_USER_FUNC_NAME_C(getSession, 0)
        param[0] = zvKey;
        call_user_function(NULL, &appObj, ANT_CALL_USER_FUNC_NAME(getSession, 0), 1, param TSRMLS_CC);
        /*}}}*/
        if(Z_TYPE_P(ANT_CALL_USER_FUNC_RET(getSession, 0)) != IS_NULL) {
            ANT_CALL_USER_FUNC_NAME_D(getSession, 0)
            goto authEnd;
        }
        else {
            zval *failUrl = read_auth_config("fail_redirect" TSRMLS_CC);
            sapi_header_line shl = {0};
            shl.response_code = 0;
            shl.line_len = (uint)spprintf(&(shl.line), 0, "%s %s", "Location:", Z_STRVAL_P(failUrl));
            if (sapi_header_op(SAPI_HEADER_REPLACE, &shl TSRMLS_CC) == SUCCESS) {
                efree(shl.line);
            }

            efree(shl.line);
        }

        /*uint64_t id = 0;
        zval *param[1], *zvKey, **pData, *currentUserId;
        ANT_MAKE_ZVAL_STRINGL(zvKey, current_user)
        ANT_CALL_USER_FUNC_NAME_C(getSession, 0)
        param[0] = zvKey;
        call_user_function(NULL, &appObj, ANT_CALL_USER_FUNC_NAME(getSession, 0), 1, param TSRMLS_CC);

        if(Z_TYPE_P(ANT_CALL_USER_FUNC_RET(getSession, 0)) != IS_NULL) {
            currentUserId = read_auth_config("current_user_id" TSRMLS_CC);
            char *name = Z_STRVAL_P(currentUserId);
            zend_hash_find(Z_ARRVAL_P(zv_call_0_getSession_ret), PROPERTY_STRINGAL(name), (void **)&pData);
            if (Z_TYPE_PP(pData) != IS_NULL) {
                id = (uint64_t)atoi(Z_STRVAL_PP(pData));
            }

            if (!id) {
                zend_error(E_ERROR, "get user's id is null when authentication in Ant\\AppController class");
            }

            AuthList *authList = shm_read(id, 2048);
            if (!authentication(authList TSRMLS_CC)) {
                zval *failUrl = read_auth_config("fail_redirect" TSRMLS_CC);
                sapi_header_line shl = {0};
                shl.response_code = 0;
                shl.line_len = (uint) spprintf(&(shl.line), 0, "%s %s", "Location:", Z_STRVAL_P(failUrl));
                if (sapi_header_op(SAPI_HEADER_REPLACE, &shl TSRMLS_CC) == SUCCESS) {
                    efree(shl.line);
                }

                efree(shl.line);
            }
        }*/
    }

authEnd:
    zend_update_property(ant_app_controller_class_ptr, getThis(), PROPERTY_STRINGL(ANT_CONTROLLER_PROPERTY_APP_OBJ), appObj TSRMLS_CC);
    zend_update_property(ant_app_controller_class_ptr, getThis(), PROPERTY_STRINGL(ANT_CONTROLLER_PROPERTY_VIEW), viewObj TSRMLS_CC);
}

ZEND_METHOD(ant_app_controller_class, registerPriviliges) {
    zval *priviliges;
    uint64_t id;
    zend_bool success = 0;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "la", &id, &priviliges) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    //success = shm_create(id, priviliges, 2048 TSRMLS_CC);
    if(success) {
        RETURN_TRUE
    }

    RETURN_FALSE
}

zend_function_entry ant_app_controller_methods[] = {
    ZEND_ME(ant_app_controller_class, __construct, arginfo_app_controller_construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    ZEND_ME(ant_app_controller_class, registerPriviliges, arginfo_app_controller_register_privilige, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

ANT_STARTUP_FUNCTION(app_controller) {
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "Ant\\AppController", ant_app_controller_methods);
    ant_app_controller_class_ptr = zend_register_internal_class_ex(&ce, ant_controller_class_ptr, "AntController" TSRMLS_CC);

    return SUCCESS;
}

ANT_SHUTDOWN_FUNCTION(app_controller) {
    return SUCCESS;
}