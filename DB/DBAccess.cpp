#include "DBAccess.h"
#include "Engine/BaseServer.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "Classes/MainServer.h"
#include "debuge_tools/DebugeTools.h"

/*void sql_c_c(){
    try {

        //sql::mysql::MySQL_Driver *driver1=new sql::Driver();
        sql::Driver * driver = sql::mysql::get_driver_instance();
        sql::Connection *con;// = driver->connect(url, user, pass);

        sql::PreparedStatement  *prep_stmt = con->prepareStatement("INSERT INTO User(id, code) VALUES (?, ?)");

        prep_stmt->setInt(1, 1);
        prep_stmt->setString(2, "a");
        prep_stmt->execute();

        prep_stmt->setInt(1, 2);
        prep_stmt->setString(2, "b");
        prep_stmt->execute();

        delete prep_stmt;
        delete con;

    } catch (sql::SQLException &e) {

      cout << "# ERR: SQLException in " << __FILE__;
      cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;

      cout << "# ERR: " << e.what();
      cout << " (MySQL error code: " << e.getErrorCode();
      cout << ", SQLState: " << e.getSQLState() << " )" << endl;
  }
}/**/


void UserData::set_app_version(int a){

}


void UserData::close_extra(){
    delete extra_data;
    extra_data=nullptr;
}

void DBAccess::connect(){

    string EXAMPLE_HOST="192.168.1.4",EXAMPLE_DB="super_goal";
    EXAMPLE_HOST="192.168.1.3";
    EXAMPLE_HOST="localhost";
    std::string EXAMPLE_USER ="super_goal1";
    std::string EXAMPLE_PASS ="amarokgames33";
    std::ifstream fin("/var/game_server/db.config");
    {
        fin>>EXAMPLE_USER;
        fin>>EXAMPLE_PASS;
    }
    std::cout<<"db user = "<<EXAMPLE_USER<<endl;

    string url(EXAMPLE_HOST);
    const string user(EXAMPLE_USER);
    const string pass(EXAMPLE_PASS);
    const string database(EXAMPLE_DB);

    //QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");

    db.setHostName(url.c_str());
    db.setDatabaseName(database.c_str());
    db.setUserName(user.c_str());
    db.setPassword(pass.c_str());

    if (!db.open()) {
        cerr << "Database error occurred";
    }
}

