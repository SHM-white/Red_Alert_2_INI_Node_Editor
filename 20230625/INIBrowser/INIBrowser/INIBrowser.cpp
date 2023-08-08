
#include "FromEngine/Include.h"
#include "PreLink.h"


#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// 此回调函数为全局函数或静态函数;
int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    (void)lParam;
    switch (uMsg)
    {
    case BFFM_INITIALIZED:
    {
        ::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)lpData);
    }
    break;
    default:
        break;
    }
    return 0;
}

bool SelectFilePath(HWND father, std::wstring& strFilePath, const std::wstring& FromPath)//No Unicode
{
    TCHAR szPathName[MAX_PATH] = { 0 };
    BROWSEINFOW bInfo = { 0 };
    bInfo.hwndOwner = father;// 父窗口;
    bInfo.lpszTitle = L"选择目录";
    bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI
        | BIF_UAHINT | BIF_SHAREABLE;
    // 关于更多的 ulFlags 参考 http://msdn.microsoft.com/en-us/library/bb773205(v=vs.85).aspx;
    bInfo.lpfn = (BrowseCallbackProc);
    bInfo.lParam = (LPARAM)(LPCTSTR)(FromPath.c_str());// 注意路径中不要带'\..\'或'\.\'符号，否则设置默认路径失败;

    LPITEMIDLIST lpDlist;
    lpDlist = SHBrowseForFolderW(&bInfo);
    if (nullptr == lpDlist) // 单击了取消或×按钮;
    {
        strFilePath.clear();
        return false;
    }
    SHGetPathFromIDListW(lpDlist, szPathName);
    strFilePath = szPathName;
    return true;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)

{
    using namespace PreLink;

    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;

    char DesktopTmp[300];
    SHGetSpecialFolderPathA(0, DesktopTmp, CSIDL_DESKTOPDIRECTORY, 0);
    GetCurrentDirectoryW(MAX_PATH, CurrentDirW);
    GetCurrentDirectoryA(MAX_PATH, CurrentDirA);
    Desktop = DesktopTmp;
    Defaultpath = MBCStoUnicode(Desktop);

    auto sa = CurrentDirA + std::string("\\browser.log");
    auto sb = CurrentDirA + std::string("\\backend.log");
    GlobalLog.SetPath(sa.c_str());
    GlobalLogB.SetPath(sb.c_str());
    GlobalLog.ClearLog();
    GlobalLogB.ClearLog();

    GlobalLog.AddLog_CurTime(false);
    GlobalLog.AddLog(("INI浏览器 V" + Version).c_str());
    GlobalLog.AddLog_CurTime(false);
    GlobalLog.AddLog("INI浏览器已开始运行。");

    hInst = hInstance;

    RScrX = GetSystemMetrics(SM_CXSCREEN);
    RScrY = GetSystemMetrics(SM_CYSCREEN);

    IBR_DynamicData::Read(1280, 720);//默认分辨率
    IBR_DynamicData::SetRandom();

    int PreLoopRet = PreLoop1();
    if (PreLoopRet)return PreLoopRet;


    if (EnableLog)
    {
        GlobalLog.AddLog_CurTime(false);
        GlobalLog.AddLog("渲染初始化完成。");
    }

    std::thread FrontThr(IBF_Thr_FrontLoop);
    FrontThr.detach();
    std::thread SaveThr(IBS_Thr_SaveLoop);
    SaveThr.detach();

    PreLoop2();

    IBR_FullView::ViewSize = { FontHeight * 8.0f, FontHeight * 8.0f };

    ShellLoop();
    CleanUp();

    return 0;
}

