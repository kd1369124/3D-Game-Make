#include "KdFPSController.h"

// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
// FPSの制御コントローラー
// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
void KdFPSController::Init()
{
	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_prevTime);
	QueryPerformanceCounter(&m_monitorBeginTime);
	QueryPerformanceCounter(&m_frameStartCounter); // 最初のカウンタ

	m_subThread = std::thread([this]() { SubThreed(); });
}

void KdFPSController::UpdateStartTime()
{
	m_frameStartTime = timeGetTime();

	QueryPerformanceCounter(&m_frameStartCounter);

}

void KdFPSController::Update()
{
	//auto now = std::chrono::steady_clock::now();
	//std::chrono::duration<float> delta = now - m_prevTime;
	//m_deltaTime = delta.count();
	//m_prevTime = now;

	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	m_deltaTime = float(now.QuadPart - m_prevTime.QuadPart) / float(m_frequency.QuadPart);

	m_prevTime = now;

	Control();

	Monitoring();
}

void KdFPSController::SubThreed()
{
	using namespace std::chrono;

	int fpsCount = 0;
	auto prev = steady_clock::now();

	while (m_subThreadRunning)
	{
		// JobSystem に任せてるので、
		// 特にここで重い処理をする必要はない

		// FPS カウントだけ更新
		fpsCount++;
		auto now = steady_clock::now();
		double elapsed = duration<double>(now - prev).count();

		if (elapsed >= 0.5)
		{
			m_nowSubThreadFps = int(fpsCount / elapsed);
			fpsCount = 0;
			prev = now;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

// FPS制御
void KdFPSController::Control()
{

	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	double elapsed = double(now.QuadPart - m_frameStartCounter.QuadPart) / double(m_frequency.QuadPart) * 1000.0;

	double timePerFrame = 1000.0 / m_maxFps;

	if (elapsed < timePerFrame)
	{
		double waitTime = timePerFrame - elapsed;

		if (waitTime > 1.0)
		{
			std::unique_lock<std::mutex> lk(m_cvMutex);
			m_cv.wait_for(lk, std::chrono::duration<double, std::milli>(waitTime - 0.5));
		}

		// 残りをスピンウェイトで微調整
		while (true)
		{
			QueryPerformanceCounter(&now);
			elapsed = double(now.QuadPart - m_frameStartCounter.QuadPart) / double(m_frequency.QuadPart) * 1000.0;
			if (elapsed >= timePerFrame) break;
		}
	}

	QueryPerformanceCounter(&m_frameStartCounter);

}

// 現在のFPS計測
void KdFPSController::Monitoring()
{

	m_fpsCnt++;

	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	double elapsed = double(now.QuadPart - m_monitorBeginTime.QuadPart) / double(m_frequency.QuadPart);

	if (elapsed >= 0.5) // 0.5秒おき
	{
		m_nowfps = int(m_fpsCnt / elapsed);
		m_monitorBeginTime = now;
		m_fpsCnt = 0;
	}
}
