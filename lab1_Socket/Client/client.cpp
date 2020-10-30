#define _WINSOCK_DEPRECATED_NO_WARNINGS //使inet_addr()函数正常使用
#include<Winsock2.h>//Windows Socket编程头文件
#include<iostream>
#include<string.h>
#pragma comment(lib,"ws2_32.lib")//链接ws2_32.libku'wen库文件到此项目中
using namespace std;

DWORD WINAPI handlerRecver(LPVOID lparam);
void str2char(char c[], string s);
bool strcmp_n(char a[], string b, int n);


//=====================全局常量=====================
const int BUF_SIZE = 512;

//=====================全局变量=====================
SOCKET sockCli;
SOCKADDR_IN addrSer, addrCli;//端口号和地址
int naddr = sizeof(SOCKADDR_IN);

char sendBuf[BUF_SIZE];
char inputBuf[BUF_SIZE];
char recvBuf[BUF_SIZE];

unsigned long dwThreadId = 100;

string userName;

bool QUIT = 0;



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

	//登录模块（设置用户名）
	cout << "请输入用户名：";
	cin >> userName;

	//创建Socket
	sockCli = socket(AF_INET, SOCK_STREAM, 0);

	//初始化客户端地址包SOCKADDR_IN
	addrCli.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	addrCli.sin_family = AF_INET;//指定协议族
	addrCli.sin_port = htons(12248);

	//初始化服务器地址包SOCKADDR_IN
	addrSer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//服务器的地址
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(12000);


	while (1)
	{
		if (connect(sockCli, (SOCKADDR*)&addrSer, sizeof(addrSer)) != SOCKET_ERROR)//如果连接成功
		{
			string nameMes = "NAME:" + userName;	//初始化发送姓名
			str2char(sendBuf,nameMes);
			if (send(sockCli, sendBuf, strlen(sendBuf), 0) < 0) {
				cout << "error: 连接断开，发送失败" << endl;
				break;
			}

			HANDLE hThread = CreateThread(NULL, NULL, handlerRecver, LPVOID(sockCli), 0, &dwThreadId);
			CloseHandle(hThread);		//无需句柄

			while (1) {	
				//发送模块置于主线程
				char sendM[BUF_SIZE] = "";
				cin >> sendM;

				strcpy_s(sendBuf, sendM);
				if (send(sockCli, sendBuf, strlen(sendBuf), 0) < 0) {
					cout << "404 ERROR:连接断开" << endl;
					break;
				}

				if (strcmp_n(sendM, "quit", 4)|QUIT) {  //退出
					cout << "关闭Socket" << endl;
					QUIT = 1;
					closesocket(sockCli);
					WSACleanup();//卸载socket库
					cout << "退出程序中" << endl;
					return 0;
				}
			}
		}
	}
	//关闭套接字
	closesocket(sockCli);

	WSACleanup();//卸载socket库
	cout << "退出程序中" << endl;
	return 0;
}

DWORD WINAPI handlerRecver(LPVOID lparam) {		//接收模块，独立线程
	while (1) {
		if (QUIT)return 0;

		if (recv(sockCli, recvBuf, sizeof(recvBuf), 0) == 0) {
			cout<< "404 ERROR:连接断开" << endl;
			return 0;
		}

		if(recvBuf[0]!='\0')cout <<"\n接收消息:\n"<< recvBuf << endl<<endl;
		memset(recvBuf, 0, BUF_SIZE); //及时清空缓存
		recvBuf[0] = '\0';
	}
	return 0;
}


//strcpy
void str2char(char c[], string s) {
	int i = 0;
	for (; s[i] != '\0'; i++) {
		c[i] = s[i];
	}
	c[i] = '\0';
}

//比较前n个字符,相等为1
bool strcmp_n(char a[],string b, int n) {
	for (int i = 0; i < n; i++) {
		if (a[i] != b[i])return 0;
	}
	return 1;
}