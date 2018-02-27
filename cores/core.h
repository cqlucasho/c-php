/**
 * @author lucasho
 * @created 2017-8-3
 * @modified 2017-8-3
 * @version 1.0
 * @link github.com/cqlucasho
 */
#ifndef PHP_ANT_CORES_CORE_H
#define PHP_ANT_CORES_CORE_H

extern zend_class_entry *ant_core_class_ptr;

#define ANT_SAVE_OLD_EG_ENVIRON() \
	{ \
		zval **old_return_value_pp  = EG(return_value_ptr_ptr); \
		zend_op **old_opline_ptr  	= EG(opline_ptr); \
		zend_op_array *old_op_array = EG(active_op_array);

#define ANT_RESTORE_EG_ENVIRON() \
		EG(return_value_ptr_ptr) = old_return_value_pp;\
		EG(opline_ptr)			 = old_opline_ptr; \
		EG(active_op_array)		 = old_op_array; \
	}

ANT_STARTUP_FUNCTION(core);

/**
 * the Pascal style translate into Camel style
 *
 * @example
 *  <pre>
 *      ant_camel_func('HelloWorld');   # hello_world
 *  </pre>
 * @param word
 * @param rHeadWord
 * @return
 */
extern char *ant_camel_func(char *word);
/**
 * the Camel style translate into Pascal style
 *
 * @example
 *  <pre>
 *      ant_pascal_func('hello_world', int rHeadWord); # HelloWorld
 *  </pre>
 * @param word
 * @param rHeadWord
 * @return
 */
extern char *ant_pascal_func(char *word, int rHeadWord);
extern char *ant_upper_to_lower(char *str);
extern void core_class_import(char *fileName, char *path TSRMLS_DC);
extern char *find_real_path(char *fileName, char *path TSRMLS_DC);
extern int load_file(char *path TSRMLS_DC);
extern zval *read_config(char *key TSRMLS_DC);
extern zval *read_db_config(char *key TSRMLS_DC);
extern zval *read_auth_config(char *key TSRMLS_DC);
extern char *read_config_path(char *path TSRMLS_DC);
void _find_file(char *fileName, char *path TSRMLS_DC);

#endif