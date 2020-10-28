#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<WinSock2.h>//Windows Socket���ͷ�ļ�
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
		cout << "����socket��ʧ��" << endl;
		system("pause");
		return 0;
	}

	//����Socket
	sockSer = socket(AF_INET, SOCK_STREAM, 0);

	//��ʼ����ַ��SOCKADDR_IN
	addrSer.sin_addr.S_un.S_addr = inet_addr("10.136.129.86");//����ipconfig�鿴����IP��ַ�����ƹ���
	addrSer.sin_family = AF_INET;//ָ��Э����
	addrSer.sin_port = htons(12248);//��Ҫ��Ҫ���Ѿ�ע��Ķ˿ں��ظ���ȷ���������ͺͿͻ��˶˿ں�һ��

	//��socket�ͱ�����ַ
	bind(sockSer, (SOCKADDR*)&addrSer, sizeof(SOCKADDR));

	while (1)
	{
		//������������
		listen(sockSer, 5);

		//������������
		sockCli = accept(sockSer, (SOCKADDR*)&addrCli, &naddr);//���ӳɹ��½�һ���׽ӿ�
		if (sockCli != INVALID_SOCKET)//������ӳɹ�
		{
			cout << "���ӳɹ�" << endl;
			strcpy_s(sendbuf, "hello!");//����Ҫ���͵�����
			send(sockCli, sendbuf, sizeof(sendbuf), 0);

		}

	}
	closesocket(sockSer);//�ر��׽���
	closesocket(sockCli);

	WSACleanup();//ж��socket��
	return 0;
}
