#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#include "iostream"
using namespace std;
//����windows�ľ�̬���ӿ�--��windows����֧������д��
//��û���������������ȥ����ws2_32.lib
//#pragma comment(lib, "ws2_32.lib")
int main() 
{
	/*����Windows socket 2.x����*/
	//�汾��
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	//socket��������������
	WSAStartup(ver, &dat);
	//---------------------------
	/*
	��Socket API��������TCP�ͻ���
	1������һ��socket
	2�����ӷ����� connect
	3�����շ�������Ϣ recv
	4���ر��׽���closesocket
	//--��Socket API��������TCP�����
	1������һ��socket
	2��bind �����ڽ��տͻ������ӵ�����˿�
	3��listen ��������˿�
	4��accept �ȴ����տͻ�������
	5��send ��ͻ��˷���һ������
	6���ر��׽���closesocket
	*/
	//---------------------------
	WSACleanup();
	cout << "hello..." << endl;
	return 0;
}