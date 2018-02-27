#include "php_ant.h"
#include "database.h"

zend_class_entry *ant_database_class_ptr;

static zval *pools[4];

void _create_pools(zval *conn TSRMLS_DC) {
    for(int i=0; i < 4; i++) {
        pools[i] = (zval *)emalloc(sizeof(zval));
        pools[i] = conn;
    }
}

int _add_conn_to_pools(zval *conn TSRMLS_DC) {
    for(int i=0; i < 4; i++) {
        if(!pools[i]) {
            pools[i] = conn;
            return 1;
        }
    }

    return 0;
}

zval *_get_conn_from_pools(TSRMLS_D) {
    for(int i=0; i < 4; i++) {
        if(pools[i]) {
            zval *conn = pools[i];

            return conn;
        }
    }

    return NULL;
}

void database_init(char *host, char *username, char *password, char *dbname, char *charset, zval *this, zval *ret TSRMLS_DC) {
    zval *connectParams[4], *setCharsetParams[2];
    zval *zvHost, *zvUser, *zvPasswd, *zvDb, *zvCharset;
    ANT_CALL_USER_FUNC_NAME_C(mysqli_connect, db)
    ANT_CALL_USER_FUNC_NAME_C(mysqli_set_charset, db)
    ANT_MAKE_ZVAL_STRINGL_D(zvHost, host, 1)
    ANT_MAKE_ZVAL_STRINGL_D(zvUser, username, 1)
    ANT_MAKE_ZVAL_STRINGL_D(zvPasswd, password, 1)
    ANT_MAKE_ZVAL_STRINGL_D(zvDb, dbname, 1)
    ANT_MAKE_ZVAL_STRINGL_D(zvCharset, charset, 1)

    // call methods
    connectParams[0] = zvHost; connectParams[1] = zvUser; connectParams[2] = zvPasswd; connectParams[3] = zvDb;
    call_user_function(EG(function_table), NULL, zv_call_db_mysqli_connect, ret, 4, connectParams TSRMLS_CC);
    setCharsetParams[0] = ret; setCharsetParams[1] = zvCharset;
    call_user_function(EG(function_table), NULL, ANT_CALL_USER_FUNC_NAME(mysqli_set_charset, db), 2, setCharsetParams TSRMLS_CC);
    // create to link pool
    //_create_pools(ret TSRMLS_CC);
    zend_update_property(ant_database_class_ptr, this, PROPERTY_STRINGL(ANT_DATABASE_PROPERTY_CONN_ID), ret TSRMLS_CC);

    ANT_CALL_USER_FUNC_NAME_D(mysqli_connect, db)
    ANT_CALL_USER_FUNC_NAME_D(mysqli_set_charset, db)
    zval_ptr_dtor(&zvHost); zval_ptr_dtor(&zvUser); zval_ptr_dtor(&zvPasswd); zval_ptr_dtor(&zvDb);
    zval_ptr_dtor(&zvCharset);
}

void query(char *sql, zval *this TSRMLS_DC) {
    zval *params[2], *zvSql;
    ANT_CALL_USER_FUNC_NAME_C(mysqli_query, db)
    ANT_MAKE_ZVAL_STRINGL_D(zvSql, sql, 1)
    zval *connID = zend_read_property(ant_database_class_ptr, this, PROPERTY_STRINGL(ANT_DATABASE_PROPERTY_CONN_ID), 1 TSRMLS_CC);
    params[0] = connID;
    params[1] = zvSql;

    call_user_function(EG(function_table), NULL, ANT_CALL_USER_FUNC_NAME(mysqli_query, db), 2, params TSRMLS_CC);
    zend_update_property(ant_database_class_ptr, this, PROPERTY_STRINGL(ANT_DATABASE_PROPERTY_QUERY_ID), ANT_CALL_USER_FUNC_RET(mysqli_query, db) TSRMLS_CC);

    ANT_CALL_USER_FUNC_NAME_D(mysqli_query, db)
    zval_ptr_dtor(&zvSql);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_database___construct, 0, 0, 5)
    ZEND_ARG_INFO(0, host)
    ZEND_ARG_INFO(0, user)
    ZEND_ARG_INFO(0, passwd)
    ZEND_ARG_INFO(0, dbname)
    ZEND_ARG_INFO(0, charset)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_database_query, 0, 0, 1)
    ZEND_ARG_INFO(0, sql)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_database_insert, 0, 0, 2)
    ZEND_ARG_INFO(0, table_name)
    ZEND_ARG_INFO(0, datas)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_database_update, 0, 0, 3)
    ZEND_ARG_INFO(0, table_name)
    ZEND_ARG_INFO(0, datas)
    ZEND_ARG_INFO(0, where)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_database_get_rows, 0, 0, 1)
    ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_database_get_field, 0, 0, 1)
    ZEND_ARG_INFO(0, field_name)
