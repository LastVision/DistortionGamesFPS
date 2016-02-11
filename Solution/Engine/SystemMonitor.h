#pragma once

namespace Prism
{
	class SystemMonitor
	{
	public:

		static int GetMemoryUsageMB();
		static int GetMemoryUsageKB();
		static float GetCPUUsage();

	private:
		static ULONGLONG FixCPUTimings(const FILETIME &a, const FILETIME &b);

		static FILETIME myPrevSysKernel;
		static FILETIME myPrevSysUser;
		static FILETIME myPrevProcKernel;
		static FILETIME myPrevProcUser;
		static bool myFirstTime;
	};
}
