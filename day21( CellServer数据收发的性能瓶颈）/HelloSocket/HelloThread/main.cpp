#include <iostream>
#include <thread>
//�߳��е���
#include <mutex>
//ԭ�Ӳ���
#include <atomic>
#include "CELLTimestamp.hpp"
using namespace std;
//ԭ�Ӳ���(���ɷָ�Ĳ���) ԭ�� ����
//����ȫ����
mutex m;
//֧�ֱ�׼C++������ƽ̨
/*�̵߳���ں���*/
const int tCount = 4;
//atomic<int> sum;
atomic_int sum = 0;//�൱��ֱ�Ӹ����������� ���������ĵ���ԴС
void workFun(int index)
{
	//��ռʽ
	for (int n = 0; n < 20000000; n ++)
	{
		//�Խ���
		//lock_guard<mutex> lg(m);//���캯��ʱlock ��������ʱunlock
		//m.lock();//������ס����������ٽ�����
		//�ٽ�����Ŀ�ʼ
		sum++;
		//�ٽ�����Ľ���
		//m.unlock();
	}//�̰߳�ȫ �̲߳���ȫ
	//cout << index << "Hello, other thread." << n << endl;
}

int main()
{
	thread t[tCount];
	for (int n = 0; n < tCount; n ++)
	{
		t[n] = thread(workFun, n);
	}
	CELLTimestamp tTime;
	for (int n = 0; n < tCount; n++)
	{
		//t[n].detach();
		t[n].join();//���߳�ִ����֮��Ż�ִ�����߳�
	}
	//t.detach();//�̺߳����߳��໥ ���� �������
	//t.join();ʹ��join�������� ��ʾ�߳�ִ����ɺ���ִ�����߳�
	cout << tTime.getElapsedTimeInMillSec() <<",sum " <<  sum << endl;
	sum = 0;
	tTime.update();
	for (int n = 0; n < 80000000; n++)
	{
		//m.lock();//������ס����������ٽ�����
				 //�ٽ�����Ŀ�ʼ
		sum++;
		//�ٽ�����Ľ���
		//m.unlock();
	}
	cout << tTime.getElapsedTimeInMillSec() << ",sum " << sum << endl;
	cout << "hello, main thread..." << endl;
	getchar();
	return 0;
}