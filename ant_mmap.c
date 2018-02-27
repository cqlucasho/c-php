#include "php_ant.h"
#include "ant_mmap.h"
#include <sys/shm.h>
#include <fcntl.h>
#include <ctype.h>

zend_bool shm_create(size_t user_id, zval *value, int size TSRMLS_DC);
void *shm_read(size_t user_id, int size);
void shm_update();
void shm_delete();
key_t _gen_key(size_t user_id);

zend_bool shm_create(size_t user_id, zval *value, int size TSRMLS_DC) {
    AuthList *authList;
    AuthListEle authListEle;
    int shm_id;
    zval **pData;
    char v[100];

    shm_id = shmget(_gen_key(user_id), sizeof(authList), 0764|IPC_CREAT);
    if(shm_id == -1) {
        zend_error(E_ERROR, "shmget error");
    }
    authList = (AuthList *)shmat(shm_id, NULL, 0);

    HashTable *htable = Z_ARRVAL_P(value);
    int len = zend_hash_num_elements(htable);
    if(len > 0) {
        zend_hash_internal_pointer_reset(htable);
        for (int i = 0; i < len; i++) {
            zend_hash_get_current_data(htable, (void **)&pData);

            strncpy(v, Z_STRVAL_PP(pData), strlen(Z_STRVAL_PP(pData)));
            memcpy(&authList->id_list.v, &v, sizeof(v));
            //authListEle.len = 2;
            //authList->id_number = 2;
            //memcpy(&authList->id_list, &authListEle, sizeof(authListEle));

            zend_hash_move_forward(htable);
        }

        return 1;
    }

    return 1;
}

void *shm_read(size_t user_id, int size) {
    AuthList *authList;
    int shm_id = shmget(_gen_key(user_id), sizeof(authList), S_IRUSR|S_IWUSR);
    if(shm_id == -1) {
        zend_error(E_ERROR, "shmget error in the shm_read()");
    }
    authList = (AuthList *)shmat(shm_id, 0, 0);
    //zend_error(E_ERROR, "--len:%d->", authList->id_list.len);
    zend_error(E_ERROR, "--str:%s->", authList->id_list.v);
    /*for(int i=0; i < authList->len; i++) {
        zend_error(E_ERROR, "--%s-->", authList->ele[i]);
    }*/

    return authList;
}

void shm_update() {

}

void shm_delete() {

}

key_t _gen_key(size_t user_id) {
    key_t key = ftok("/tmp/mmap", (int)user_id);
    return key;
}