DBAccess::DBAccess(BaseServer *_server)
{
    sorted_list.db=this;
    server=_server;
    connect();



{
             string ss="INSERT INTO ttt (id,code) VALUES (10,'salam')";
             //ss="ALTER TABLE test(id integer, name integer);";
             ss="SELECT "
                "id,code "
                "FROM ttt ";
            QSqlQuery query;
           query.exec(ss.c_str());
           cerr<<query.lastError().text().toStdString()<<endl;
           //query.exec("ALTER TABLE User  DROP PRIMARY KEY");
           //cerr<<query.lastError().text().toStdString()<<endl;
          // query.exec("ALTER TABLE User MODIFY id INT AUTO_INCREMENT;");
           cerr<<query.lastError().text().toStdString()<<endl;
           while (query.next()) {
             int user_id = query.value(0).toInt();
             cerr<<user_id<<" "<<query.value(1).toString().toStdString()<<endl;
           }
           //return ;/**/
}
         if(false){
                  QSqlQuery query;
             query.prepare("INSERT INTO ttt (nnn) VALUES (now())");

                    query.exec();
                    cerr<<query.lastError().text().toStdString()<<endl;
                    //query.exec("ALTER TABLE User  DROP PRIMARY KEY");
                    //cerr<<query.lastError().text().toStdString()<<endl;
                   // query.exec("ALTER TABLE User MODIFY id INT AUTO_INCREMENT;");
                    cerr<<query.lastError().text().toStdString()<<endl;
                    while (query.next()) {
                      int user_id = query.value(0).toInt();
                      cerr<<user_id<<" "<<query.value(1).toString().toStdString()<<endl;
                    }
                    //return ;/**/
         }
         QSqlQuery query;
         string qs="SELECT "
                       "id, password, "
                       "display_name, "
                       "level, "
                       "coin ,"
                       "win_number ,"
                       "lose_number ,"
                        "avatar_id ,"
                        "exp ,"
                        "total_coin_winer "
                       "FROM player  "
                       ;

        if(query.exec(qs.c_str())){
            cerr<<"first query exec "<<endl;
        }else{
            cerr<<"error0:"<<query.lastError().text().toStdString()<<endl;

        }
        max_user_id=0;
         while (query.next()) {
           int user_id = query.value(0).toInt();
           auto user_data=std::make_shared<UserData>();
           //users[user_id].id=user_id;
           strcpy(user_data->pass,query.value(1).toString().toStdString().c_str());
           strcpy(user_data->display_name,query.value(2).toString().toStdString().c_str());
           user_data->level=query.value(3).toInt();
           user_data->coin=query.value(4).toInt();
           user_data->win_number=query.value(5).toInt();
           user_data->lose_number=query.value(6).toInt();
           user_data->avatar_id=query.value(7).toInt();
           user_data->exp=query.value(8).toInt();
           user_data->total_coin_winer=query.value(9).toLongLong();
          // user_data->app_version=query.value(10).toInt();
          // user_data->name_changed=query.value(11).toInt();

           ++max_user_id;
           sorted_list.change_user(user_data->id);
           users[user_data->id]=user_data;

         }


}
void DBAccess::add_user_mony(USER_ID user_id,unsigned long long mony){
    if(user_id<10 &&  user_id>200000 &&
            users.find(user_id)==users.end()
            && users[user_id]->state!=UserData::STATE::ACTIVE)
        return ;
    users[user_id]->coin+=mony;

}

void DBAccess::set_buy_item_used(int db_id){
    QSqlQuery query;
    //auto &data=users[db_id];
    query.prepare("UPDATE buying_row SET "
                        "is_cunsumed = TRUE "
                   " WHERE id = :id");
    query.bindValue(":id", db_id);



    if(query.exec()){
        cerr<<"buy item used"<<endl;
        return ;
    }else{

        cerr<<"buy item not used"<<endl;
        cerr<<"error"<<query.lastError().text().toStdString()<<endl;
        connect();
        return ;
    }

}
bool DBAccess::save_app_version(USER_ID user_id ){
    QSqlQuery query;
    auto &data=users[user_id];
    query.prepare("UPDATE player SET "
                        "app_version=:app_version"
                   " WHERE id=:id");
    query.bindValue(":app_version", data->app_version);

    if(query.exec()){
        return true;
    }else{
        cerr<<"error"<<query.lastError().text().toStdString()<<endl;
        connect();
        return false;
    }

}
void DBAccess::load_game_data(){


}

void DBAccess::change_score(USER_ID user_id){
    sorted_list.change_user(user_id);
}
int DBAccess::get_user_rank(UserData *user_data){
    ScoreIndex s;
    s.user_id=user_data->id;
    s.score=user_data->cup_on_set_not_updated;
    int rank;
    sorted_list.cups_users.search(s,rank);
    return rank;

}

int DBAccess::get_top_hundred(UserData *user_data,int shift, int count,int &rank, USER_ID users[]){
    RBTree<ScoreIndex>::my_iterator it=sorted_list.cups_users.by_index(shift);

    ScoreIndex s;
    s.user_id=user_data->id;
    s.score=user_data->cup_on_set_not_updated;
    //RBTree<ScoreIndex>::my_iterator it2=sorted_list.cups_users.search(s,rank);


    if(shift+count > sorted_list.cups_users.size())
        count=sorted_list.cups_users.size()-shift;
    if(count<0){
        count=0;
        return count;
    }
    for(int i=0; i<count; ++i){
        users[i]=it.n->value.user_id;
        ++it;
    }
    return count;/**/
}

