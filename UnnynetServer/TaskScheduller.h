#ifndef _TASK_SCHEDULLER_H_
#define _TASK_SCHEDULLER_H_

#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

namespace unhg 
{

namespace unnynet
{

struct Task
{
	boost::function<void ()> callback;
	boost::function<void ()> action;
};

class TaskScheduller : private boost::noncopyable
{
public:

	typedef boost::function<void (const std::string& error)> error_callback_t;

private:

	boost::thread_group threads;
	boost::asio::io_service& m_io_service;
	boost::shared_ptr<boost::asio::io_service::work> m_work;
	error_callback_t m_err_callback;

public:

	TaskScheduller (boost::asio::io_service& service, error_callback_t err_callback); 
	~TaskScheduller ();

	void Schedule (const Task& task);

private:

	void Run ();

};

typedef boost::shared_ptr<TaskScheduller> TaskSchedullerPtr;

}
}

#endif

