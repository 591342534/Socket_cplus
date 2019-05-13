#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <WinSock2.h>
#include "iostream"
#include "stdio.h"
using namespace std;
//����windows�ľ�̬���ӿ�--��windows����֧������д��
//��û���������������ȥ����ws2_32.lib
//#pragma comment(lib, "ws2_32.lib")
int main() 
{
	/*����Windows socket 2.x����*/
	//�汾��
	/*
	socket����У�
	�������ò�ͬ��Winsock�汾������MAKEWORD(2,2)���ǵ���2.2�棬MAKEWORD(1,1)���ǵ���1.1�档
	��ͬ�汾��������ģ�����1.1��ֻ֧��TCP/IPЭ�飬��2.0�����֧�ֶ�Э�顣2.0�������õ���
	������ԣ��κ�ʹ��1.1���Դ���롢�������ļ���Ӧ�ó��򶼿��Բ����޸ĵ���2.0�淶��ʹ�á�
	����winsock 2.0֧���첽 1.1��֧���첽.
	*/
	WORD ver = MAKEWORD(2, 2);
	/*WSADATA��һ�����ݽṹ������ṹ�������洢��WSAStartup�������ú󷵻ص�Windows Sockets���ݡ�������Winsock.dllִ�е����ݡ�*/
	WSADATA dat;
	//socket�������������� ����������
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
	//IPPROTO_TCP ʹ��TCPЭ��

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
	//5 ������ǽӿ����5���ͻ��˿����������������
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
	//char msgBuf[] = "Hello, I'm Server.";
	_cSocket = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (INVALID_SOCKET == _cSocket)
	{
		printf("���󣬽��յ���Ч�ͻ���SOCKET...\n");
	}
	printf("�¿ͻ��˼���: socket = %d IP = %s \n", _cSocket, inet_ntoa(clientAddr.sin_addr));//inet_ntoaת��Ϊ�ɶ���ַ
	//һֱ��ͣ���շ�����
	char _recvBuf[128] = {};//���ջ�����
	while (true)
	{
		//5 ���տͻ�����������
		/*recv(
			_In_ SOCKET s,
			_Out_writes_bytes_to_(len, return) __out_data_source(NETWORK) char FAR * buf,
			_In_ int len,
			_In_ int flags
		);*/
		//��128����������
		int nLen = recv(_cSocket, _recvBuf, 128, 0);
		if (nLen <= 0)
		{
			printf("�ͻ����Ѿ��˳�, ���������");
			break;
		}
		printf("�յ�����: %s\n", _recvBuf);
		//6 ��������
		if (0 == strcmp(_recvBuf, "getName"))
		{
			//7 send ��ͻ��˷���һ������
			char msgBuf[] = "xiao qiang.";
			//��1��Ŀ����Ϊ�˽��ַ�����ĩβһ������ ����ͻ��˽����ַ������ȵļ���
			send(_cSocket, msgBuf, strlen(msgBuf) + 1, 0);
		}
		else if(0 == strcmp(_recvBuf, "getAge"))
		{
			//7 send ��ͻ��˷���һ������
			char msgBuf[] = "22";
			send(_cSocket, msgBuf, strlen(msgBuf) + 1, 0);
		}
		else
		{
			//7 send ��ͻ��˷���һ������
			char msgBuf[] = "?????";
			send(_cSocket, msgBuf, strlen(msgBuf) + 1, 0);
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