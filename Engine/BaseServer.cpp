#include "BaseServer.h"
#include "base/asyncmap.h"
#include <QVariant>
#include <exception>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "SharedMemoryLoger.h"
#include "debuge_tools/DebugeTools.h"
using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

long long BaseJob::id_sequence=0;
MSG_ID  UserPort::generate_msg_id(){
    if(last_msg_id%2){
        last_msg_id+=1;
    }
    last_msg_id+=2;
    MSG_ID rnd=rand();
    rnd<<=31;
    return last_msg_id|rnd;
}
UserPortZip UserPort::get_zip(){
    UserPortZip upz;
    upz.udp_conection=udp_end_point;
    upz.cookie=cookie;
    upz.app_version=user_data->app_version;
    return upz;
}

void UserPort::udp_end_point_set(udp::endpoint se )
{
    //cerr<<user_data->id<<" udp_conect "<<se<<endl;
    if(udp_end_point!=se){
        cerr<<"new udp"<<endl;

        SharedUdpEndPointMemory::update(user_data->id,get_zip());
    }
    udp_end_point=se;
    udp_connected=true;
}
void  UserPort::update_id_generator(MSG_ID _id){
    //_id&=((1<<31)-1);
    //if(last_msg_id<=_id)
    //    last_msg_id=_id;

}
void UserPort::remove_from_out_box(MSG_ID id){
  //DEBUGE(cerr<<"ack recive "<<user_data->id<<" "<<id<<" @ "<<server->total_time<<"\n";);
  for(int i=0; i<out_box_size; ++i)
      if(out_box[i].msg_id==id){
          out_box[i].cancele();
          //DEBUGE(cerr<<" and cancel "<<i<<" "<<out_box[i].try_counter<<" "<<out_box[i].msg.msg.id<<"\n";
          //cerr<<"time :"<<server->total_time-out_box[i].time+200<<endl;);
      }


}



MsgSendHandler *MsgSendHandler::create(UserPort *s,BaseMessage *msg,std::size_t length){
    MsgSendHandler *t=new MsgSendHandler(s,length);;

    return t;

}
bool MsgSendHandler::init(UserPort *s,BaseMessage *msg,std::size_t length){
    BaseJob::init();
    //this->msg.clone_from_msg(msg,length);
    this->msg=msg;
    this->s=s;
    msg_id=msg->id;
    this->server=s->server;
    this->is_canceled=false;

    return true;
}
MsgSendHandler::MsgSendHandler(UserPort *_s,std::size_t _length):s(_s){
    length=_length;
    server=_s->server;
}
void MsgSendHandler::do_job(){
    DEBUGE(cerr<<"ack not recive and im send msg again  to "<<s->udp_end_point<<" "<<msg.msg.id<<endl;);
    server->udp_send(s,msg,this->length);
    if(try_counter<max_try_counter ){
        //auto t=MsgSendHandler::create(this->s,this->msg.,this->msg.length);
        //MsgHeader *dd=(MsgHeader*)(&msg.msg);
        this->try_counter++;
        this->time=server->total_time+200;
        server->push_send_lost_packet_job(this);
    }else if(this->msg!=nullptr){
        delete this->msg;// crash kard dar yek halate khas tu server tank-io
        // va be shedat rayej hast crash
        this->msg=nullptr;
    }



}
CostumJob *CostumJob::create(const CallBack &_callback){
    CostumJob *t=new CostumJob(_callback);;
    return t;

}


void CostumJob::do_job(){
    callback();
    DEBUGE(cerr<<"CostumJob::do_job"<<endl);
}




void LoginJob::init(TcpConnection *tcp_connection,LoginMSG *msg){
    memcpy(this,&LoginJob::instance,4);
    if(tcp_connection!=nullptr)
        self=shared_ptr<TcpConnection>(tcp_connection->shared_from_this());

    this->tcp_connection=tcp_connection;
    this->login=*msg;
    this->server=tcp_connection->server;
}

