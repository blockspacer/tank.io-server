#ifndef LOGGER_H
#define LOGGER_H
#include<string>
using namespace std;

typedef long long SAVE_ID;
class Logger
{
    SAVE_ID count=1;
    bool connected=false;
public:
    Logger();
    void connect();
    SAVE_ID generator_id();
    void set_parametr_name(SAVE_ID id,string name);
    void save_data(SAVE_ID id,int cat,int length,char data[]);
    SAVE_ID load_next_data(int cat,char data[],SAVE_ID id_biger_than);
    void please_send_all_data_with_catagory(int cat);



};

#endif // LOGGER_H
