#include "utils.h"
#include <thread>

constexpr auto ds_test_port = static_cast<u_short>(68000);
constexpr auto max_threads = 5;

int main()
{
	//
	SetConsoleOutputCP(CP_UTF8);
	// Initialise winsock
	WSADATA ws_data;
	std::cout << "Initialising Winsock...\n";
	if (WSAStartup(MAKEWORD(2, 2), &ws_data) != 0)
	{
		std::cerr << "\nWinsock setup failed! Error Code : " << WSAGetLastError() << '\n';
		return 1;
	}
	read_theaters();
	read_clients();
	// Create a socket
	const SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		std::cerr << "\nSocket creation failed! Error Code : " << WSAGetLastError() << '\n';
		return 1;
	}
	std::cout << "\nSocket created.\n";
	// Bind the socket (ip address and port)
	SOCKADDR_IN hint{};
	hint.sin_family = AF_INET;
	hint.sin_port = htons(ds_test_port);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(listening, reinterpret_cast<SOCKADDR*>(&hint), sizeof(hint)) == SOCKET_ERROR)
	{
		std::cerr << "\nAddress binding failed! Error Code : " << WSAGetLastError() << '\n';
		return 1;
	}
	// Setup the socket for listening
	listen(listening, SOMAXCONN);
	// Wait for connection
	SOCKADDR_IN client_addr{};
	int client_size = sizeof(client_addr);
	SOCKET client_socket;
	std::list<std::thread> threads;
	int i = 0;
	while (i++ < max_threads && (client_socket = accept(listening, reinterpret_cast<SOCKADDR*>(&client_addr), &client_size)) != INVALID_SOCKET)
	{
		threads.emplace_back(main_call, client_socket, client_addr);
	}
	closesocket(listening);
	for (auto& thread : threads)
	{
		thread.join();
	}
	// Save data to files
	write_theaters();
	write_clients();
	// Cleanup winsock
	WSACleanup();
	return 0;
}
