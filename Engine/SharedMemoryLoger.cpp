#include "SharedMemoryLoger.h"
#include "BaseServer.h"
using namespace boost::interprocess;
SharedMemoryLoger::SharedMemoryLoger()
{




}
void SharedMemoryLoger::init_memory(){
    //Remove shared memory on construction and destruction
    struct shm_remove
    {
       shm_remove() { shared_memory_object::remove("MySharedMemory"); }
       ~shm_remove(){ shared_memory_object::remove("MySharedMemory"); }
    };//remover;
    cerr<<"remove"<<endl;

    //Create a shared memory object.
    shared_memory_object *_shm;

    mapped_region region;
    try{
      _shm=new shared_memory_object(create_only, "MySharedMemory", read_write);
      shared_memory_object &shm=*_shm;
      shm.truncate(sizeof(MSGS)+10);
      region=mapped_region(shm, read_write);
      s=(MSGS*)region.get_address();
      s->clear();
    }catch(std::exception& e)
         {
           _shm=new shared_memory_object(open_only, "MySharedMemory", read_write);
           std::cerr << "Exception: " << e.what() << "\n";

           shared_memory_object &shm=*_shm;
           shm.truncate(sizeof(MSGS)+10);
           region=mapped_region(shm, read_write);
           s=(MSGS*)region.get_address();
           cerr<<s->completed_size()<<endl;

         }
    for(int i=0; i<100000000;++i){
        //boost::this_thread::sleep(boost::posix_time::milliseconds(100));
         auto msg=s->reserve_new<LogMsg>();
        if(msg==nullptr){
            cerr<<"buffer por shod";
            boost::this_thread::sleep(boost::posix_time::milliseconds(10));
            continue;
        }
        msg->id=i;
        msg->type=i+10;
        s->complte_last_reserved();
    }

}

void SharedMemoryLoger::use_memory(){
    {
        cerr<<"child"<<endl;
       //Open already created shared memory object.
       shared_memory_object shm (open_only, "MySharedMemory", read_write);
        cerr<<"oepn shared"<<endl;
       //Map the whole shared memory in this process
       mapped_region region(shm, read_write);

       //Check that memory was initialized to 1
       //char *mem = static_cast<char*>(region.get_address());

       s=(MSGS*)region.get_address();
       cerr<<"n = "<<s->completed_size()<<endl;
       auto t=clock();
       while(true){
             //boost::this_thread::sleep(boost::posix_time::milliseconds(100));
           if(s->completed_size()==0){
               cerr<<"empty"<<endl;
               //break;
               boost::this_thread::sleep(boost::posix_time::milliseconds(10));
               continue;
           }
           //cerr<<s->completed_size()<<" >> ";
           if(s->completed_size()<0){
               cerr<<"khak alam"<<endl;
           }
           LogMsg *msg=s->front();
           cerr<<msg->id<<" "<<msg->type<<endl;
           s->pop();
       }
       cerr<<clock()-t<<endl;



    }
}

MyHashMap *SharedRoomMemory::myhashmap=nullptr;

void SharedRoomMemory::init(){
    //using namespace boost::interprocess;

    /*struct shm_remove
    {
       shm_remove() { shared_memory_object::remove("rRoomSharedMemory"); }
       ~shm_remove(){ shared_memory_object::remove("rRoomSharedMemory"); }
    };/**/
    managed_shared_memory *_shm;

    try{
      _shm=new managed_shared_memory(boost::interprocess::open_or_create, "tank-RoomSharedMemory", 100000000);



    }catch(std::exception& e)
         {

            cerr<<"error dad ke"<<endl;
            return ;

         }


    long long time=BaseServer::get_time();
    myhashmap=_shm->find_or_construct<MyHashMap>("tank-MyHashMap3")  //object name
          ( 3, boost::hash<int>(), std::equal_to<int>()                  //
          , _shm->get_allocator<ValueType>());                         //allocator instance

        cerr<<"room sizee "<<myhashmap->size()<<endl;
        for(MyHashMap::iterator i=myhashmap->begin(); i!=myhashmap->end();++i){
            Room::RoomData &rd=i->second;

        }

}

void SharedRoomMemory::update(int id,const Room::RoomData &msg){
    if(myhashmap==nullptr)
        return;
    /*if(msg.game_section.winer!=0)
        myhashmap->erase(id);
    else/**/
    (*myhashmap)[id]=msg;
}
void SharedRoomMemory::remove(int id){
    if(myhashmap!=nullptr){
        myhashmap->erase(id);
        cerr<<"remove "<<id<<endl;
    }
}
void SharedRoomMemory::clear(){
    if(myhashmap!=nullptr)
        myhashmap->clear();
}




