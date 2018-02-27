/**
 * @author lucasho
 * @created 2017-8-3
 * @modified 2017-8-3
 * @version 1.0
 * @link github.com/cqlucasho
 */
#ifndef PHP_ANT_CONTROLLER_H
#define PHP_ANT_CONTROLLER_H

extern zend_class_entry *ant_controller_class_ptr;

#define ANT_CONTROLLER_PROPERTY_VIEW "_view_obj"
#define ANT_CONTROLLER_PROPERTY_APP_OBJ "_app_obj"

ANT_STARTUP_FUNCTION(controller);
ANT_SHUTDOWN_FUNCTION(controller);

#endif