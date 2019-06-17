#ifndef _CELL_TASK_H_
#include <thread>
#include <mutex>
//���������д洢���� �ܿ��ٽ�������ɾ���Ȳ���
#include <list>
//��������-����
//ִ������ķ�������
/*���������� ����������*/
class CellTask
{
public:
	CellTask()
	{

	};
	//����������
	virtual ~CellTask() 
	{
		
	};
	//ִ������
	virtual void doTask()
	{

	}
private:

};

//ִ������ķ�������
class CellTaskServer
{
private:
	//��������
	std::list<CellTask*> _tasks;
	//�������ݻ�����
	std::list<CellTask*> _tasksBuff;
	//�ı����ݻ�����ʱ��Ҫ����
	std::mutex _mutex;
	//�߳�
	//std::thread _thread;
public:
	//�������
	void addTask(CellTask* task)
	{
		//�������������ж����Խ��м���
		std::lock_guard<std::mutex> lock(_mutex);
		_tasksBuff.push_back(task);
	}
	//���������߳�
	void Start()
	{
		//�߳�
		std::thread t(std::mem_fn(&CellTaskServer::OnRun), this);
		t.detach();
	}
protected:
	//��������
	void OnRun()
	{
		while (true)
		{
			//�ӻ�����ȡ������
			if (!_tasksBuff.empty())
			{
				std::lock_guard<std::mutex> lock(_mutex);
				for (auto pTask : _tasksBuff)
				{
					_tasks.push_back(pTask);
				}
				_tasksBuff.clear();/*��ջ���*/
			}
			//���û������
			if (_tasks.empty())
			{
				//û�������ʱ��ȴ�һ���� ����cpuռ�ù�����Դ
				std::chrono::milliseconds t(1);
				std::this_thread::sleep_for(t);
				continue;
			}
			//��������
			for (auto pTask : _tasks)
			{
				pTask->doTask();
				delete pTask;
			}
			//�������
			_tasks.clear();
		}
		
	}
private:
};
#endif