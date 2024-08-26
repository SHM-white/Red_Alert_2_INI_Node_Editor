#pragma once

#include "IBFront.h"
#include "IBRender.h"
#include "FromEngine/RFBump.h"
#include <atomic>

#define IBD_ShutDownDestructor(x) x##_ShutDownDestructor
#define IBD_Inst_ShutDownDestructor(x) x##_Inst_ShutDownDestructor
#define IBD_ShutDownDtorClass(x,fn) \
struct IBD_ShutDownDestructor(x)\
{\
    IBD_ShutDownDestructor(x)() {}\
    ~IBD_ShutDownDestructor(x)() { fn(); }\
}IBD_Inst_ShutDownDestructor(x)

#define IBD_BoolStr(v) ((v)?"true":"false")

//���а汾���������
extern const std::string Version;
extern const int VersionMajor;
extern const int VersionMinor;
extern const int VersionRelease;
extern const int VersionN;
extern const std::string VersionNStr;
extern const char* _TEXT_UTF8 AppName;
extern const char* _TEXT_ANSI AppNameA;
extern const wchar_t* AppNameW;

//ͳһ���ļ�ͷ
extern const int32_t SaveFileHeaderSign;

//���õ�ʵ��
extern IBF_Setting IBF_Inst_Setting;
extern IBR_Setting IBR_Inst_Setting;

//���õı�ʶ
extern std::atomic<bool> SettingLoadComplete;
extern std::atomic<bool> SettingSaveComplete;
extern const wchar_t* SettingFileName;

//��־
extern LogClass GlobalLog;
extern LogClass GlobalLogB;
extern BufString LogBuf, LogBufB;
extern bool EnableLog;//LOG
extern bool EnableLogEx;//EXTRA LOG

//�߳���Ϣ����
extern IBRF_Bump IBRF_CoreBump;
#define IBD_RInterruptF(x) IBG_RInterruptF_RangeLock __IBD_RInterruptF_VariableA_##x{ IBRF_CoreBump };
#define IBD_FInterruptR(x) IBG_FInterruptR_RangeLock __IBD_FInterruptR_VariableA_##x{ IBRF_CoreBump };
extern uint64_t ShellLoopLastTime;
extern DWORD BackThreadID;

//��������
extern int KeyPerPage;
extern int FontHeight;
extern int WindowSizeAdjustX, WindowSizeAdjustY;
extern bool IsProjectOpen;
extern HWND MainWindowHandle;
extern int RScrX, RScrY, ScrX, ScrY;

//��ʽ���ͱ�
extern IBF_DefaultTypeList IBF_Inst_DefaultTypeList;
extern IBF_DefaultModuleList IBF_Inst_DefaultModuleList;
extern IBF_Project IBF_Inst_Project;
extern IBR_Project IBR_Inst_Project;
extern IBS_Project IBS_Inst_Project;

//����
extern IBR_Debug IBR_Inst_Debug;

//�������
extern std::default_random_engine GlobalRnd;
extern const int ModuleRandomParameterLength;

//prelink
namespace PreLink
{
    extern GLFWwindow* window;
    extern HINSTANCE hInst;
    extern ImFont* font;
}
