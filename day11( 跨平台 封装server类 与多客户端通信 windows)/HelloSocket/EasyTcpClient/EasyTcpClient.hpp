#ifndef _EasyTcpClient_hpp_ 
#define _EasyTcpClient_hpp_
#ifdef _WIN32
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <WinSock2.h>
#else
	#include <unistd.h> //uni std
	#include <arpa/inet.h> 
	#include <string.h>
	#define SOCKET int 
	#define INVALID_SOCKET   (SOCKET)(~0)
	#define SOCKET_ERROR             (-1)
#endif
#include "stdio.h"
#include "MessageHeader.hpp"
class EasyTcpClient 
{
	SOCKET _sock;
public:
	EasyTcpClient() 
	{
		_sock = INVALID_SOCKET;
	}
	/*����������*/
	virtual ~EasyTcpClient() 
	{
		Close();
	}

	/*��ʼ��socket*/
	void initSocket() 
	{
#ifdef _WIN32
		/*����Windows socket 2.x����*/
		//�汾��
		WORD ver = MAKEWORD(2, 2);
		WSADATA dat;
		//socket��������������
		WSAStartup(ver, &dat);
#endif
		if (INVALID_SOCKET != _sock) 
		{
			printf("<socket = %d>�رվ�����...\n", _sock);
			//����������ȹر�
			Close();
		}
		//����һ��socket
		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == _sock)
		{
			printf("���󣬽���Socketʧ��...\n");
		}
		else
		{
			printf("����socket=<%d>�ɹ�...\n", _sock);
		}
	}

	/*���ӷ�����*/
	//�˴���һ��������ʶ�� Ϊ�˱�����linux�еı���
	int Connect(const char* ip, unsigned short port)
	{
		if (INVALID_SOCKET == _sock)
		{
			initSocket();
		}
		//2�����ӷ����� connect
		sockaddr_in _sin = {};//���ṹ���ʼ��
		_sin.sin_family = AF_INET;//ipv4
		_sin.sin_port = htons(port);//������ת��Ϊ���޷�������
#ifdef _WIN32
		_sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
		_sin.sin_addr.s_addr = inet_addr(ip);
#endif
		printf("<socket=%d>�������ӷ�����...\n", _sock);
		int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in)); //ʹ��sizeof(sockaddr_in)���͸���ȫ
		if (SOCKET_ERROR == ret)
		{
			printf("<socket=%d>�������ӷ�����<%s : %d>ʧ��...\n", _sock, ip, port);
		}
		else
		{
			printf("<socket=%d>���ӷ�����<%s : %d>�ɹ�...\n", _sock, ip, port);
		}
		return ret;
	}

	/*�ر�socket*/
	void Close() 
	{
		//��ֹ�ظ�����
		if (_sock != INVALID_SOCKET)
		{
			//�ر�win socket 2.x����
#ifdef _WIN32
			closesocket(_sock);//windows��ʹ��
			WSACleanup();
#else
			close(_sock);
#endif
			_sock = INVALID_SOCKET;
		}

	}

	/*����������Ϣ*/
	bool OnRun() 
	{
		if (isRun())
		{
			fd_set fdReads;
			FD_ZERO(&fdReads);
			FD_SET(_sock, &fdReads);
			timeval t = { 0, 0 };
			int ret = select(_sock + 1, &fdReads, 0, 0, &t);
			if (ret < 0)
			{
				printf("<socket = %d>�������1������\n", _sock);
				Close();
				return false;
			}
			if (FD_ISSET(_sock, &fdReads))
			{
				FD_CLR(_sock, &fdReads);
				if (RecvData(_sock) == -1)
				{
					printf("<socket = %d>�������2������\n", _sock);
					Close();
					return false;
				}
			}
			return true;
		}
		return false;
	}

	bool isRun()
	{
		return _sock != INVALID_SOCKET;
	}

	/*�������� ����ճ�� ��ְ�*/
	int RecvData(SOCKET _cSocket)
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
			printf("<socket = %d>��������Ͽ�����, ���������\n", _cSocket);
			return -1;
		}
		recv(_cSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		onNetMsg(header);
		return 0;
	}

	/*��Ӧ*/
	virtual void onNetMsg(DataHeader* header)
	{
		//printf("�յ�����: %d ���ݳ��ȣ�%d\n", header.cmd, header.dataLength);
		/*�ж����յ�������*/ //��ͻ��˽����շ����ݵ������ʹ��
		switch (header->cmd)
		{
		case CMD_LOGIN_RESULT:
		{
			LoginResult* login = (LoginResult*)header;
			printf("<socket = %d>�յ��������Ϣ:CMD_LOGIN_RESULT ���ݳ��ȣ�%d \n", _sock, login->dataLength);
		}
		break;
		case CMD_LOGINOUT_RESULT:
		{
			LoginOutResult* loginout = (LoginOutResult*)header;
			printf("<socket = %d>�յ��������Ϣ:CMD_LOGINOUT_RESULT ���ݳ��ȣ�%d \n", _sock, loginout->dataLength);
		}
		break;
		case CMD_NEW_USER_JOIN:
		{
			NewUserJoin* userJoin = (NewUserJoin*)header;
			printf("<socket = %d>�յ��������Ϣ:CMD_NEW_USER_JOIN ���ݳ��ȣ�%d \n", _sock, userJoin->dataLength);
		}
		break;
		}
	}
	
	//��������
	int SendData(DataHeader* header)
	{
		if (isRun() && header)
		{
			return send(_sock, (const char*)header, header->dataLength, 0);
		}
		return SOCKET_ERROR;
	}
private:
};
#endif