#include "php_ant.h"
#include <main/SAPI.h>
#include "Zend/zend_interfaces.h"
#include "ant_application.h"
#include "cores/core.h"
#include "cores/controller.h"
#include "cores/view.h"
#include "session/php_session.h"

zend_class_entry *ant_controller_class_ptr;

ZEND_BEGIN_ARG_INFO_EX(arginfo_controller_query, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_controller_form, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_controller_render, 0, 0, 3)
    ZEND_ARG_INFO(0, file_name)
    ZEND_ARG_INFO(0, path)
    ZEND_ARG_INFO(0, error_page)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_controller_message, 0, 0, 2)
    ZEND_ARG_INFO(0, message)
    ZEND_ARG_INFO(0, tag)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_controller_redirect, 0, 0, 2)
    ZEND_ARG_INFO(0, urls)
    ZEND_ARG_INFO(0, params)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_controller_assign, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_controller_paginate, 0, 0, 1)
    ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

void _render(char *path, zval *ret TSRMLS_DC) {
    zval *view,  *param;

    ANT_MAKE_ZVAL_OBJ_INIT(view, ant_view_class_ptr)
    ANT_MAKE_ZVAL_STRINGL_D(param, path, 1)

    zend_call_method_with_1_params(&view, ant_view_class_ptr, NULL, "render", &ret, param);
    zval_ptr_dtor(&view);
    zval_ptr_dtor(&param);
}

zval *_get_app_obj(zval *this TSRMLS_DC) {
    zval *instance = zend_read_property(ant_controller_class_ptr, this, PROPERTY_STRINGL(ANT_CONTROLLER_PROPERTY_APP_OBJ), 1 TSRMLS_CC);
    return instance;
}

zval *_get_view_obj(zval *this TSRMLS_DC) {
    zval *instance = zend_read_property(ant_controller_class_ptr, this, PROPERTY_STRINGL(ANT_CONTROLLER_PROPERTY_VIEW), 1 TSRMLS_CC);
    return instance;
}

ZEND_METHOD(ant_controller_class, getUserInfo) {
    zval *appObj = _get_app_obj(getThis() TSRMLS_CC);
    if(Z_TYPE_P(appObj) == IS_OBJECT) {
        zval *param[1], *paramValue;
        ANT_CALL_USER_FUNC_NAME_C(getSession, 0)
        ANT_MAKE_ZVAL_STRINGL(paramValue, current_user)
        param[0] = paramValue;
        call_user_function(NULL, &appObj, zv_call_0_getSession, return_value, 1, param TSRMLS_CC);

        zval_ptr_dtor(&paramValue);
        ANT_CALL_USER_FUNC_NAME_D(getSession, 0)
        return;
    }

    RETURN_NULL()
}

ZEND_METHOD(ant_controller_class, render) {
    char *name = NULL, *path = ".", errorPage = NULL, *fileName;
    int nameLen, pathLen, errorPageLen;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sss", &name, &nameLen, &path, &pathLen, &errorPage, &errorPageLen) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    if(name == NULL) name = ANT_G(action);
    if(strncmp(path, ".", 1) == 0) {
        spprintf(&fileName, 0, "%s/views/%s/%s", ANT_G(module), ANT_G(controller), name);
        path = find_real_path(fileName, "path_apps" TSRMLS_CC);
    }
    else {
        spprintf(&fileName, 0, "%s%s.inc", path, name);
        if(VCWD_ACCESS(fileName, 0) == FAILURE) {
            zend_error(E_ERROR, "the specify loading file `%s` is not exists.", fileName);
        }

        path = fileName;
    }

    // record to current request module path
    char *appsPath = read_config_path("path_apps" TSRMLS_CC);
    spprintf(&ANT_G(request_module_path), 0, "%s%s", appsPath, ANT_G(module));

    _render(path, return_value TSRMLS_CC);
    efree(path);
}

ZEND_METHOD(ant_controller_class, closeRender) {
    ANT_G(auto_render) = 0;
}

ZEND_METHOD(ant_controller_class, message) {
    char *info, *flag = "alert-success", *message;
    int infoLen, flagLen;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &info, &infoLen, &flag, &flagLen) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    spprintf(&message, 0,
             "<div class='alert %s alert-dismissible' role='alert'>"
                     "<button type='button' class='close' data-dismiss='alert' aria-label='Close'>"
                     "<span aria-hidden='true'>&times;</span></button> %s </div>",
             flag, info);

    ANT_G(show_message) = message;
}

