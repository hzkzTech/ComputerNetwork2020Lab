// client.cpp : 定义控制台应用程序的入口点。
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<Winsock2.h>//Windows Socket编程头文件
#include<iostream>
#include<string.h>
#pragma comment(lib,"ws2_32.lib")//链接ws2_32.libku'wen库文件到此项目中
using namespace std;

//注意，关闭安全声明周期
//属性-C/C++-SDL检查-否

//=====================全局常量=====================
const int BUF_SIZE = 2048;//2048是随意定的，定的比较大

//=====================全局变量=====================
SOCKET sockSer, sockCli;
SOCKADDR_IN addrSer, addrCli;//端口号和地址
int naddr = sizeof(SOCKADDR_IN);

char sendbuf[BUF_SIZE];
char inputbuf[BUF_SIZE];
char recvbuf[BUF_SIZE];


//=====================函数声明=====================


int main()
{
	//加载socket库
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)//加载成功返回0
	{
		//输出出错信息
		cout << "加载socket库成功" << endl;
		system("pause");
		return 0;
	}

	//创建Socket
	sockCli = socket(AF_INET, SOCK_STREAM, 0);

	//初始化客户端地址包SOCKADDR_IN
	addrCli.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//127.0.0.1是回传地址，使用此地址时，不进行任何网路的传输，直接把本机的地址返回过去
	//127.0.0.1指本地机，一般用来测试使用

	addrCli.sin_family = AF_INET;//指定协议族
	addrCli.sin_port = htons(12248);//主要不要和已经注册的端口号重复，确保服务器和和客户端端口号一致

	//初始化服务器地址包SOCKADDR_IN
	addrSer.sin_addr.S_un.S_addr = inet_addr("10.136.129.86");//此处应该是服务器的地址
	addrSer.sin_family = AF_INET;//指定协议族
	addrSer.sin_port = htons(12248);//主要不要和已经注册的端口号重复，确保服务器和和客户端端口号一致

	while (1)
	{
		if (connect(sockCli, (SOCKADDR*)&addrSer, sizeof(addrSer)) != SOCKET_ERROR)//如果连接成功
		{
			recv(sockCli, recvbuf, sizeof(recvbuf), 0);
			cout << recvbuf << endl;
		}

	}
	closesocket(sockSer);//关闭套接字
	closesocket(sockCli);

	WSACleanup();//卸载socket库
	return 0;
}