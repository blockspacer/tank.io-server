#ifndef ASYNCMAP_H
#define ASYNCMAP_H
#include <map>
#include "game_message/Messages.h"
#include "base_message/BaseMessage.h"
#include "DB/DBAccess.h"
#include <queue>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/move/move.hpp>
#include <iostream>
using namespace std;
template<typename Key,typename Value>
struct AsyncMap:public std::map<Key,Value>
{
    boost::mutex lock;
public:
    AsyncMap();
    Value &operator[](const Key& key);


};



template<typename Key,typename Value>
struct SafeAsyncMap
{
    std::map<Key,Value> map;
    mutable boost::mutex lock;
public:
    SafeAsyncMap(){}
    void push(const Key &key,const Value &value){
        lock.lock();
        map[key]=value;
        lock.unlock();
    }
    void lock_rw(){
        lock.lock();
    }
    void unlock_rw(){
        lock.unlock();
    }

    void remove(const Key &key){
        lock.lock();
        map.erase(key);
        lock.unlock();
    }
    Value &get(const Key &key){
        lock.lock();
        Value &v=map[key];
        lock.unlock();
        return v;
    }



};


template<typename Value>
struct SafeAsyncPriorityQueue
{
    //std::priority_queue<Value> q;
    std::queue<Value> q;
    mutable boost::mutex lock;
public:
    SafeAsyncPriorityQueue(){}
    void push(const Value &value){
        lock.lock();
        q.push(value);
        lock.unlock();
    }
    Value top(){
        lock.lock();
        Value v=q.front();
        lock.unlock();
        return v;
    }
    void pop(){
        lock.lock();
            q.pop();
        lock.unlock();

    }
    bool empty(){
        return q.empty();
    }
    unsigned int size(){
        return q.size();
    }


};





/**
 * in class camelan async nist faghat theard i ke push mikonad mitavand ba threadi ke pop mikonad motafaveet bashad
 * va hamzaman to thread mokhtalef nemitavand push konad
*/
template<typename T,int _size>
struct AsyncQueue{
    T array[_size];
    int reserv_head=0;
    int completed_head=0;
    int tail=0;

    mutable boost::mutex lock;
    //template<typename T2>
    void clear(){
        reserv_head=completed_head=tail=0;
    }
    template<class T2>
    T2 *reserve_new(){

        if(reserved_number()+3>_size){
            //cerr<<"buffer out of size"<<endl;
            return nullptr;
        }


        int tmp=reserv_head;
        ++reserv_head;

        reserv_head%=_size;

        return (T2*)(array+tmp);
    }
    void complte_last_reserved(){
        if(uncomplted_reserved_size()<=0)
            return;
        int _completed_head=(completed_head+1)%_size;
        completed_head=_completed_head;

    }

    void pop(){
        if(completed_size()<1)
            return ;
        int _tail=(tail+1)%_size;
        tail=_tail;
    }


    int reserved_number()const{
        int tmp=reserv_head-tail;
        if(tmp<0)
            tmp+=_size;
        return tmp;
    }
    int uncomplted_reserved_size()const{
        int tmp=reserv_head-completed_head;
        if(tmp<0)
            tmp+=_size;
        return tmp;
    }
     int completed_size()const{
        int tmp=completed_head-tail;
        if(tmp<0)
            tmp+=_size;
        return tmp;
    }

    bool empty()const{
        return completed_size()==0;
    }


    T* front(){
        return array+tail;
    }

};








#endif // ASYNCMAP_H