ZEND_METHOD(ant_controller_class, smessage) {
    char *info, *flag = "alert-success", *message;
    int infoLen, flagLen;
    char *name = "notice-message";
    zval *httpSession;
    php_unserialize_data_t var_hash;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &info, &infoLen, &flag, &flagLen) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    spprintf(&message, 0,
             "<div class='alert %s alert-dismissible' role='alert'>"
                     "<button type='button' class='close' data-dismiss='alert' aria-label='Close'>"
                     "<span aria-hidden='true'>&times;</span></button> %s </div>",
             flag, info);

    httpSession = PS(http_session_vars);
    if(zend_hash_find(HASH_OF(httpSession), PROPERTY_STRINGAL(name), (void **)&return_value_ptr) != SUCCESS) {
        php_add_session_var(name, strlen(name) TSRMLS_CC);
    }

    PHP_VAR_UNSERIALIZE_INIT(var_hash);
    MAKE_STD_ZVAL(return_value);
    ZVAL_STRINGL(return_value, message, strlen(message), 1);
    php_set_session_var(name, strlen(name), return_value, &var_hash  TSRMLS_CC);

    var_push_dtor_no_addref(&var_hash, &return_value);
    efree(message);
    PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
}

ZEND_METHOD(ant_controller_class, query) {
    char *argName;
    int argLen;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &argName, &argLen) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    zval *result = application_query(argName TSRMLS_CC);
    ANT_SWITCH_RETURN_VAL_P(result)
}

ZEND_METHOD(ant_controller_class, form) {
    char *argName;
    int argLen;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &argName, &argLen) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    zval *result = application_form(argName TSRMLS_CC);
    ANT_SWITCH_RETURN_VAL_P(result)
}

ZEND_METHOD(ant_controller_class, isGet) {
    zval *app = _get_app_obj(getThis() TSRMLS_CC);
    if(Z_TYPE_P(app) == IS_OBJECT) {
        zval *method;
        ANT_MAKE_ZVAL_STRINGL(method, isGet)

        call_user_function(NULL, &app, method, return_value, 0, NULL TSRMLS_CC);
        zval_ptr_dtor(&method);

        if(Z_TYPE_P(return_value) == IS_BOOL) {
            RETURN_BOOL(Z_BVAL_P(return_value))
        }
    }

    RETURN_FALSE
}

ZEND_METHOD(ant_controller_class, isPost) {
    zval *app = _get_app_obj(getThis() TSRMLS_CC);
    if(Z_TYPE_P(app) == IS_OBJECT) {
        ANT_CALL_USER_FUNC_NAME_C(isPost, 0)
        call_user_function(NULL, &app, zv_call_0_isPost, return_value, 0, NULL TSRMLS_CC);
        ANT_CALL_USER_FUNC_NAME_D(isPost, 0)

        return;
    }

    RETURN_FALSE
}

ZEND_METHOD(ant_controller_class, module) {
    zval *app = _get_app_obj(getThis() TSRMLS_CC);
    if(Z_TYPE_P(app) == IS_OBJECT) {
        zval *method;
        ANT_MAKE_ZVAL_STRINGL(method, module)

        call_user_function(NULL, &app, method, return_value, 0, NULL TSRMLS_CC);
        zval_ptr_dtor(&method);

        if(Z_TYPE_P(return_value) == IS_STRING) {
            RETURN_STRING(Z_STRVAL_P(return_value), 1)
        }
    }

    RETURN_NULL()
}

ZEND_METHOD(ant_controller_class, controller) {
    zval *app = _get_app_obj(getThis() TSRMLS_CC);
    if(Z_TYPE_P(app) == IS_OBJECT) {
        zval *method;
        ANT_MAKE_ZVAL_STRINGL(method, controller)

        call_user_function(NULL, &app, method, return_value, 0, NULL TSRMLS_CC);
        zval_ptr_dtor(&method);

        if(Z_TYPE_P(return_value) == IS_STRING) {
            RETURN_STRING(Z_STRVAL_P(return_value), 1)
        }
    }

    RETURN_NULL()
}

ZEND_METHOD(ant_controller_class, action) {
    zval *app = _get_app_obj(getThis() TSRMLS_CC);
    if(Z_TYPE_P(app) == IS_OBJECT) {
        zval *method;
        ANT_MAKE_ZVAL_STRINGL(method, action)

        call_user_function(NULL, &app, method, return_value, 0, NULL TSRMLS_CC);
        zval_ptr_dtor(&method);

        if(Z_TYPE_P(return_value) == IS_STRING) {
            RETURN_STRING(Z_STRVAL_P(return_value), 1)
        }
    }

    RETURN_NULL()
}

