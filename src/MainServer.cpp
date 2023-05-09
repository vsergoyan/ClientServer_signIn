/*
	UDP Server
*/

#include <stdio.h>
#include "UDPServer.h"
#include <iostream>
#include <queue>
#include <unordered_map>
//using namespace std;
//#include "source.h"

#ifndef _WIN32
using SOCKET = int
#define WSAGetLastError() 1
#else
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#endif

//#define SERVER "127.0.0.1"	//ip address of udp server
#define BUFLEN 	1024		    //Max length of buffer
#define PORT    8888			//The port on which to listen for incoming data

std::string generateKey(std::string str, std::string key)
{
	int x = str.size();
	for (int i = 0; ; i++)
	{
		if (x == i)
			i = 0;
		if (key.size() == str.size())
			break;
		key.push_back(key[i]);
	}
	return key;
}

std::string Decoder(std::string str, std::string key)
{
	std::string result;
	for (int i = 0; i < str.size(); i++)
	{
		char x = (str[i] + key[i]) % 26;
		x += 'A';
		result.push_back(x);
	}
	return result;
}

std::string Ncoder(std::string decod_string, std::string key)
{
	std::string orig_string;
	for (int i = 0; i < decod_string.size(); i++)
	{
		char x = (decod_string[i] - key[i] + 26) % 26;
		x += 'A';
		orig_string.push_back(x);
	}
	return orig_string;
}


struct Node {
	std::string data;
	int freq;
	Node* left;
	Node* right;

	Node(std::string data, int freq) {
		this->data = data;
		this->freq = freq;
		this->left = nullptr;
		this->right = nullptr;
	}

	~Node() {
		delete left;
		delete right;
	}
};

struct CompareNodes {
	bool operator()(Node* n1, Node* n2) {
		return n1->freq > n2->freq;
	}
};

void encode(Node* root, std::string code, std::unordered_map<std::string, std::string>& codes) {
	if (root == nullptr) {
		return;
	}

	if (root->left == nullptr && root->right == nullptr) {
		codes[root->data] = code;
	}

	encode(root->left, code + "0", codes);
	encode(root->right, code + "1", codes);
}

std::unordered_map<std::string, std::string> huffman(std::string str) {
	// count frequency of each character
	std::unordered_map<std::string, int> freq;
	for (char c : str) {
		freq[std::string(1, c)]++;
	}

	// create a priority queue of nodes
	std::priority_queue<Node*, std::vector<Node*>, CompareNodes> pq;
	for (auto p : freq) {
		pq.push(new Node(p.first, p.second));
	}

	// build the Huffman tree
	while (pq.size() > 1) {
		Node* n1 = pq.top();
		pq.pop();
		Node* n2 = pq.top();
		pq.pop();

		Node* parent = new Node("", n1->freq + n2->freq);
		parent->left = n1;
		parent->right = n2;

		pq.push(parent);
	}

	// encode the characters
	std::unordered_map<std::string, std::string> codes;
	Node* root = pq.top();
	encode(root, "", codes);
	delete root;

	return codes;
}

bool includes(std::string temp, std::unordered_map<std::string, std::string> codes) {
	for (auto p : codes) {
		if (temp == p.second) {
			return true;
		}
	}
	return false;
}

std::string Dcode(std::unordered_map<std::string, std::string> codes, std::string str) {
	std::string temp = "";
	std::string result1 = "";
	bool inc = false;
	int i = 0;
	while (str[i]) {
		while (!inc) {
			temp.push_back(str[i]);
			i++;
			inc = includes(temp, codes);
		}
		for (auto p : codes) {
			if (temp == p.second) {
				result1.append(p.first);
				break;
			}
		}
		inc = false;
		temp = "";
	}
	return result1;
}


int main(int argc, char* argv[])
{
	struct sockaddr_in si_other;
	//struct sockaddr_in si_other1;
	unsigned short srvport;
	int slen;
	char buf[BUFLEN];
	char username[BUFLEN];
	char password[BUFLEN];
	char msg[BUFLEN];
	char yes[BUFLEN] = "yes";
	char no[BUFLEN] = "no";
	char us[2][BUFLEN] = { {"vsergoyan"}, {"elhovhan"} };
	char ps[2][BUFLEN] = { {"12345678"}, {"12345678"} };
	int i, k = 0;

	std::string keyword = "LIMON";
	std::string key;
	std::string ncode_string;
	std::string decod_stringg = "PZQJNYQEHUPKMDVEIXCSLZYSATI";

	srvport = (1 == argc) ? PORT : atoi(argv[1]);

	UDPServer server(srvport);
	slen = sizeof(si_other);

	while (1)
	{
		memset(username, '\0', BUFLEN);
		memset(password, '\0', BUFLEN);
		memset(msg, '\0', BUFLEN);
		memset(buf, '\0', BUFLEN);

		printf("Waiting for username...   ");
		fflush(stdout);
		server.RecvDatagram(username, BUFLEN, (struct sockaddr*)&si_other, &slen);

		printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		i = 0;
		while (i < 2)
		{
			if (strcmp(username, us[i]) == 0)
			{
				printf("Username: %s\n", username);
				server.SendDatagram(yes, (int)strlen(yes), (struct sockaddr*)&si_other, slen);
				server.RecvDatagram(password, BUFLEN, (struct sockaddr*)&si_other, &slen);
				if (strcmp(password, ps[i]) == 0)
				{
					server.SendDatagram(yes, (int)strlen(yes), (struct sockaddr*)&si_other, slen);
					printf("Everything is OK.\n");
					while (1) 
					{
						printf("Waiting for data...   ");
						server.RecvDatagram(buf, BUFLEN, (struct sockaddr*)&si_other, &slen);
						std::unordered_map<std::string, std::string> codes1 = huffman(decod_stringg);
						std::string decod_string(buf);
						std::string strForNcode = Dcode(codes1, decod_string);

						key = generateKey(strForNcode, keyword);
						ncode_string = Ncoder(strForNcode, key);
						strcpy_s(buf, ncode_string.c_str());
						printf("%s ", buf);
						memset(buf, '\0', BUFLEN);

						printf("\nAnswer : ");
						gets_s(msg, BUFLEN);
						std::string strForDcode(msg);
						key = generateKey(strForDcode, keyword);
						decod_string = Decoder(strForDcode, key);
						std::string result = "";
						std::unordered_map<std::string, std::string> codes2 = huffman(decod_string);

						for (char c : decod_string) {
							std::string s(1, c);
							for (auto p : codes2) {
								if (s == p.first) {
									result.append(p.second);
								}
							}
						}
						strcpy_s(msg, result.c_str());
						server.SendDatagram(msg, (int)strlen(msg), (struct sockaddr*)&si_other, slen);
						memset(msg, '\0', BUFLEN);
					}
				}
				else
				{
					printf("Password is not correct...   \n");
					server.SendDatagram(no, (int)strlen(no), (struct sockaddr*)&si_other, slen);
					k = 1;
					break;
				}
			}
			i++;
		}
		if (k == 0)
		{
			server.SendDatagram(no, (int)strlen(no), (struct sockaddr*)&si_other, slen);
			printf("Username is not defined!!!\n");
		}
	}
	return 0;
}