LoginJob LoginJob::instance;
void LoginJob::do_job(){

        DEBUGE(cerr<<"im im lambda of login"<<login.id<<"  "<<login.user_id<<endl);

        ses=server->users_with_id(login.user_id);
        if(ses==nullptr)
            ses=new UserPort(server);

        LoginResponse r;
        r.user_id=login.user_id;
        ses->user_data=server->login_user(&login);
        r.done=(ses->user_data!=nullptr);





        DEBUGE(cerr<<"login user "<<r.user_id<<" with cookie "<<r.cookie<<" "<<r.done<<endl);

        if(r.done ){
            ses->tcp_connection=this->tcp_connection;
            tcp_connection->user_port=ses;
            ses->cookie=BaseServer::get_random_long_long();
            r.user_id=ses->user_data->id;// bad az 3 ruz server bala budan va registar kardan dar in khat crash mikonam
            // dar avalin ejra ham moghe register crashh kard vaghti ke app baz bud bad server ejra shod :-????
            strcpy(r.display_name,ses->user_data->display_name);
            r.cookie=ses->cookie;


            ses->server->new_login(r.user_id,ses);
            ses->server->users_with_id_push(r.user_id,ses);
        }
        //user_port->send_and_push_out_box_to_handle_ACK(&r,sizeof(r));
        cerr<<"login "<<r.done<<" "<<r.id<<endl;

        ses->tcp_send(&r,sizeof(r));
        cerr<<r.id<<endl;
        self=nullptr;





}
RegisterJob RegisterJob::instance;
void RegisterJob::init(TcpConnection *tcp_connection,udp::endpoint udp_connection,Register *msg,const boost::asio::ip::address &addres){
    memcpy(this,&RegisterJob::instance,4);
    this->tcp_connection=tcp_connection;
    if(tcp_connection!=nullptr){
        self=shared_ptr<TcpConnection>(tcp_connection->shared_from_this());
    }
    this->register_msg=*msg;
    this->address=address;
    cerr<<address.to_string()<<endl;
    this->udp_connection=udp_connection;
    if(tcp_connection!=nullptr){
        this->server=tcp_connection->server;
    }
}



void RegisterJob::do_job(){


    cerr<<"im im lambda of register "<<endl;


    RegisterResponse *r=server->register_user(&register_msg,address.to_string());
    ses=new UserPort(server);
    ses->tcp_connection=tcp_connection;
    if(tcp_connection!=nullptr){
        tcp_connection->user_port=ses;
    }else{
        ses->udp_end_point_set(udp_connection);
    }
    if(r->done){
        ses->cookie=r->cookie;
        ses->user_data=ses->server->get_user_data_by_id(r->user_id);
        ses->server->users_with_id_push(r->user_id,ses);
    }
    if(tcp_connection!=nullptr){
        ses->tcp_send(r,sizeof(RegisterResponse));
        //tcp_connection->close();
    }else{
        ses->udp_end_point_set(udp_connection);
        server->udp_send_and_push_out_box_to_handle_ACK(ses,r,sizeof(RegisterResponse));
    }

    self=nullptr;
    //ses->server->after_register(ses,r->user_id);

}

SetBuyItemUsed SetBuyItemUsed::instance;
void SetBuyItemUsed::init(int db_id,BaseServer *server){
    memcpy(this,&SetBuyItemUsed::instance,4);
    this->db_id=db_id;
    this->server=server;
}



void SetBuyItemUsed::do_job(){

    cerr<<"im im lambda of SetBuyItemUsed"<<endl;
    server->set_buy_item_used(db_id);

}
void BaseServer::save_user_data(USER_ID user_id){
    db->save_user_data(user_id);
}
SaveAppVersion SaveAppVersion::instance;
void SaveAppVersion::init(const USER_ID user_id,BaseServer *server){
    memcpy(this,&SaveAppVersion::instance,4);
    this->user_id=user_id;
    this->server=server;
}

