#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include "cache.h"

int main() {
    mcache* m_cache = new mcache(5,512);
    cache* pcache = m_cache->create_cache();
    //unsigned int sign[2]={100,100};
    unsigned int sign[2]={100,100};
    char data[512]="AAA";
    int result = 0;

    char tmp[512];
    unsigned int len = 10;
    int in_data = 0;
    for(int i = 0; i < 20; ++i) {
        //result = m_cache->add_item(pcache, sign, &in_data, sizeof(int));
        result = m_cache->add_item(pcache, sign, data, sizeof(data));
        if (result != 0) {
            std::cout << "add item failed " << std::endl;
            return -1;
        }
        sign[0]++;
        sign[1]++;
        //in_data++;
        data[2]++;
        //std::cout <<"data:"<< data << std::endl;
        //std::cout <<"in data:"<< in_data << std::endl;
        //print first
        //m_cache->get_first_item(pcache, tmp, len);
        //std::cout << "get first" << tmp << std::endl;
    }
    
    m_cache->get_first_item(pcache, tmp, len);
    std::cout << "get first" << tmp << std::endl;
    std::cout << "begin seek" << std::endl;
    unsigned int sign2[2] = {116, 116};
    
    result = m_cache->seek_item(pcache, sign2, tmp, len);
    if(result == 0) {
        std::cout <<"================find :"<< tmp << "==========" << std::endl;
    } else {
        std::cout << "=== not found ===" << std::endl;
    }
    
    m_cache->get_first_item(pcache, tmp, len);
    std::cout << "get first" << tmp << std::endl;
    std::cout << "begin seek" << std::endl;
    
    /// test save cache and load cache
    int ret = m_cache->save_cache(pcache, "./", "ac");
    std::cout << "save_cache ret:" << ret << std::endl;
    cache* p_new_cache;
    ret = m_cache->load_cache(p_new_cache, "./", "ac");
    std::cout << "load_cache ret:" << ret << std::endl;
    result = m_cache->seek_item(pcache, sign2, tmp, len);
    if(result == 0) {
        std::cout << tmp<< std::endl;
    } else {
        std::cout << "=== not found ===" << std::endl;
    }

    return 0;
}
