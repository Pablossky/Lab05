#include "Client.h"
#include "asio.hpp"
#include <iostream>
#include "Frame.h"
#include "BinFile.h"

using namespace std;
Client::Client()
{
}

Client::~Client()
{
}

/*void Client::Echo(std::string text, const asio::ip::address_v4 ip, const unsigned short port)  // �w 9
{
	//prepare frame
	char frame[1024];
	strcpy_s(frame, text.data());

	//send frame to server
	asio::io_service io_service; // Any program that uses asio need to have at least one io_service object
	asio::ip::tcp::socket socket(io_service); //create client socket
	socket.connect(asio::ip::tcp::endpoint(ip, port)); //connect to server

	std::cout << "C: Send echo request with data: " << text << std::endl;
	asio::write(socket, asio::buffer(&frame, strlen(frame)));

	//receive echo
	asio::read(socket, asio::buffer(&frame, strlen(frame)));
	std::cout << "C: Echo is: " << frame << std::endl;
}*/

void Client::Echo(const std::string text, const asio::ip::address_v4 address, uint16_t port)
{
	asio::io_service io_service;
	asio::ip::tcp::socket socket(io_service);
	Frame frame;
	frame.type = Frame::Type::ECHO;
	frame.len = text.size();
	memcpy(frame.data, text.data(), text.length());
	cout << "Client: Connect to server " << address.to_string() << ":" << port << "\n";
	socket.connect(asio::ip::tcp::endpoint(address, port));
	cout << "Client: Send " << text.size() << " bytes of text data"
		 << "\n";
	asio::write(socket, asio::buffer(&frame, sizeof(Frame)));
	asio::read(socket, asio::buffer(&frame, sizeof(Frame)));
	cout << "Client: Received response : " << string((const char *)frame.data, frame.len) << "\n";
	socket.close();
}

void Client::SendText(const std::string text, const asio::ip::address_v4 address, uint16_t port)
{
	asio::io_service io_service;
	asio::ip::tcp::socket socket(io_service);
	Frame frame;

	frame.type = Frame::Type::TEXT;
	frame.len = text.size();
	memcpy(frame.data, text.data(), text.length());
	cout << "Client: Connect to server " << address.to_string() << ":" << port << "\n";
	socket.connect(asio::ip::tcp::endpoint(address, port));
	cout << "Client: Send " << text.size() << " bytes to text data"
		 << "\n";
	asio::write(socket, asio::buffer(&frame, sizeof(Frame)));
	socket.close();
}

bool Client::RecvFile(const std::string serFile, const std::string cliFile, const asio::ip::address_v4 address, const int port)
{
	asio::io_service io_service;
	asio::ip::tcp::socket socket(io_service);
	Frame frame;
	BinFile file(serFile, false);
	uint8_t *fileBuffer;
	uint8_t *swapBuffer = new uint8_t[1024];
	size_t chunkLength = file.Read(swapBuffer, 1024);
	socket.connect(asio::ip::tcp::endpoint(address, port));

	frame.type = Frame::RECV_FILE;
	frame.len = cliFile.size();
	memcpy(frame.data, serFile.data(), serFile.size());
	asio::write(socket, asio::buffer(&frame, sizeof(Frame)));

	frame.type = Frame::RECV_FILE;

	while (chunkLength != 0)
	{
		fileBuffer = swapBuffer;
		chunkLength = file.Read(swapBuffer, 1024);
		if (chunkLength == 0)
		{
			frame.type = Frame::END_OF_FILE;
		}
		frame.len = chunkLength;
		memcpy(frame.data, fileBuffer, chunkLength);
		asio::write(socket, asio::buffer(&frame, sizeof(Frame)));

		frame.type = Frame::SEND_FILE;
	}
	socket.close();
	return true;
}

bool Client::SendFile(const std::string serFile, const std::string cliFile, const asio::ip::address_v4 address, const int port)
{
	asio::io_service io_service;
	asio::ip::tcp::socket socket(io_service);
	Frame frame;
	BinFile file(cliFile, false);
	uint8_t *fileBuffer = new uint8_t;
	uint8_t *swapBuffer = new uint8_t[1024];
	size_t chunkLength = 0;
	socket.connect(asio::ip::tcp::endpoint(address, port));

	frame.type = Frame::SEND_FILE;
	frame.len = serFile.size();
	memcpy(frame.data, fileBuffer, chunkLength);
	asio::write(socket, asio::buffer(&frame, sizeof(Frame)));

	frame.type = Frame::SEND_FILE;
	frame.len = chunkLength;
	chunkLength = file.Read(swapBuffer, 1024);

	socket.close();
	return true;
}