#ifndef ASYNCCYIRCULARBUFFER_H
#define ASYNCCYIRCULARBUFFER_H



class AsyncCyirCularBuffer
{
public:
    AsyncCyirCularBuffer();
};

struct MemBlock{
    bool use=false;
};

/*class MemoryManager{
    //const int block_size=16;
    const int max_2_pow=10;
    bool use[16][1000000];
    long long _data[1<<(max_2_pow+1)-1];
};/**/

#endif // ASYNCCYIRCULARBUFFER_H
