
#include "Global.h"

//所有版本号相关数据
const std::string Version = u8"0.2b1";
const int VersionMajor = 0;
const int VersionMinor = 2;
const int VersionRelease = 1;
const int VersionN = VersionMajor * 10000 + VersionMinor * 100 + VersionRelease;
const std::string VersionNStr = u8"000201";
const char* AppName = u8"INI浏览器";
const char* AppNameA = "INI浏览器";
const wchar_t* AppNameW = L"INI浏览器";

//统一的文件头
const int32_t SaveFileHeaderSign = 0x00114514;

//设置的实例
//IBF_Setting IBF_Inst_Setting;
//IBR_Setting IBR_Inst_Setting;

//设置的标识
std::atomic<bool> SettingLoadComplete{ false };
std::atomic<bool> SettingSaveComplete{ false };
const wchar_t* SettingFileName = L"./Resources/Setting.hex";

//日志
//LogClass GlobalLog{ "browser.log" };
//LogClass GlobalLogB{ "backend.log" };
//BufString LogBuf, LogBufB;
bool EnableLog = true;
bool EnableLogEx = false;

//线程信息交换
IBRF_Bump IBRF_CoreBump;
uint64_t ShellLoopLastTime;
//DWORD BackThreadID{ INT_MAX };

//设置内容
int KeyPerPage = 10;
int FontHeight = 24;
int WindowSizeAdjustX = 15, WindowSizeAdjustY = 5;
bool IsProjectOpen = false;//TODO:打开项目
//HWND MainWindowHandle = 0;
int RScrX, RScrY, ScrX, ScrY;

//格式类型表
//IBF_DefaultTypeList IBF_Inst_DefaultTypeList;
//IBF_DefaultModuleList IBF_Inst_DefaultModuleList;
//IBF_Project IBF_Inst_Project;
//IBR_Project IBR_Inst_Project;
//IBS_Project IBS_Inst_Project;

//调试
//IBR_Debug IBR_Inst_Debug;

//随机种子
//std::default_random_engine GlobalRnd;
const int ModuleRandomParameterLength = 16;

//prelink
//namespace PreLink
//{
//    GLFWwindow* window;
//    HINSTANCE hInst;
//    ImFont* font;
//}
