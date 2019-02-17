#ifndef BOUNDEDARRAY_H
#define BOUNDEDARRAY_H

template<typename T,int _size>
class BoundedArray
{
    T data[_size];
public:
    BoundedArray(){

    }

    const T&operator[](int idx){
        return data[idx];
    }
};

#endif // BOUNDEDARRAY_H
