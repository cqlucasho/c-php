/**
 * @author lucasho
 * @created 2017-8-4
 * @modified 2017-8-4
 * @version 1.0
 * @link github.com/cqlucasho
 */
#ifndef PHP_ANT_CORES_ROUTE_H
#define PHP_ANT_CORES_ROUTE_H

extern zend_class_entry *ant_route_interface_ptr, *ant_route_abstract_class_ptr, *ant_route_default_class_ptr, *ant_route_pathinfo_class_ptr;

#define ANT_ROUTE_PROPERTY_APP_OBJ    "_application_obj"
#define ANT_ROUTE_PROPERTY_URL_PARAMS "_url_params"

extern zval *route_instance(TSRMLS_D);
extern char *route_default_generate(char *module, char *controller, char *action, zval *params TSRMLS_DC);
extern char *route_pathinfo_generate(char *module, char *controller, char *action, zval *params TSRMLS_DC);

ANT_STARTUP_FUNCTION(route);
ANT_SHUTDOWN_FUNCTION(route);

#endif
