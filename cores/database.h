/**
 * @author lucasho
 * @created 2017-8-16
 * @modified 2017-8-16
 * @version 1.0
 * @link github.com/cqlucasho
 */
#ifndef PHP_ANT_DATABASE_H
#define PHP_ANT_DATABASE_H

extern zend_class_entry *ant_database_class_ptr;
ANT_STARTUP_FUNCTION(database);

#define ANT_DATABASE_PROPERTY_CONN_ID "_conn_id"
#define ANT_DATABASE_PROPERTY_QUERY_ID "_query_id"
#define ANT_DATABASE_PROPERTY_DATABASE "_database"

extern void database_init(char *host, char *username, char *password, char *dbname, char *charset, zval *this, zval *ret TSRMLS_DC);

#endif
