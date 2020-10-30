#define _WINSOCK_DEPRECATED_NO_WARNINGS //使inet_addr()函数正常使用
#include<WinSock2.h>
#include<iostream>
#include<string.h>
#include<string>
#include<vector>
#include<set>
#include<map>
#pragma comment(lib,"ws2_32.lib")//链接ws2_32.libku'wen库文件到此项目中
using namespace std;

void str2char(char c[], string s);
bool strcmp_n(char a[], string b, int n);
DWORD WINAPI handlerRequest(LPVOID lparam);
DWORD WINAPI handlerRev(LPVOID lparam);
string add_to_MES(char recv[], string recfrom);

const int BUF_SIZE = 512;

//=====================全局变量=====================
SOCKADDR_IN addrSrv, addrClient;//端口号和地址
int len = sizeof(SOCKADDR_IN);

char sendBuf[BUF_SIZE];
char inputbuf[BUF_SIZE];
char recvBuf[BUF_SIZE];

unsigned long dwThreadId=100;

set<string> Users; //在线的用户列表

map<string, vector<string>> MES; //有序消息组映射


//=====================函数定义=====================

struct para {
	SOCKET sock;
	string userName;
};

void str2char(char c[], string s) {
	int i = 0;
	for (; s[i] != '\0'; i++) {
		c[i] = s[i];
	}
	c[i] = '\0';
}

//比较前n个字符,相等为1
bool strcmp_n(char a[], string b, int n) {
	for (int i = 0; i < n; i++) {
		if (a[i] != b[i])return 0;
	}
	return 1;
}

string add_to_MES(char recv[],string recfrom) { //将收到的消息存向队列，等待转发
	string sendto = "";
	string mes = recfrom+"";
	int i=0;
	for (; recv[i] != ':'&& recv[i] != '\0'; i++) {
		sendto += recv[i];
	}
	if (Users.find(sendto) == Users.end()) //无此用户
		return sendto;
	for (; recv[i] != '\0'; i++) {
		mes += recv[i];
	}
	MES[sendto].push_back(mes);

	return "";
}

DWORD WINAPI handlerRev(LPVOID lparam) {		//接收模块，独立线程
	char recvbuf[BUF_SIZE] = "";
	char sendbuf[BUF_SIZE] = "";
	para* P= (para*)(LPVOID)lparam;
	SOCKET ClientSocket = P->sock;
	string userName = P->userName;
	while (1) {
		//接收消息
		if (recv(ClientSocket, recvbuf, 50, 0) == 0) {
			cout << ClientSocket << "404 ERROR:连接断开" << endl;
			break;
		}

		if (recvbuf[0] != '\0') {
			cout << "\n收到来自" << userName << "的消息:\"" << recvbuf << "\"" << endl;

			if (strcmp_n(recvbuf, "quit", 4)) {  //退出
				cout << "用户" << userName << "退出" << endl;
				cout << "关闭Socket" << ClientSocket << endl;
				break;
			}
			string sendto;
			if ((sendto = add_to_MES(recvbuf, userName)) != "") {
				//无该用户
				string t = "403 用户" + sendto + "未上线";
				str2char(sendbuf, t);
				send(ClientSocket, sendbuf, strlen(sendbuf), 0);
				memset(sendbuf, 0, sizeof(sendbuf) / sizeof(char)); //及时清空缓存
				sendbuf[0] = '\0';
			}
			else {
				strcpy_s(sendbuf, "200 已发送");
				send(ClientSocket, sendbuf, strlen(sendbuf), 0);
				memset(sendbuf, 0, sizeof(sendbuf) / sizeof(char)); //及时清空缓存
				sendbuf[0] = '\0';
			}
		}
		memset(recvbuf, 0, sizeof(recvbuf) / sizeof(char)); //及时清空缓存
		recvbuf[0] = '\0';

	}
	return 0;
}


