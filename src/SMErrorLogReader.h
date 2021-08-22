#pragma once
#include <string>
#include <filesystem>
#include <memory>
#include <unordered_set>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "convar.h"

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

		SMErrorLogReader(std::string& _errorLogPath, ConVar* _waitTime);
		/// <summary>
		/// Stop the error log watcher thread.
		/// </summary>
		void Stop();

	private:
		bool active;
		ConVar* waitTime;
		std::string errorLogRegex;
		std::string errorLogPath;
		std::unordered_set<std::string> pastLogContents;
		std::unique_ptr<std::thread> thread;
		std::mutex loopMutex;
		std::condition_variable loopConditionVar;

		/// <summary>
		/// Loops on a new thread to find new errors in the source mod managed logs.
		/// </summary>
		void WatchErrorLog ();
		/// <summary>
		/// Return the path to the most recent error log file
		/// </summary>
		std::filesystem::path GetLatestErrorLogPath();
		/// <summary>
		/// Check if this string contains any of the 'ignored' strings.
		/// </summary>
		bool ContainsIgnoredStrings(std::string& str);
};

