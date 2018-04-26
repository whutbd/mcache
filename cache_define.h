#ifndef CACHE_DEF_h
#define CACHE_DEF_H

struct mem_index_node {
    unsigned int sign1;
    unsigned int sign2;
    unsigned int left;
    unsigned int right;
    unsigned int next;
};
typedef struct mem_index_node index_node;

struct mem_hashtable {
    unsigned int* table;
    unsigned int bucket_num;
};
typedef struct mem_hashtable hash_table;

struct mem_cache {
    unsigned int capacity;
    unsigned int unitsize;
    hash_table hashtable;
    index_node* indenode;
    char* pinfolist;
    unsigned int curpos;
    unsigned int head;
    unsigned int tail;
};
typedef struct mem_cache cache;

enum {
    RT_NOTICE_OK=0,
    RT_ERROR_GENERAL_ERROR = -1,
    RT_ERROR_MEMORY_FAILED=-2,
    RT_ERROR_FILE_FAILED=-3,
    RT_ERROR_BEYOND_CAPACITY = -4,
    RT_ERROR_BEYOND_LIMIT = -5,
    RT_NOT_FOUND = -6
};


enum {
    CACHE_NULL=0xFFFFFFFF,
    MAX_PATH_SIZE = 1024,
    SHM_KEY = 100000
};


#endif

