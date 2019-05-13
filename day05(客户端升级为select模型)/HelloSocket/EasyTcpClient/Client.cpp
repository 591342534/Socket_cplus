#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>
#include "iostream"
using namespace std;
enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGINOUT,
	CMD_LOGINOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};
struct DataHeader
{
	short dataLength;//���ݳ��� 
	short cmd;//����
};
//DataPackage
//����
struct Login : public DataHeader
{
	//DataHeader header;
	Login()
	{
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char PassWord[32];
};
struct LoginResult : public DataHeader
{
	LoginResult()
	{
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};
struct LoginOut : public DataHeader
{
	LoginOut()
	{
		dataLength = sizeof(LoginOut);
		cmd = CMD_LOGINOUT;
	}
	char userName[32];
};
struct LoginOutResult : public DataHeader
{
	LoginOutResult()
	{
		dataLength = sizeof(LoginOutResult);
		cmd = CMD_LOGINOUT_RESULT;
		result = 0;
	}
	int result;
};
//�¿ͻ��˼���
struct NewUserJoin : public DataHeader
{
	NewUserJoin()
	{
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		sock = 0;
	}
	int sock;
};
int processor(SOCKET _cSocket)
{
	//ʹ�û���������������
	char szRecv[1024] = {};
	//5 ���տͻ�����������
	int nLen = recv(_cSocket, (char*)&szRecv, sizeof(DataHeader), 0);
	//��� �� �ְ�
	/*����ͷְ���������Ҫ�����ڷ���˽�������ʱһ�ν������ݹ��� �� ���̵����*/
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0)
	{
		printf("��������Ͽ�����, ���������\n", _cSocket);
		return -1;
	}
	//printf("�յ�����: %d ���ݳ��ȣ�%d\n", header.cmd, header.dataLength);
	/*�ж����յ�������*/ //��ͻ��˽����շ����ݵ������ʹ��
				 //if (nLen > sizeof(DataHeader))
				 //{
				 //}
	switch (header->cmd)
	{
		case CMD_LOGIN_RESULT:
		{
			recv(_cSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			LoginResult* login = (LoginResult*)szRecv;
			printf("�յ��������Ϣ:CMD_LOGIN_RESULT ���ݳ��ȣ�%d \n", _cSocket, login->dataLength);
		}
		break;
		case CMD_LOGINOUT_RESULT:
		{
			recv(_cSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			LoginOutResult* loginout = (LoginOutResult*)szRecv;
			printf("�յ��������Ϣ:CMD_LOGINOUT_RESULT ���ݳ��ȣ�%d \n", _cSocket, loginout->dataLength);
		}
		break;
		case CMD_NEW_USER_JOIN:
		{
			recv(_cSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			NewUserJoin* userJoin = (NewUserJoin*)szRecv;
			printf("�յ��������Ϣ:CMD_NEW_USER_JOIN ���ݳ��ȣ�%d \n", _cSocket, userJoin->dataLength);
		}
		break;
	}
}
int main()
{
	/*����Windows socket 2.x����*/
	//�汾��
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	//socket��������������
	WSAStartup(ver, &dat);
	//---------------------------
	//��Socket API��������TCP�ͻ���
	//1������һ��socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _sock)
	{
		printf("���󣬽���Socketʧ��...\n");
	}
	else
	{
		printf("����socket�ɹ�...\n");
	}

	//2�����ӷ����� connect
	sockaddr_in _sin = {};//���ṹ���ʼ��
	_sin.sin_family = AF_INET;//ipv4
	_sin.sin_port = htons(4567);//������ת��Ϊ���޷�������
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in)); //ʹ��sizeof(sockaddr_in)���͸���ȫ
	if (SOCKET_ERROR == ret)
	{
		printf("�������ӷ�����ʧ��...\n");
	}
	else
	{
		printf("���ӷ������ɹ�...\n");
	}
	while (true)
	{
		fd_set fdReads;
		FD_ZERO(&fdReads);
		FD_SET(_sock, &fdReads);
		timeval t = { 0, 0 };
		int ret = select(_sock, &fdReads, 0, 0, &t);
		if (ret < 0)
		{
			printf("select�������������\n");
			break;
		}
		if (FD_ISSET(_sock, &fdReads))
		{
			FD_CLR(_sock, &fdReads);
			if (processor(_sock) == -1)
			{
				printf("select�������2\n");
				break;
			}
			
		}
		printf("����ʱ�䴦������ҵ��\n");
		Login login;
		strcpy(login.userName, "zjj");
		strcpy(login.PassWord, "456451");
		send(_sock, (const char*)&login, sizeof(login), 0);
		Sleep(1000);
	}

	//7 �ر��׽���closesocket
	closesocket(_sock);
	//---------------------------
	WSACleanup();
	printf("���˳�.\n");
	getchar();
	return 0;
}