ZEND_METHOD(ant_controller_class, redirect) {
    zval *routeObj, *appObj, *urls, *getParams, *params[2];
    ANT_MAKE_ZVAL_ARRAY(getParams)
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|a", &urls, &getParams) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    appObj = zend_read_property(ant_controller_class_ptr, getThis(), PROPERTY_STRINGL(ANT_CONTROLLER_PROPERTY_APP_OBJ), 1 TSRMLS_CC);
    zend_class_entry *appCE = Z_OBJCE_P(appObj);
    if(ANT_G(route_style) == 1) {
        routeObj = zend_read_property(appCE, appObj, PROPERTY_STRINGL(ANT_APPLICATION_PROPERTY_ROUTE_DEFAULT_OBJ), 1 TSRMLS_CC);
    } else {
        routeObj = zend_read_property(appCE, appObj, PROPERTY_STRINGL(ANT_APPLICATION_PROPERTY_ROUTE_PATHINFO_OBJ), 1 TSRMLS_CC);
    }
    ANT_CALL_USER_FUNC_NAME_C(generate, 0)

    params[0] = urls;
    params[1] = getParams;
    call_user_function(NULL, &routeObj, zv_call_0_generate, return_value, 2, params TSRMLS_CC);
    zval_ptr_dtor(&getParams);
    ANT_CALL_USER_FUNC_NAME_D(generate, 0)

    sapi_header_line shl = {0};
    shl.response_code = 0;
    shl.line_len = (uint)spprintf(&(shl.line), 0, "%s %s", "Location:", Z_STRVAL_P(return_value));
    if (sapi_header_op(SAPI_HEADER_REPLACE, &shl TSRMLS_CC) == SUCCESS) {
        efree(shl.line);
        RETURN_TRUE
    }

    efree(shl.line);
    RETURN_FALSE
}

ZEND_METHOD(ant_controller_class, setResponeHeader) {
    char *key;
    zval *value;
    int keyLen;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &key, &keyLen, &value) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    zval *app = _get_app_obj(getThis() TSRMLS_CC);
    if(Z_TYPE_P(app) == IS_OBJECT) {
        int flag = application_set_header(app, key, value TSRMLS_CC);
        if(flag) {
            RETURN_TRUE
        }
    }

    RETURN_FALSE
}

ZEND_METHOD(ant_controller_class, assign) {
    char *name;
    zval *value;
    int nameLen;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &nameLen, &value)) {
        WRONG_PARAM_COUNT
    }

    view_assign(name, value TSRMLS_CC);
}

ZEND_METHOD(ant_controller_class, paginate) {
    zend_class_entry *ant_view_ce = NULL;
    int number;
    char *ret, *start;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &number)) {
        WRONG_PARAM_COUNT
    }

    zval *app = zend_read_property(ant_controller_class_ptr, getThis(), PROPERTY_STRINGL(ANT_CONTROLLER_PROPERTY_APP_OBJ), 1 TSRMLS_CC);
    zval *view = _get_view_obj(getThis() TSRMLS_CC);
    zval *db = get_db(app TSRMLS_CC);
    ant_view_ce = Z_OBJCE_P(view);
    zend_update_static_property(ant_view_ce, PROPERTY_STRINGL(ANT_VIEW_PROPERTY_DB), db TSRMLS_CC);

    zval *currPage = application_query("page" TSRMLS_CC);
    if(Z_TYPE_P(currPage) == IS_NULL) {
        spprintf(&ret, 0, "%c, %d", '0', number);
    } else {
        start = Z_STRVAL_P(currPage);
        spprintf(&ret, 0, "%d, %d", atoi(start)*number, number);
    }

    ZVAL_STRINGL(return_value, ret, strlen(ret), 1);
    efree(ret);
    return;
}

zend_function_entry ant_controller_methods[] = {
    ZEND_ME(ant_controller_class, getUserInfo, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_controller_class, render, arginfo_controller_render, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_controller_class, closeRender, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_controller_class, message, arginfo_controller_message, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_controller_class, smessage, arginfo_controller_message, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_controller_class, query, arginfo_controller_query, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_controller_class, form, arginfo_controller_form, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_controller_class, isGet, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_controller_class, isPost, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_controller_class, module, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_controller_class, controller, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_controller_class, action, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_controller_class, setResponeHeader, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_controller_class, redirect, arginfo_controller_redirect, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_controller_class, assign, arginfo_controller_assign, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_controller_class, paginate, arginfo_controller_paginate, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

ANT_STARTUP_FUNCTION(controller) {
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "AntController", ant_controller_methods);
    ant_controller_class_ptr = zend_register_internal_class(&ce TSRMLS_CC);
    ant_controller_class_ptr->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

    zend_declare_property_null(ant_controller_class_ptr, PROPERTY_STRINGL(ANT_CONTROLLER_PROPERTY_APP_OBJ), ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(ant_controller_class_ptr, PROPERTY_STRINGL(ANT_CONTROLLER_PROPERTY_VIEW), ZEND_ACC_PROTECTED TSRMLS_CC);

    return SUCCESS;
}

ANT_SHUTDOWN_FUNCTION(controller) {
    return SUCCESS;
}