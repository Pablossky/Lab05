#pragma once
#include "Client.h"
#include "Server.h"
#include <iostream>
#define PORT 1111
using namespace std;
int main()
{
	Server server(PORT);
	server.Start();

	while (true)
	{
		this_thread::sleep_for(chrono::milliseconds(500));
		cout << "Message ";
		string msg;
		cin >> msg;
		Client::Echo(msg, asio::ip::address_v4::from_string("127.0.0.1"), PORT);
	}
}