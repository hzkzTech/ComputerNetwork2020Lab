// client.cpp : �������̨Ӧ�ó������ڵ㡣
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<Winsock2.h>//Windows Socket���ͷ�ļ�
#include<iostream>
#include<string.h>
#pragma comment(lib,"ws2_32.lib")//����ws2_32.libku'wen���ļ�������Ŀ��
using namespace std;

//ע�⣬�رհ�ȫ��������
//����-C/C++-SDL���-��

//=====================ȫ�ֳ���=====================
const int BUF_SIZE = 2048;//2048�����ⶨ�ģ����ıȽϴ�

//=====================ȫ�ֱ���=====================
SOCKET sockSer, sockCli;
SOCKADDR_IN addrSer, addrCli;//�˿ںź͵�ַ
int naddr = sizeof(SOCKADDR_IN);

char sendbuf[BUF_SIZE];
char inputbuf[BUF_SIZE];
char recvbuf[BUF_SIZE];


//=====================��������=====================


int main()
{
	//����socket��
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)//���سɹ�����0
	{
		//���������Ϣ
		cout << "����socket��ɹ�" << endl;
		system("pause");
		return 0;
	}

	//����Socket
	sockCli = socket(AF_INET, SOCK_STREAM, 0);

	//��ʼ���ͻ��˵�ַ��SOCKADDR_IN
	addrCli.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//127.0.0.1�ǻش���ַ��ʹ�ô˵�ַʱ���������κ���·�Ĵ��䣬ֱ�Ӱѱ����ĵ�ַ���ع�ȥ
	//127.0.0.1ָ���ػ���һ����������ʹ��

	addrCli.sin_family = AF_INET;//ָ��Э����
	addrCli.sin_port = htons(12248);//��Ҫ��Ҫ���Ѿ�ע��Ķ˿ں��ظ���ȷ���������ͺͿͻ��˶˿ں�һ��

	//��ʼ����������ַ��SOCKADDR_IN
	addrSer.sin_addr.S_un.S_addr = inet_addr("10.136.129.86");//�˴�Ӧ���Ƿ������ĵ�ַ
	addrSer.sin_family = AF_INET;//ָ��Э����
	addrSer.sin_port = htons(12248);//��Ҫ��Ҫ���Ѿ�ע��Ķ˿ں��ظ���ȷ���������ͺͿͻ��˶˿ں�һ��

	while (1)
	{
		if (connect(sockCli, (SOCKADDR*)&addrSer, sizeof(addrSer)) != SOCKET_ERROR)//������ӳɹ�
		{
			recv(sockCli, recvbuf, sizeof(recvbuf), 0);
			cout << recvbuf << endl;
		}

	}
	closesocket(sockSer);//�ر��׽���
	closesocket(sockCli);

	WSACleanup();//ж��socket��
	return 0;
}