#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<WinSock2.h>//Windows Socket编程头文件
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
		cout << "加载socket库失败" << endl;
		system("pause");
		return 0;
	}

	//创建Socket
	sockSer = socket(AF_INET, SOCK_STREAM, 0);

	//初始化地址包SOCKADDR_IN
	addrSer.sin_addr.S_un.S_addr = inet_addr("10.136.129.86");//利用ipconfig查看本机IP地址，复制过来
	addrSer.sin_family = AF_INET;//指定协议族
	addrSer.sin_port = htons(12248);//主要不要和已经注册的端口号重复，确保服务器和和客户端端口号一致

	//绑定socket和本机地址
	bind(sockSer, (SOCKADDR*)&addrSer, sizeof(SOCKADDR));

	while (1)
	{
		//监听连接请求
		listen(sockSer, 5);

		//接受连接请求
		sockCli = accept(sockSer, (SOCKADDR*)&addrCli, &naddr);//连接成功新建一个套接口
		if (sockCli != INVALID_SOCKET)//如果连接成功
		{
			cout << "连接成功" << endl;
			strcpy_s(sendbuf, "hello!");//主备要发送的数据
			send(sockCli, sendbuf, sizeof(sendbuf), 0);

		}

	}
	closesocket(sockSer);//关闭套接字
	closesocket(sockCli);

	WSACleanup();//卸载socket库
	return 0;
}