ZEND_END_ARG_INFO()

/* {{{ initialize the database link handler */
ZEND_METHOD(ant_database_class, __construct) {
    char *host, *user, *passwd, *dbname, *charset = "utf8";
    int hostLen, userLen, passwdLen, dbnameLen, charsetLen;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssss|s", &host, &hostLen, &user, &userLen, &passwd, &passwdLen, &dbname, &dbnameLen, &charset, &charsetLen) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    zval *ret;
    ANT_MAKE_ZVAL_NULL(ret)
    database_init(host, user, passwd, dbname, charset, getThis(), ret TSRMLS_CC);

    zval_ptr_dtor(&ret);
    RETURN_ZVAL(getThis(), 1, 1)
}
/* }}} */

ZEND_METHOD(ant_database_class, query) {
    char *sql;
    int sqlLen;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &sql, &sqlLen)) {
        WRONG_PARAM_COUNT
    }

    zval *params[2], *zvSql;
    ANT_CALL_USER_FUNC_NAME_C(mysqli_query, db)
    ANT_MAKE_ZVAL_STRINGL_D(zvSql, sql, 1)
    zval *connID = zend_read_property(ant_database_class_ptr, getThis(), PROPERTY_STRINGL(ANT_DATABASE_PROPERTY_CONN_ID), 1 TSRMLS_CC);
    //params[0] = _get_conn_from_pools(TSRMLS_C);
    params[0] = connID;
    params[1] = zvSql;

    call_user_function(EG(function_table), NULL, ANT_CALL_USER_FUNC_NAME(mysqli_query, db), 2, params TSRMLS_CC);
    if(!Z_BVAL_P(zv_call_db_mysqli_query_ret)) {
        zend_error(E_ERROR, "database query result is null, please checking the `%s` whether is error.", sql);
    }
    zend_update_property(ant_database_class_ptr, getThis(), PROPERTY_STRINGL(ANT_DATABASE_PROPERTY_QUERY_ID), ANT_CALL_USER_FUNC_RET(mysqli_query, db) TSRMLS_CC);

    ANT_CALL_USER_FUNC_NAME_D(mysqli_query, db)
    zval_ptr_dtor(&zvSql);
    RETURN_ZVAL(getThis(), 1, 0)
}

ZEND_METHOD(ant_database_class, insert) {
    zval *datas, **pData;
    char *tableName;
    int datasNum, tableNameLen;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa", &tableName, &tableNameLen, &datas)) {
        WRONG_PARAM_COUNT
    }

    datasNum = zend_hash_num_elements(Z_ARRVAL_P(datas));
    if(datasNum > 0) {
        zend_hash_internal_pointer_reset(Z_ARRVAL_P(datas));
        char *tempFields = NULL, *tempValues = NULL, fields[200] = {0}, values[2048] = {0}, *strKey;

        for(int i=0; i < datasNum; i++) {
            ulong numindex;
            zend_hash_get_current_key(Z_ARRVAL_P(datas), &strKey, &numindex, 0);
            zend_hash_get_current_data(Z_ARRVAL_P(datas), (void **)&pData);

            ANT_SWITCH_VAL_PP(pData)
            spprintf(&tempFields, 0, "%s,", strKey);

            if(_v_type == 1) {
                spprintf(&tempValues, 0, "\'%s\',", _v_str);
                _v_str = NULL;
            }
            else if(_v_type == 2) {
                spprintf(&tempValues, 0, "%f,", _v_double);
                _v_double = 0;
            }
            else if(_v_type == 3) {
                spprintf(&tempValues, 0, "%ld,", _v_long);
                _v_long = 0;
            }
            else if(_v_type == 4) {
                spprintf(&tempValues, 0, "%d,", _v_bool);
                _v_bool = 0;
            }

            if(!fields && !values) {
                strncpy(fields, tempFields, strlen(tempFields));
                strncpy(values, tempValues, strlen(tempValues));
            } else {
                strncat(fields, tempFields, strlen(tempFields));
                strncat(values, tempValues, strlen(tempValues));
            }

            efree(tempFields); efree(tempValues);
            tempFields = NULL; tempValues = NULL; strKey = NULL;
            if(zend_hash_move_forward(Z_ARRVAL_P(datas)) == FAILURE) {
                break;
            }
        }

        char *sql, *pFields, *pValues;
        pFields = estrndup(fields, strlen(fields)-1);
        pValues = estrndup(values, strlen(values)-1);
        spprintf(&sql, 0, "insert into %s(%s) values (%s)", tableName, pFields, pValues);
        query(sql, getThis() TSRMLS_CC);

        efree(pFields); efree(pValues); efree(sql);
        RETURN_ZVAL(getThis(), 1, 1)
    }

    RETURN_FALSE
}

