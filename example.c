MYSQLI_RESOURCE *my_res; 
mysqli_object *intern = (mysqli_object *)zend_object_store_get_object(*(__id) TSRMLS_CC);
if (!(my_res = (MYSQLI_RESOURCE *)intern->ptr)) {
    php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't fetch %s", intern->zo.ce->name);
    RETURN_NULL();
}
__ptr = (__type)my_res->ptr; 
if (__check && my_res->status < __check) { 
    php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid object or resource %sn", intern->zo.ce->name); 
    RETURN_NULL();
}

if (!(__ptr)->mysql) { 
    mysqli_object *intern = (mysqli_object *)zend_object_store_get_object(*(__id) TSRMLS_CC);
    php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid object or resource %sn", intern->zo.ce->name);
    RETURN_NULL();
}