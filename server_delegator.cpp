//
// async_udp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <set>
#include <hash_map>
#include <map>
#include <unordered_map>
#include "Classes/MainServer.h"
#include "game_message/Messages.h"
#include "base_message/BaseMessage.h"
#include <boost/function.hpp>
#include <boost/chrono/chrono.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/move/move.hpp>
#include <iostream>
#include <unistd.h>
#include <map>
#include "Engine/SharedMemoryLoger.h"
#include "base/rbtree.h"

//include for debuge trace
//http://stackoverflow.com/questions/77005/how-to-generate-a-stacktrace-when-my-gcc-c-app-crashes
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "cocos2d_server/CocoBase.h"
//end
using boost::asio::ip::udp;
using namespace std;



void error(const char *msg)
{
    perror(msg);
    exit(1);
}


int main2(int argc, char *argv[])
{
    vector<int> clients;
    int sockfd, newsockfd, portno;
         socklen_t clilen;
         //char buffer[256];
         struct sockaddr_in serv_addr, cli_addr;
         //int n;

         sockfd = socket(AF_INET, SOCK_STREAM, 0);
         if (sockfd < 0)
            error("ERROR opening socket");

         int flags = fcntl(sockfd, F_GETFL, 0);
        fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

         bzero((char *) &serv_addr, sizeof(serv_addr));
         portno = 3000;
         serv_addr.sin_family = AF_INET;
         serv_addr.sin_addr.s_addr = INADDR_ANY;
         serv_addr.sin_port = htons(portno);
         if (bind(sockfd, (struct sockaddr *) &serv_addr,
                  sizeof(serv_addr)) < 0)
                  error("ERROR on binding");
         listen(sockfd,5);
         clilen = sizeof(cli_addr);

         //bzero(buffer,256);
         //n = read(newsockfd,buffer,255);
            int max_newsockfd=0;
         for(;true;){
              boost::this_thread::sleep(boost::posix_time::milliseconds(100));

              newsockfd = accept(sockfd,
                       (struct sockaddr *) &cli_addr,
                       &clilen);
              if(newsockfd>=0){
                  max_newsockfd=newsockfd;
                  int flags = fcntl(sockfd, F_GETFL, 0);
                 fcntl(newsockfd, F_SETFL, flags | O_NONBLOCK);
                  clients.push_back(newsockfd);
                  cerr<<"new client "<<newsockfd<<endl;
              }

             struct sockaddr_in   server_addr;     // Server Internet address
             socklen_t                  addr_len;        // Internet address length




             addr_len = sizeof(server_addr);
             int retcode = 0;

             char in_buf[1024];
             //for(int j=0;j<clients.size();++j)
             fd_set read_fds ;
             if(max_newsockfd<=0)
                 continue;

             int sss=select(max_newsockfd+1, &read_fds, NULL, NULL, NULL);

             int j=0;
             if(sss!=-1){
                 cerr<<sss<<endl;
                 retcode = recvfrom(sss, in_buf, sizeof(in_buf), 0, (struct sockaddr *)&server_addr, &addr_len);
                 if (retcode <= 0){
                     //cerr<<".";
                     continue;
                }
                printf("Here %d is the message: %s\n",retcode,in_buf);
                //int n = write(newsockfd,"I got your message",18);
             }
             //if (n < 0) error("ERROR writing to socket");
         }




         close(newsockfd);
         close(sockfd);
         return 0;
}




int main0 (int argc, char *argv[])
{


    //SharedMemoryLoger m2;
    //m2.init_memory();
    cerr<<"end"<<endl;
}
void handler(int sig) {
  void *array[1000];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 1000);

  // print out all the frames to stderr
  cerr<<SharedLogParametr::last_msg_base_type<<endl;
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}


void baz() {
 int *foo = (int*)-1; // make a bad pointer
  printf("%d\n", *foo);       // causes segfault
}

void bar() {
    //baz();
}
void foo(int n) {
    if(n==0)
        bar();
    else
        foo(n+1);

}
int main(int argc, char* argv[])
{
    cerr<<BOOST_LIB_VERSION<<endl;
    //RBTree<int> t;
    //signal(SIGSEGV, handler);
    //cerr<<"foo "<<(void*)(foo)<<endl;
    //foo(20);

    srand(time(0));
    cerr<<"shared room test"<<endl;
    cerr<<boost::thread::hardware_concurrency()<<endl;

    cerr<<sizeof(UserData)<<endl;
    //cerr<<sizeof(udp::endpoint)<<endl;
    SharedRoomMemory::init();
    SharedUdpEndPointMemory::init();
    SharedLogParametr::init();
    auto m=SharedLogParametr::get_map();
    for(LogHashMap::iterator i=m->begin(); i!=m->end();++i){
        cerr<<i->first<<" >> "<<i->second.file<<" "<<
           i->second.func<<" "<<i->second.line<<endl;
    }
    //SharedRoomMemory::clear();
  try
  {


    boost::asio::io_service io_service;

    MainServer s(io_service, 3100);

    s.reload_rooms();
    s.run_rooms();



    s.run_io();


  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
//main
