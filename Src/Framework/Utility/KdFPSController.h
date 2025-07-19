#pragma once

//============================================================
// アプリケーションのFPS制御 + 測定
//============================================================

struct KdFPSController
{
	// FPS制御
	int		m_nowfps = 0;		// 現在のFPS値
	int		m_maxFps = 240;		// 最大FPS

	void Init();

	void UpdateStartTime();

	void Update();

	float GetDeltaTime() const { return m_deltaTime; }

	std::function<void()> m_SubFunction = nullptr;

	std::mutex m_Mutex;

	void StartSubThread()
	{
		m_subThreadRunning = true;
		m_subThread = std::thread(&KdFPSController::SubThreed, this);
	}

	void StopSubThread()
	{
		m_subThreadRunning = false;
		m_cv.notify_all();
		if (m_subThread.joinable()) m_subThread.join();
	}

	void PushSubJob(const std::function<void()>& job)
	{
		m_subThreadPool.PushJob(job);
	}

	std::atomic<int> m_nowSubThreadFps = 0;


	void WaitAllJobs()
	{
		m_subThreadPool.WaitAll();
	}

private:

	//std::chrono::steady_clock::time_point m_prevTime;
	//float m_deltaTime = 0.0f;

	void SubThreed();

	void Control();

	void Monitoring();

	DWORD		m_frameStartTime = 0;		// フレームの開始時間

	int			m_fpsCnt = 0;				// FPS計測用カウント
	DWORD		m_fpsMonitorBeginTime = 0;	// FPS計測開始時間

	LARGE_INTEGER m_frequency;   
	LARGE_INTEGER m_prevTime;    
	float m_deltaTime = 0.0f;
	LARGE_INTEGER m_monitorBeginTime;

	LARGE_INTEGER m_frameStartCounter;   // フレーム開始時のカウンタ値

	//===========================サブスレッド==============================

	std::thread m_subThread;
//	std::atomic<int> m_nowSubThreadFps = 0;
//	std::mutex m_Mutex;
	std::mutex m_cvMutex;
	std::condition_variable m_cv;
//	std::function<void()> m_SubFunction = nullptr;
	bool m_subThreadRunning = true;
	
	//=====================================================================

	JobSystem m_subThreadPool;  // Workerプール

	const int	kSecond = 1000;				// １秒のミリ秒
};
