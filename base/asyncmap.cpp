#include "asyncmap.h"



template<typename Key,typename Value>
AsyncMap<Key,Value>::AsyncMap(): std::map<Key,Value>()
{

}

template<typename Key,typename Value>
Value &AsyncMap<Key,Value>::operator[](const Key& key){
    lock.lock();
    Value &v=std::map<Key,Value>::operator[](key);
    lock.unlock();
    return v;
}








/*template<typename T,int _size>
T *AsyncQueue<T,_size>::reserve_new(){
    lock.lock();
    if(_uper_n+3>_size)
        return nullptr;
    ++_uper_n;
    int tmp=head;
    ++head;

    head%=_size_size;
    lock.unlock();
    //TODO unlock new
    return array+tmp;
}


template<typename T,int _size>
void AsyncQueue<T,_size>::pop(){
    //TODO lock pop
    lock.lock();
    ++tail;
    tail%=_size;
    --_uper_n;
    lock.unlock();
}

template<typename T,int _size>
int AsyncQueue<T,_size>::size(){
    int tmp=head-tail;
    if(tmp<0)
        tmp+=_size;
    return tmp;
}

template<typename T,int _size>
bool AsyncQueue<T,_size>::empty(){
    return size()==0;
}

template<typename T,int _size>
T* AsyncQueue<T,_size>::front(){
    return array+tail;
}
/**/