bool DBAccess::save_user_data(USER_ID user_id ){
    QSqlQuery query;
    auto &data=users[user_id];
    query.prepare("UPDATE player SET "
                        "mony=:mony ,"
                        "coin=:coin ,"
                        "lose_number=:lose_number,"
                        "win_number=:win_number,"
                        "goal_number=:goal_number,"
                        "achivment_set=:achivment_set ,"
                        "team_id=:team_id ,"
                        "avatar_id=:avatar_id ,"
                        "exp=:exp ,"
                        "cup=:cup ,"
                        "level=:level ,"
                        "total_coin_winer=:total_coin_winer ,"
                        "app_version=:app_version "
                   " WHERE id=:id");
    query.bindValue(":id", user_id);
    query.bindValue(":coin", data->coin);
    query.bindValue(":lose_number", data->lose_number);
    query.bindValue(":win_number", data->win_number);
    query.bindValue(":avatar_id", data->avatar_id);
    query.bindValue(":exp", data->exp);
    query.bindValue(":level", data->level);
    query.bindValue(":total_coin_winer", data->total_coin_winer);
    query.bindValue(":app_version", data->app_version);

    if(query.exec()){
        return true;
    }else{
        cerr<<"error"<<query.lastError().text().toStdString()<<endl;
        connect();
        return false;
    }

}


void DBAccess::bug_check_point(USER_ID user_id){

    QSqlQuery query;


    query.prepare("INSERT INTO bug_check (user_id) VALUES (:user_id)");
    query.bindValue(":user_id",user_id);

   if(!query.exec()){
       cerr<<"error"<<query.lastError().text().toStdString()<<endl;
   }

}

bool DBAccess::save_use_mony(USER_ID uid, int mony, string code){
    cerr<<"DBAccess::save_use_mony "<<code<<endl;
    QSqlQuery query;
    query.prepare("INSERT INTO use_mony_row (user_id,mony,code) VALUES ( :user_id ,:mony, :code)");
    query.bindValue(":user_id", uid);
    query.bindValue(":mony", mony);
    query.bindValue(":code", code.c_str());


    if(query.exec()){

       return true;
    }
    cerr<<"error"<<query.lastError().text().toStdString()<<endl;
    return false;



}

bool DBAccess::change_display_name(UserData *user){

    QSqlQuery query;
    query.prepare("UPDATE player SET"
                  " display_name=:display_name ,"
                  " name_changed=:name_changed "
                  "WHERE id=:id");
    query.bindValue(":id", user->id);
    query.bindValue(":display_name", user->display_name);
    query.bindValue(":name_changed", user->name_changed);


    if(query.exec()){
        return true;
    }else{
        connect();
        cerr<<"error"<<query.lastError().text().toStdString()<<endl;
        return false;
    }



}
UserData *DBAccess::get_user_data_by_id(USER_ID user_id){
    if(users.find(user_id)==users.end())
        return nullptr;
    return users[user_id].get();
}
void DBAccess::load_extra_if_not_exist(USER_ID user_id){
    UserData *ud=get_user_data_by_id(user_id);
    if(ud->extra_data==nullptr)
        load_extra(user_id);
}
void DBAccess::save_postions(USER_ID user_id, int postion_id){
    UserData *ud=get_user_data_by_id(user_id);
    UserExtraData *UED=ud->extra_data;
    if(UED==nullptr)
        return ;
    QSqlQuery query;

    QString s=QString("UPDATE player SET "
              " configs = jsonb_set(configs, '{")+QString::number(postion_id)+"}', '1') "
              " WHERE id= :user_id";
    query.prepare(s);
    query.bindValue(":user_id", user_id);


    if(query.exec()){
        //cerr<<"first query exec "<<endl;
    }else{
        cerr<<"error0:"<<query.lastError().text().toStdString()<<endl;
    }
}

