# mcache
一种实现高效的本地定长cache
#数据结构
![原理图](https://github.com/whutbd/mcache/blob/master/mcache.jpg)

功能：
1.mcache是一种定长cache,初始化的时候必须制定cache的size和cache的block块个数
2.mcach是非线程安全的，在多线程环境下使用必须业务自己加锁
3.mcache的内存分配方式采取一次全部分配的方式，在内部维护了一个LRU队列，最近被访问的节点放到头部，当队列满时，将队列的最后一个元素挤出去
