#ifndef LOG_BUFFER_MODULE_H_
#define LOG_BUFFER_MODULE_H_

#include <SafeQueue.h>

class CLogBufferModule
{
public:
	static CLogBufferModule& getInstance() {
		static CLogBufferModule instance;
		return instance;
	}

	virtual ~CLogBufferModule();
	CLogBufferModule(const CLogBufferModule&) = delete;
	CLogBufferModule& operator=(const CLogBufferModule&) = delete;
	CLogBufferModule(CLogBufferModule&&) = delete;
	CLogBufferModule& operator=(CLogBufferModule&&) = delete;

	void setLogLevel(const unsigned int level);
	void log(const std::string& _logMsg, const unsigned int _logLevel = 1);
	void setLogFilePath(const std::string& strLogFileName);
	std::string getDateTimeStampMS();
	std::string getDateTimeStamp();
	std::string getDateStamp();

private:
	CLogBufferModule();
	std::string m_strLogFileName;
	std::ofstream m_logFile;
	unsigned int m_logLevel;
	CSafeQueue<std::string> m_queLog;											// 로그를 받아 넣는다
	std::queue<std::string> m_queBufferQueue;								// 로그를 꺼내 저장한다 -> 사이즈가 100이 넘어가면 한번에 저장한다
	std::atomic_bool m_exitFlag;
	std::mutex m_mtxFile;

	void processLogQueue();				// 큐 메시지를 처리하는 함수
	void createLogFolder(const std::string& strLogFileName);
};

#endif			LOG_BUFFER_MODULE_H_
