/**
 * authorization and authentication
 *
 * @author lucasho
 * @created 2017-8-27
 * @modified 2017-8-27
 * @version 1.0
 */
#ifndef PHP_ANT_AUTHORIZE_H
#define PHP_ANT_AUTHORIZE_H

#include "ant_mmap.h"

extern zend_bool search_ignore_lists(TSRMLS_D);
extern zend_bool authentication(AuthList *list TSRMLS_DC);

#endif
