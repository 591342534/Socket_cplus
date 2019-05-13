#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <WinSock2.h>
#include "iostream"
#include "stdio.h"
using namespace std;
/*ʹ�ñ��ĵķ�ʽ���д���*/
//����ͷ
enum CMD
{
	CMD_LOGIN, 
	CMD_LOGINOUT,
	CMD_ERROR
};
struct DataHeader
{
	short dataLength;//���ݳ���
	short cmd;//����
};
//DataPackage
//����
struct Login 
{
	char userName[32];
	char PassWord[32];
};
struct LoginResult
{
	int result;
};
struct LoginOut
{
	char userName[32];
};
struct LoginOutResult
{
	int result;
};

int main() 
{
	/*����Windows socket 2.x����*/
	//�汾��
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	//socket��������������
	WSAStartup(ver, &dat);
	//---------------------------
	//--��Socket API��������TCP�����
	//1������һ��socket  �׽��� ��windows�� linux��ָ����ָ��
	/*socket(
	_In_ int af,(��ʾʲô���͵��׽���)
	_In_ int type,(������)
	_In_ int protocol
	);*/
	//IPV4�������׽��� AF_INET
	//IPV6�������׽��� AF_INET6
	SOCKET _sock =  socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	char msgBuf[] = "Hello, I'm Server.";
	//2��bind �����ڽ��տͻ������ӵ�����˿�
	/*
	bind(
	_In_ SOCKET s,
	_In_reads_bytes_(namelen) const struct sockaddr FAR * name,
	_In_ int namelen
	);
	*/
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;//ipv4
	_sin.sin_port = htons(4567);//�˿ں� ����������������������Ͳ�ͬ �����Ҫ����ת�� ʹ�� host to net unsigned short
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;//inet_addr("127.0.0.1");//��������ip��ַ INADDR_ANY�������е�Ip��ַ�����Է��� һ������
	//�п��ܰ�ʧ��
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin))) 
	{
		printf("���󣬰�����˿�ʧ��...\n");
	}
	else
	{
		printf("�󶨶˿ڳɹ�...\n");
	}

	//3��listen ��������˿�
	/*
	listen(
	_In_ SOCKET s,
	_In_ int backlog
	);*/
	if (SOCKET_ERROR == listen(_sock, 5))
	{
		printf("���󣬼�������˿�ʧ��...\n");
	}
	else 
	{
		printf("��������˿ڳɹ�...\n");
	}

	//4��accept �ȴ����տͻ�������
	/*
	accept(
	_In_ SOCKET s,
	_Out_writes_bytes_opt_(*addrlen) struct sockaddr FAR * addr,
	_Inout_opt_ int FAR * addrlen
	);*/
	sockaddr_in clientAddr = {};//�ͻ��˵�ַ
	int nAddrLen = sizeof(sockaddr_in);//��ַ����
	SOCKET _cSocket = INVALID_SOCKET;

	_cSocket = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (INVALID_SOCKET == _cSocket)
	{
		printf("���󣬽��յ���Ч�ͻ���SOCKET...\n");
	}
	printf("�¿ͻ��˼���: socket = %d IP = %s \n", _cSocket, inet_ntoa(clientAddr.sin_addr));//inet_ntoaת��Ϊ�ɶ���ַ
	//һֱ��ͣ���շ�����
	//char _recvBuf[128] = {};//���ջ�����
	while (true)
	{
		//ʹ��header�����ܿͻ��˴��������������� ͨ�������жϿͻ��˵�����
		DataHeader header = {};
		//5 ���տͻ�����������
		int nLen = recv(_cSocket, (char*)&header, sizeof(DataHeader), 0);
		if (nLen <= 0)
		{
			printf("�ͻ����Ѿ��˳�, ���������");
			break;
		}
		printf("�յ�����: %d ���ݳ��ȣ�%d\n", header.cmd, header.dataLength);
		switch (header.cmd)
		{
			case CMD_LOGIN:
				{
					Login login = {};
					recv(_cSocket, (char*)&login, sizeof(login), 0);
					//�����ж��û��������Ƿ���ȷ�Ĺ���
					LoginResult ret = { 1 };
					send(_cSocket, (char*)&header, sizeof(DataHeader), 0);
					send(_cSocket, (char*)&ret, sizeof(LoginResult), 0);
				}
				break;
			case CMD_LOGINOUT:
				{
					LoginOut loginout = {};
					recv(_cSocket, (char*)&loginout, sizeof(LoginOut), 0);
					//�����ж��û��������Ƿ���ȷ�Ĺ���
					LoginOutResult ret = { 1 };
					send(_cSocket, (char*)&header, sizeof(DataHeader), 0);
					send(_cSocket, (char*)&ret, sizeof(LoginOutResult), 0);
				}
				break;
			default:
				header.cmd = CMD_ERROR;
				header.dataLength = 0;
				send(_cSocket, (char*)&header, sizeof(DataHeader), 0);
			break;
		}
		
	}
	//5��send ��ͻ��˷���һ������
	/*send(
	_In_ SOCKET s,
	_In_reads_bytes_(len) const char FAR * buf,
	_In_ int len,
	_In_ int flags
	);*/
	//char msgBuf[] = "Hello, I'm Server.";
	//+1��ʾ����β��һ�����͹�ȥ
	//send(_cSocket, msgBuf, strlen(msgBuf) + 1, 0);

	//6���ر��׽���closesocket
	closesocket(_sock);

	//---------------------------
	WSACleanup();
	printf("���˳����������");
	getchar();
	cout << "hello..." << endl;
	return 0;
}