ZEND_METHOD(ant_database_class, update) {
    char *tableName, *where;
    int tableNameLen, whereLen, datasNum;
    zval *datas, **pData;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa|s", &tableName, &tableNameLen, &datas, &where, &whereLen) == FAILURE) {
        WRONG_PARAM_COUNT
    }

    datasNum = zend_hash_num_elements(Z_ARRVAL_P(datas));
    if(datasNum > 0) {
        zend_hash_internal_pointer_reset(Z_ARRVAL_P(datas));
        char *tempValues = NULL, values[2048], *strKey;

        for(int i=0; i < datasNum; i++) {
            ulong numindex;
            zend_hash_get_current_key(Z_ARRVAL_P(datas), &strKey, &numindex, 0);
            zend_hash_get_current_data(Z_ARRVAL_P(datas), (void **)&pData);
            ANT_SWITCH_VAL_PP(pData)

            if(_v_type == 1) {
                spprintf(&tempValues, 0, "%s=\'%s\',", strKey, _v_str);
                _v_str = NULL;
            }
            else if(_v_type == 2) {
                spprintf(&tempValues, 0, "%s=%f,", strKey, _v_double);
                _v_double = 0;
            }
            else if(_v_type == 3) {
                spprintf(&tempValues, 0, "%s=%ld,", strKey, _v_long);
                _v_long = 0;
            }
            else if(_v_type == 4) {
                spprintf(&tempValues, 0, "%s=%d,", strKey, _v_bool);
                _v_bool = 0;
            }

            if(!values) {
                strncpy(values, tempValues, strlen(tempValues));
            } else {
                strncat(values, tempValues, strlen(tempValues));
            }

            efree(tempValues);
            tempValues = NULL; strKey = NULL;
            if(zend_hash_move_forward(Z_ARRVAL_P(datas)) == FAILURE) {
                break;
            }
        }

        char *sql, *pDatas;
        pDatas = estrndup(values, strlen(values)-1);
        spprintf(&sql, 0, "update %s set %s where %s", tableName, pDatas, where);
        query(sql, getThis() TSRMLS_CC);

        efree(pDatas); efree(sql);
        RETURN_TRUE
    }

    RETURN_FALSE
}

ZEND_METHOD(ant_database_class, getRow) {
    int type = 1;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &type)) {
        WRONG_PARAM_COUNT
    }

    zval *queryID = zend_read_property(ant_database_class_ptr, getThis(), PROPERTY_STRINGL(ANT_DATABASE_PROPERTY_QUERY_ID), 1 TSRMLS_CC);
    if(Z_TYPE_P(queryID) == IS_OBJECT) {
        zval *params[2], *zvType;
        ANT_CALL_USER_FUNC_NAME_C(mysqli_fetch_array, db)
        ANT_MAKE_ZVAL_LONG(zvType, type)
        params[0] = queryID;
        params[1] = zvType;

        call_user_function(EG(function_table), NULL, zv_call_db_mysqli_fetch_array, return_value, 2, params TSRMLS_CC);

        ANT_CALL_USER_FUNC_NAME_D(mysqli_fetch_array, db)
        zval_ptr_dtor(&zvType);
        return;
    }

    RETURN_NULL()
}

ZEND_METHOD(ant_database_class, getRows) {
    int type = 1;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &type)) {
        WRONG_PARAM_COUNT
    }

    zval *queryID = zend_read_property(ant_database_class_ptr, getThis(), PROPERTY_STRINGL(ANT_DATABASE_PROPERTY_QUERY_ID), 1 TSRMLS_CC);
    if(Z_TYPE_P(queryID) == IS_OBJECT) {
        zval *params[2], *zvType;
        ANT_CALL_USER_FUNC_NAME_C(mysqli_fetch_all, db)
        ANT_MAKE_ZVAL_LONG(zvType, type)
        params[0] = queryID;
        params[1] = zvType;

        call_user_function(EG(function_table), NULL, zv_call_db_mysqli_fetch_all, return_value, 2, params TSRMLS_CC);

        ANT_CALL_USER_FUNC_NAME_D(mysqli_fetch_all, db)
        zval_ptr_dtor(&zvType);
        return;
    }

    RETURN_NULL()
}

