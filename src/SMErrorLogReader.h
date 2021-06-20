#pragma once
#include <string>
#include <filesystem>
#include <memory>
#include <unordered_set>
#include <thread>

class IErrorLogEventReciever
{
	public:
		virtual void OnSMErrorFound (std::string& error) = 0;
};

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

		void WatchErrorLog ();
		std::filesystem::path GetLatestErrorLogPath();
		bool ContainsIgnoredStrings(std::string& str);
};

