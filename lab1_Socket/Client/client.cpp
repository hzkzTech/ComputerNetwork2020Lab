#define _WINSOCK_DEPRECATED_NO_WARNINGS //ʹinet_addr()��������ʹ��
#include<Winsock2.h>//Windows Socket���ͷ�ļ�
#include<iostream>
#include<string.h>
#pragma comment(lib,"ws2_32.lib")//����ws2_32.libku'wen���ļ�������Ŀ��
using namespace std;

DWORD WINAPI handlerRecver(LPVOID lparam);
void str2char(char c[], string s);
bool strcmp_n(char a[], string b, int n);


//=====================ȫ�ֳ���=====================
const int BUF_SIZE = 512;

//=====================ȫ�ֱ���=====================
SOCKET sockCli;
SOCKADDR_IN addrSer, addrCli;//�˿ںź͵�ַ
int naddr = sizeof(SOCKADDR_IN);

char sendBuf[BUF_SIZE];
char inputBuf[BUF_SIZE];
char recvBuf[BUF_SIZE];

unsigned long dwThreadId = 100;

string userName;

bool QUIT = 0;



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

	//��¼ģ�飨�����û�����
	cout << "�������û�����";
	cin >> userName;

	//����Socket
	sockCli = socket(AF_INET, SOCK_STREAM, 0);

	//��ʼ���ͻ��˵�ַ��SOCKADDR_IN
	addrCli.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	addrCli.sin_family = AF_INET;//ָ��Э����
	addrCli.sin_port = htons(12248);

	//��ʼ����������ַ��SOCKADDR_IN
	addrSer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//�������ĵ�ַ
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(12000);


	while (1)
	{
		if (connect(sockCli, (SOCKADDR*)&addrSer, sizeof(addrSer)) != SOCKET_ERROR)//������ӳɹ�
		{
			string nameMes = "NAME:" + userName;	//��ʼ����������
			str2char(sendBuf,nameMes);
			if (send(sockCli, sendBuf, strlen(sendBuf), 0) < 0) {
				cout << "error: ���ӶϿ�������ʧ��" << endl;
				break;
			}

			HANDLE hThread = CreateThread(NULL, NULL, handlerRecver, LPVOID(sockCli), 0, &dwThreadId);
			CloseHandle(hThread);		//������

			while (1) {	
				//����ģ���������߳�
				char sendM[BUF_SIZE] = "";
				cin >> sendM;

				strcpy_s(sendBuf, sendM);
				if (send(sockCli, sendBuf, strlen(sendBuf), 0) < 0) {
					cout << "404 ERROR:���ӶϿ�" << endl;
					break;
				}

				if (strcmp_n(sendM, "quit", 4)|QUIT) {  //�˳�
					cout << "�ر�Socket" << endl;
					QUIT = 1;
					closesocket(sockCli);
					WSACleanup();//ж��socket��
					cout << "�˳�������" << endl;
					return 0;
				}
			}
		}
	}
	//�ر��׽���
	closesocket(sockCli);

	WSACleanup();//ж��socket��
	cout << "�˳�������" << endl;
	return 0;
}

DWORD WINAPI handlerRecver(LPVOID lparam) {		//����ģ�飬�����߳�
	while (1) {
		if (QUIT)return 0;

		if (recv(sockCli, recvBuf, sizeof(recvBuf), 0) == 0) {
			cout<< "404 ERROR:���ӶϿ�" << endl;
			return 0;
		}

		if(recvBuf[0]!='\0')cout <<"\n������Ϣ:\n"<< recvBuf << endl<<endl;
		memset(recvBuf, 0, BUF_SIZE); //��ʱ��ջ���
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

//�Ƚ�ǰn���ַ�,���Ϊ1
bool strcmp_n(char a[],string b, int n) {
	for (int i = 0; i < n; i++) {
		if (a[i] != b[i])return 0;
	}
	return 1;
}