void DBAccess::save_team_levl(USER_ID user_id, int team_id){
    UserData *ud=get_user_data_by_id(user_id);
    UserExtraData *UED=ud->extra_data;
    if(UED==nullptr)
        return ;
    QSqlQuery query;

    QString s=QString("UPDATE player SET "
              " teams = jsonb_set(teams, '{")+QString::number(team_id)+"}', '"+QString::number(UED->team_level[team_id])+"') "
              " WHERE id= :user_id";
    query.prepare(s);
    query.bindValue(":user_id", user_id);


    if(query.exec()){

    }else{
        cerr<<"error0:"<<query.lastError().text().toStdString()<<endl;
    }


}
void DBAccess::save_room(Room *room){
   /* if(room->db_id==0){
        QSqlQuery query;

       query.prepare("INSERT INTO MATCH_RESULT "
                          "(player1,player2,start_time) "
                          " VALUES (:player1,:player2,:start_time)");

        query.bindValue(":player1", room->player[1]);
        query.bindValue(":player2", room->player[2]);
        query.bindValue(":start_time",server->total_time);
        query.exec();
        room->db_id=query.lastInsertId().toInt();
    }else{

        QSqlQuery query;

        query.prepare("UPDATE match_result SET "
                            "winer = :winer , "
                            "score1 = :score1 , "
                            "score2 = :score2 , "
                            "cup1 = :cup1 , "
                            "cup2 = :cup2 , "
                            "price1 = :price1 , "
                            "price2 = :price2 ,"
                            "game_time = :game_time "
                       " WHERE id = :id");
        query.bindValue(":id", room->db_id);
        query.bindValue(":winer", room->game_section.winer);
        query.bindValue(":score1", room->game_section.player1_score);
        query.bindValue(":score2", room->game_section.player2_score);
        query.bindValue(":price1", room->game_section.winer_prize);
        query.bindValue(":price2", room->game_section.winer_prize);
        query.bindValue(":cup1", room->cup1);
        query.bindValue(":cup2", room->cup2);
        query.bindValue(":game_time", server->total_time-room->starting_time);

        if(query.exec()){
            //cerr<<"first query exec "<<endl;
        }else{
            cerr<<"error0:"<<query.lastError().text().toStdString()<<endl;
        }

    }/**/
}

void DBAccess::load_extra(USER_ID user_id){

    UserData *ud=get_user_data_by_id(user_id);
    QSqlQuery query;
    query.prepare("SELECT "
                  "id , "
                  "teams ,"
                  "configs "
                  "FROM player WHERE id= :user_id  ");
    query.bindValue(":user_id", user_id);

   if(query.exec()){
       //cerr<<"first query exec "<<endl;
   }else{
       cerr<<"error0:"<<query.lastError().text().toStdString()<<endl;
   }

   UserExtraData *UED=ud->extra_data;
   if(UED==nullptr){
       UED=new UserExtraData;
       ud->extra_data=UED;
   }
   query.next();
   QJsonDocument o=QJsonDocument::fromJson(query.value(1).toString().toUtf8());
   //cerr<<o.object().size()<<endl;
    QJsonObject j=o.object();

    for(QJsonObject::iterator t=j.begin(); t!=j.end() ; ++t){
        int x=t.key().toInt();
        if(x<0 || x>64)
            continue;
        UED->team_level[x]=t.value().toInt();
    }

    {
        QJsonDocument o=QJsonDocument::fromJson(query.value(2).toString().toUtf8());
        QJsonObject j=o.object();
        for(QJsonObject::iterator t=j.begin(); t!=j.end() ; ++t){
            int x=t.key().toInt();
            UED->postions[x]=1;
        }
    }
}