///
MyHashMap2 *SharedUdpEndPointMemory::myhashmap=nullptr;
SpeedTestMap *SharedUdpEndPointMemory::forTest=nullptr;
void SharedUdpEndPointMemory::init(){
    //using namespace boost::interprocess;

    /*struct shm_remove
    {
       shm_remove() { shared_memory_object::remove("rRoomSharedMemory"); }
       ~shm_remove(){ shared_memory_object::remove("rRoomSharedMemory"); }
    };/**/
    managed_shared_memory *_shm;

    try{
      _shm=new managed_shared_memory(boost::interprocess::open_or_create, "tank-UserPortZip", 100000000);



    }catch(std::exception& e)
         {
            cerr<<"error dad ke"<<endl;
            return ;

         }


    long long time=BaseServer::get_time();
    myhashmap=_shm->find_or_construct<MyHashMap2>("tank-UserPortZip")  //object name
          ( 3, boost::hash<int>(), std::equal_to<int>()                  //
          , _shm->get_allocator<ValueType>());                         //allocator instance

        cerr<<"room sizee "<<myhashmap->size()<<endl;
        for(MyHashMap2::iterator i=myhashmap->begin(); i!=myhashmap->end();++i){
            UserPortZip &rd=i->second;

        }

        {
            forTest=_shm->find_or_construct<SpeedTestMap>("tank-test-ABC")( 3, boost::hash<int>(), std::equal_to<int>()                  //
                                                                             , _shm->get_allocator<SpeedTest>());  ;

            SpeedTest &A=(*forTest)[0];
            SpeedTest &B=(*forTest)[1];
            SpeedTest &C=(*forTest)[2];
            A.init();
            B.init();
            C.init();

            //auto B=forTest[1];
            //auto C=forTest[2];
        }

}

void SharedUdpEndPointMemory::update(int id,const UserPortZip &msg){
    if(myhashmap==nullptr)
        return;
    (*myhashmap)[id]=msg;
}
void SharedUdpEndPointMemory::update_time(int id, TIME_VALUE time){
    if(myhashmap==nullptr)
        return;
    UserPortZip &upz=(*myhashmap)[id];
    upz.last_pakage_time=time;
}

void SharedUdpEndPointMemory::remove(int id){
    if(myhashmap!=nullptr){
        myhashmap->erase(id);
    }
}
void SharedUdpEndPointMemory::clear(){
    if(myhashmap!=nullptr)
        myhashmap->clear();
}

///
LogHashMap *SharedLogParametr::myhashmap=nullptr;



int SharedLogParametr::pid;
int SharedLogParametr::last_user_id;
int SharedLogParametr::last_msg_base_type;
int SharedLogParametr::last_msg_type;


void SharedLogParametr::init(){
    SharedLogParametr::pid=getpid();
    managed_shared_memory *_shm;

    try{
      _shm=new managed_shared_memory(boost::interprocess::open_or_create, "tank-UserPortZip", 100000000);



    }catch(std::exception& e)
         {
            cerr<<"error dad ke"<<endl;
            return ;

         }


    long long time=BaseServer::get_time();
    myhashmap=_shm->find_or_construct<LogHashMap>("tank-LogMemory3")  //object name
          ( 3, boost::hash<int>(), std::equal_to<int>()                  //
          , _shm->get_allocator<ValueType>());                         //allocator instance

        for(LogHashMap::iterator i=myhashmap->begin(); i!=myhashmap->end();++i){
            //UserPortZip &rd=i->second;

        }

}

void SharedLogParametr::log(int id,std::string msg){
    if(myhashmap==nullptr)
        return;
    LogParametr &p=(*myhashmap)[id];
    p.pid=pid;
    strcpy(p.data,msg.c_str());
}
void SharedLogParametr::log_line(const char file_name[],const char func_name[],int line,const char msg[]){
    LogParametr l;
    l.id=0;
    l.pid=pid;
    l.line=line;
    strcpy(l.data,msg);
    strcpy(l.file,file_name);
    strcpy(l.func,func_name);
    (*myhashmap)[l.id]=l;
}

void SharedLogParametr::remove(int id){
    if(myhashmap!=nullptr){
        myhashmap->erase(id);
    }
}
void SharedLogParametr::clear(){
    if(myhashmap!=nullptr)
        myhashmap->clear();
}
