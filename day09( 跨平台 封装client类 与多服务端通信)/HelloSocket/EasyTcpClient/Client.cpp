#include "EasyTcpClient.hpp"
#include <thread>
#include "iostream"
using namespace std;

//����������������
void cmdThread(EasyTcpClient* client) {
	while (true)
	{
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			client->Close();
			printf("�˳�cmdThread�߳�\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			Login login;
			strcpy(login.userName, "zjj");
			strcpy(login.PassWord, "969513");
			client->SendData(&login);
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			LoginOut logout;
			strcpy(logout.userName, "zjj");
			client->SendData(&logout);
		}
		else
		{
			printf("��֧�ֵ�����\n");
		}
	}
}
int main()
{
	EasyTcpClient client;
	client.Connect("127.0.0.1", 4567);

	EasyTcpClient client2;
	client2.Connect("127.0.0.1", 4568);

	//����UI�߳�
	thread t1(cmdThread, &client);
	t1.detach();//���������߳̽��з���

	thread t2(cmdThread, &client2);
	t2.detach();//���������߳̽��з���

	while (client.isRun() || client2.isRun())
	{
		client.OnRun();
		client2.OnRun();
	}
	client.Close();
	client2.Close();
	printf("���˳�.\n");
	getchar();
	return 0;
}