bool DBAccess::login(USER_ID user_id,PASSWORD pass,UserData **user_data){
    auto p=get_user_data_by_id(user_id);
    if(p!=nullptr && strcmp(p->pass,pass)==0){
        return true;
    }
    return false;
    //cerr<<users[user_id].pass<<" <> "<<pass<<endl;
    //printf("%s %s",users[user_id].pass,pass);
    return false;/**/

   /* QSqlQuery query;
    query.prepare("SELECT "
                  "id,password,"
                  "display_name,"
                  "level,"
                  "mony,"
                  "coin,"
                  "win_number,"
                  "lose_number,"
                  "goal_number "
                  "FROM user WHERE id=:id AND password=:password");
    query.bindValue(":id",user_id);
    query.bindValue(":password",pass);

    DEBUGE(cerr<<user_id<<endl);
    if(!query.exec()){
        cerr<<"error"<<query.lastError().text().toStdString()<<endl;
        connect();
        query.exec();
    }
    while (query.next()) {
      int name = query.value(0).toInt();

      display_name.display_name=query.value(2).toString().toStdString();
      display_name.level=query.value(3).toInt();
      display_name.mony=query.value(4).toInt();
      display_name.coin=query.value(5).toInt();
      display_name.win_number=query.value(6).toInt();
      display_name.lose_number=query.value(7).toInt();
      display_name.goal_number=query.value(8).toInt();


      display_name.id=user_id;

      //display_name="سیبش";
      DEBUGE(cerr << "name:" << name<<"  "<<query.value(1).toString().toStdString()<<endl);
      return true;
    }
    DEBUGE(cerr<<"user not found"<<endl);
    return false;/**/
}

RegisterResponse *DBAccess::register_user(const REGISTER_CODE code,std::string ip){
    cerr<<"DBAccess::register_user "<<code<<endl;
    auto rand_pass =server->get_random_long_long();
    char pass_c[32];
    sprintf(pass_c,"%d",rand_pass);
    QString pass(pass_c) ;
    QSqlQuery query;
    query.prepare("INSERT INTO player "
                  "(code,"
                  "password,"
                  "display_name,"
                  "coin) "
                  "VALUES "
                  "( :code,"
                  ":password,"
                  ":display_name,"
                  ":coin)");
    query.bindValue(":code", code);
    query.bindValue(":password", pass);
    query.bindValue(":coin", 400);
    query.bindValue(":level", 1);
    char dname[256];
    sprintf(dname,"مهمان %d",max_user_id);
    query.bindValue(":display_name", dname);



    RegisterResponse *r=new RegisterResponse();
    if(query.exec()){
        r->done=true;
        r->user_id=query.lastInsertId().toInt();
        cerr<<"insert to user with id"<<r->user_id<<endl;
    }else{
        r->done=false;
        cerr<<"error0:"<<query.lastError().text().toStdString()<<endl;
        r->user_id=users.size()+1;
        r->done=true;
        //return r;
        //r->done=true;
        //r->user_id=fake_register_id++;
    }


    strcpy(r->pass,pass.toStdString().c_str());
    if(r->done)
        cerr<<"user created with id "<<r->user_id<<endl;
    else
        cerr<<"can not create"<<endl;
    if(r->done){
        auto user_data=std::make_shared<UserData>();
        user_data->id=r->user_id;
        strcpy(user_data->pass,r->pass);
        char dname[256];
        sprintf(dname,"مهمان %d",max_user_id);
        strcpy(user_data->display_name,dname);
        user_data->coin=50;
        user_data->win_number=0;
        user_data->lose_number=0;
        user_data->extra_data=new UserExtraData();
        max_user_id=r->user_id+1;
        users[user_data->id]=user_data;
    }
    return r;
}
bool ScoreIndex::operator <(const ScoreIndex &left)const{
    if(score!=left.score)
        return score>left.score;
    return user_id>left.user_id;
}
void SortedList::remove_user(USER_ID user_id){

}
void SortedList::change_user(USER_ID user_id){
    if(user_id<1010)
        return ;
    //return ;
    UserData *ud=db->get_user_data_by_id(user_id);
    ScoreIndex sc;
    sc.user_id=user_id;
    sc.score=ud->cup_on_set_not_updated;




    cups_users.my_delete(sc);

    //sc.score=ud->cup;
    cups_users.insert(sc);/**/

    //ud->cup_on_set_not_updated=ud->cup;
}