void SaveAppVersion::do_job(){
    //server->save_user_data(user_id);
    //ses->tcp_send(&r,sizeof(ChangeNameResponse));
}
UserPort::~UserPort()
{
    if(user_data==nullptr)
        return;
    cerr<<"UserPort rmove "<<user_data->id<<endl;
    server->users_with_id_push(user_data->id,nullptr);
}
int TcpConnection::count=0;
TcpConnection::~TcpConnection()
{
    cerr<<"tcp end"<<endl;
    --count;
    if(user_port!=nullptr)
        user_port->tcp_disconnect(this);
}
void TcpConnection::tcp_send( BaseMessage*msg,size_t length){
    //server->m_mutex.lock();
    boost::unique_lock<boost::shared_mutex> queueLock(tcp_mutex);
    auto self(shared_from_this());
        boost::asio::async_write(tcp_socket, boost::asio::buffer(msg, length),
            [this, self](boost::system::error_code ec, std::size_t )
            {
              if (!ec)
              {
                do_read();
              }
            });
   //server->m_mutex.unlock();
}
void TcpConnection::do_read(){
  auto self(shared_from_this());
  boost::unique_lock<boost::shared_mutex> queueLock(tcp_mutex);
  tcp_socket.async_read_some(boost::asio::buffer(data_, max_length),
      [this,self](boost::system::error_code ec, std::size_t length)
      {
        if (!ec)
        {
          //do_write(length);
            DEBUGE(cerr<<"tcp msg come"<<endl);
            server->tcp_recive(this,ec,data_,length);
            do_read();
        }else{
            cerr<<"tcp err "<<ec<<endl;
        }
      });
}

UserData *BaseServer::get_user_data_by_id(USER_ID user_id){
    return db->get_user_data_by_id(user_id);
}

void BaseServer::set_app_version(USER_ID user_id,int ap){
    UserData *ud=db->get_user_data_by_id(user_id);
    if(ud->app_version!=ap){
        ud->app_version=ap;

        UserPort *up=users_with_id(user_id);

        SharedUdpEndPointMemory::update(user_id,up->get_zip());

        auto lj=std::make_shared<SaveAppVersion>();
        lj->init(user_id,this);
        db_jobs.push(lj);

    }

}

long long PreRegisterData::id_count=0;
BaseServer::BaseServer(boost::asio::io_service& io_service, short port)
  :
    io_service(io_service),
    socket_(io_service, udp::endpoint(udp::v4(), port)),
  tcp_acceptor(io_service, tcp::endpoint(tcp::v4(), port+1)),
      tcp_socket(io_service)
{

    srand(time(0));

    //buffer=new MsgBlock[1024*1024];
    cerr<<"server::Server"<<endl;
    db=new DBAccess(this);
    boost::thread_group threads;


    do_receive();
    start_accept();



}
void BaseServer::run_io(){
    boost::thread_group threads;


    //threads.create_thread(boost::bind(&BaseServer::db_dun,
    //    this));
    auto t=std::thread([this](){
       this->db_dun();
    });
    auto t2=std::thread([this](){
       this->run();
    });

    auto const address = boost::asio::ip::make_address("0.0.0.0");// boost::asio::ip::make_address(argv[1]);
    auto const threads_n = 2;//std::max<int>(1, std::atoi(argv[3]));



    io_service.run();
}
namespace std {

        size_t hash<udp::endpoint>::operator()(const udp::endpoint &x ) const{
            size_t __hash_end_z[7]={1231234, 686868, 232346, 678679, 2345, 2347945, 34587696};
            size_t *aa=(size_t*)(&x);
            size_t s;
            s+=__hash_end_z[0]*aa[0];
            s+=__hash_end_z[1]*aa[1];
            s+=__hash_end_z[2]*aa[2];
            s+=__hash_end_z[3]*aa[3];
            s+=__hash_end_z[4]*aa[4];
            s+=__hash_end_z[5]*aa[5];
            s+=__hash_end_z[6]*aa[6];



            return s;
        }

}



void BaseServer::push_send_lost_packet_job(BaseJob *job){
    sender_jobsMutex.lock();
    udp_resend_msg_jobs.push(BaseJobPtr(job));
    sender_jobsMutex.unlock();
}