ZEND_METHOD(ant_database_class, getRowsNum) {
    ANT_CALL_USER_FUNC_NAME_C(mysqli_num_rows, db)
    zval *params[1];
    zval *queryID = zend_read_property(ant_database_class_ptr, getThis(), PROPERTY_STRINGL(ANT_DATABASE_PROPERTY_QUERY_ID), 1 TSRMLS_CC);
    params[0] = queryID;

    call_user_function(EG(function_table), NULL, zv_call_db_mysqli_num_rows, return_value, 1, params TSRMLS_CC);
    ANT_CALL_USER_FUNC_NAME_D(mysqli_num_rows, db)
}

ZEND_METHOD(ant_database_class, getField) {
    char *name;
    int nameLen;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLen)) {
        WRONG_PARAM_COUNT
    }

    zval *queryID = zend_read_property(ant_database_class_ptr, getThis(), PROPERTY_STRINGL(ANT_DATABASE_PROPERTY_QUERY_ID), 1 TSRMLS_CC);
    if(Z_TYPE_P(queryID) == IS_OBJECT) {
        zval *params[2], *zvType, **pData;
        ANT_CALL_USER_FUNC_NAME_C(mysqli_fetch_array, db)
        ANT_MAKE_ZVAL_LONG(zvType, 1)
        params[0] = queryID;
        params[1] = zvType;

        call_user_function(EG(function_table), NULL, ANT_CALL_USER_FUNC_NAME(mysqli_fetch_array, db), 2, params TSRMLS_CC);
        if(zend_hash_find(Z_ARRVAL_P(zv_call_db_mysqli_fetch_array_ret), PROPERTY_STRINGAL(name), (void **)&pData) == FAILURE) {
            RETURN_NULL()
        }

        ANT_SWITCH_ZVAL_PP(return_value, pData)
        ANT_CALL_USER_FUNC_NAME_D(mysqli_fetch_array, db)
        zval_ptr_dtor(&zvType);
        return;
    }

    RETURN_NULL()
}

ZEND_METHOD(ant_database_class, getLastId) {
    ANT_CALL_USER_FUNC_NAME_C(mysqli_insert_id, db)
    zval *params[1];
    zval *connID = zend_read_property(ant_database_class_ptr, getThis(), PROPERTY_STRINGL(ANT_DATABASE_PROPERTY_CONN_ID), 1 TSRMLS_CC);
    params[0] = connID;
    call_user_function(EG(function_table), NULL, ANT_CALL_USER_FUNC_NAME(mysqli_insert_id, db), 1, params TSRMLS_CC);

    ANT_SWITCH_ZVAL_P(return_value, zv_call_db_mysqli_insert_id_ret)
    ANT_CALL_USER_FUNC_NAME_D(mysqli_insert_id, db)
    return;
}

ZEND_METHOD(ant_database_class, begin_transaction) {
    query("SET AUTOCOMMIT=0", getThis() TSRMLS_CC);
    query("BEGIN", getThis() TSRMLS_CC);
}

ZEND_METHOD(ant_database_class, rollback_transaction) {
    query("ROLLBACK", getThis() TSRMLS_CC);
}

ZEND_METHOD(ant_database_class, commit_transaction) {
    query("COMMIT", getThis() TSRMLS_CC);
    query("SET AUTOCOMMIT=1", getThis() TSRMLS_CC);
}

zend_function_entry ant_database_methods[] = {
    ZEND_ME(ant_database_class, __construct, arginfo_database___construct, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_database_class, query, arginfo_database_query, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_database_class, insert, arginfo_database_insert, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_database_class, update, arginfo_database_update, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_database_class, getRow, arginfo_database_get_rows, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_database_class, getRows, arginfo_database_get_rows, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_database_class, getField, arginfo_database_get_field, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_database_class, getLastId, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_database_class, getRowsNum, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_database_class, begin_transaction, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_database_class, rollback_transaction, NULL, ZEND_ACC_PUBLIC)
    ZEND_ME(ant_database_class, commit_transaction, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

ANT_STARTUP_FUNCTION(database) {
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "AntDatabase", ant_database_methods);
    ant_database_class_ptr = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(ant_database_class_ptr, PROPERTY_STRINGL(ANT_DATABASE_PROPERTY_QUERY_ID), ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(ant_database_class_ptr, PROPERTY_STRINGL(ANT_DATABASE_PROPERTY_CONN_ID), ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(ant_database_class_ptr, PROPERTY_STRINGL(ANT_DATABASE_PROPERTY_DATABASE), ZEND_ACC_PROTECTED TSRMLS_CC);

    return SUCCESS;
}