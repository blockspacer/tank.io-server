#ifndef BASESERVER_H
#define BASESERVER_H
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <set>
#include <hash_map>
#include <map>
#include <unordered_map>
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
#include <boost/circular_buffer.hpp>
#include "SharedMemoryLoger.h"
#include "base/asyncmap.h"
#include <chrono>
using namespace std::chrono;
using boost::asio::ip::udp;
using boost::asio::ip::tcp;
using namespace std;


struct BaseServer;
struct UserPort;


#define SERVER_LOG(x) x


struct Ref{
    virtual void release(){
        //delete *this;
    }
};



struct BaseJob:public Ref{
    TIME_VALUE time;
    static long long id_sequence;
    OBJ_ID id;
    bool is_canceled=false;
    bool operator<(const BaseJob &p)const{
        return time<p.time;
    }
    void init(){
        id=id_sequence++;
    }

    virtual void do_job()=0;
    static bool cmp(const BaseJob *l,const BaseJob *r){
        return l->time<r->time;
    }
};
struct BaseJobPtr{
    BaseJob *job=nullptr;
    int ptr_id=-1;
    TIME_VALUE time;
    /*bool operator<(const BaseJobPtr &p)const{
        return job->time<p.job->time;
    }/**/
    BaseJobPtr(BaseJob *ptr):job(ptr){
        ptr_id=ptr->id;
        time=ptr->time;
    }
    bool is_changed(){
        return ptr_id!=job->id;
    }

};

struct UserPort;


struct MsgBlock{
    int refrence_count=0;
    int length;
    union{
        BaseMessage msg;
        char data[1024];
    };
    void retain(){
        ++refrence_count;
    }
    void release(){
        --refrence_count;
    }
    void clone_from_msg(BaseMessage *msg,size_t length){
        //cerr<<sizeof(MsgBlock)<<endl;
        this->length=length;
        memcpy(data,msg,length);
    }
    MsgBlock():msg(){

    }
};/**/
struct MsgSendHandler:public BaseJob{
    //MsgBlock msg;
    //std::size_t length;
    BaseMessage *msg;
    MSG_ID msg_id;
    int length;
    std::size_t try_counter=0;
    std::size_t max_try_counter=4;
    UserPort *s;
    BaseServer *server;

    MsgSendHandler(){}
    MsgSendHandler(UserPort *_s,std::size_t _length);
    bool init(UserPort *s,BaseMessage *msg,std::size_t length);
    void cancele(){
        is_canceled=true;
        //msg=nullptr; TODO memory release
        // delete msg
    }
    void do_job();
    static MsgSendHandler *create(UserPort *s,BaseMessage *msg,std::size_t length);
};

struct CostumJob:public BaseJob{
    typedef std::function<void()> CallBack;
    CallBack callback;
    CostumJob(const CallBack &_callback):callback(_callback){

    }

    void cancele(){
        cerr<<"sendig mssg job canceled"<<endl;
        is_canceled=true;

    }
    void do_job();
    static CostumJob *create(const CallBack &_callback);
};
//struct session ;
struct TcpConnection;
struct DBJob:public BaseJob{
    //UserPort *user_port;
    udp::endpoint udp_connection;
    UserPort *ses;
    TcpConnection *tcp_connection;
    BaseServer *server;



    void do_job(){
        cerr<<"this function can not be call its wrong"<<endl;
    }

};
struct TcpConnection;
struct LoginJob:public DBJob{
    static LoginJob instance;
    LoginMSG login;
    shared_ptr<TcpConnection> self;
    void do_job();

    void init(TcpConnection *user_port,LoginMSG *login);
};
struct RegisterJob:public DBJob{
    static RegisterJob instance;
    boost::asio::ip::address address;
    Register register_msg;
    shared_ptr<TcpConnection> self;
    void do_job();

    void init(TcpConnection *user_port,udp::endpoint udp_connection,Register *login,const boost::asio::ip::address &address);
};

struct SetBuyItemUsed:public DBJob{
    static SetBuyItemUsed instance;
    BaseServer *server;
    int db_id;
    void do_job();
    void init(int db_id,BaseServer *server);
};

struct SaveAppVersion:public DBJob{
    static SaveAppVersion instance;
    BaseServer *server;
    USER_ID user_id;
    void do_job();
    void init(USER_ID user_id,BaseServer *server);
};


struct BigestDBJob:public DBJob{

