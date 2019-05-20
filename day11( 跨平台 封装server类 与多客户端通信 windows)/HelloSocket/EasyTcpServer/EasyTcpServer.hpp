#ifndef _EasyTcpServer_hpp_
#define _EasyTcpServer_hpp_
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
using namespace std;
#include "stdio.h"
#include <vector>
#include "MessageHeader.hpp"
#include "iostream"

class EasyTcpServer
{
private:
	SOCKET _sock;
	vector<SOCKET> g_clients;
public:
	int getSock() {
		return this->_sock;
	}
public:
	EasyTcpServer()
	{
		_sock = INVALID_SOCKET;
	}

	virtual ~EasyTcpServer()
	{
		Close();
	}

	//��ʼ��Socket
	SOCKET InitSocket()
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
			printf("<socket = %d>�رվ�����...\n", (int)_sock);
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
		return _sock;
	}

	//��IP �� �˿ں�
	int Bind(const char* ip, unsigned short port)
	{
		/*if (INVALID_SOCKET == _sock)
		{
			InitSocket();
		}*/
		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;//ipv4
		_sin.sin_port = htons(port);//�˿ں� ����������������������Ͳ�ͬ �����Ҫ����ת�� ʹ�� host to net unsigned short
#ifdef _WIN32
		if (ip)
		{
			_sin.sin_addr.S_un.S_addr = inet_addr(ip);
		}
		else
		{
			_sin.sin_addr.S_un.S_addr = INADDR_ANY;//inet_addr("127.0.0.1");//��������ip��ַ INADDR_ANY�������е�Ip��ַ�����Է��� һ������
		}
#else
		if (ip)
		{
			_sin.sin_addr.s_addr = inet_addr(ip);
		}
		else
		{
			_sin.sin_addr.s_addr = INADDR_ANY;//inet_addr("127.0.0.1");//��������ip��ַ INADDR_ANY�������е�Ip��ַ�����Է��� һ������
		}
#endif
		int ret = bind(_sock, (sockaddr*)&_sin, sizeof(_sin));

		//�п��ܰ�ʧ��
		if (SOCKET_ERROR == ret)
		{
			printf("���󣬰�����˿�<%d>ʧ��...\n", port);
		}
		else
		{
			printf("�󶨶˿�<%d>�ɹ�...\n", port);
		}
		return ret;
	}

	//�����˿ں�
	int Listen(int n)
	{
		//�����˿�
		int ret = listen(_sock, n);
		if (SOCKET_ERROR == ret)
		{
			printf("socket = <%d>���󣬼�������˿�ʧ��...\n", (int)_sock);
		}
		else
		{
			printf("socket = <%d>��������˿ڳɹ�...\n", (int)_sock);
		}
		return ret;
	}

	//���տͻ�������
	int Accept()
	{
		//accept �ȴ����ܿͻ�������
		sockaddr_in clientAddr = {};
		int nAddrLen = sizeof(sockaddr_in);
		SOCKET _cSocket = INVALID_SOCKET;
		//nAddrLen������windows��linux֮�䲻һ����
#ifdef _WIN32
		_cSocket = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
#else
		_cSocket = accept(_sock, (sockaddr*)&clientAddr, (socklen_t*)&nAddrLen);
#endif
		if (INVALID_SOCKET == _cSocket)
		{
			printf("socket<%d>���󣬽��յ���Ч�ͻ���SOCKET...\n", (int)_sock);
		}
		else
		{
			NewUserJoin userJoin;
			SendDataToAll(&userJoin);
			g_clients.push_back(_cSocket);
			printf("socket<%d>�¿ͻ��˼���: socket = %d IP = %s \n", (int)_sock, (int)_cSocket, inet_ntoa(clientAddr.sin_addr));//inet_ntoaת��Ϊ�ɶ���ַ
		}
		return _cSocket;
	}

	//�ر�Socket
	void Close()
	{
		if (_sock != INVALID_SOCKET)
		{
#ifdef _WIN32
			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				closesocket(g_clients[n]);
			}
			closesocket(_sock);
			_sock = INVALID_SOCKET;
			WSACleanup();
#else
			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				close(g_clients[n]);
			}
			close(_sock);