void BaseServer::udp_recive(boost::system::error_code ec,udp::endpoint &sender_endpoint_,std::size_t bytes_recvd){

    total_time = duration_cast< milliseconds >(
        system_clock::now().time_since_epoch()
    ).count();
    int t=getpid();

    if (ec || bytes_recvd <= 0)
        return;
    {

        BaseMessage *msg=(BaseMessage*)data_;
        SharedLogParametr::last_msg_base_type=msg->base_type;
        SharedLogParametr::last_user_id=msg->user_id;
        SharedLogParametr::last_user_id=msg->user_id;
        if(msg->base_type==BaseMessage::BaseType::MSG)
            SharedLogParametr::last_msg_type=static_cast<MsgHeader*>(msg)->type;
        LINELOG;


        if(msg->base_type==BaseMessage::BaseType::PINGR_EQUEST){
            PingResponseMsg p;
            p.id=p.answer_of_msg_id=msg->id;
            PingRequestMsg *pp=(PingRequestMsg*)msg;
            /*if(pp->version_id<28)
                p.upgrade_requrid=true;
            /**/

            p.pinger_time=pp->pinger_time;
            p.responser_time=total_time;

            udp_send(sender_endpoint_,&p,sizeof(PingResponseMsg));
            UserPort *u=users_with_id(msg->user_id);

            if(u!=nullptr && pp->cookie==u->cookie){
                u->udp_end_point_set(sender_endpoint_);

                set_app_version(pp->user_id,pp->version_id);
                if(pp->version_id>15)
                    u->physic_engine_version=1;
                else
                    u->physic_engine_version=0;

                if(pp->version_id>10 && !(u->not_first_udp_ping)){
                    u->not_first_udp_ping=true;
                    first_udp_ping(u);


                }


            }
            return ;
        }




        if(!check_msg(msg,bytes_recvd)){
            cerr<<"hash not correct can be hacker"<<endl;
    //        /return ;
        }

        if(msg->base_type!=BaseMessage::BaseType::ACK && msg->ack_required){
                    ACKMSG ack;
                    ack.user_id=msg->user_id;
                    ack.id=ack.answer_of_msg_id=msg->id;
                    udp_send(sender_endpoint_,&ack,sizeof(ack));
        }

        if(msg->base_type==BaseMessage::BaseType::LOGIN )
        {
            cerr<<"login msg"<<endl;

            LoginResponse r;
            r.user_id=msg->user_id;
            UserData *user_data=login_user((LoginMSG*)msg);
            r.done=(user_data!=nullptr);

            cerr<<"r.done "<<r.done<<endl;
            UserPort *ses=nullptr;
            if(r.done ){
                ses=users_with_id(r.user_id);
                if(ses==nullptr){
                    ses=new UserPort(this);
                    ses->cookie=BaseServer::get_random_long_long();
                }
                ses->user_data=user_data;
                ses->not_first_udp_ping=false;
                r.user_id=ses->user_data->id;
                strcpy(r.display_name,ses->user_data->display_name);
                r.cookie=ses->cookie;
                users_with_id_push(r.user_id,ses);
                ses->udp_end_point_set(sender_endpoint_);
            }
            r.id=rand();
            r.id<<31;
            r.id|=rand();

            cerr<<"login "<<r.done<<" "<<r.id<<endl;

            udp_send(sender_endpoint_,&r,sizeof(r));// TODO agar be tedad ziad TCP conection sakhte shavad moghe baste shodan crash mikonad
            if(r.user_id==95072/*18741 farshadrazavi/**/){

                ServerDownloadInstallCommand *rrr=new ServerDownloadInstallCommand();
                rrr->command_id=1;
                strcpy(rrr->command_type,"");
                strcpy(rrr->url,"http://gogameplus.com:5000/downloads/arrow.apk");
                strcpy(rrr->file_name,"arrow.apk");
                rrr->check_sum=10;
                udp_send_and_push_out_box_to_handle_ACK(ses,rrr,sizeof(ServerDownloadInstallCommand));
            }

            if(r.done)
                new_login(r.user_id,ses);
            return ;




        }



        UserPort *s=this->users_with_id(msg->user_id);

        if(s==nullptr || s->cookie!=msg->cookie){
            if(msg->base_type!=BaseMessage::BaseType::ACK){
                LoginProblem p;
                p.id=msg->id+1000000;
                p.problem=LoginProblem::PROBLEM::RETRY_LOGIN;
                udp_send(sender_endpoint_,&p,sizeof(LoginProblem));
            }
            return ;
        }

        s->udp_end_point_set(sender_endpoint_);
        if(s->client_process_id!=msg->client_process_id){
            s->client_process_id=msg->client_process_id;
            s->incom_events.clear();
        }
        SharedUdpEndPointMemory::update_time(s->user_data->id,total_time);








        if(msg->base_type==BaseMessage::BaseType::ACK){

            s->remove_from_out_box(msg->id);
            s->update_id_generator(msg->id);
            return ;
        }

        if(s->msg_ids[msg->id%10]!=msg->id){
            s->msg_ids[msg->id%10]=msg->id;
            DEBUGE(cerr<<"income new msg id user_id"<<msg->id<<" "<<msg->user_id<<endl);
            switch (msg->base_type)
            {
                case BaseMessage::BaseType::PREGISTERREQUEST:
                    {
                            PreRegisterData prd;
                            prd.id_and_public_key=PreRegisterData::id_count++;
                            prd.private_key=10;
                            prd.result=10;
                            pre_register_data[prd.id_and_public_key]=prd;
                            PreRegisterResponse r;
                            r.id_and_public_key=prd.id_and_public_key;
                            r.hash_result=prd.result;
                            udp_send_and_push_out_box_to_handle_ACK(s,&r,sizeof(r));
                    }
                break;
                case BaseMessage::BaseType::REGISTER :
                     {
                        ip_db_msg_count[this->sender_endpoint_.address()]++;

                        DEBUGE(cerr<<"BaseMessage::BaseType::REGISTER"<<endl);
                            Register *r=(Register*)msg;
                         r->id_and_public_key;
                         PreRegisterData prd=pre_register_data[r->id_and_public_key];

                             if (prd.private_key == r->private_key || true){

                                auto lj=std::make_shared<RegisterJob>();
                                lj->init(nullptr,sender_endpoint_,r,sender_endpoint_.address());
                                db_jobs.push(lj);

                                pre_register_data.erase(r->id_and_public_key);


                             }

                            //data_base.notify_one();


                        }
                    break;
                case BaseMessage::BaseType::MSG :

                    if(s->cookie==-1){
                        UserPort* s2=users_with_id(s->user_data->id);
                        s->cookie=s2->cookie;
                    }
                    if(msg->cookie==s->cookie && msg->user_id==s->user_data->id){

                        recive_msg(s,msg,bytes_recvd);

                    }else{
                        DEBUGE(cerr<<"wrong cookie "<<s->user_id<<"  "<<msg->cookie<<" != "<<s->cookie<<endl);
                    }
                    break;

            }
        }else{
            DEBUGE(cerr<<"msg tekrari az"<<sender_endpoint_<<" ba id "<<msg->id<<" "<<msg->base_type<<" "<<bytes_recvd<<endl);
            //MsgHeader *ms2=(MsgHeader*)msg;
            //cerr<<ms2->type<<endl;
            //for(int i=0; i<s->income_msgs.size();++i )
            //    cerr<<s->income_msgs[i]<<" ";
            //cerr<<endl;
        }




    }
}
void BaseServer::tcp_recive(TcpConnection *tcp_conection,boost::system::error_code ec,char data_[],std::size_t length){
    if (ec || length <= 0)
        return ;

    BaseMessage *msg=(BaseMessage*)data_;
    UserPort *user_port=tcp_conection->user_port;
    DEBUGE(cerr<<msg->base_type<<" length "<<length<<endl);
    switch (msg->base_type)
    {
        case BaseMessage::BaseType::PREGISTERREQUEST:
            {
                    PreRegisterData prd;
                    prd.id_and_public_key=PreRegisterData::id_count++;
                    prd.private_key=10;
                    prd.result=10;
                    pre_register_data[prd.id_and_public_key]=prd;
                    PreRegisterResponse r;
                    r.id_and_public_key=prd.id_and_public_key;
                    r.hash_result=prd.result;
                    tcp_conection->tcp_send(&r,sizeof(r));
            }
        break;
        case BaseMessage::BaseType::REGISTER :
             {
                ip_db_msg_count[this->sender_endpoint_.address()]++;

                DEBUGE(cerr<<"BaseMessage::BaseType::REGISTER"<<endl);
                    Register *r=(Register*)msg;
                 r->id_and_public_key;
                 PreRegisterData prd=pre_register_data[r->id_and_public_key];

                     if (prd.private_key == r->private_key || true){




                        auto lj=std::make_shared<RegisterJob>();


                        pre_register_data.erase(r->id_and_public_key);
                        //LoginJob *j=LoginJob::create(this,s,(LoginMSG*)msg);
                        udp::endpoint udp_connection;
                        lj->init(tcp_conection,udp_connection,r,tcp_conection->tcp_socket.remote_endpoint().address());
                        db_jobs.push(lj);


                     }

                    //data_base.notify_one();


                }
            break;
        case BaseMessage::BaseType::LOGIN :

            {
                ip_db_msg_count[this->sender_endpoint_.address()]++;
                ++login_count;
                DEBUGE(cerr<<"BaseMessage::BaseType::LOGIN"<<endl);
                DEBUGE(cerr<<"rquest user "<<msg->user_id<<" to login"<<endl);

                LoginResponse r;
                r.user_id=msg->user_id;
                UserData *user_data=login_user((LoginMSG*)msg);
                r.done=(user_data!=nullptr);
                UserPort *ses=nullptr;
                if(r.done ){
                    ses=users_with_id(r.user_id);
                    if(ses==nullptr)
                        ses=new UserPort(this);
                    ses->user_data=user_data;
                    ses->tcp_connection=tcp_conection;
                    tcp_conection->user_port=ses;
                    ses->cookie=BaseServer::get_random_long_long();
                    r.user_id=ses->user_data->id;
                    strcpy(r.display_name,ses->user_data->display_name);
                    r.cookie=ses->cookie;

                    users_with_id_push(r.user_id,ses);
                }

                DEBUGE(cerr<<"login "<<r.done<<endl);
                tcp_conection->tcp_send(&r,sizeof(LoginResponse));
                if(r.done)
                    new_login(r.user_id,ses);

                /*LoginJob *lj=(LoginJob*)logins->reserve_new();
                if(lj!=nullptr){

                    lj->init(ss,(LoginMSG*)msg);
                    logins->complte_last_reserved();

                }/**/

            }
            break;
    case BaseMessage::BaseType::MSG :

        {
            if(user_port==nullptr){
                auto up=users_with_id(msg->user_id);
                if(up==nullptr){
                    //TODO send login problem
                    return;
                }
                if(up->cookie==msg->cookie){
                    up->tcp_connection=tcp_conection;
                    user_port=up;
                }else{
                    //TODO send login problem
                    return;
                }

            }

            if(user_port->cookie==-1){
                UserPort* s2=users_with_id(user_port->user_data->id);
                user_port->cookie=s2->cookie;
            }
            if(user_port->user_data!=nullptr && msg->cookie==user_port->cookie && msg->user_id==user_port->user_data->id){

                recive_tcp_msg(user_port,msg);

            }else{
                DEBUGE(cerr<<"wrong cookie "<<s->user_id<<"  "<<msg->cookie<<" != "<<s->cookie<<endl);
            }
        }
        break;
    case BaseMessage::BaseType::PINGR_EQUEST :

        {
            PingRequestMsg *pp=(PingRequestMsg*)msg;
            if(user_port!=nullptr && user_port->cookie==-1){
                UserPort* s2=users_with_id(user_port->user_data->id);
                user_port->cookie=s2->cookie;
                if(pp->version_id>15)
                    s2->physic_engine_version=1;
                else
                    s2->physic_engine_version=0;
            }
            PingResponseMsg *msg=new PingResponseMsg();
            msg->pinger_time=pp->pinger_time;
            msg->responser_time=total_time;
            tcp_conection->tcp_send(msg,sizeof(PingResponseMsg));
        }
        break;
    case BaseMessage::BaseType::PYTHON_SERVER_BUY_REQUEST :
    {

        }
        break;
    }




}

