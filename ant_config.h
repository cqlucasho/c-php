/**
 * @author lucasho
 * @created 2017-8-3
 * @modified 2017-8-3
 * @version 1.0
 * @link github.com/cqlucasho
 */
#ifndef PHP_ANT_CONFIG_H
#define PHP_ANT_CONFIG_H

extern zend_class_entry *ant_config_class_ptr;
extern zval *config_instance(TSRMLS_D);
extern void config_free_instance();

#define ConfigClass ant_config_class_ptr
#define ANT_CONFIG_PROPERTY_CONFIGS "_configs"
#define ANT_CONFIG_PROPERTY_DBS     "_dbs"
#define ANT_CONFIG_PROPERTY_AUTHES  "_authes"
#define ANT_CONFIG_PROPERTY_INSTANCE "_instance"

ANT_STARTUP_FUNCTION(config);
ANT_SHUTDOWN_FUNCTION(config);

#endif
