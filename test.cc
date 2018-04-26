#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <string.h>
#include "cache.h"
#include "timer.h"


int main(int argc, char** argv) {
    
    int len = 4*1024;
    mcache* m_cache = new mcache(100000, len);
    cache* pcache = m_cache->create_cache();
    //unsigned int sign[2]={100,100};
    unsigned int sign[2]={0,0};
    char data[512]="AAA";
    int result = 0;

    char tmp[len];
    int in_data = 0;
    for(int i = 0; i < 150000; ++i) {
        
        std::stringstream ss;
        ss << in_data;
        std::string tmp = ss.str();
        std::string tmp_num;
        for(int k = 1; k <=300;k++) {
            tmp_num += tmp;
        }
        unsigned int str_len = strlen(tmp_num.c_str());
        char* data[len];
        memcpy(data, tmp_num.c_str(), str_len);
        result = m_cache->add_item(pcache, sign, data, str_len);
        if (result != 0) {
            std::cout << "result:" << result << std::endl;
            return -1;
        }
        sign[0]++;
        sign[1]++;
        in_data++;
    }
    std::cout << "capacity:" << pcache->capacity << std::endl;
    
    m_cache->get_first_item(pcache, tmp, len);
    std::cout << "get first" << tmp << std::endl;
    m_cache->get_last_item(pcache, tmp, len);
    std::cout << "get last" << tmp << std::endl;
    std::cout << "begin seek" << std::endl;
    unsigned int sign2[2] = {130000, 130000};
    AutoTimer at(0, "seek_item", argv[0]);
    result = m_cache->seek_item(pcache, sign2, tmp, len);
    if(result == 0) {
        std::cout <<"================find :"<< tmp << "==========" << std::endl;
    } else {
        std::cout << "=== not found ===" << std::endl;
    }

    return 0;
}
