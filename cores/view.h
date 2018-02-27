/**
 * @author lucasho
 * @created 2017-8-14
 * @modified 2017-8-14
 * @version 1.0
 * @link github.com/cqlucasho
 */
#ifndef PHP_ANT_VIEW_H
#define PHP_ANT_VIEW_H

extern zend_class_entry *ant_view_class_ptr;
ANT_STARTUP_FUNCTION(view);

#define ANT_VIEW_PROPERTY_DB "_db"

extern void view_assign(char *key, zval *value TSRMLS_DC);
void _view_render(char *path, zval *ret TSRMLS_DC);

#endif
