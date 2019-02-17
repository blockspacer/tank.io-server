#ifndef SHAREDMEMORYLOGER_H
#define SHAREDMEMORYLOGER_H
#include "game_message/Messages.h"
#include "base_message/BaseMessage.h"
#include <boost/interprocess/managed_shared_memory.hpp>
#include <cstdlib> //std::system
#include <sstream>
#include "base/asyncmap.h"


#include <boost/interprocess/allocators/allocator.hpp>

#include <boost/unordered_map.hpp>     //boost::unordered_map
#include <functional>                  //std::equal_to
#include <boost/functional/hash.hpp>   //boost::hash


#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/move/move.hpp>
#include <boost/circular_buffer.hpp>

#include "base/asyncmap.h"
#include "Classes/Room.h"

using boost::asio::ip::udp;




struct LogMsg{
    int id;
    TIME_VALUE time;
    /*enum Type{
        a=1
    }/**/
    int type;
    USER_ID user_id;
    OBJ_ID room_id;
    int ip;
    int file;
    int line;
};


struct Parametr{
    int length;

};

typedef AsyncQueue<LogMsg,1024*1024> MSGS;

class SharedMemoryLoger
{
    MSGS *s;
    int id_counter=1;
public:

    SharedMemoryLoger();
    void init_memory();
    void use_memory();
    void save_log(LogMsg &msg);
};

struct SpeedTest{
    static const int maxn=200;
    int a[maxn][maxn];
    void init(){
        for(int i=0; i<maxn; ++i)
            for(int j=0; j<maxn; ++j)
                a[i][j]=i+j;
    }
    void mul(const SpeedTest &A,const SpeedTest &B){
        int n=maxn;
        for(int i=0; i<n; ++i)
           for(int j=0; j<n; ++j)
               a[i][j]=0;

        for(int i=0; i<n; ++i)
            for(int k=0; k<n; ++k)
                for(int j=0; j<n; ++j)
                    a[i][j]+=A.a[i][k]*B.a[k][j];
    }

};


typedef int    KeyType;
typedef Room::RoomData MappedType;
typedef std::pair<const int, Room::RoomData> ValueType;
typedef boost::interprocess::allocator<ValueType, boost::interprocess::managed_shared_memory::segment_manager> ShmemAllocator;


typedef boost::unordered_map
      < KeyType               , MappedType
      , boost::hash<KeyType>  ,std::equal_to<KeyType>
      , ShmemAllocator>
   MyHashMap;
class SharedRoomMemory
{


    static MyHashMap *myhashmap;
public:

    static MyHashMap *get_map(){
        return myhashmap;
    }

    SharedRoomMemory(){}
    static void init();
    static void update(int id,const Room::RoomData &msg);
    static void remove(int id);
    static void clear();
};



struct UserPortZip{
    COOCKIE cookie;
    int app_version;
    TIME_VALUE last_pakage_time=0;
    udp::endpoint udp_conection;
};


typedef std::pair<const int, UserPortZip> ValueType2;
typedef boost::interprocess::allocator<ValueType2, boost::interprocess::managed_shared_memory::segment_manager> ShmemAllocator2;


typedef boost::unordered_map
      < KeyType               , UserPortZip
      , boost::hash<KeyType>  ,std::equal_to<KeyType>
      , ShmemAllocator2>
   MyHashMap2;

typedef boost::unordered_map
< int               , SpeedTest
, boost::hash<int>  ,std::equal_to<int>
, ShmemAllocator> SpeedTestMap;

class SharedUdpEndPointMemory
{


    static MyHashMap2 *myhashmap;


    static SpeedTestMap *forTest;
public:

    static MyHashMap2 *get_map(){
        return myhashmap;
    }

    SharedUdpEndPointMemory(){}
    static void init();
    static void update(int id,const UserPortZip &msg);
    static void update_time(int id,TIME_VALUE time);
    static void remove(int id);
    static void clear();
};

struct LogParametr{
    int id;
    int pid;//procces id
    int line;//line of file
    char func[64];
    char file[64];
    char data[256];
};


typedef std::pair<const int, LogParametr> LogValueType;
typedef boost::interprocess::allocator<LogValueType, boost::interprocess::managed_shared_memory::segment_manager> LogAllocator;


typedef boost::unordered_map
      < KeyType               , LogParametr
      , boost::hash<KeyType>  ,std::equal_to<KeyType>
      , LogAllocator>
   LogHashMap;
class SharedLogParametr
{


    static LogHashMap *myhashmap;
public:
    static int pid;
    static int last_user_id;
    static int last_msg_base_type;
    static int last_msg_type;
    static LogHashMap *get_map(){
        return myhashmap;
    }

    SharedLogParametr(){}
    static void init();
    static void log(int id,std::string msg);
    static void log_line(const char file_name[],const char func_name[],int line,const char msg[]);
    static void remove(int id);
    static void clear();
};

#define LINELOG SharedLogParametr::log_line(__FILE__,__func__,__LINE__,"see line")
#endif // SHAREDMEMORYLOGER_H
