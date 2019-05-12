#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>
#include "iostream"
using namespace std;
//����windows�ľ�̬���ӿ�--��windows����֧������д��
//��û���������������ȥ����ws2_32.lib
//#pragma comment(lib, "ws2_32.lib")
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
	_sin.sin_family = AF_INET;
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
		//3 ������������
		char cmdBuf[128] = {};
		scanf("%s", cmdBuf);

		//4 ������������
		if (0 == strcmp(cmdBuf, "exit"))
		{
			printf("�յ�exit����������");
			break;
		}
		else
		{
			/*send(
			_In_ SOCKET s,
			_In_reads_bytes_(len) const char FAR * buf,
			_In_ int len,
			_In_ int flags
			);*/
			//5 �������������������
			send(_sock, cmdBuf, strlen(cmdBuf) + 1, 0);
		}

		//6 ���շ�������Ϣ recv
		char recvBuf[128] = {};
		int nlen = recv(_sock, recvBuf, 128, 0);
		if (nlen > 0)
		{
			printf("���յ�����: %s\n", recvBuf);
		}
	}

	//7 �ر��׽���closesocket
	closesocket(_sock);

	//---------------------------
	WSACleanup();
	printf("���˳�.");
	getchar();
	return 0;
}