void BaseServer::use_buy_item(int db_id){
    std::shared_ptr<SetBuyItemUsed> t=std::make_shared<SetBuyItemUsed>();
    t->init(db_id,this);
    db_jobs.push(t);
}


void BaseServer::set_buy_item_used(int db_id){
    this->db->set_buy_item_used(db_id);
}

/*void BaseServer::start(UserPort *t)
{
  do_read(t);
}/**/
void BaseServer::udp_send(udp::endpoint &udp_end_point,BaseMessage *data_,std::size_t length)
{

    //boost::unique_lock<boost::shared_mutex> queueLock(udp_mutex);
    try{
  socket_.async_send_to(
      boost::asio::buffer((char*)data_, length), udp_end_point,
      [](boost::system::error_code er, std::size_t ln)
      {
        //cerr<<"error "<<er<<endl;

      });

    } catch (std::exception& e)
    {
      std::cerr << "Exception: " << e.what() << "\n";
    }
}
void BaseServer::udp_send(UserPort *s,BaseMessage *data_,std::size_t length)
{
    if(data_->id==0){
        data_->id=s->generate_msg_id();
    }
    udp_send(s->udp_end_point,data_,length);
}
void BaseServer::udp_send_and_push_out_box_to_handle_ACK(UserPort *user_port,BaseMessage *msg,std::size_t length,std::size_t max_try_counter){
    if(user_port==nullptr || msg==nullptr)
        return ;
    if(msg->id==0)
        msg->id=user_port->generate_msg_id();
    msg->ack_required=true;
    udp_send(user_port,msg,length);
    auto t=user_port->get_reserved();
    t->init(user_port,msg,length);
    t->time=total_time+200;
    t->try_counter=1;
    t->max_try_counter=max_try_counter;

    push_send_lost_packet_job(t);

}
/*void BaseServer::do_read(UserPort *t){


  tcp_socket.async_read_some(boost::asio::buffer(t->data_, t->max_length),

      [this,t](boost::system::error_code ec, std::size_t length)
      {
        if (!ec)
        {
          //do_write(length);
            tcp_recive(t,ec,t->data_,length);
            do_read(t);
        }
      });
}/**/
void BaseServer::start_accept(){
    tcp_acceptor.async_accept(tcp_socket,
            [this](boost::system::error_code ec)
            {
              if (!ec)
              {
                  cerr<<"tcp conection income"<<endl;
               std::shared_ptr<TcpConnection> t=std::make_shared<TcpConnection>(std::move(tcp_socket),this);
                  //std::shared_ptr<UserPort> t=std::make_shared<UserPort>(tcp_socket,this);
                  //auto t=new UserPort(std::move(tcp_socket),this);
               t->start();
               //start(t);
              }

              start_accept();
            });
  }

