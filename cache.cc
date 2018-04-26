#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "cache.h"
#include <iostream>

mcache::mcache(unsigned int capacity, unsigned int unitsize) {
    assert(capacity>0);
    assert(unitsize>0);
    _capacity = capacity;
    _unitsize = unitsize;
}

//cache* mcache::create_cache(unsigned int capacity, unsigned int unitsize) {
cache* mcache::create_cache() {
    cache* pcache = NULL;
    index_node* pnodelist = NULL;
    unsigned int * ht=NULL;
    char* infodata = NULL;
    unsigned int bucket_num = 0;
    //assert(capacity>0);
    //assert(unitsize>0);

    bucket_num = _capacity;
    /*if((bucket_num % 2) == 0){
        bucket_num++;
    }*/
    if ((pcache = (cache*)malloc(sizeof(cache)))==NULL) {
        return NULL;
    }
    if ((pnodelist = (index_node*)malloc(sizeof(index_node)*_capacity))==NULL) {
        return NULL;
    }
    if((infodata = (char*)malloc(_capacity*_unitsize))==NULL) {
        return NULL;
    } 
    //init
    infodata[0] = '0';
    if((ht=(unsigned int*)malloc(bucket_num*sizeof(unsigned int))) == NULL) {
        return NULL;
    }
    //init hashtable
    memset(ht, 0XFF, bucket_num*sizeof(unsigned int));
    for(size_t i = 0; i < _capacity; ++i) {
        pnodelist[i].left = CACHE_NULL;
        pnodelist[i].right = CACHE_NULL;
        pnodelist[i].next = CACHE_NULL;
    }
    pcache->capacity = _capacity;
    pcache->unitsize = _unitsize;
    pcache->hashtable.table = ht;
    pcache->hashtable.bucket_num = bucket_num;
    pcache->indenode = pnodelist;
    pcache->pinfolist = infodata;
    pcache->curpos = 0;
    pcache->head = CACHE_NULL;
    pcache->tail = CACHE_NULL;
    
    return pcache;
}

int mcache::clean_cache(cache* pcache) {
    assert(pcache);
    assert((pcache->capacity >0) && (pcache->unitsize >0));
    //init hashtable
    memset(pcache->hashtable.table, 0XFF, pcache->hashtable.bucket_num*sizeof(unsigned int));
    for(size_t i = 0; i < _capacity; ++i) {
        pcache->indenode[i].left = CACHE_NULL;
        pcache->indenode[i].right = CACHE_NULL;
        pcache->indenode[i].next = CACHE_NULL;
    }
    memset(pcache->pinfolist,0, pcache->capacity * pcache->unitsize);
    pcache->curpos = 0;
    pcache->head = CACHE_NULL;
    pcache->tail = CACHE_NULL;
}

int mcache::destroy_cache(cache* pcache) {
    assert(pcache);
    if (pcache->hashtable.table != NULL) {
        free(pcache->hashtable.table);
    }
    if (pcache->indenode != NULL) {
        free(pcache->indenode);
    }
    if (pcache->pinfolist != NULL) {
        free(pcache->pinfolist);
    }
    free(pcache);
    return 0;
}

unsigned int* mcache::hash_getpos(cache* pcache, unsigned int* sign) {
    assert(pcache != NULL && sign != NULL);
    unsigned int hash_entry = (sign[0] + sign[1]) % pcache->hashtable.bucket_num;
    unsigned int* pos = pcache->hashtable.table + hash_entry;
    
    index_node* node = NULL;
    while (*pos != CACHE_NULL) {
        assert(*pos <= pcache->capacity);
        node = pcache->indenode + *pos;
        if (node->sign1 == sign[0] && node->sign2 == sign[1]) {
            break;
        }else {
            pos = &(node->next);
        }
    }
    return pos;
}

int mcache::add_item(cache* pcache, unsigned int* sign,void* data,unsigned int data_len) {
    assert(pcache != NULL && data !=  NULL && data_len >0);
    unsigned int cur_pos;
    index_node* node;
    
    if (data_len > pcache->unitsize) {
        return RT_ERROR_BEYOND_LIMIT;
    }
    unsigned int* idx;
    if ((idx = hash_getpos(pcache, sign)) == NULL) {
        return RT_ERROR_GENERAL_ERROR;
    }
    if (*idx != CACHE_NULL) {
        // the node exists in the cache ,do nothing 
        return RT_NOTICE_OK;
    }
    if (pcache->curpos < pcache->capacity) {
        cur_pos = pcache->curpos;
        pcache->curpos++;
    } else {
        // the cache is full, wash out the oldest one
        index_node* tmp_node = pcache->indenode + pcache->tail;
        cur_pos = pcache->tail;
        unsigned int oldsign[2];
        oldsign[0] = tmp_node->sign1; 
        oldsign[1] = tmp_node->sign2;
        unsigned int* tmp_idx = hash_getpos(pcache, oldsign);
        int result =  remove_item(pcache, tmp_idx);
        idx = tmp_idx;
        if (result < 0) {
            return result;
        }
    }   
    node = pcache->indenode + cur_pos;
    node->sign1 = sign[0]; 
    node->sign2 = sign[1]; 
    *idx = cur_pos;
    
    //copy data to infolist
    memcpy(pcache->pinfolist + pcache->unitsize*cur_pos, data, data_len);
    lru_append(pcache, cur_pos);
    return RT_NOTICE_OK;
}

