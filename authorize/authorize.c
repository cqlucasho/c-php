#include "php_ant.h"
//#include "ant_mmap.h"
#include "authorize.h"
#include "cores/core.h"

zend_bool search_ignore_lists(TSRMLS_D) {
    zval *ignoreList = read_auth_config("ignore_list" TSRMLS_CC);
    HashTable *ht = HASH_OF(ignoreList);
    int len = zend_hash_num_elements(ht);
    if(len > 0) {
        zval **pData;
        char *path;
        spprintf(&path, 0, "%s/%s/%s", ANT_G(module), ANT_G(controller), ANT_G(action));

        zend_hash_internal_pointer_reset(ht);
        for(int i = 0; i < len; i++) {
            zend_hash_get_current_data(ht, (void **)&pData);
            char *vpath = Z_STRVAL_PP(pData);
            if(strncmp(path, vpath, strlen(vpath)) == 0) {
                efree(path);
                return 1;
            }

            zend_hash_move_forward(ht);
        }

        efree(path);
    }

    return 0;
}

zend_bool authentication(AuthList *list TSRMLS_DC) {
    zend_bool success = 0;
    char *path;

    spprintf(&path, 0, "%s/%s/%s", ANT_G(module), ANT_G(controller), ANT_G(action));
    /*for(int i=0; i < list->len; i++) {
        if(strncmp(list->ele[i], path, strlen(path)) == 0) {
            efree(path);
            return 1;
        }
    }*/

    efree(path);
    return success;
}