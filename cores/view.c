#include "php_ant.h"
#include "cores/view.h"
#include "cores/core.h"
#include "cores/route.h"
#include "session/php_session.h"

zend_class_entry *ant_view_class_ptr;

void view_assign(char *key, zval *value TSRMLS_DC) {
    zval *viewVars = ANT_G(view_vars);

    if(viewVars && Z_TYPE_P(viewVars) == IS_ARRAY) {
        Z_ADDREF_P(value);
        zend_hash_update(Z_ARRVAL_P(viewVars), key, strlen(key)+1, &value, sizeof(zval *), NULL);
        ANT_G(view_vars) = viewVars;
    }
    else {
        zval *vars;
        ANT_MAKE_ZVAL_ARRAY(vars)

        Z_ADDREF_P(value);
        zend_hash_update(Z_ARRVAL_P(vars), key, strlen(key)+1, &value, sizeof(zval *), NULL);
        ANT_G(view_vars) = vars;
    }
}

void _view_render(char *path, zval *ret TSRMLS_DC) {
    HashTable *currCallSymbolTable;
    ZVAL_NULL(ret);

    if(EG(active_symbol_table)) {
        currCallSymbolTable = EG(active_symbol_table);
    } else {
        currCallSymbolTable = NULL;
    }

    ALLOC_HASHTABLE(EG(active_symbol_table));
    zend_hash_init(EG(active_symbol_table), 0, NULL, ZVAL_PTR_DTOR, 0);

    if (php_output_start_user(NULL, 0, PHP_OUTPUT_HANDLER_STDFLAGS TSRMLS_CC) == FAILURE) {
        zend_error(E_ERROR, "failed to create buffer");
    }

    if (load_file(path TSRMLS_CC) == 0) {
        php_output_end(TSRMLS_C);

        if (currCallSymbolTable) {
            zend_hash_destroy(EG(active_symbol_table));
            FREE_HASHTABLE(EG(active_symbol_table));
            EG(active_symbol_table) = currCallSymbolTable;
        }

        zend_error(E_ERROR, "render view failed.");
    }

    if (currCallSymbolTable) {
        zend_hash_destroy(EG(active_symbol_table));
        FREE_HASHTABLE(EG(active_symbol_table));
        EG(active_symbol_table) = currCallSymbolTable;
    }

    if (php_output_get_contents(ret TSRMLS_CC) == FAILURE) {
        php_output_end(TSRMLS_C);
        zend_error(E_ERROR, "loading file failed.");
    }

    if (php_output_discard(TSRMLS_C) != SUCCESS) {
        return;
    }

    if(Z_TYPE_P(ret) == IS_STRING) {
        char *buf = Z_STRVAL_P(ret);
        php_write(buf, strlen(buf) TSRMLS_CC);
    }
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_view_render, 0, 0, 1)
    ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_view_vars, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_view_inflate_file, 0, 0, 2)
    ZEND_ARG_INFO(0, fileName)
    ZEND_ARG_INFO(0, module)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_view_url, 0, 0, 4)
    ZEND_ARG_INFO(0, module)
    ZEND_ARG_INFO(0, controller)
    ZEND_ARG_INFO(0, action)
    ZEND_ARG_INFO(0, params)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_view_pages, 0, 0, 5)
    ZEND_ARG_INFO(0, showNumber)
    ZEND_ARG_INFO(0, currPage)
    ZEND_ARG_INFO(0, count)
    ZEND_ARG_INFO(0, params)
    ZEND_ARG_INFO(0, class)
ZEND_END_ARG_INFO()

ZEND_METHOD(ant_view_class, render) {
    char *path;
    int pathLen;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &pathLen) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    _view_render(path, return_value TSRMLS_CC);
}

ZEND_METHOD(ant_view_class, viewVars) {
    char *name;
    int nameLen;
    zval *vars = ANT_G(view_vars);

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLen) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    if(Z_TYPE_P(vars) == IS_ARRAY && (zend_hash_exists(HASH_OF(vars), PROPERTY_STRINGL(name)) == SUCCESS)) {
        zval **pData;
        ulong hashval = zend_get_hash_value(name, strlen(name)+1);
        zend_hash_quick_find(HASH_OF(vars), PROPERTY_STRINGAL(name), hashval, (void **)&pData);

        RETURN_ZVAL(*pData, 1, 0)
    }

    RETURN_NULL()
}

ZEND_METHOD(ant_view_class, inflateFile) {
    char *fileName, *path = ".", *realPath;
    int fileNameLen, pathLen;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &fileName, &fileNameLen, &path, &pathLen) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    if(strncmp(path, ".", 1) == 0) {
        if(strstr(fileName, "/") == NULL) {
            spprintf(&realPath, 0, "%s/views/%s/%s.inc", ANT_G(request_module_path), ANT_G(controller), fileName);
        } else {
            spprintf(&realPath, 0, "%s/views/%s.inc", ANT_G(request_module_path), fileName);
        }
    } else {
        spprintf(&realPath, 0, "%s/%s.inc", path, fileName);
    }

    if(VCWD_ACCESS(realPath, 0) == FAILURE) {
        zend_error(E_ERROR, "the specify loading `%s` file is not exists.", fileName);
        efree(realPath);
        return;
    }

    load_file(realPath TSRMLS_CC);
}

