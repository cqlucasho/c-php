#include "php_ant.h"
#include "core.h"
#include "ant_config.h"

zend_class_entry *ant_core_class_ptr;

const char upperWords[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
const char lowerWords[] = {'a', 'c', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'm', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
const char *lineLowerWord[] = {"_a", "_b", "_c", "_d", "_e", "_f", "_g", "_h", "_i", "_j", "_k", "_l", "_m", "_n", "_o", "_p", "_q", "_r", "_s", "_t", "_u", "_v", "_w", "_x", "_y", "_z"};
const char *words = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char *wordss = "abcdefghijklmnopqrstuvwxyz";
int currFindPos = 0;
int upperWordNum = 0;
int lineWordNum = 0;

char *_camel_head_word(char *name, char *newName) {
    char headWord;

    // get first word of name
    strncpy(&headWord, name, 1);
    // computing position of word by words
    char *ptr = strchr(words, headWord);
    int pos = ptr - words;
    memcpy(newName, name, (size_t)strlen(name));
    // instead of first word
    newName[0] = lowerWords[pos];

    return newName;
}

char *_get_camel_replaced_string(char *newName) {
    char headWord, prefix[40] = {0}, suffix[40] = {0};
    size_t len = strlen(newName);

    for(int i = currFindPos; i < len; i++) {
        if(isupper(newName[i])) {
            currFindPos = i;
            break;
        }
    }

    strncpy(&headWord, newName+currFindPos, 1);
    // computing position of word by words
    char *upperWordList = strchr(words, headWord);
    size_t upperWordPos = upperWordList - words;

    strncpy(prefix, newName, (size_t)currFindPos);
    strncpy(suffix, newName+currFindPos+1, (len-currFindPos)-1);
    sprintf(newName, "%s%s%s", prefix, lineLowerWord[upperWordPos], suffix);
    return newName;
}

char *_pascal_head_word(char *name, char *newName) {
    char headWord;

    // get first word of name
    strncpy(&headWord, name, 1);
    // computing position of word by wordss
    char *ptr = strchr(wordss, headWord);
    int pos = ptr - wordss;
    // instead of first word
    newName[0] = upperWords[pos];

    return newName;
}

char *_get_pascal_replaced_string(char *newName) {
    char headWord = NULL, prefix[40] = {0}, suffix[40] = {0};
    size_t len = strlen(newName);
    for(int i = currFindPos; i < len; i++) {
        if(ispunct(newName[i])) {
            currFindPos = i;
            break;
        }
    }

    strncpy(&headWord, newName+currFindPos+1, 1);
    // computing position of word by words
    char *lowerWordList = strchr(wordss, headWord);
    size_t lowerWordPos = lowerWordList - wordss;

    strncpy(prefix, newName, (size_t)currFindPos);
    strncpy(suffix, newName+currFindPos+2, (len-currFindPos)-2);
    sprintf(newName, "%s%c%s", prefix, upperWords[lowerWordPos], suffix);
    return newName;
}

/**
 * @see core.h/ant_camel_func
 */
char *ant_camel_func(char *word) {
    currFindPos = upperWordNum = 0;
    char *newName = ecalloc(40, sizeof(char));

    size_t len = strlen(word);
    for(int i = 0; i < len; i++) {
        if(isupper(word[i])) {
            upperWordNum += 1;
        }
    }

    newName = _camel_head_word(word, newName);
    upperWordNum -= 1;
    for(int i = 0; i < upperWordNum; i++) {
        newName = _get_camel_replaced_string(newName);
    }

    return newName;
}

/**
 * @see core.h/ant_pascal_func
 */
char *ant_pascal_func(char *word, int rHeadWord) {
    currFindPos = lineWordNum = 0;
    char *newName = ecalloc(40, sizeof(char));

    size_t len = strlen(word);
    for(int i = 0; i < len; i++) {
        if(ispunct(word[i])) {
            lineWordNum += 1;
        }
    }

    memcpy(newName, word, len);
    if(lineWordNum) {
        if(rHeadWord) newName = _pascal_head_word(word, newName);
        for(int i = 0; i < lineWordNum; i++) {
            newName = _get_pascal_replaced_string(newName);
        }
    }
    else if(rHeadWord) {
        newName = _pascal_head_word(word, newName);
    }

    // note: don't forget to free 'newName'
    return newName;
}

char *ant_upper_to_lower(char *str) {
    size_t len = strlen(str);
    for(int i=0; i<len; i++) {
        if(isupper(str[i])) {
            str[i] = (char)tolower(str[i]);
        }
    }

    return str;
}

int load_file(char *path TSRMLS_DC) {
    zend_file_handle fileHandle;
    zend_op_array *opArray;

    fileHandle.type = ZEND_HANDLE_FILENAME;
    fileHandle.filename = path;
    fileHandle.free_filename = 0;
    fileHandle.opened_path = NULL;
    fileHandle.handle.fp = NULL;

    opArray = zend_compile_file(&fileHandle, ZEND_INCLUDE TSRMLS_CC);
    if(opArray && fileHandle.handle.stream.handle) {
        if(!fileHandle.opened_path) {
            fileHandle.opened_path = path;
        }

        int pData = 1;
        zend_hash_add(&EG(included_files), fileHandle.opened_path, strlen(fileHandle.opened_path)+1, (void *)&pData, sizeof(int), NULL);
    }
    zend_destroy_file_handle(&fileHandle TSRMLS_CC);

    if(opArray) {
        zval *result = NULL;

        ANT_SAVE_OLD_EG_ENVIRON();
        EG(return_value_ptr_ptr) = &result;
        EG(active_op_array) 	 = opArray;

        if(!EG(active_symbol_table)) {
            zend_rebuild_symbol_table(TSRMLS_C);
        }

        zend_execute(opArray TSRMLS_CC);
        destroy_op_array(opArray TSRMLS_CC);
        efree(opArray);

        if(!EG(exception)) {
            if(EG(return_value_ptr_ptr) && *EG(return_value_ptr_ptr)) {
                zval_ptr_dtor(EG(return_value_ptr_ptr));
            }
        }

        ANT_RESTORE_EG_ENVIRON();
        return 1;
    }

    return 0;
}

zval *read_config(char *key TSRMLS_DC) {
    zval *ret, *configObj, *zvGetConfig, *zvKey, *params[1];
    ANT_MAKE_ZVAL_OBJ_INIT(configObj, ant_config_class_ptr)
    ANT_MAKE_ZVAL_STRINGL(zvGetConfig, getConfig)
    ANT_MAKE_ZVAL_STRINGL_D(zvKey, key, 1)
    ANT_MAKE_ZVAL_NULL(ret)

    params[0] = zvKey;
    call_user_function(NULL, &configObj, zvGetConfig, ret, 1, params TSRMLS_CC);

    zval_ptr_dtor(&configObj);
    zval_ptr_dtor(&zvGetConfig);
    zval_ptr_dtor(&zvKey);
    return ret;
}

zval *read_db_config(char *key TSRMLS_DC) {
    zval *ret, *configObj, *zvGetConfig, *zvKey, *params[1];
    ANT_MAKE_ZVAL_OBJ_INIT(configObj, ant_config_class_ptr)
    ANT_MAKE_ZVAL_STRINGL(zvGetConfig, getDb)
    ANT_MAKE_ZVAL_STRINGL_D(zvKey, key, 1)
    ANT_MAKE_ZVAL_NULL(ret)

    params[0] = zvKey;
    call_user_function(NULL, &configObj, zvGetConfig, ret, 1, params TSRMLS_CC);

    zval_ptr_dtor(&configObj);
    zval_ptr_dtor(&zvGetConfig);
    zval_ptr_dtor(&zvKey);
    return ret;
}

zval *read_auth_config(char *key TSRMLS_DC) {
    zval *configObj, *zvKey, *params[1];

    // get config object
    configObj = config_instance(TSRMLS_C);
    // call user's function
    ANT_CALL_USER_FUNC_NAME_C(getAuth, 0);
    ANT_MAKE_ZVAL_STRINGL_D(zvKey, key, 1)
    params[0] = zvKey;
    call_user_function(NULL, &configObj, ANT_CALL_USER_FUNC_NAME(getAuth, 0), 1, params TSRMLS_CC);

    zval_ptr_dtor(&zvKey);
    zval_ptr_dtor(&zv_call_0_getAuth);
    return zv_call_0_getAuth_ret;
}

char *read_config_path(char *path TSRMLS_DC) {
    zval *configObj, *zvGetConfig, *zvPath, *ret, *params[1];
    char *dirPath;

    ANT_MAKE_ZVAL_OBJ_INIT(configObj, ant_config_class_ptr)
    ANT_MAKE_ZVAL_STRINGL(zvGetConfig, getConfig)
    ANT_MAKE_ZVAL_NULL(ret)

    if(!path) {
        ANT_MAKE_ZVAL_STRINGL(zvPath, path_vendors)
        params[0] = zvPath;
        call_user_function(NULL, &configObj, zvGetConfig, ret, 1, params TSRMLS_CC);
        dirPath = estrdup(Z_STRVAL_P(ret));
    }
    else {
        ANT_MAKE_ZVAL_STRINGL_D(zvPath, path, 1)
        params[0] = zvPath;
        call_user_function(NULL, &configObj, zvGetConfig, ret, 1, params TSRMLS_CC);
        dirPath = estrdup(Z_STRVAL_P(ret));
    }

    zval_ptr_dtor(&configObj);
    zval_ptr_dtor(&zvGetConfig);
    zval_ptr_dtor(&ret);
    return dirPath;
}

void _find_file(char *fileName, char *path TSRMLS_DC) {
    char *realPath;
    char *absolutePath = read_config_path(path TSRMLS_CC);
    spprintf(&realPath, 0, "%s%s.inc", absolutePath, fileName);

    /* the files whether exists */
    if(VCWD_ACCESS(realPath, 0) == FAILURE) {
        zend_error(E_ERROR, "the specify loading file `%s` is not exists.", realPath);
        efree(absolutePath);
        efree(realPath);
        return;
    }

    if(zend_hash_exists(&EG(included_files), realPath, strlen(realPath)+1) < 1) {
        int success = load_file(realPath TSRMLS_CC);
        if(!success) {
            zend_error(E_ERROR, "Ant\\Cores::import failed: %s", realPath);
        }
    }

    efree(absolutePath);
    efree(realPath);
}

void core_class_import(char *fileName, char *path TSRMLS_DC) {
    _find_file(fileName, path TSRMLS_CC);
}

char *find_real_path(char *fileName, char *path TSRMLS_DC) {
    char *realPath;
    char *absolutePath = read_config_path(path TSRMLS_CC);
    spprintf(&realPath, 0, "%s%s.inc", absolutePath, fileName);

    if(VCWD_ACCESS(realPath, 0) == FAILURE) {
        zend_error(E_ERROR, "the specify loading file `%s` is not exists.", realPath);
    }

    return realPath;
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_core_class_import, 0, 0, 2)
    ZEND_ARG_INFO(0, fileName)
    ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_METHOD(ant_core_class, import) {
    char *fileName, *path = NULL;
    uint fileNameLen, pathLen;
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &fileName, &fileNameLen, &path, &pathLen)) {
        WRONG_PARAM_COUNT
    }

    _find_file(fileName, path TSRMLS_CC);
}

zend_function_entry ant_core_methods[] = {
    ZEND_ME(ant_core_class, import, arginfo_core_class_import, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    PHP_FE_END
};

ANT_STARTUP_FUNCTION(core) {
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "Core", ant_core_methods);
    ant_core_class_ptr = zend_register_internal_class(&ce TSRMLS_CC);
    ant_core_class_ptr->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
    zend_register_class_alias("Ant\\Core", ant_core_class_ptr);

    return SUCCESS;
}