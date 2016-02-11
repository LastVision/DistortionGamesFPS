#pragma once


#define WATCH_FILE(FILE, FUNCTION) (Prism::FileWatcher::GetInstance()->WatchFile(FILE, std::bind(&FUNCTION, this, FILE)))
#define UNWATCH_FILE(FILE) (Prism::FileWatcher::GetInstance()->UnWatchFile(FILE))

#include <functional>
#include <string>

namespace Prism
{
	class FileWatcher
	{
	public:
		static FileWatcher* GetInstance();
		static void Destroy();

		void WatchFile(const std::string& aFile, std::function<void()> aCallBack);
		void UnWatchFile(const std::string& aFile);

		void CheckFiles();
		void Clear();

	private:
		FileWatcher();

		struct FileData
		{
			std::string myFilePath;
			std::function<void()> myCallBack;
			FILETIME myFileTime;
		};

		CU::GrowingArray<FileData> myFileDatas;

		static FileWatcher* myInstance;
	};
}