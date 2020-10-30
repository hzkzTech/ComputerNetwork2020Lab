#define _WINSOCK_DEPRECATED_NO_WARNINGS //ʹinet_addr()��������ʹ��
#include<WinSock2.h>
#include<iostream>
#include<string.h>
#include<string>
#include<vector>
#include<set>
#include<map>
#pragma comment(lib,"ws2_32.lib")//����ws2_32.libku'wen���ļ�������Ŀ��
using namespace std;

void str2char(char c[], string s);
bool strcmp_n(char a[], string b, int n);
DWORD WINAPI handlerRequest(LPVOID lparam);
DWORD WINAPI handlerRev(LPVOID lparam);
string add_to_MES(char recv[], string recfrom);

const int BUF_SIZE = 512;

//=====================ȫ�ֱ���=====================
SOCKADDR_IN addrSrv, addrClient;//�˿ںź͵�ַ
int len = sizeof(SOCKADDR_IN);

char sendBuf[BUF_SIZE];
char inputbuf[BUF_SIZE];
char recvBuf[BUF_SIZE];

unsigned long dwThreadId=100;

set<string> Users; //���ߵ��û��б�

map<string, vector<string>> MES; //������Ϣ��ӳ��


//=====================��������=====================

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

//�Ƚ�ǰn���ַ�,���Ϊ1
bool strcmp_n(char a[], string b, int n) {
	for (int i = 0; i < n; i++) {
		if (a[i] != b[i])return 0;
	}
	return 1;
}

string add_to_MES(char recv[],string recfrom) { //���յ�����Ϣ������У��ȴ�ת��
	string sendto = "";
	string mes = recfrom+"";
	int i=0;
	for (; recv[i] != ':'&& recv[i] != '\0'; i++) {
		sendto += recv[i];
	}
	if (Users.find(sendto) == Users.end()) //�޴��û�
		return sendto;
	for (; recv[i] != '\0'; i++) {
		mes += recv[i];
	}
	MES[sendto].push_back(mes);

	return "";
}

DWORD WINAPI handlerRev(LPVOID lparam) {		//����ģ�飬�����߳�
	char recvbuf[BUF_SIZE] = "";
	char sendbuf[BUF_SIZE] = "";
	para* P= (para*)(LPVOID)lparam;
	SOCKET ClientSocket = P->sock;
	string userName = P->userName;
	while (1) {
		//������Ϣ
		if (recv(ClientSocket, recvbuf, 50, 0) == 0) {
			cout << ClientSocket << "404 ERROR:���ӶϿ�" << endl;
			break;
		}

		if (recvbuf[0] != '\0') {
			cout << "\n�յ�����" << userName << "����Ϣ:\"" << recvbuf << "\"" << endl;

			if (strcmp_n(recvbuf, "quit", 4)) {  //�˳�
				cout << "�û�" << userName << "�˳�" << endl;
				cout << "�ر�Socket" << ClientSocket << endl;
				break;
			}
			string sendto;
			if ((sendto = add_to_MES(recvbuf, userName)) != "") {
				//�޸��û�
				string t = "403 �û�" + sendto + "δ����";
				str2char(sendbuf, t);
				send(ClientSocket, sendbuf, strlen(sendbuf), 0);
				memset(sendbuf, 0, sizeof(sendbuf) / sizeof(char)); //��ʱ��ջ���
				sendbuf[0] = '\0';
			}
			else {
				strcpy_s(sendbuf, "200 �ѷ���");
				send(ClientSocket, sendbuf, strlen(sendbuf), 0);
				memset(sendbuf, 0, sizeof(sendbuf) / sizeof(char)); //��ʱ��ջ���
				sendbuf[0] = '\0';
			}
		}
		memset(recvbuf, 0, sizeof(recvbuf) / sizeof(char)); //��ʱ��ջ���
		recvbuf[0] = '\0';

	}
	return 0;
}


DWORD WINAPI handlerRequest(LPVOID lparam) {
	SOCKET ClientSocket = (SOCKET)(LPVOID)lparam;
	string userName="";
	char recvbuf[BUF_SIZE]="";
	char sendbuf[BUF_SIZE]="";

	while (1) { //ȷ���û���
		if (recv(ClientSocket, recvbuf, 50, 0) == 0) {
			cout << ClientSocket << "404 ERROR:���ӶϿ�" << endl;
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
					strcpy_s(sendbuf, "401 ERROR:�û����ظ�");
					send(ClientSocket, sendbuf, strlen(sendbuf), 0);
					memset(sendbuf, 0, sizeof(sendbuf) / sizeof(char)); //��ʱ��ջ���
					sendbuf[0] = '\0';
				}
				else {
					break;  //�����յ��û���
				}
			}
			else {
				strcpy_s(sendbuf, "402 ERROR:δ�յ��û���");
				send(ClientSocket, sendbuf, strlen(sendbuf), 0);
				memset(sendbuf, 0, sizeof(sendbuf) / sizeof(char)); //��ʱ��ջ���
				sendbuf[0] = '\0';
			}
		}
	}

	//��ʼ��
	Users.insert(userName);
	vector<string> wait_mes;
	MES.insert(make_pair(userName, wait_mes));
	int iter = 0;

	cout << "�û�" << userName << "��������" << endl;

	memset(recvbuf, 0, sizeof(recvbuf) / sizeof(char)); //��ʱ��ջ���
	recvbuf[0] = '\0';

	para P = { ClientSocket ,userName };
	dwThreadId += 1;
	HANDLE hThread = CreateThread(NULL, NULL, handlerRev, LPVOID(&P), 0, &dwThreadId);
	CloseHandle(hThread);		//������

	//���գ�������Ϣ
	while (1) {

		//����δ������Ϣ
		int sizeMes = MES[userName].size();
		while (iter < sizeMes) {
			string t = MES[userName][iter];
			t += '\n';
			cout<< "\n������Ϣ��" << userName << ":\"" << t <<"\""<< endl;
			str2char(sendbuf, t);
			send(ClientSocket, sendbuf, strlen(sendbuf), 0);
			memset(sendbuf, 0, sizeof(sendbuf) / sizeof(char)); //��ʱ��ջ���
			sendbuf[0] = '\0';
			++iter;
		}
	}
	closesocket(ClientSocket);
	return 0;
}

//=====================������=====================

int main() { 

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)//���سɹ�����0
	{
		//���������Ϣ
		cout << "101 ERROR:����socket��ʧ��" << endl;
		system("pause");
		return 0;
	}
	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);

	//��ʼ����ַ��SOCKADDR_IN
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//����IP��ַ
	addrSrv.sin_family = AF_INET;//ָ��Э����
	addrSrv.sin_port = htons(12000);

	bind(sockSrv, (SOCKADDR*)& addrSrv, sizeof(SOCKADDR)); 

	cout << "��������ʼ���ɹ�" << endl;
	listen(sockSrv, 5); 
	while (1) { 
		SOCKET sockConn = accept(sockSrv, (SOCKADDR*)& addrClient, &len); 
		dwThreadId += 1;
		if (sockConn != INVALID_SOCKET)//������ӳɹ�
		{
			cout << "һ̨�ͻ����ӳɹ�" << endl;
			strcpy_s(sendBuf, "���ӳɹ�!");
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
