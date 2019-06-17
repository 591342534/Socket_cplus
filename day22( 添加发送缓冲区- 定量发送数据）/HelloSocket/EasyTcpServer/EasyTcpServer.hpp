#ifndef _EasyTcpServer_hpp_
#define _EasyTcpServer_hpp_
#ifdef _WIN32
	#define FD_SETSIZE	2506
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
//using namespace std;
#include "stdio.h"
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include "MessageHeader.hpp"
#include "iostream"
#include "CELLTimestamp.hpp"
#include <map>


//��������С��Ԫ��С
#ifndef RECV_BUFF_SIZE
#define RECV_BUFF_SIZE 10240 * 5
#define SEND_BUFF_SIZE RECV_BUFF_SIZE
#endif

//�ͻ�����������
class ClientSocket
{
public:
	ClientSocket(SOCKET sockfd = INVALID_SOCKET) 
	{
		/*�ʺϸ��ϵ�c++������*/
		_sockfd = sockfd;
		memset(_szMsgBuf, 0, sizeof(RECV_BUFF_SIZE));
		_lastPos = 0;

		memset(_szSendBuf, 0, sizeof(SEND_BUFF_SIZE));
		_lastSendPos = 0;
	}
	SOCKET sockfd()
	{
		return _sockfd;
	}
	char* msgBuf()
	{
		return _szMsgBuf;
	}
	int getLastPos()
	{
		return _lastPos;
	}
	void setLastPos(int pos)
	{
		this->_lastPos = pos;
	}

	//����ָ��sock����
	int SendData(DataHeader* header)
	{
		int ret = SOCKET_ERROR;
		//Ҫ���͵ĳ���
		int nSendLen = header->dataLength;
		//Ҫ���͵�����
		const char* pSendData = (const char*)header;
		while (true)
		{
			/******************************������������**********************************/
			if ((_lastSendPos + nSendLen) >= SEND_BUFF_SIZE)
			{
				//������Կ��������ݳ���
				int nCopyLen = SEND_BUFF_SIZE - _lastSendPos;
				memcpy(_szSendBuf + _lastSendPos, pSendData, nCopyLen);
				//����ʣ������λ��
				pSendData += nCopyLen;
				//����ʣ�����ݳ���
				nSendLen -= nSendLen;
				//��������
				ret = send(_sockfd, _szSendBuf, SEND_BUFF_SIZE, 0);
				//����β��λ��Ϊ0
				_lastSendPos = 0;
				//���ʹ��� ֱ�ӷ���
				if (SOCKET_ERROR == ret)
				{
					return ret;
				}
			}
			else
			{
				//��Ҫ���͵����� ���������ͻ�����β��
				memcpy(_szSendBuf + _lastSendPos, pSendData, nSendLen);
				//����β��λ����Ϊ
				_lastSendPos += nSendLen;
				break;
			}
			/****************************************************************************/
		}
		return ret;
		//
		//if (header)
		//{
		//	//������������
		//	return send(_sockfd, (const char*)header, header->dataLength, 0);
		//}	
	}
private:
	SOCKET _sockfd;//fdset�ļ������� file 
	//�ڶ������� ��Ϣ������
	char _szMsgBuf[RECV_BUFF_SIZE];
	//��Ϣ������������β��λ��
	int _lastPos;

	//�ڶ������� ���ͻ�����
	char _szSendBuf[RECV_BUFF_SIZE];
	//��Ϣ������������β��λ��
	int _lastSendPos;
};

//�����¼��ӿ�
class INetEvent
{
public:
	//���麯��
	//�ͻ��˼����¼�
	virtual void OnNetJoin(ClientSocket* pClient) = 0;
	//�ͻ����뿪�¼�
	virtual void Onleave(ClientSocket* pClient) = 0;//���麯��
	//�ͻ�����Ϣ�¼�
	virtual void OnNetMsg(ClientSocket* pClient, DataHeader* header) = 0;
	//recv�¼�
	virtual void OnNetRecv(ClientSocket* pClient) = 0;
	//virtual void OnNetMsg(ClientSocket* pClient) = 0;
private:
};

//������
class CellServer
{
public:
	CellServer(SOCKET sock = INVALID_SOCKET)
	{
		_sock = sock;
		_pNetEvent = nullptr;
	}
	
	~CellServer()
	{
		Close();
		_sock = INVALID_SOCKET;
	}

	void setEventObj(INetEvent* event)
	{
		_pNetEvent = event;
	}
	//�ر�Socket
	void Close()
	{
		if (_sock != INVALID_SOCKET)
		{
#ifdef _WIN32
			for (auto iter : _clients)
			{
				closesocket(iter.second->sockfd());
				delete iter.second;
			}
			//�ر��׽���
			closesocket(_sock);
#else
			for (auto iter : _clients)
			{
				close(iter.second->sockfd());
				delete iter.second;
			}
			//�ر��׽���
			close(_sock);
#endif
			//����һ��
			_clients.clear();
		}
	}
	//�Ƿ�����
	bool isRun()
	{
		return _sock != INVALID_SOCKET;
	}

	//int _nCount = 0;
	//����������Ϣ
	//���ݿͻ�socket fd_set
	fd_set _fdRead_bak;//����
	bool _clients_change;
	SOCKET _maxSock;
	bool OnRun()
	{
		_clients_change = true;
		while (isRun())
		{
			if (_clientsBuff.size() > 0)
			{			//�ӻ�����������ȡ���ͻ�����
				std::lock_guard<std::mutex> lock(_mutex);
				//c++11�бȽ����еı�������
				for (auto pClient : _clientsBuff)
				{
					_clients[pClient->sockfd()] = pClient;
					//_clients.push_back(pClient);
				}
				_clientsBuff.clear();
				_clients_change = true;
			}
			//���û����Ҫ����Ŀͻ��˾�����
			if (_clients.empty())
			{
				//�����ڴ˴���ͣһ����
				std::chrono::milliseconds t(1);
				std::this_thread::sleep_for(t);
				continue;
			}
			fd_set fdRead;//������(socket)���� 
			FD_ZERO(&fdRead);//���fd_set�������͵����� ��ʵ���ǽ�fd_count ��Ϊ0;(����������Ҫ�У�����ᷢ������Ԥ�ϵĺ��)
			//FD_SET(_sock, &fdRead);//�����������뼯����(�ڼ������¼���һ���ļ�������)

			if (_clients_change)
			{
				_clients_change = false;
				//��������(socket)���뼯��
				//_maxSock = _clients[0]->sockfd();
				_maxSock = _clients.begin()->second->sockfd();
				for (auto iter : _clients)
				{
					//���ͻ���SOCKET����ɶ�������
					/*�Ż��ô� �������ƿ��*/
					FD_SET(iter.second->sockfd(), &fdRead);
					//FD_SET(_clients[n]->sockfd(), &fdRead);
					if (_maxSock < iter.second->sockfd())
					{
						//��ʾ�ͻ��˵�SOCKET ��ÿͻ���SOCKET
						_maxSock = iter.second->sockfd();
					}
				}
				memcpy(&_fdRead_bak, &fdRead, sizeof(fd_set));
			}
			else
			{
				memcpy(&fdRead, &_fdRead_bak, sizeof(fd_set));
			}
			int ret = select(_maxSock + 1, &fdRead, nullptr, nullptr, nullptr);	 /*���Ϸ�ʽΪ������ʽ�����û�пͻ��˽��뽫�����ڴ˴�*/
			if (ret < 0)
			{
				printf("<socket = %d>���������\n", _sock);
				Close();
				return false;//��ʾ���� ����ѭ��
			}
			else if(ret == 0)
			{
				continue;
			}
			
#ifdef _WIN32
			for (int n = 0; n < fdRead.fd_count; n++)
			{
				auto iter = _clients.find(fdRead.fd_array[n]);
				if (iter != _clients.end())
				{
					if (-1 == RecvData(iter->second))
					{
						if (_pNetEvent)
						{
							_pNetEvent->Onleave(iter->second);
						}
						_clients_change = true;
						_clients.erase(iter->first);
					}
				}else{
					printf("error.if (iter !=_clients.end() )");
				}
			}
#else
			std::vector <ClientSocket *> temp;
			for (auto iter : _clients)
			{
				if (FD_ISSET(iter.second->sockfd(), &fdRead))
				{
					if (-1 == RecvData(iter.second))
					{
						if (_pNetEvent)
						{
							_pNetEvent->Onleave(iter.second);
						}
						_clients_change = false;
						temp.push_back(iter.second);
					}
				}
			}
			for (auto pClient : temp)
			{
				_clients.erase(pClient->sockfd());
				delete pClient;
			}
#endif
		}
	}

	//�������� ����ճ��
	int RecvData(ClientSocket* pClient)
	{
		//5 ���տͻ�����������
		/*���������� ȥ������Ŀ������� ����ϵͳ��Դ����*/
		char* szRecv = pClient->msgBuf() + pClient->getLastPos();
		//int nLen = (int)recv(pClient->sockfd(), _szRecv, RECV_BUFF_SIZE, 0);
		int nLen = (int)recv(pClient->sockfd(), szRecv, (RECV_BUFF_SIZE) - pClient->getLastPos(), 0);
		_pNetEvent->OnNetRecv(pClient);
		if (nLen <= 0)
		{
			printf("�ͻ���<Socket = %d>�Ѿ��˳�, ���������\n", pClient->sockfd());
			return -1;
		}
		//����ȡ�����ݿ�������Ϣ������
		//memcpy(pClient->msgBuf() + pClient->getLastPos(), _szRecv, nLen);
		//����Ϣ������������β��λ�ú���
		pClient->setLastPos(pClient->getLastPos() + nLen);

		//�ж���Ϣ�����������ݳ��ȴ�����ϢͷDataHeader����
		while (pClient->getLastPos() >= sizeof(DataHeader))//ѭ����Ϊ�˽��ճ��
		{
			//��ʱ�Ϳ���֪����ǰ��Ϣ�ĳ���
			DataHeader* header = (DataHeader*)pClient->msgBuf();
			//�ж���Ϣ�����������ݳ��ȴ�����Ϣ����
			if (pClient->getLastPos() >= header->dataLength)
			{
				//ʣ��δ������Ϣ���������ݵĳ���
				int nSize = pClient->getLastPos() - header->dataLength;
				//����������Ϣ
				onNetMsg(pClient, header);
				//����Ϣ������ʣ��δ��������ǰ��
				memcpy(pClient->msgBuf(), pClient->msgBuf() + header->dataLength, nSize);
				//��Ϣ��������β��λ��ǰ��
				pClient->setLastPos(nSize);
			}
			else
			{
				//��Ϣ������ʣ�����ݲ���һ��������Ϣ
				break;
			}
		}
		return 0;
	}

	//��Ӧ������Ϣ
	virtual void onNetMsg(ClientSocket* pClient, DataHeader* header)
	{
		_pNetEvent->OnNetMsg(pClient, header);
	}

	void addClient(ClientSocket* pClient)
	{
		//�����������
		//����Խ��� - ���������ռ�Ƚ�׼ȷ
		std::lock_guard<std::mutex> lock(_mutex);
		_clientsBuff.push_back(pClient);
	}

	void Start()
	{
		//mem_fun���Ӱ�ȫ��ת�� mem_fn�����
		_pThread = std::thread(std::mem_fn(&CellServer::OnRun), this);
	}

	size_t getClientCount()
	{
		//�Ѿ�ȡ�õ� �� �����������δȡ�õ�
		return _clients.size() + _clientsBuff.size();
	}

private:
	SOCKET _sock;
	//��ʽ�ͻ�����
	std::map<SOCKET, ClientSocket*> _clients;
	//����ͻ����� �����ڳ���ִ�еĹ������𲽼���ͻ��߳�
	std::vector<ClientSocket*> _clientsBuff;
	//������е���
	std::mutex _mutex;
	std::thread _pThread;//������й���
	//�����¼�����  ������ϵ
	INetEvent* _pNetEvent;
};

//new ���ڴ�(�ڴ���)   һ��������Ƿ���ջ�ռ��е�
class EasyTcpServer : public INetEvent
{
private:
	SOCKET _sock;
	//��Ϣ������� �ڲ��ᴴ���߳�
	std::vector<CellServer*> _cellServers;
	//����һ����ʱ�� ÿ��
	CELLTimestamp _tTime;
protected:
	//SOCKET recv����
	std::atomic_int _recvCount;
	//�յ���Ϣ
	std::atomic_int _msgCount;
	//�ͻ��˼���
	std::atomic_int _clientCount;
public:
	int getSock() {
		return this->_sock;
	}

public:
	EasyTcpServer()
	{
		_sock = INVALID_SOCKET;
		_recvCount = 0;
		_msgCount = 0;
		_clientCount = 0;
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
		//���������÷� ��using namespace std�����������ͻ
		int ret = ::bind(_sock, (sockaddr*)&_sin, sizeof(_sin));

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
		SOCKET cSock = INVALID_SOCKET;
		//nAddrLen������windows��linux֮�䲻һ����
#ifdef _WIN32
		cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
#else
		cSock = accept(_sock, (sockaddr*)&clientAddr, (socklen_t*)&nAddrLen);
#endif
		if (INVALID_SOCKET == cSock)
		{
			printf("socket<%d>���󣬽��յ���Ч�ͻ���SOCKET...\n", (int)_sock);
		}
		else
		{
			//���¿ͻ��˷�����ͻ�����С��cellserver
			addClientToCellServer(new ClientSocket(cSock));
			//��ȡIP��ַ inet_ntoa(clientAddr.sin_addr)
		}
		return cSock;
	}

	void addClientToCellServer(ClientSocket* pClient)
	{
		//���ҿͻ��������ٵ�CellServer��Ϣ�����̶߳���
		auto pMinServer = _cellServers[0];
		for (auto pCellServer : _cellServers)
		{
			if (pMinServer->getClientCount() > pCellServer->getClientCount())
			{
				pMinServer = pCellServer;
			}
		}
		pMinServer->addClient(pClient);
		OnNetJoin(pClient);
	}

	void Start(int nCellServer)
	{
		for (int n = 0; n < nCellServer; n ++)
		{
			auto ser = new CellServer(_sock);
			_cellServers.push_back(ser);
			//ע�������¼�
			ser->setEventObj(this);
			//������Ϣ�����߳�
			ser->Start();
		}
	}

	//�ر�Socket
	void Close()
	{
		if (_sock != INVALID_SOCKET)
		{
#ifdef _WIN32
			closesocket(_sock);
			_sock = INVALID_SOCKET;
			WSACleanup();
#else
			close(_sock);

#endif
		}
	}

	//int _nCount = 0;
	//����������Ϣ
	bool OnRun()
	{
		if (isRun())
		{
			time4msg();
			//cout << isRun() << endl;
			fd_set fdRead;//������(socket)����
			FD_ZERO(&fdRead);//���fd_set�������͵����� ��ʵ���ǽ�fd_count ��Ϊ0
			FD_SET(_sock, &fdRead);//�����������뼯����
			SOCKET maxSock = _sock;
			timeval t = { 0, 10 };//ʱ����� &t ���Ϊ1��
			int ret = select(maxSock + 1, &fdRead, 0, 0, &t);//NULL��������
			if (ret < 0)
			{
				printf("Accept Select���������\n");
				Close();
				return false;//��ʾ���� ����ѭ��
			}
			//������socket�ɶ��Ļ���ʾ �пͻ����Ѿ����ӽ�����
			if (FD_ISSET(_sock, &fdRead))//�ж��������Ƿ��ڼ�����
			{
				//����
				FD_CLR(_sock, &fdRead);
				Accept();
				//�����ӵ�ʱ��ȥ�������� �����ӿ������ٶ�
				return true;
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

	//���㲢���ÿ���յ���������Ϣ
	virtual void time4msg()
	{
		auto t1 = _tTime.getElapsedSecond();
		//���ﵽһ����ʱ�����
		if (t1 >= 1.0)
		{
			printf("thread<%d>, time<%lf>, socket<%d>, clients<%d>, recv<%d>, msg<%d>\n", _cellServers.size(), t1, _sock, (int)_clientCount,(int)(_recvCount / t1), (int)(_msgCount / t1));
			_recvCount = 0;
			_msgCount = 0;
			_tTime.update();
		}
	}

	//ֻ�ᱻһ���̵߳��� ��ȫ
	virtual void OnNetJoin(ClientSocket* pClient)
	{
		_clientCount++;
	}

	//cellServer 4 ����̴߳��� ����ȫ ���ֻ����һ��cellServer ���ǰ�ȫ��
	virtual void Onleave(ClientSocket* pClient)
	{
		_clientCount--;
	}

	//cellServer 4 ����̴߳��� ����ȫ
	virtual void OnNetMsg(ClientSocket* pClient, DataHeader* header)
	{
		_recvCount++;
	}
};
#endif