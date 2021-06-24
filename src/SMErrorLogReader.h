#pragma once
#include <string>
#include <filesystem>
#include <memory>
#include <unordered_set>
#include <thread>
#include <mutex>
#include <condition_variable>

/// <summary>
/// Can be notifified of a new SM Error Log Error
/// </summary>
class IErrorLogEventReciever
{
	public:
		virtual void OnSMErrorFound (std::string& error) = 0;
};

/// <summary>
/// Watches the latest SM Error Log to report new errors.
/// </summary>
class SMErrorLogReader
{
	public:
		IErrorLogEventReciever* EventReciever;

		SMErrorLogReader(std::string& _errorLogPath, int32_t& _waitTime);
		void Stop();

	private:
		bool active;
		int32_t waitTime;
		std::string errorLogRegex;
		std::string errorLogPath;
		std::unordered_set<std::string> pastLogContents;
		std::unique_ptr<std::thread> thread;
		std::mutex loopMutex;
		std::condition_variable loopConditionVar;

		void WatchErrorLog ();
		std::filesystem::path GetLatestErrorLogPath();
		bool ContainsIgnoredStrings(std::string& str);
};

