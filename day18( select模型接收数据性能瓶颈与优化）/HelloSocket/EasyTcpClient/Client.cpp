#include "EasyTcpClient.hpp"
#include <thread>
#include "iostream"
using namespace std;
bool g_bRun = true;

//�ͻ�������
const int cCount = 400;

//�����߳�����
const int tCount = 4;

//�ͻ�������
EasyTcpClient* client[cCount];

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

void sendThread(int id) //4���߳� 1 - 4
{
	printf("thread<%d>, start\n", id);
	//ID 1-4
	int c = cCount / tCount;
	int begin = (id - 1) * c;
	int end = id * c;
	for (int n = begin; n < end; n++)
	{
		client[n] = new EasyTcpClient();
	}
	for (int n = begin; n < end; n++)
	{
		//client[n]->Connect("101.132.170.175", 80);
		client[n]->Connect("127.0.0.1", 4567);
	}
	printf("thread<%d>, Connect<begin=%d, end=%d>\n", id, begin, end);
	std::chrono::milliseconds t(3000);
	std::this_thread::sleep_for(t);
	
	Login login[10];
	for (int n = 0; n < 10; n++)
	{
		strcpy(login[n].userName, "zjj");
		strcpy(login[n].PassWord, "969513");
	}
	const int nLen = sizeof(login);
	while (g_bRun)
	{
		for (int n = begin; n < end; n++)
		{
			client[n]->SendData(login, nLen);
			//client[n]->OnRun();
		}
	}
	for (int n = begin; n < end; n++)
	{
		client[n]->Close();
		delete client[n];
	}
	printf("thread<%d>, exit\n", id);
}

//һ���ͻ���ͬʱ���Ӷ�������� ������һ�ַ�ʽ�滻���̷߳�ʽ
//�ͻ����Լ��������������
int main()
{
	thread t1(cmdThread);
	t1.detach();

	//���������߳�
	for (int n = 0; n < tCount; n ++)
	{
		thread t1(sendThread, n + 1);
		t1.detach();
	}
	while (g_bRun)
	{
		Sleep(100);
	}
	printf("���˳�.\n");
	return 0;
}