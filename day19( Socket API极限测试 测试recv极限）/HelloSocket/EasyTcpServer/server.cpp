#include "EasyTcpServer.hpp"
#include <thread>
#include "iostream"
using namespace std;

bool g_bRun = true;
//����������������
void cmdThread1() {
	while (true)
	{
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			g_bRun = false;
			printf("�˳�cmdThread�߳�\n");
			break;
		}
		else
		{
			printf("��֧�ֵ�����\n");
		}
	}
}

class MyServer : public EasyTcpServer
{
public:
	//ֻ�ᱻһ���̵߳��� ��ȫ
	virtual void OnNetJoin(ClientSocket* pClient)
	{
		_clientCount++;
		//printf("client<%d> join\n", pClient->sockfd());
	}

	//cellServer 4 ����̴߳��� ����ȫ ���ֻ����һ��cellServer ���ǰ�ȫ��
	virtual void Onleave(ClientSocket* pClient)
	{
		_clientCount--;
		//printf("client<%d> leave\n", pClient->sockfd());
	}

	//cellServer 4 ����̴߳��� ����ȫ
	virtual void OnNetMsg(ClientSocket* pClient, DataHeader* header)
	{
		_msgCount++;
		switch (header->cmd)
		{
		case CMD_LOGIN:
		{
			Login* login = (Login*)header;
			//printf("�յ��ͻ���<Socket = %d>����:CMD_LOGIN ���ݳ��ȣ�%d userName = %s passWord = %s\n", cSock, login->dataLength, login->userName, login->PassWord);
			//LoginResult ret;
			//send(cSock, (char*)&ret, sizeof(LoginResult), 0);
			//pClient->SendData(&ret);
		}
		break;
		case CMD_LOGINOUT:
		{
			LoginOut *loginout = (LoginOut*)header;
			//printf("�յ�����:CMD_LOGINOUT ���ݳ��ȣ�%d userName = %s\n", loginout->dataLength, loginout->userName);
			//LoginOutResult ret;
			//send(cSock, (char*)&ret, sizeof(LoginOutResult), 0);
			//SendData(cSock, &ret);
		}
		break;
		default:
		{
			printf("<socket = %d>�յ�δ������Ϣ ���ݳ��ȣ�%d \n", pClient->sockfd(), header->dataLength);
			//DataHeader ret;
			//send(cSock, (char*)&header, sizeof(DataHeader), 0);
			//SendData(cSock, &ret);
		}
		break;
		}
	}
	virtual void OnNetRecv(ClientSocket* pClient)
	{
		_recvCount++;
		//printf("client<%d> leave\n", pClient->sockfd());
	}
private:
};

int main()
{
	/*��ӹ��� -> �����߳�ʹ�÷����������� �ͻ���һ��ͨ������exit�˳�*/
	//EasyTcpServer server;
	MyServer server;
	server.InitSocket();
	server.Bind(nullptr, 4567);
	server.Listen(5);
	server.Start(4);
	thread t(cmdThread1);
	t.detach();

	/*EasyTcpServer server1;
	server1.InitSocket();
	server1.Bind(nullptr, 4568);
	server1.Listen(5);
	thread t1(cmdThread1, &server1);
	t1.detach();*/

	/*while (server.isRun() || server1.isRun())*/
	while (g_bRun)
	{
		server.OnRun();
		//server1.OnRun();
		//printf("����ʱ�䴦������ҵ��...\n");
	}
	server.Close();
	//server1.Close();
	printf("���˳���\n");
	getchar();
	return 0;
}