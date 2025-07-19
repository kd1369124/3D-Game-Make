#pragma once
class JobSystem
{
public:
	JobSystem(size_t numThreads = std::thread::hardware_concurrency())
	{
		m_running = true;

		for (size_t i = 0; i < numThreads; ++i)
		{
			m_workers.emplace_back([this]()
				{
					while (m_running)
					{
						std::function<void()> job;

						{
							std::unique_lock<std::mutex> lock(m_mutex);

							if (m_jobs.empty())
							{
								m_cv.wait(lock);
								continue;
							}

							job = std::move(m_jobs.front());
							m_jobs.pop();
						}

						if (job) job();
					}
				});
		}
	}

	~JobSystem()
	{
		m_running = false;
		m_cv.notify_all();

		for (auto& worker : m_workers)
		{
			if (worker.joinable()) worker.join();
		}
	}

	void PushJob(std::function<void()> job)
	{
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_jobs.push(std::move(job));
		}
		m_cv.notify_one();
	}

	void WaitAll()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		while (!m_jobs.empty())
		{
			m_cv.wait(lock);
		}
	}


	template<class T>
	std::future<T> Enqueue(std::function<T()> job)
	{
		auto task = std::make_shared<std::packaged_task<T()>>(job);
		std::future<T> result = task->get_future();

		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_jobs.push([task]() { (*task)(); });
		}
		m_condition.notify_one();
		return result;
	}

private:
	std::vector<std::thread> m_workers;
	std::queue<std::function<void()>> m_jobs;

	std::mutex m_mutex;
	std::condition_variable m_cv;
	std::atomic<bool> m_running = true;
	std::mutex m_resultMutex;
	std::condition_variable m_condition;


};