#pragma once
#include <string>
#include <functional>
#include <map>
#include <vector>
#include <windows.h>
#include <thread>
#include <mutex>

namespace CU
{
	typedef std::function<void(const std::string&)> callback_function_file;

	class FileWatcher
	{
	public:
		FileWatcher();
		~FileWatcher();

		/* Will check the file for includes and add them as well*/
		bool WatchFileChangeWithDependencies(std::string aFile, callback_function_file aFunctionToCallOnChange);
		bool WatchFileChange(std::string aFile, callback_function_file aFunctionToCallOnChange);


		void FlushChanges();
	private:
		void UpdateChanges(const std::string& aDir);
		void OnFolderChange(const std::string& aDir);
		bool WatchDirectory(const std::string& aDir);
		void OnFileChange(std::string& aFile);
		std::vector<WIN32_FIND_DATA> GetAllFilesInFolder(std::string aDir);

		std::thread* myThread;

		typedef std::map<std::string, std::vector<WIN32_FIND_DATA>> FolderMap;
		FolderMap myFolders;

		std::vector<std::string> myFileChangedThreaded;
		std::vector<std::string> myFileChanged;
		std::map<std::string, std::vector<callback_function_file> > myCallbacks;

		typedef std::map<std::string, std::vector<std::string>> Dependencies;
		Dependencies myDependencies;

		std::mutex myMutex;
		bool myShouldEndThread;
		bool myThreadIsDone;
	};

}