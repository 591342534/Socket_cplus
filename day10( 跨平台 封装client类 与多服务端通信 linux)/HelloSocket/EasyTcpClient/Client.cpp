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

//һ���ͻ���ͬʱ���Ӷ�������� ������һ�ַ�ʽ�滻���̷߳�ʽ
//�ͻ����Լ��������������
int main()
{
	/*�������Ӷ���*/
	EasyTcpClient client;
	//�ͱ�����˵��ip��ַӦ���� 192.168.242.1
	client.Connect("192.168.242.1", 4567);
	EasyTcpClient client2;
	//�ͱ�����˵��ip��ַӦ���� 192.168.242.1
	client2.Connect("127.0.0.1", 4568);
	EasyTcpClient client3;
	//�ͱ�����˵��ip��ַӦ���� 192.168.242.1
	client3.Connect("192.168.242.128", 4567);


	///*���������߳� ͨ���߳������������������*/
	////����UI�߳�
	//thread t1(cmdThread, &client);
	//t1.detach();//���������߳̽��з���
	////����UI�߳�
	//thread t2(cmdThread, &client2);
	//t2.detach();//���������߳̽��з���
	////����UI�߳�
	//thread t3(cmdThread, &client3);
	//t3.detach();//���������߳̽��з���

	Login login;
	strcpy(login.userName, "zjj");
	strcpy(login.PassWord, "969513");
	while (client.isRun() || client2.isRun() || client3.isRun())
	{
		client.OnRun();
		client2.OnRun();
		client3.OnRun();
		//����ʱ�ͻ����Լ�������Ϣ
		client.SendData(&login);
		client2.SendData(&login);
		client3.SendData(&login);
	}
	client.Close();
	client2.Close();
	client3.Close();
	printf("���˳�.\n");
	getchar();
	return 0;
}