DWORD WINAPI handlerRequest(LPVOID lparam) {
	SOCKET ClientSocket = (SOCKET)(LPVOID)lparam;
	string userName="";
	char recvbuf[BUF_SIZE]="";
	char sendbuf[BUF_SIZE]="";

	while (1) { //确认用户名
		if (recv(ClientSocket, recvbuf, 50, 0) == 0) {
			cout << ClientSocket << "404 ERROR:连接断开" << endl;
			break;
		}
		if (recvbuf[0] != '\0') {
			if (strcmp_n(recvbuf, "NAME:", 5)) {

				int i = 5;
				while (recvbuf[i] != '\0') {
					userName += recvbuf[i];
					i++;
				}

				if (Users.find(userName)!=Users.end()) {
					strcpy_s(sendbuf, "401 ERROR:用户名重复");
					send(ClientSocket, sendbuf, strlen(sendbuf), 0);
					memset(sendbuf, 0, sizeof(sendbuf) / sizeof(char)); //及时清空缓存
					sendbuf[0] = '\0';
				}
				else {
					break;  //正常收到用户名
				}
			}
			else {
				strcpy_s(sendbuf, "402 ERROR:未收到用户名");
				send(ClientSocket, sendbuf, strlen(sendbuf), 0);
				memset(sendbuf, 0, sizeof(sendbuf) / sizeof(char)); //及时清空缓存
				sendbuf[0] = '\0';
			}
		}
	}

	//初始化
	Users.insert(userName);
	vector<string> wait_mes;
	MES.insert(make_pair(userName, wait_mes));
	int iter = 0;

	cout << "用户" << userName << "建立连接" << endl;

	memset(recvbuf, 0, sizeof(recvbuf) / sizeof(char)); //及时清空缓存
	recvbuf[0] = '\0';

	para P = { ClientSocket ,userName };
	dwThreadId += 1;
	HANDLE hThread = CreateThread(NULL, NULL, handlerRev, LPVOID(&P), 0, &dwThreadId);
	CloseHandle(hThread);		//无需句柄

	//接收，发送消息
	while (1) {

		//发送未发送消息
		int sizeMes = MES[userName].size();
		while (iter < sizeMes) {
			string t = MES[userName][iter];
			t += '\n';
			cout<< "\n发送消息给" << userName << ":\"" << t <<"\""<< endl;
			str2char(sendbuf, t);
			send(ClientSocket, sendbuf, strlen(sendbuf), 0);
			memset(sendbuf, 0, sizeof(sendbuf) / sizeof(char)); //及时清空缓存
			sendbuf[0] = '\0';
			++iter;
		}
	}
	closesocket(ClientSocket);
	return 0;
}

//=====================主函数=====================

int main() { 

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)//加载成功返回0
	{
		//输出出错信息
		cout << "101 ERROR:加载socket库失败" << endl;
		system("pause");
		return 0;
	}
	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);

	//初始化地址包SOCKADDR_IN
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//本机IP地址
	addrSrv.sin_family = AF_INET;//指定协议族
	addrSrv.sin_port = htons(12000);

	bind(sockSrv, (SOCKADDR*)& addrSrv, sizeof(SOCKADDR)); 

	cout << "服务器初始化成功" << endl;
	listen(sockSrv, 5); 
	while (1) { 
		SOCKET sockConn = accept(sockSrv, (SOCKADDR*)& addrClient, &len); 
		dwThreadId += 1;
		if (sockConn != INVALID_SOCKET)//如果连接成功
		{
			cout << "一台客户连接成功" << endl;
			strcpy_s(sendBuf, "连接成功!");
			send(sockConn, sendBuf, sizeof(sendBuf), 0);
			dwThreadId += 1;
			HANDLE WSAAPI hThread = CreateThread(NULL, NULL, handlerRequest, LPVOID(sockConn), 0, &dwThreadId);
			if (hThread) { 
				CloseHandle(hThread); 
			}
		}
	}
	closesocket(sockSrv); 
	WSACleanup();
}