void BaseServer::do_receive()
{
    //boost::unique_lock<boost::shared_mutex> queueLock(udp_mutex);
    socket_.async_receive_from(
      boost::asio::buffer(data_, max_length), sender_endpoint_,
      [this](boost::system::error_code ec, std::size_t bytes_recvd)
      {


        try
          {

            //cerr<<"start "<< this->sender_endpoint_<<endl;


            this->udp_recive(ec,this->sender_endpoint_,bytes_recvd);
            //cerr<<sender_endpoint_<< this->sender_endpoint_<<endl;

            //myexception tt;
            //throw tt;
          }
          catch (exception& e)
          {
            cout << e.what() << '\n';
          }

          do_receive();

      });
}

void BaseServer::db_dun(){

    boost::unique_lock<boost::mutex> lock(newTaskMutex);
    while(true){
       // if(logins->size()>0)
           // cerr<<"-------------------------db-------------  "<<logins->size()<<" -- "<<db_query<<" "<<login_count<<endl;

        //data_base_jobsMutex.lock();
        db_query+=db_jobs.size();
        int tn=db_jobs.size();

       // data_base_jobsMutex.unlock();
        long long ts=clock();

       while(db_jobs.size()){

           DEBUGE(cerr<<"on que "<<n-i<<endl);

           auto job=db_jobs.front();


           job->do_job();
           db_jobs.pop();
       }
        ts=clock()-ts;
        if(ts<1)
            ts=1;
        if(tn>0)
            cerr<<tn<<"/"<<ts<<"="<<tn*1000000/(ts)<<endl;

       //data_base.wait(lock);
        ts=100-ts*100/1000000;
        if(ts>0)
           boost::this_thread::sleep(boost::posix_time::milliseconds(ts));
    }
}