    void do_job(){
        cerr<<"this function can not be call its wrong BigestDBJob::do_job"<<endl;
    }
    unsigned char data[1024];// in arraye lazem ast garche hich vaght estefade nemishvad am digar kelas ha az in hafeze estefade digari mikonan BigestDBJob az nazare hafeze bayad az hame bozrgtar bashad


};

struct TcpConnection : public std::enable_shared_from_this<TcpConnection>{
    static int count;
    BaseServer *server=nullptr;
    boost::shared_mutex tcp_mutex;
    UserPort *user_port=nullptr;
    tcp::socket tcp_socket;
    TcpConnection(tcp::socket socket,BaseServer *server_)
      : tcp_socket(std::move(socket)),
        server(server_)
    {
        ++count;
        cerr<<"tcp count "<<count<<endl;
    }
    ~TcpConnection();
    void tcp_send( BaseMessage*msg,size_t length);
    void do_read();
    void start()
    {
      do_read();
    }
    void close(){
        tcp_socket.close();
    }

    enum { max_length = 1024*8 };
    char data_[max_length];
};
struct UserPort//: public std::enable_shared_from_this<UserPort>
{
  BaseServer *server;
  UserData *user_data=nullptr;
  int physic_engine_version=0;
  //USER_ID user_id;
  COOCKIE cookie;
  OBJ_ID client_process_id;
  MSG_ID last_msg_id=0;
  OBJ_ID last_got_event=0;
  //vector<MSG_ID> income_msgs;
  set<OBJ_ID> incom_events;
  vector<MsgHeader*> events;
  MSG_ID msg_ids[16];
  BaseRoom *on_room=nullptr;
  TcpConnection *tcp_connection=nullptr;
  udp::endpoint udp_end_point;
  struct RequestData{
    TIME_VALUE time=0;
    OBJ_ID id=0;
    USER_ID user_id=0;
    int lig_id;
  }request;
  bool not_first_udp_ping=false;
  TIME_VALUE last_top_hundred_request=-1;
  TIME_VALUE last_hundred_request=-1;

  UserPort(BaseServer *server_):
      server(server_)
  {

  }
        ~UserPort();
  void  tcp_disconnect(TcpConnection *tcp_connection){
      if(this->tcp_connection==tcp_connection)
          this->tcp_connection=nullptr;
  }
  UserPortZip get_zip();

  bool udp_connected=false;
  void udp_end_point_set(udp::endpoint se );
  MSG_ID generate_msg_id();
  void update_id_generator(MSG_ID _id);



  const static int out_box_size=16;
  MsgSendHandler out_box[out_box_size];

  int head_of_out_box=0,tail_of_out_box=0;


  void tcp_send( BaseMessage*msg,size_t length){
      if(tcp_connection!=nullptr)
          tcp_connection->tcp_send(msg,length);
  }


  MsgSendHandler *get_reserved(){
      for(int i=0; i<out_box_size; ++i)
          if(out_box[i].is_canceled){
              //cerr<<" reserve "<<user_data->id<<" < "<<i<<endl;
              return out_box+i;
          }
      head_of_out_box=(head_of_out_box+1)%out_box_size;
      //cerr<<" reserve "<<user_data->id<<" < "<<head_of_out_box<<endl;
      return out_box+head_of_out_box;//=msgh;
  }
  void remove_from_out_box(MSG_ID id);



};

namespace std {
    template <>
        class hash<udp::endpoint>{
        public :
        size_t operator()(const udp::endpoint &x ) const;
    };
}






class myexception: public exception
{
  virtual const char* what() const throw()
  {
    return "My exception happened";
  }
} ;


struct PreRegisterData{
    static long long id_count;
    OBJ_ID id_and_public_key;
    long long time;
    OBJ_ID private_key;
    OBJ_ID result;
};

class  BaseServer
{

    size_t buffer_head=0;
    //MsgBlock *buffer;
    map<boost::asio::ip::address,int> ip_db_msg_count,ip_msg_count;
    map<long long,PreRegisterData> pre_register_data;
public:

    //boost::shared_mutex udp_mutex;
    DBAccess *db=nullptr;
    boost::asio::io_service& io_service;
    UserData *get_user_data_by_id(USER_ID user_id);
    BaseServer(boost::asio::io_service& io_service, short port);
    void run_io();
    SafeAsyncPriorityQueue<BaseJobPtr > udp_resend_msg_jobs;
    virtual void flush(){}
    bool check_msg(BaseMessage *msg,size_t lenght);

