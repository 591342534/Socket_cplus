#include "EasyTcpServer.hpp"
#include <thread>
#include "iostream"
using namespace std;
//����������������
void cmdThread1(EasyTcpServer* server) {
	while (true)
	{
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			printf("�˳�������<%d>�߳�\n", server->getSock());
			server->Close();
			break;
		}
		else
		{
			printf("��֧�ֵ�����\n");
		}
	}
}

int main()
{
	/*��ӹ��� -> �����߳�ʹ�÷����������� �ͻ���һ��ͨ������exit�˳�*/
	EasyTcpServer server;
	server.InitSocket();
	server.Bind(nullptr, 4567);
	server.Listen(5);
	/*thread t(cmdThread1, &server);
	t.detach();

	EasyTcpServer server1;
	server1.InitSocket();
	server1.Bind(nullptr, 4568);
	server1.Listen(5);
	thread t1(cmdThread1, &server1);
	t1.detach();*/

	/*while (server.isRun() || server1.isRun())*/
	while (server.isRun())
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