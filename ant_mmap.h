#ifndef PHP_ANT_MMAP_H
#define PHP_ANT_MMAP_H

typedef struct {
    char *ele[1024];
    char *v;
    int len;
}AuthListEle;

typedef struct {
    int id_number;
    AuthListEle id_list;
}AuthList;

extern zend_bool shm_create(size_t user_id, zval *value, int size TSRMLS_DC);
extern void *shm_read(size_t user_id, int size);
extern void shm_update();
extern void shm_delete();

#endif
