#include "EasyTcpClient.hpp"
#include <thread>
#include "iostream"
using namespace std;
bool g_bRun = true;
//����������������
void cmdThread() {
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

//һ���ͻ���ͬʱ���Ӷ�������� ������һ�ַ�ʽ�滻���̷߳�ʽ
//�ͻ����Լ��������������
int main()
{
	const int cCount = 1000;
	//ʹ�����鷽ʽ �Ѿ�������c++��ջ�ڴ�
	EasyTcpClient* client[cCount];
	for (int n = 0; n < cCount; n++)
	{
		client[n] = new EasyTcpClient();
	}
	for (int n = 0; n < cCount; n++)
	{
		client[n]->Connect("192.168.242.128", 4567);
	}
	thread t1(cmdThread);
	t1.detach();
	Login login;
	strcpy(login.userName, "zjj");
	strcpy(login.PassWord, "969513");
	/*while (client.isRun() || client2.isRun())*/
	while (g_bRun)
	{
		for (int n = 0; n < cCount; n++)
		{
			client[n]->SendData(&login);
			client[n]->OnRun();
		}
	}
	for (int n = 0; n < cCount; n++)
	{
		client[n]->Close();
	}
	printf("���˳�.\n");
	getchar();
	return 0;
}