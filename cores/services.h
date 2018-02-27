/**
 * @author lucasho
 * @created 2017-8-23
 * @modified 2017-8-23
 * @version 1.0
 * @link github.com/cqlucasho
 */
#ifndef PHP_ANT_CORES_SERVICES_H
#define PHP_ANT_CORES_SERVICES_H

extern zend_class_entry *ant_services_class_ptr;
ANT_STARTUP_FUNCTION(services);

#define ANT_SERVICE_PROPERTY_DB "_db"

#endif
