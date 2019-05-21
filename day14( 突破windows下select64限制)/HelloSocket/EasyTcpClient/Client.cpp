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
	const int cCount = 2000;
	//ʹ�����鷽ʽ �Ѿ�������c++��ջ�ڴ� ��˽�����ָ�����ͼ���
	EasyTcpClient* client[cCount];
	for (int n = 0; n < cCount; n++)
	{
		if (!g_bRun)
		{
			break;
		}
		client[n] = new EasyTcpClient();
	}
	for (int n = 0; n < cCount; n++)
	{
		if (!g_bRun)
		{
			return 0;
		}
		client[n]->Connect("127.0.0.1", 4567);
		printf("Connect = %d\n", n);
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
			//client[n]->OnRun();
		}
	}
	for (int n = 0; n < cCount; n++)
	{
		client[n]->Close();
	}
	printf("���˳�.\n");
	return 0;
}