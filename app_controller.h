/**
 * @author lucasho
 * @created 2017-8-7
 * @modified 2017-8-7
 * @version 1.0
 * @link github.com/cqlucasho
 */
#ifndef PHP_ANT_APP_CONTROLLER_H
#define PHP_ANT_APP_CONTROLLER_H

extern zend_class_entry *ant_app_controller_class_ptr;

ANT_STARTUP_FUNCTION(app_controller);
ANT_SHUTDOWN_FUNCTION(app_controller);

#endif
