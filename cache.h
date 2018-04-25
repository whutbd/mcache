#ifndef CACHE_H
#define CACHE_H

#include "cache_define.h"

class mcache {

public:
    mcache(unsigned int capacity, unsigned int unitsize);
    //cache* create_cache(unsigned int capacity, unsigned int unitsize);
    //cache* create_cache(unsigned int capacity, unsigned int unitsize);
    cache* create_cache();
    int clean_cache(cache* pcache);
    int destroy_cache(cache* pcache);
    int add_item(cache* pcache, unsigned int* sign,void* data,unsigned int data_len);
    int seek_item(cache* pcache, unsigned int* sign,void* data,unsigned int data_len);
    int remove_item(cache* pcache, unsigned int* pos);
    int get_first_item(cache* pcache,void* pdata,unsigned int data_len);
    int save_cache(cache* pcache, const char* path, const char* fname);
    int load_cache(cache* pcache, const char* path, const char* fname);
    //int get_first_item(cache* pcache,void* pdata,unsigned int data_len);
    //int clear_item(mc_cache* pcache,unsigned int* pos);
private:
    int lru_append(cache* pcache, unsigned int pos);
    int lru_remove(cache* pcache, unsigned int pos);
    inline unsigned int* hash_getpos(cache* pcache, unsigned int* sign);
    void get_full_path(char * buf, int len, const char * path, const char * fname, const char * subfix);
private:
    unsigned int _capacity;
    unsigned int _unitsize;


};

#endif
