#include "stdafx.h"
#include "FileWatcher.h"


namespace Prism
{
	FileWatcher* FileWatcher::myInstance = nullptr;

	FileWatcher* FileWatcher::GetInstance()
	{
		if (myInstance == nullptr)
		{
			myInstance = new FileWatcher();
		}

		return myInstance;
	}

	void FileWatcher::Destroy()
	{
		delete myInstance;
	}

	FileWatcher::FileWatcher()
	{
		myFileDatas.Init(256);
	}

	void FileWatcher::WatchFile(const std::string& aFile, std::function<void()> aCallBack)
	{

#ifdef DLL_EXPORT
		WIN32_FIND_DATA findData;
		HANDLE findHandle = FindFirstFile(aFile.c_str(), &findData);
		if (findHandle != INVALID_HANDLE_VALUE)
		{
			FileData newData;
			newData.myCallBack = aCallBack;
			newData.myFilePath = aFile;
			newData.myFileTime = findData.ftLastWriteTime;
			myFileDatas.Add(newData),

				FindClose(findHandle);
		}
		else
		{
			ENGINE_LOG("[FileWatcher]: Tried to watch a file that couldnt be found, %s", aFile.c_str());
			DL_MESSAGE_BOX("Failed to watch file", "[FileWatcher]", MB_ICONWARNING);
		}

#define FILE_WATCHER_ACTIVE
#endif

#ifndef RELEASE_BUILD
#ifndef FILE_WATCHER_ACTIVe
		WIN32_FIND_DATA findData;
		HANDLE findHandle = FindFirstFile(aFile.c_str(), &findData);
		if (findHandle != INVALID_HANDLE_VALUE)
		{
			FileData newData;
			newData.myCallBack = aCallBack;
			newData.myFilePath = aFile;
			newData.myFileTime = findData.ftLastWriteTime;
			myFileDatas.Add(newData),

				FindClose(findHandle);
		}
		else
		{
			ENGINE_LOG("[FileWatcher]: Tried to watch a file that couldnt be found, %s", aFile.c_str());
			DL_MESSAGE_BOX("Failed to watch file", "[FileWatcher]", MB_ICONWARNING);
		}
#endif
#endif
	}

	void FileWatcher::UnWatchFile(const std::string& aFile)
	{
		for (int i = 0; i < myFileDatas.Size(); ++i)
		{
			if (myFileDatas[i].myFilePath == aFile)
			{
				myFileDatas.RemoveCyclicAtIndex(i);
				return;
			}
		}
	}

	void FileWatcher::CheckFiles()
	{
		for (int i = 0; i < myFileDatas.Size(); ++i)
		{
			WIN32_FIND_DATA findData;
			HANDLE findHandle = FindFirstFile(myFileDatas[i].myFilePath.c_str(), &findData);
			if (findHandle != INVALID_HANDLE_VALUE)
			{
				if (CompareFileTime(&myFileDatas[i].myFileTime, &findData.ftLastWriteTime) != 0)
				{
					Sleep(100);
					myFileDatas[i].myFileTime = findData.ftLastWriteTime;
					myFileDatas[i].myCallBack();
				}


				FindClose(findHandle);	
			}
			else
			{
				ENGINE_LOG("[FileWatcher]: Tried to check a file that couldnt be found, %s", myFileDatas[i].myFilePath.c_str());
				DL_MESSAGE_BOX("Failed to check file", "[FileWatcher]", MB_ICONWARNING);
			}
		}
	}

	void FileWatcher::Clear()
	{
		myFileDatas.RemoveAll();
	}

}