int mcache::seek_item(cache* pcache, unsigned int* sign,void* data,unsigned int data_len) {
    assert(pcache != NULL && sign != NULL && data != NULL);
    unsigned int* tmp_bucket;
    if ((tmp_bucket = hash_getpos(pcache, sign)) == NULL) {
        return RT_ERROR_GENERAL_ERROR;
    }
    if (*tmp_bucket == CACHE_NULL){
        return RT_NOT_FOUND;
    }
    if (*tmp_bucket > pcache->capacity) {
        return RT_ERROR_GENERAL_ERROR;
    }
    if (lru_remove(pcache, *tmp_bucket) < 0) {
        return RT_ERROR_GENERAL_ERROR;
    }
    if (lru_append(pcache, *tmp_bucket) < 0) {
        return RT_ERROR_GENERAL_ERROR;
    }
    memcpy(data, pcache->pinfolist + (*tmp_bucket * pcache->unitsize), pcache->unitsize);
    return RT_NOTICE_OK;
}

int mcache::lru_remove(cache* pcache, unsigned int pos) {
    if (pos == CACHE_NULL) {
        return RT_ERROR_GENERAL_ERROR;
    }
    if (pos > pcache->capacity) {
        return RT_ERROR_BEYOND_CAPACITY;
    }

    index_node* pin = pcache->indenode + pos;
    unsigned int * left_right, *right_left;
    if (pin->left == CACHE_NULL) {
        left_right = &(pcache->head);
    } else {
        if (pin->left > pcache->capacity) {
            return RT_ERROR_BEYOND_LIMIT;
        }
        index_node* left_node = pcache->indenode + pin->left;
        left_right = &(left_node->right);
    }
    if (pin->right == CACHE_NULL) {
        right_left = &(pcache->tail);
    } else {
        if (pin->right >= pcache->capacity) {
            return RT_ERROR_BEYOND_LIMIT;
        }
        index_node* right_node = pcache->indenode + pin->right;
        right_left = &(right_node->left);
    }
    *left_right = pin->right;
    *right_left = pin->left;
    
    pin->left = CACHE_NULL;
    pin->right = CACHE_NULL;

    return RT_NOTICE_OK;
}

int mcache::lru_append(cache* pcache, unsigned int pos) {
    assert(pcache);
    index_node* node;
    if (pos == CACHE_NULL) {
        return RT_ERROR_GENERAL_ERROR;
    }
    if (pos > pcache->capacity) {
        return RT_ERROR_BEYOND_CAPACITY;
    }
    if (pcache->head == CACHE_NULL) {
        //add first
        node = pcache->indenode + pos;
        pcache->head = pos;
        pcache->tail = pos;
        node->left = CACHE_NULL;
        node->right = CACHE_NULL;
    } else {
        node = pcache->indenode + pos;
        index_node* head_node_old =  pcache->indenode + pcache->head;
        head_node_old->left = pos;
        node->left = CACHE_NULL;
        node->right = pcache->head;
        pcache->head = pos; 
    }
    return RT_NOTICE_OK;
}

int mcache::remove_item(cache* pcache, unsigned int* pos) {
    assert(pcache != NULL);
    unsigned int tmp_pos = *pos;
    
    if (pos == NULL) {
        return RT_ERROR_GENERAL_ERROR;
    }
    if (*pos == CACHE_NULL) {
        return RT_NOT_FOUND;
    }
    if (*pos > pcache->capacity) {
        return RT_ERROR_BEYOND_CAPACITY;
    }
    index_node* node = pcache->indenode + tmp_pos;
    *pos = node->next;
    int ret = lru_remove(pcache, tmp_pos);
    return ret;
}


int mcache::get_first_item(cache* pcache,void* pdata,unsigned int data_len) {
    assert(pcache != NULL);
    if (pcache->head == CACHE_NULL || pcache->head == CACHE_NULL) {
        return RT_NOT_FOUND;
    }
    index_node* node = pcache->indenode + pcache->head;
    unsigned int sign[2];
    sign[0] = node->sign1;
    sign[1] = node->sign2;

    unsigned int* pos = hash_getpos(pcache, sign);
    memcpy(pdata, pcache->pinfolist + pcache->unitsize * (*pos), data_len);
    return 0;
}


int mcache::get_last_item(cache* pcache,void* pdata,unsigned int data_len) {
    assert(pcache != NULL);
    if (pcache->tail == CACHE_NULL || pcache->tail == CACHE_NULL) {
        return RT_NOT_FOUND;
    }
    index_node* node = pcache->indenode + pcache->tail;
    unsigned int sign[2];
    sign[0] = node->sign1;
    sign[1] = node->sign2;

    unsigned int* pos = hash_getpos(pcache, sign);
    memcpy(pdata, pcache->pinfolist + pcache->unitsize * (*pos), data_len);
    return 0;
}



















