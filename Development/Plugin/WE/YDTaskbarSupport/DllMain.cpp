#include <windows.h>
#include <random>
#include <string>				 	  		
#include <base/filesystem.h>
#include <bee/platform/version.h>
#include <base/path/ydwe.h>
#include "Windows7.h"
#include "JumpList.h"

static const wchar_t* namelist[] = {
	L"��̰�ŵ˹��YDWE",
	L"�ƶ�˹��YDWE",
	L"������˹��YDWE",
	L"�������̼�YDWE",
	L"��������˹��YDWE",
	L"߯�Ǽ�YDWE",
	L"���Ǽ�YDWE",
	L"߯��˹��YDWE",
	L"����Ħ������YDWE",
	L"���ؼ�YDWE",
	L"߯��˹��YDWE",
	L"����ŵ˹��YDWE",
};

static const wchar_t* getCurrentAppID()
{
	for (size_t i = 0; i < sizeof(namelist) / sizeof(namelist[0]); ++i)
	{
		const wchar_t* name = namelist[i];
		HANDLE mutex = CreateMutexW(NULL, TRUE, name);
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			if (mutex)
				CloseHandle(mutex);  // C6001: ʹ��δ��ʼ�����ڴ�"*mutex" ???
			continue;
		}
		else if (mutex)
		{
			return name;
		}
	}

	static std::wstring	randomresult = L"YDWE.WorldEdit." + std::to_wstring((long long)std::random_device()());
	return randomresult.c_str();
}

bool JumpListAddRecentTask(JumpList& jumpList, fs::path const& ydweDirectory, fs::path const& filePath)
{
	HRESULT hr = jumpList.AddTask(filePath.filename().c_str(), [&](base::com::unique_ptr<IShellLinkW>& shellLinkPtr)
	{
		shellLinkPtr->SetPath((ydweDirectory / L"YDWE.exe").c_str());
		shellLinkPtr->SetArguments((L" -loadfile \"" + filePath.wstring() + L"\"").c_str());
		shellLinkPtr->SetDescription(filePath.c_str());
		shellLinkPtr->SetIconLocation((ydweDirectory / L"bin" / L"logo.ico").c_str(), 0);
	});

	return SUCCEEDED(hr);
}

extern "C" void Initialize()
{
	if (bee::platform::get_version().ver < bee::platform::WinVer::Win7)
	{
		return ;
	}

	const wchar_t* currentAppID = 0;
	HRESULT hr = windows7::GetCurrentProcessExplicitAppUserModelID((wchar_t**)&currentAppID);
	if (FAILED(hr) || !currentAppID)
	{
		currentAppID = getCurrentAppID();
		windows7::SetCurrentProcessExplicitAppUserModelID(currentAppID);
	}

	JumpList jumpList;
	jumpList.SetAppID(currentAppID);
	if (jumpList.InitializeList())
	{
		fs::path ydweDirectory = base::path::ydwe(false);

		jumpList.AddTaskSeparator();

		jumpList.AddTask(L"YDWE����", [&](base::com::unique_ptr<IShellLinkW>& shellLinkPtr)
		{
			shellLinkPtr->SetPath((ydweDirectory / L"bin" / L"YDWEConfig.exe").c_str());
			shellLinkPtr->SetDescription(L"��YDWE���ó���");
			shellLinkPtr->SetIconLocation((ydweDirectory / L"bin" / L"logo.ico").c_str(), 0);
		});
	}
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		::DisableThreadLibraryCalls(module);
	}

	return TRUE;
}