#endif
		}
	}

	//����������Ϣ
	bool OnRun()
	{
		if (isRun())
		{
			//cout << isRun() << endl;
			fd_set fdRead;//������(socket)����
			fd_set fdWrite;
			fd_set fdExp;
			FD_ZERO(&fdRead);//���fd_set�������͵����� ��ʵ���ǽ�fd_count ��Ϊ0
			FD_ZERO(&fdWrite);//������
			FD_ZERO(&fdExp);
			FD_SET(_sock, &fdRead);//�����������뼯����
			FD_SET(_sock, &fdWrite);
			FD_SET(_sock, &fdExp);
			SOCKET maxSock = _sock;
			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				FD_SET(g_clients[n], &fdRead);//����ɶ������в�ѯ �Ƿ��пɶ�����
				if (maxSock < g_clients[n])
				{
					//�ҵ������������е����������
					maxSock = g_clients[n];
				}
			}
			timeval t = { 0, 0 };//ʱ����� &t ���Ϊ1��
			int ret = select(maxSock + 1, &fdRead, &fdWrite, &fdExp, &t);
			/*_sock + 1�ܹ� ʹ����linux������ʹ��*/
			/*���Ϸ�ʽΪ������ʽ�����û�пͻ��˽��뽫�����ڴ˴�*/
			if (ret < 0)
			{
				printf("<socket = %d>���������\n", _sock);
				Close();
				return false;//��ʾ���� ����ѭ��
			}
			//������socket�ɶ��Ļ���ʾ �пͻ����Ѿ����ӽ�����
			if (FD_ISSET(_sock, &fdRead))//�ж��������Ƿ��ڼ�����
			{
				//����
				FD_CLR(_sock, &fdRead);
				Accept();
			}
			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				if (FD_ISSET(g_clients[n], &fdRead))
				{
					if (RecvData(g_clients[n]) == -1)
					{
						auto iter = g_clients.begin() + n;
						if (iter != g_clients.end())
						{
							g_clients.erase(iter);
						}
					}
				}
			}
			return true;
		}
		return false;
	}

	//�Ƿ�����
	bool isRun()
	{
		return _sock != INVALID_SOCKET;
	}

	//�������� ����ճ��
	int RecvData(SOCKET _cSocket)
	{
		//ʹ�û���������������
		char szRecv[1024] = {};
		//5 ���տͻ�����������
		int nLen = (int)recv(_cSocket, (char*)&szRecv, sizeof(DataHeader), 0);
		//��� �� �ְ�
		/*����ͷְ���������Ҫ�����ڷ���˽�������ʱһ�ν������ݹ��� �� ���̵����*/
		DataHeader* header = (DataHeader*)szRecv;
		if (nLen <= 0)
		{
			printf("�ͻ���<Socket = %d>�Ѿ��˳�, ���������\n", _cSocket);
			return -1;
		}
		recv(_cSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		onNetMsg(_cSocket, header);
		return 0;
	}

	//��Ӧ������Ϣ
	virtual void onNetMsg(SOCKET _cSocket, DataHeader* header)
	{
		switch (header->cmd)
		{
		case CMD_LOGIN:
		{
			
			Login* login = (Login*)header;
			printf("�յ��ͻ���<Socket = %d>����:CMD_LOGIN ���ݳ��ȣ�%d userName = %s passWord = %s\n", _cSocket, login->dataLength, login->userName, login->PassWord);
			LoginResult ret;
			send(_cSocket, (char*)&ret, sizeof(LoginResult), 0);
		}
		break;
		case CMD_LOGINOUT:
		{
			LoginOut *loginout = (LoginOut*)header;
			printf("�յ�����:CMD_LOGINOUT ���ݳ��ȣ�%d userName = %s\n", loginout->dataLength, loginout->userName);
			LoginOutResult ret;
			send(_cSocket, (char*)&ret, sizeof(LoginOutResult), 0);
		}
		break;
		default:
		{
			DataHeader header = { 0, CMD_ERROR };
			send(_cSocket, (char*)&header, sizeof(DataHeader), 0);
		}
		break;
		}
	}

	//����ָ��sock����
	int SendData(SOCKET _cSock, DataHeader* header)
	{
		if (isRun() && header)
		{
			return send(_cSock, (const char*)header, header->dataLength, 0);
		}
		return SOCKET_ERROR;
	}

	//Ⱥ����Ϣ
	void SendDataToAll(DataHeader* header)
	{
		//���¿ͻ��˼���Ⱥ���������û� ������������ ��������ɱ����
		for (int n = (int)g_clients.size() - 1; n >= 0; n--)
		{
			SendData(g_clients[n], header);
		}
	}
};
#endif