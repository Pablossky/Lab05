#include <iostream>
#include <thread> 
#include "Server.h"
#include "asio.hpp"
#include "BinFile.h"
#include "Frame.h"

using namespace std;

Server::Server(unsigned short port) : port(port)
{
}


Server::~Server()
{
}


//communication session handler (ECHO)
/*void session(asio::ip::tcp::socket socket)  //z æw 9
{
	try
	{
		while(true)
		{
			char frame[1024];

			size_t length = socket.read_some(asio::buffer(frame));
			asio::write(socket, asio::buffer(frame, length));
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception in thread: " << e.what() << "\n";
	}
}*/

void session(asio::ip::tcp::socket socket)
{
	try
	{
		while (true)
		{
			Frame rframe;
			Frame sframe;
			asio::read(socket, asio::buffer(&rframe, sizeof(Frame)));

			switch (rframe.type)
			{
			case Frame::Type::ECHO:
				cout << "Session: sending echo \n";
				asio::write(socket, asio::buffer(&rframe, sizeof(Frame)));
				break;
			case Frame::Type::TEXT:
				cout << "Session: received text: " << string((const char*)rframe.data, rframe.len) << "\n";
				break;
			case Frame::Type::SEND_FILE:
			{
				BinFile file(string((const char*)rframe.data, rframe.len), true);
				do
				{
					asio::read(socket, asio::buffer(&rframe, sizeof(Frame)));
					if (rframe.type == Frame::Type::FILE_DATA)
					{
						file.Append(rframe.data, rframe.len);
					}
				} while (rframe.type == Frame::Type::FILE_DATA);
				cout << "Session: File recived\n";
				break;
			}
			case Frame::Type::RECV_FILE:
			{
				BinFile file(string((const char*)rframe.data, rframe.len), false);

				while (size_t len = file.Read(sframe.data, rframe.len))
				{
					sframe.type = Frame::Type::FILE_DATA;
					sframe.len = len;
					asio::write(socket, asio::buffer(&sframe, sizeof(Frame)));
					//mo¿e czegoœ brakowaæ
				}

				break;
			}
			default:
				sframe.type = Frame::Type::INVALID_COMMAND;
				sframe.len = 0;
				asio::write(socket, asio::buffer(&sframe, sizeof(Frame)));
				break;
			}
		}
	}
	catch (exception& e)
	{
		cerr << "Exception in thread: " << e.what() << "\n";
		//mo¿e czegoœ brakowaæ
	}
}

void Server::ThreadRoutine()
{
	asio::io_service io_service; // Any program that uses asio need to have at least one io_service object
	asio::ip::tcp::acceptor acceptor(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)); //create acceptor for listening purposes
	acceptor.listen(1024);
	while (isRunning())
	{
		asio::ip::tcp::socket socket(io_service); //create a socket for communication purposes
		acceptor.accept(socket); //then accept connection

		//read data sent by the client, then send echo
		std::thread(session, std::move(socket)).detach();

		std::this_thread::yield();
	}
}