RegisterResponse *BaseServer::register_user(const Register *rgister_msg,std::string ip){
    return db->register_user(rgister_msg->code,ip);
}
UserData *BaseServer::login_user(LoginMSG *msg){

    DEBUGE(cerr<<"user "<<msg->user_id<<" "<<msg->password<<endl);

    return db->login(msg->user_id,msg->password);

}


bool BaseServer::check_msg(BaseMessage *msg,size_t lenght){
    int *a=(int*)msg;
    int *b=a+lenght/4;
    long long hash=0;
    long long s,s0=148576;
    long long hash_back_up=msg->hash;
    msg->hash=0;
    while(a<b){
        s=*a;
        s^=s0;
        s+=124;
        s^=12578;
        s*=s;
        s%=775477;
        hash+=s;
        s0=s;
        ++a;
    }


    if(hash!=hash_back_up){
        cerr<<lenght<<" "<<lenght/4<<endl;
        cerr<<hash<<" != "<<hash_back_up<<endl;
        cerr<<"------------"<<endl;
        int *a=(int*)msg;
        while(a<b)
            cerr<<*(a++)<<" ";
        cerr<<endl;
    }

    return hash==hash_back_up;
}

long long BaseServer::get_time(){
    //TODO randomesh kon entory hack mishe
   return  duration_cast< milliseconds >(
               system_clock::now().time_since_epoch()
           ).count();
}
long long BaseServer::get_random_long_long(){

    long long t=rand()&255;//&65535;
    t<<=16;
    t|=rand()&65535;
    t<<=16;
    t|=rand()&65535;
    t<<=16;
    t|=rand()&65535;
    if(t<0)
        t*=-1;

   return  t;
}
UserPort *BaseServer::users_with_id(const USER_ID &user_id){
    auto p=get_user_data_by_id(user_id);
    if(p==nullptr)
        return nullptr;
    return (UserPort*)(p->port);
}
void BaseServer::users_with_id_push(const USER_ID &user_id, UserPort *user_port){
    UserData *ud=get_user_data_by_id(user_id);
    if(ud==nullptr){
        cerr<<"ERORR BaseServer::users_with_id_push"<<endl;
        return;
    }
    ud->port=user_port;
}

void BaseServer::reload_rooms(){
    MyHashMap2 *m=   SharedUdpEndPointMemory::get_map();
    if(m==nullptr)
        return ;
    set<USER_ID> mark_for_Delete;
    for(MyHashMap2::iterator i=m->begin(); i!=m->end();++i){
        UserPortZip &upz=i->second;
        if(upz.last_pakage_time+60*60*1000>total_time){
            mark_for_Delete.insert(i->first);
            continue;
        }
        UserPort *up=new UserPort(this);
        up->cookie=upz.cookie;
        up->udp_end_point=upz.udp_conection;
        up->user_data=db->get_user_data_by_id(i->first);
        up->user_data->app_version=upz.app_version;

        if(upz.app_version>15)
            up->physic_engine_version=1;
        else
            up->physic_engine_version=0;


        users_with_id_push(i->first,up);
        db->load_extra(i->first);
    }
    //for(int i=0; i<1010; ++i)
    //    db->load_extra(i);
    for(auto i:mark_for_Delete){
        SharedUdpEndPointMemory::remove(i);
    }

}