ZEND_METHOD(ant_view_class, url) {
    char *module = "", *controller = "", *action = "", *retUrl;
    int moduleLen, controllerLen, actionLen;
    zval *getParams;
    ANT_MAKE_ZVAL_ARRAY(getParams)
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss|a", &module, &moduleLen, &controller, &controllerLen, &action, &actionLen, &getParams)) {
        WRONG_PARAM_COUNT
    }

    if(strncmp(module, "", 1) == 0) module = ANT_G(module);
    if(strncmp(controller, "", 1) == 0) controller = ANT_G(controller);
    if(strncmp(action, "", 1) == 0) action = ANT_G(action);
    if(ANT_G(route_style) == 1) {
        retUrl = route_default_generate(module, controller, action, getParams TSRMLS_CC);
    } else {
        retUrl = route_pathinfo_generate(module, controller, action, getParams TSRMLS_CC);
    }

    ZVAL_STRINGL(return_value, retUrl, strlen(retUrl), 1);
    zval_ptr_dtor(&getParams);
    efree(retUrl);
    return;
}

ZEND_METHOD(ant_view_class, showMessage) {
    if(ANT_G(show_message)) {
        char *message = ANT_G(show_message);
        RETURN_STRINGL(message, strlen(message), 1);
    }

    RETURN_NULL()
}

ZEND_METHOD(ant_view_class, sshowMessage) {
    char *name = "notice-message", *notice;
    php_unserialize_data_t var_hash;
    zval *ret_default;

    php_get_session_var(name, strlen(name), &return_value_ptr TSRMLS_CC);
    if(Z_TYPE_PP(return_value_ptr) == NULL) {
        RETURN_NULL()
    }

    notice = Z_STRVAL_PP(return_value_ptr);
    ZVAL_STRINGL(return_value, notice, strlen(notice), 1);

    PHP_VAR_UNSERIALIZE_INIT(var_hash);
    MAKE_STD_ZVAL(ret_default);
    ZVAL_NULL(ret_default);
    php_set_session_var(name, strlen(name), ret_default, &var_hash  TSRMLS_CC);
    PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
}

ZEND_METHOD(ant_view_class, pages) {
    zval *extParams;
    char *class, *url;
    int showNumber, currPage, count, classLen, startPage = 1;
    double showPageNumber = 0;
    ANT_MAKE_ZVAL_ARRAY(extParams)
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll|as", &showNumber, &currPage, &count, &extParams, &class, &classLen) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    char *module    = ANT_G(module);
    char *controller = ANT_G(controller);
    char *action    = ANT_G(action);
    if(ANT_G(route_style) == 1) {
        url = route_default_generate(module, controller, action, extParams TSRMLS_CC);
    } else {
        url = route_pathinfo_generate(module, controller, action, extParams TSRMLS_CC);
    }

    // called database's getRowsNum method
    /*ANT_CALL_USER_FUNC_NAME_C(getRowsNum, 0)
    zval *db = zend_read_static_property(ant_view_class_ptr, PROPERTY_STRINGL(ANT_VIEW_PROPERTY_DB), 1 TSRMLS_CC);
    if(Z_TYPE_P(db) != IS_OBJECT) {
        zend_error(E_ERROR, "%s", "the _db property which is not object in view file");
    }
    call_user_function(NULL, &db, ANT_CALL_USER_FUNC_NAME(getRowsNum, 0), 0, NULL TSRMLS_CC);
    ANT_CALL_USER_FUNC_NAME_D(getRowsNum, 0)
*/
    showPageNumber = ceil(count/showNumber);
    if(showPageNumber == 0) {
        showPageNumber = 1;
    } else if(showPageNumber > 20) {
        showPageNumber = 20;
    }

    char *tempStr = NULL;
    char html[2400] = {0};
    strncpy(html, "<ul class='pagination'>", strlen("<ul class='pagination'>"));
    for(; startPage <= showPageNumber; ++startPage) {
        if(startPage == currPage) {
            spprintf(&tempStr, 0, "<li class='active'><span>%d</span></li>", startPage);
            strncat(html, tempStr, strlen(tempStr));
        }
        else {
            spprintf(&tempStr, 0, "<li><a href='%s&page=%d'>%d</a></li>", url, startPage, startPage);
            strncat(html, tempStr, strlen(tempStr));
        }

        tempStr = NULL;
    }
    strncat(html, "</ul>", strlen("</ul>"));
    spprintf(&url, 0, html, class);

    ZVAL_STRINGL(return_value, url, strlen(url), 1);
    zval_ptr_dtor(&extParams);
    efree(url);
    efree(tempStr);
    return;
}

zend_function_entry ant_view_methods[] = {
    ZEND_ME(ant_view_class, render, arginfo_view_render, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_view_class, viewVars, arginfo_view_vars, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_view_class, inflateFile, arginfo_view_inflate_file, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_view_class, showMessage, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_view_class, sshowMessage, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_view_class, url, arginfo_view_url, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_view_class, pages, arginfo_view_pages, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

ANT_STARTUP_FUNCTION(view) {
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "AntView", ant_view_methods);
    ant_view_class_ptr = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(ant_view_class_ptr, PROPERTY_STRINGL(ANT_VIEW_PROPERTY_DB), ZEND_ACC_PUBLIC|ZEND_ACC_STATIC TSRMLS_CC);

    return SUCCESS;
}

