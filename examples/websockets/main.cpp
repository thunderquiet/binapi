
// ----------------------------------------------------------------------------
//                              Apache License
//                        Version 2.0, January 2004
//                     http://www.apache.org/licenses/
//
// This file is part of binapi(https://github.com/niXman/binapi) project.
//
// Copyright (c) 2019-2021 niXman (github dot nixman dog pm.me). All rights reserved.
// ----------------------------------------------------------------------------

#include <binapi/api.hpp>
#include <binapi/websocket.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>

#include <iostream>

int main() {
    boost::asio::io_context ioctx;

    binapi::ws::websockets_pool ws{
         ioctx
        ,"stream.binance.com"
        ,"9443"
    };

    ws.depth("BTCUSDT", 250,
        [](const char *fl, int ec, std::string emsg, auto depths) {
            if ( ec ) {
                std::cerr << "subscribe depth error: fl=" << fl << ", ec=" << ec << ", emsg=" << emsg << std::endl;

                return false;
            }

            std::cout << "depths: " << depths << std::endl;

            return true;
        }
    );

    ws.trade("BTCUSDT",
        [](const char *fl, int ec, std::string emsg, auto trades) {
            if ( ec ) {
                std::cerr << "subscribe trades error: fl=" << fl << ", ec=" << ec << ", emsg=" << emsg << std::endl;

                return false;
            }

            std::cout << "trades: " << trades << std::endl;

            return true;
        }
    );

    auto book_handler = ws.book("BTCUSDT",
        [](const char *fl, int ec, std::string emsg, auto book) {
            if ( ec ) {
                std::cerr << "subscribe book error: fl=" << fl << ", ec=" << ec << ", emsg=" << emsg << std::endl;

                return false;
            }

            std::cout << "book: " << book << std::endl;

            return true;
        }
    );

    auto books_handler = ws.books(
        [](const char *fl, int ec, std::string emsg, auto books) {
            if ( ec ) {
                std::cerr << "subscribe books error: fl=" << fl << ", ec=" << ec << ", emsg=" << emsg << std::endl;

                return false;
            }

            std::cout << "books: " << books << std::endl;

            return true;
        }
    );

    boost::asio::steady_timer timer0{ioctx, std::chrono::steady_clock::now() + std::chrono::seconds(5)};
    timer0.async_wait([&ws, book_handler](const auto &/*ec*/){
        std::cout << "unsubscribing book_handler: " << book_handler << std::endl;
        ws.unsubscribe(book_handler);
    });

    boost::asio::steady_timer timer1{ioctx, std::chrono::steady_clock::now() + std::chrono::seconds(10)};
    timer1.async_wait([&ws, books_handler](const auto &/*ec*/){
        std::cout << "async unsubscribing books_handler: " << books_handler << std::endl;
        ws.async_unsubscribe(books_handler);
    });

    boost::asio::steady_timer timer2{ioctx, std::chrono::steady_clock::now() + std::chrono::seconds(15)};
    timer2.async_wait([&ws](const auto &/*ec*/){
        std::cout << "async unsubscribing all" << std::endl;
        ws.async_unsubscribe_all();
    });

    ioctx.run();

    return EXIT_SUCCESS;
}
