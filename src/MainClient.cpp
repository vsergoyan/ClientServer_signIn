 /*
	UDP Client routine
*/

#include "UDPSocket.h"
//#include "source.h"
#include <stdio.h>
#include <iostream>
#include <queue>
#include <unordered_map>
//using namespace std;

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
	//	SOCKET s;
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);

	//struct sockaddr_in si_other1;
	//int slen1 = sizeof(si_other1);

	char buf[BUFLEN];
	char message[BUFLEN];
	char encrypted[BUFLEN];
	char username[BUFLEN];
	char answ[BUFLEN];
	char password[BUFLEN];

	std::string keyword = "LIMON";
	std::string key;
	std::string decod_string, ncode_string;
	std::string decod_stringg = "PZQJNYQEHUPKMDVEIXCSLZYSATI";


	unsigned short srv_port = 0;
	char srv_ip_addr[40];
	memset(srv_ip_addr, 0, 40);


	//create socket
	UDPSocket client_sock;

	//setup address structure
	memset((char*)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;

	if (1 == argc)
	{
		si_other.sin_port = htons(PORT);
		si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);
		printf("1: Server - addr=%s , port=%d\n", SERVER, PORT);
	}
	else if (2 == argc)
	{
		si_other.sin_port = htons(atoi(argv[1]));
		si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);
		printf("2: argv[0]: Server - addr=%s , port=%d\n", SERVER, atoi(argv[1]));
	}
	else
	{
		si_other.sin_port = htons(atoi(argv[2]));
		si_other.sin_addr.S_un.S_addr = inet_addr(argv[1]);
		printf("3: Server - addr=%s , port=%d\n", argv[1], atoi(argv[2]));
	}

	while (1)
	{
		memset(answ, '\0', BUFLEN);
		memset(buf, '\0', BUFLEN);
		memset(message, '\0', BUFLEN);
		memset(username, '\0', BUFLEN);
		printf("\nEnter username : ");
		gets_s(username, BUFLEN);

		client_sock.SendDatagram(username, (int)strlen(username), (struct sockaddr*)&si_other, slen);

		client_sock.RecvDatagram(answ, BUFLEN, (struct sockaddr*)&si_other, &slen);
		if (strcmp(answ, "yes") == 0)
		{
			printf("\nEnter password : ");
			gets_s(password, BUFLEN);
			client_sock.SendDatagram(password, (int)strlen(password), (struct sockaddr*)&si_other, slen);
			memset(answ, '\0', BUFLEN);
			client_sock.RecvDatagram(answ, BUFLEN, (struct sockaddr*)&si_other, &slen);
			if (strcmp(answ, "yes") == 0)
			{
				printf("OK!!!\n");
				while (1)
				{
					//popoxutyunner
					printf("Enter message: ");
					gets_s(message, BUFLEN);
					std::string strForDcode(message);
					key = generateKey(strForDcode, keyword);
					decod_string = Decoder(strForDcode, key);

					std::string result = "";
					std::unordered_map<std::string, std::string> codes1 = huffman(decod_string);

					for (char c : decod_string) {
						std::string s(1, c);
						for (auto p : codes1) {
							if (s == p.first) {
								result.append(p.second);
							}
						}
					}
					strcpy_s(message, result.c_str());
					client_sock.SendDatagram(message, (int)strlen(message), (struct sockaddr*)&si_other, slen);
					memset(message, '\0', BUFLEN);

					client_sock.RecvDatagram(buf, BUFLEN, (struct sockaddr*)&si_other, &slen);
					std::unordered_map<std::string, std::string> codes2 = huffman(decod_stringg);
					std::string decod_string(buf);
					std::string strForNcode = Dcode(codes2, decod_string);
					key = generateKey(strForNcode, keyword);
					ncode_string = Ncoder(strForNcode, key);
					strcpy_s(buf, ncode_string.c_str());
					printf("Answer: %s\n", buf);
					memset(buf, '\0', BUFLEN);
				}
			}
			else
			{
				printf("Password is false!!!");
			}
		}
		else
		{
			printf("Username is not defined");
		}
	}
	return 0;
}