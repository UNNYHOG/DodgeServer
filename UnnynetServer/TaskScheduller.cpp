#include "TaskScheduller.h"

namespace unhg 
{

namespace unnynet
{

class TaskWorker
{
private:

	Task m_task;

public:

	TaskWorker (const Task& task) : m_task (task) {};
	void TaskWorker::operator() ()
	{
		m_task.action ();
		m_task.callback ();
	}

};

// ----------------------------------------------------------------------------------------------
TaskScheduller::TaskScheduller (boost::asio::io_service& service, error_callback_t err_callback) :
	m_io_service (service), m_err_callback (err_callback)
// ----------------------------------------------------------------------------------------------
{
	int cores_number = boost::thread::hardware_concurrency ();
	for (std::size_t i = 0; i < cores_number; ++i)
		threads.create_thread (boost::bind (&TaskScheduller::Run, this));

	m_work.reset (new boost::asio::io_service::work (m_io_service));
}

// ----------------------------------------------------------------------------------------------
TaskScheduller::~TaskScheduller ()
// ----------------------------------------------------------------------------------------------
{
	m_work.reset ();
	threads.join_all ();
}

// ----------------------------------------------------------------------------------------------
void TaskScheduller::Schedule (const Task& task)
// ----------------------------------------------------------------------------------------------
{
	m_io_service.post (TaskWorker(task));
}

// ----------------------------------------------------------------------------------------------
void TaskScheduller::Run ()
// ----------------------------------------------------------------------------------------------
{
	while (true)
	{
		try
		{
			m_io_service.run();
			break;
		}
		catch (std::exception& exp)
		{
			std::string err = exp.what();
			m_err_callback (err);
		}
		catch (...)
		{
			std::string err = "Unknown server error";
			m_err_callback (err);
		}
	}
}

}
}