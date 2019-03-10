//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection.hpp"
#include <utility>
#include <vector>
#include "connection_manager.hpp"
#include "request_handler.hpp"
#include <iostream>
namespace http {
namespace server {

connection::connection(boost::asio::ip::tcp::socket socket,
    connection_manager& manager, request_handler& handler)
  : socket_(std::move(socket)),
    connection_manager_(manager),
    request_handler_(handler)
{
}

void connection::start()
{
  do_read();
}

void connection::stop()
{
  socket_.close();
}

void connection::do_read()
{
  auto self(shared_from_this());
  //boost::detail::DynamicBuffer buffers_;

  /*boost::asio::async_read_until(socket_,buf, "\r\n\r\n", [this, self](boost::system::error_code ec, std::size_t length){
         std::istream in(&buf);
         std::cout << length << std::endl;
         std::cout << buf.size() << std::endl;
         in.ignore(length);
         boost::asio::async_read(socket_, buf, boost::asio::transfer_exactly(2), [this, self](boost::system::error_code ec, std::size_t length){
             std::istream in(&buf);
             std::cout << length << std::endl;
         });
     });
  return ;/**/
  socket_.async_read_some(boost::asio::buffer(buffer_),
      [this, self](boost::system::error_code ec, std::size_t bytes_transferred)
      {
        if (!ec)
        {
          request_parser::result_type result;
          std::string s="";
          for(int i=0; i<bytes_transferred; ++i)
              s+=buffer_[i];
          int x=s.find("\r\n\r\n");
          request_.body=std::string(s.begin()+x+4,s.end());
          std::cout<<"http recive :"<<s<<std::endl;
          std::cout<<"body start body  :"<<request_.body<<std::endl;
          {
              //rapidjson::Document d;
                  request_.json.Parse(request_.body.c_str());

                  // 2. Modify it by DOM.
                  //Value& s = d["text"];

                  // Output {"project":"rapidjson","stars":11}
                  //std::cout << s.GetString() << std::endl;
          }/**/
          std::tie(result, std::ignore) = request_parser_.parse(
              request_, buffer_.data(), buffer_.data() + bytes_transferred);

          if (result == request_parser::good)
          {

            if(request_handler_.handle_request(this,request_, reply_))
                do_write();
          }
          else if (result == request_parser::bad)
          {
            reply_ = reply::stock_reply(reply::bad_request);
            do_write();
          }
          else
          {
            do_read();
          }
        }
        else if (ec != boost::asio::error::operation_aborted)
        {
          connection_manager_.stop(shared_from_this());
        }
      });
}

void connection::do_write()
{
  auto self(shared_from_this());
  boost::asio::async_write(socket_, reply_.to_buffers(),
      [this, self](boost::system::error_code ec, std::size_t)
      {
        if (!ec)
        {
          // Initiate graceful connection closure.
          boost::system::error_code ignored_ec;
          socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
            ignored_ec);
        }

        if (ec != boost::asio::error::operation_aborted)
        {
          connection_manager_.stop(shared_from_this());
        }
      });
}

} // namespace server
} // namespace http