    static long long get_time();
    static long long get_random_long_long();
    virtual void first_udp_ping(UserPort *u){};
    virtual RegisterResponse *register_user(const Register *rgister_msg,std::string);
    virtual UserData *login_user(LoginMSG *msg);

    virtual void recive_msg(UserPort *s, BaseMessage *msg,size_t byte_size){}
    virtual void recive_tcp_msg(UserPort *s, BaseMessage *msg){}
    virtual void change_user_data(USER_ID){}
    virtual void use_buy_item(int db_id);
    virtual void reload_rooms();
    void set_app_version(USER_ID ui,int ap_v);
    void set_buy_item_used(int db);
    void save_user_data(USER_ID user_id);
    void push_send_lost_packet_job(BaseJob *job);
    void do_receive();
    void start_accept();

    void start(UserPort *t);
    void do_read(UserPort *t);


    void udp_send(UserPort *s ,BaseMessage *data_,std::size_t length);
    void udp_send(udp::endpoint &s,BaseMessage *data_,std::size_t length);

    void udp_send_and_push_out_box_to_handle_ACK(UserPort *s,BaseMessage *msg,std::size_t length,std::size_t try_counter=4);
    void udp_send_and_push_out_box_to_handle_ACK_with_custom_handler(UserPort *s,BaseMessage *msg,std::size_t length,std::size_t try_counter=4);

    void tcp_recive(TcpConnection *s,boost::system::error_code ec,char data_[],std::size_t length);
    void udp_recive(boost::system::error_code ec,udp::endpoint &sender_endpoint_,std::size_t bytes_recvd);

    virtual void after_register(UserPort *s,USER_ID user_id){}

    virtual void new_login(USER_ID user_id,UserPort *user_port){}
    udp::endpoint sender_endpoint_;
    enum { max_length = 2048*8};//2048*8 };
    char data_[max_length];
    map<udp::endpoint,UserPort* > ends;
    //map<UserPort *,UserPort* > tcp_ends;

    //map<USER_ID,UserPort* > users_with_id;
    UserPort* users_with_id(const USER_ID &user_id);
    void users_with_id_push(const USER_ID &user_id,UserPort *user_port);
    /*bool check_user_ports(){
        users_with_id.lock_rw();
        for(auto i=users_with_id.map.begin();i!=users_with_id.map.end(); ++i)
            if(i->second!=nullptr && i->first!=i->second->user_data->id){
                cerr<<i->second->udp_end_point<<endl;
                cerr << i->first << " <> " << i->second->user_data->id <<endl;
                return false;

            }
        users_with_id.unlock_rw();
        return true;
    }/**/

    //SafeAsyncMap<USER_ID,session*> users_with_id;
    /*unordered_map<udp::endpoint,
                   UserPort*
                   //std::hash<udp::endpoint>,
                   //std::equal_to<udp::endpoint>,
                   //std::polymorphic_allocator<std::pair<const udp::endpoint,Sender*> >
                   > ends;/**/

    long long total_time;
    boost::mutex sender_jobsMutex;
    virtual void update(long long dt){
            long long now_time=total_time=get_time();

            sender_jobsMutex.lock();
            std::queue<BaseJobPtr> local_array;
            while(!udp_resend_msg_jobs.empty()){
                BaseJobPtr job_ptr=udp_resend_msg_jobs.top();
                if(job_ptr.time<=now_time){
                    if(!job_ptr.job->is_canceled && !job_ptr.is_changed())
                        local_array.push(job_ptr);
                    else{

                    }
                    udp_resend_msg_jobs.pop();
                }else
                    break;

            }
            sender_jobsMutex.unlock();


            while(!local_array.empty()){
                local_array.front().job->do_job();
                local_array.pop();

            }


    }

    virtual void run(){
        while(true){
            int dt=10;
            update(dt);
            //boost::this_thread::sleep(1);
             boost::this_thread::sleep(boost::posix_time::milliseconds(dt));

        }
    }
    //vector<CostumJob*> data_base_jobs;
    //boost::mutex data_base_jobsMutex;
    //std::queue<BaseJob*> data_base_jobs;
    //LoginJob logins[1024*1024];
    struct DbJobManager{

    };
    queue<std::shared_ptr<DBJob>> db_jobs;

    boost::condition_variable data_base;
    mutable boost::mutex newTaskMutex;
    long long  db_query=0;
    long long login_count=0;
    virtual void db_dun();

    udp::socket socket_;
    tcp::acceptor tcp_acceptor;
    tcp::socket tcp_socket;

};
#endif // BASESERVER_H




