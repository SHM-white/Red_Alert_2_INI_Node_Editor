

/***********************************************************************************************
 ***                                 ??????????����Դ����                                    ***
 ***********************************************************************************************
 *                                                                                             *
 *                       �ļ��� : global_tool_func.cpp                                         *
 *                                                                                             *
 *                     ��д���� : Old Sovok                                                    *
 *                                                                                             *
 *                     �½����� : 2022/3/21                                                    *
 *                                                                                             *
 *                     ����༭ : 2022/3/23 Old Sovok                                          *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * ���ݣ�
 *   --
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "global_tool_func.h"
#include "external_file.h"
#include "external_log.h"
#include "..\Global.h"
#include <Windows.h>
#include <ctime>
#include <regex>

extern LogClass GlobalLogB;
extern bool EnableLog;

hash_t StrHash(const std::string& __Str)
{
	std::hash<std::string> hash_str;
	return ((hash_t)hash_str(__Str));
}
hash_t StrHash(const char* __Str)
{
	std::string ____String____(__Str);
	return StrHash(____String____);
}

char* RemoveFrontSpace(char* src)
{
	char* res = src;
	while (*res == ' ' || *res == '\t')++res;
	return res;
}
void RemoveBackSpace(char* src)
{
	char* res = src;
	--res;
	while (*(++res) != 0); --res;
	while (*res == ' ' || *res == '\t' || *res == '\r' || *res == '\n')--res;
	*(res + 1) = 0;
}
void RemoveBackSpace(std::string& src)
{
	while (1)
	{
		char i = src[src.length() - 1];
		if (i == ' ' || i == '\t' || i == '\r' || i == '\n')src.pop_back();
		else break;
	}
}



// ANSI�ַ���ת����Unicode
std::wstring MBCStoUnicode(const std::string& MBCS)
{
    int nLength = MultiByteToWideChar(CP_ACP, 0, MBCS.c_str(), -1, NULL, NULL);   // ��ȡ���������ȣ��ٷ����ڴ�
    WCHAR* tch = new WCHAR[nLength + 4]();
    nLength = MultiByteToWideChar(CP_ACP, 0, MBCS.c_str(), -1, tch, nLength);     // ��MBCSת����Unicode
    std::wstring ret = tch;
    delete[] tch;
    return ret;
}
// UTF-8�ַ���ת����Unicode
std::wstring UTF8toUnicode(const std::string& UTF8)
{
    int nLength = MultiByteToWideChar(CP_UTF8, 0, UTF8.c_str(), -1, NULL, NULL);   // ��ȡ���������ȣ��ٷ����ڴ�
    WCHAR* tch = new WCHAR[nLength + 4]{};
    MultiByteToWideChar(CP_UTF8, 0, UTF8.c_str(), -1, tch, nLength);     // ��UTF-8ת����Unicode
    std::wstring ret = tch;
    delete[] tch;
    return ret;
}
// Unicode�ַ���ת����UTF-8
std::string UnicodetoUTF8(const std::wstring& Unicode)
{
    int UTF8len = WideCharToMultiByte(CP_UTF8, 0, Unicode.c_str(), -1, 0, 0, 0, 0);// ��ȡUTF-8���볤��
    char* UTF8 = new CHAR[UTF8len + 4]{};
    WideCharToMultiByte(CP_UTF8, 0, Unicode.c_str(), -1, UTF8, UTF8len, 0, 0); //ת����UTF-8����
    std::string ret = UTF8;
    delete[] UTF8;
    return ret;
}
// Unicode�ַ���ת����ANSI
std::string UnicodetoMBCS(const std::wstring& Unicode)
{
    int ANSIlen = WideCharToMultiByte(CP_ACP, 0, Unicode.c_str(), -1, 0, 0, 0, 0);// ��ȡUTF-8���볤��
    char* ANSI = new CHAR[ANSIlen + 4]{};
    WideCharToMultiByte(CP_ACP, 0, Unicode.c_str(), -1, ANSI, ANSIlen, 0, 0); //ת����UTF-8����
    std::string ret = ANSI;
    delete[] ANSI;
    return ret;
}
// ANSI�ַ���ת����UTF-8
std::string MBCStoUTF8(const std::string& MBCS)
{
    return UnicodetoUTF8(MBCStoUnicode(MBCS));
}
// UTF-8�ַ���ת����ANSI
std::string UTF8toMBCS(const std::string& MBCS)
{
    return UnicodetoMBCS(UTF8toUnicode(MBCS));
}

uint64_t GetSysTimeMicros()
{
    const uint64_t EpochFIleTime = (116444736000000000ULL);
    FILETIME ft;
    LARGE_INTEGER li;
    uint64_t tt = 0;
    GetSystemTimeAsFileTime(&ft);
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;
    // ��1970��1��1��0:0:0:000�����ڵ�΢����(UTCʱ��)
    tt = (li.QuadPart - EpochFIleTime) / 10;
    return tt;
}




//const static char �·�[12][20] = 
//{ "һ��", "����", "����", "����", "����", "����", "����", "����", "����", "ʮ��", "ʮһ��", "ʮ����", };
//{ "1��", "2��", "3��", "4��", "5��", "6��", "7��", "8��", "9��", "10��", "11��", "12��", };
std::string TimeNow()
{
    const static char ����[7][20] =
    { "������", "����һ", "���ڶ�", "������", "������", "������", "������" };
    char* TBuf = new char[5000]();
    std::tm stm;
    std::time_t tt = std::time(0);
    localtime_s(&stm, &tt);
    sprintf(TBuf, "%04d��%02d��%02d�� %s %02d:%02d:%02d", stm.tm_year + 1900, stm.tm_mon + 1, stm.tm_mday, ����[stm.tm_wday], stm.tm_hour, stm.tm_min, stm.tm_sec);
    std::string rt = TBuf;
    delete[]TBuf;
    return rt;
}


bool IsTrueString(const std::string& s1)
{
    return s1 == "true" || s1 == "yes" || s1 == "1" || s1 == "T" || s1 == "True" || s1 == "Yes" || s1 == "t" || s1 == "Y" || s1 == "y";
}

std::string RandStr(int i)
{
    static const char Pool[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";
    std::string ss; ss.reserve(i + 1);
    for (int p = 0; p < i; p++)ss.push_back(Pool[GlobalRnd() % 62]);
    return ss;
}



bool system_hide(const char* CommandLine)
{
    SECURITY_ATTRIBUTES   sa;
    HANDLE   hRead, hWrite;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    if (!CreatePipe(&hRead, &hWrite, &sa, 0))
    {
        return   FALSE;
    }



    STARTUPINFOA   si;

    PROCESS_INFORMATION   pi;

    si.cb = sizeof(STARTUPINFOA);

    GetStartupInfoA(&si);

    si.hStdError = hWrite;
    si.hStdOutput = hWrite;

    si.wShowWindow = SW_HIDE;

    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    //�ؼ����裬CreateProcess�����������������MSDN   

    if (!CreateProcessA(NULL, (LPSTR)CommandLine, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
    {
        return   FALSE;
    }
    CloseHandle(hWrite);
    char   buffer[4096] = { 0 };
    DWORD   bytesRead;
    while (true)
    {
        memset(buffer, 0, strlen(buffer));
        if (ReadFile(hRead, buffer, 4095, &bytesRead, NULL) == NULL)
            break;

        //buffer�о���ִ�еĽ�������Ա��浽�ı���Ҳ����ֱ�����   

        //printf(buffer);//����ע�͵��Ϳ�����  

        Sleep(100);

    }

    return   TRUE;

}


std::vector<int> cJSON_GetVectorInt(cJSON* Array)
{
    int N = cJSON_GetArraySize(Array);
    cJSON* Item = cJSON_GetArrayItem(Array, 0);
    std::vector<int> Ret;
    Ret.reserve(N);
    for (int i = 0; i < N; i++)
    {
        Ret.push_back(Item->valueint);
        Item = Item->next;
    }
    return Ret;
}
std::vector<uint8_t> cJSON_GetVectorBool(cJSON* Array)
{
    int N = cJSON_GetArraySize(Array);
    cJSON* Item = cJSON_GetArrayItem(Array, 0);
    std::vector<uint8_t> Ret;
    Ret.reserve(N);
    for (int i = 0; i < N; i++)
    {
        Ret.push_back(((Item->type & 0xFF) == cJSON_True) ? true : false);
        Item = Item->next;
    }
    return Ret;
}
std::vector<std::string> cJSON_GetVectorString(cJSON* Array)
{
    int N = cJSON_GetArraySize(Array);
    cJSON* Item = cJSON_GetArrayItem(Array, 0);
    std::vector<std::string> Ret;
    Ret.reserve(N);
    for (int i = 0; i < N; i++)
    {
        Ret.push_back(Item->valuestring);
        Item = Item->next;
    }
    return Ret;
}
std::vector<cJSON*> cJSON_GetVectorObject(cJSON* Array)
{
    int N = cJSON_GetArraySize(Array);
    cJSON* Item = cJSON_GetArrayItem(Array, 0);
    std::vector<cJSON*> Ret;
    Ret.reserve(N);
    for (int i = 0; i < N; i++)
    {
        Ret.push_back(Item);
        Item = Item->next;
    }
    return Ret;
}

std::vector<JsonObject> JsonObject::GetArrayObject() const
{
    int N = cJSON_GetArraySize(Object);
    cJSON* Item = cJSON_GetArrayItem(Object, 0);
    std::vector<JsonObject> Ret;
    Ret.reserve(N);
    for (int i = 0; i < N; i++)
    {
        Ret.push_back(JsonObject{ Item });
        Item = Item->next;
    }
    return Ret;
}

std::unordered_map<std::string, JsonObject> JsonObject::GetMapObject() const
{
    std::unordered_map<std::string, JsonObject> Ret;
    JsonObject Node = GetChildItem();
    while (Node.Available())
    {
        Ret.insert({ Node.GetName(),Node });
        Node = Node.GetNextItem();
    }
    return Ret;
}
std::unordered_map<std::string, double> JsonObject::GetMapDouble() const
{
    std::unordered_map<std::string, double> Ret;
    JsonObject Node = GetChildItem();
    while (Node.Available())
    {
        Ret.insert({ Node.GetName(),Node.GetDouble() });
        Node = Node.GetNextItem();
    }
    return Ret;
}
std::unordered_map<std::string, int> JsonObject::GetMapInt() const
{
    std::unordered_map<std::string, int> Ret;
    JsonObject Node = GetChildItem();
    while (Node.Available())
    {
        Ret.insert({ Node.GetName(),Node.GetInt() });
        Node = Node.GetNextItem();
    }
    return Ret;
}
std::unordered_map<std::string, bool> JsonObject::GetMapBool() const
{
    std::unordered_map<std::string, bool> Ret;
    JsonObject Node = GetChildItem();
    while (Node.Available())
    {
        Ret.insert({ Node.GetName(),Node.GetBool() });
        Node = Node.GetNextItem();
    }
    return Ret;
}
std::unordered_map<std::string, std::string> JsonObject::GetMapString() const
{
    std::unordered_map<std::string, std::string> Ret;
    JsonObject Node = GetChildItem();
    while (Node.Available())
    {
        Ret.insert({ Node.GetName(),Node.GetString() });
        Node = Node.GetNextItem();
    }
    return Ret;
}

std::string GetStringFromFile(const char* FileName)
{
    ExtFileClass File;
    if (!File.Open(FileName, "r"))return "";
    File.Seek(0, SEEK_END);
    int Pos = File.Position();
    char* FileStr;
    FileStr = new char[Pos + 50];
    if (FileStr == nullptr)return "";
    File.Seek(0, SEEK_SET);
    File.Read(FileStr, 1, Pos);
    FileStr[Pos] = 0;
    File.Close();

    std::string LoadStr = FileStr ;
    delete[]FileStr;
    return LoadStr;
}

void JsonFile::Parse(std::string Str)
{
    if (EnableLogEx)
    {
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "JsonFile::ParseFromFile <- std::string Str=%s", Str.c_str()); GlobalLogB.AddLog(LogBufB);
    }
    cJSON_Minify(Str.data());
    Clear();
    File = cJSON_Parse(Str.data());
}
void JsonFile::ParseWithOpts(std::string Str, const char** ReturnParseEnd, int RequireNullTerminated)
{
    if (EnableLogEx)
    {
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "JsonFile::ParseFromFile <- std::string Str=%s, int RequireNullTerminated=%d", Str.c_str(), RequireNullTerminated); GlobalLogB.AddLog(LogBufB);
    }
    cJSON_Minify(Str.data());
    Clear();
    File = cJSON_ParseWithOpts(Str.data(), ReturnParseEnd, RequireNullTerminated);
}

void JsonFile::ParseFromFile(const char* FileName)
{
    if (EnableLogEx)
    {
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "JsonFile::ParseFromFile <- char* FileName=%s",FileName); GlobalLogB.AddLog(LogBufB);
    }
    Clear();

    ExtFileClass Fp;
    if (!Fp.Open(FileName, "r"))return;
    Fp.Seek(0, SEEK_END);
    int Pos = Fp.Position();
    char* FileStr;
    FileStr = new char[Pos + 50];
    if (FileStr == nullptr)return;
    Fp.Seek(0, SEEK_SET);
    Fp.Read(FileStr, 1, Pos);
    FileStr[Pos] = 0;
    Fp.Close();

    int iPos = 0;
    while (FileStr[iPos] != '{' && iPos < Pos)iPos++;
    if (iPos == Pos) { delete[]FileStr; return; }

    cJSON_Minify(FileStr + iPos);
    File = cJSON_Parse(FileStr + iPos);

    if (EnableLogEx)
    {
        GlobalLogB.AddLog_CurTime(false);
        GlobalLogB.AddLog("JsonFile::ParseFromFile �� JSON�ı���");
        GlobalLogB.AddLog_CurTime(false);
        GlobalLogB.AddLog("\"", false);
        GlobalLogB.AddLog(UTF8toMBCS(FileStr + iPos).c_str(), false);
        GlobalLogB.AddLog("\"");
    }

    delete[]FileStr;
}

void JsonFile::ParseFromFileWithOpts(const char* FileName, int RequireNullTerminated)
{
    if (EnableLogEx)
    {
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "JsonFile::ParseFromFile <- char* FileName=%s, int RequireNullTerminated=%d", FileName, RequireNullTerminated); GlobalLogB.AddLog(LogBufB);
    }
    Clear();

    ExtFileClass Fp;
    if (!Fp.Open(FileName, "r"))return;
    Fp.Seek(0, SEEK_END);
    int Pos = Fp.Position();
    char* FileStr;
    FileStr = new char[Pos + 50];
    if (FileStr == nullptr)return;
    Fp.Seek(0, SEEK_SET);
    Fp.Read(FileStr, 1, Pos);
    FileStr[Pos] = 0;
    Fp.Close();

    int iPos = 0;
    while (FileStr[iPos] != '{' && iPos < Pos)iPos++;
    if (iPos == Pos) { delete[]FileStr; return; }

    const char* ReturnParseEnd;
    cJSON_Minify(FileStr + iPos);
    File = cJSON_ParseWithOpts(FileStr + iPos, &ReturnParseEnd, RequireNullTerminated);

    if (EnableLogEx)
    {
        GlobalLogB.AddLog_CurTime(false);
        GlobalLogB.AddLog("JsonFile::ParseFromFileWithOpts �� JSON�ı���");
        GlobalLogB.AddLog_CurTime(false);
        GlobalLogB.AddLog("\"", false);
        GlobalLogB.AddLog(UTF8toMBCS(FileStr + iPos).c_str(), false);
        GlobalLogB.AddLog("\"");
    }

    delete[]FileStr;
}

bool RegexFull_Nothrow(const std::string& Str, const std::string& Regex) throw()
{
    try { return std::regex_match(Str, std::regex(Regex)); }
    catch (std::exception& e) { (void)e; return false; }
}
bool RegexFull_Throw(const std::string& Str, const std::string& Regex)
{
    try { return std::regex_match(Str, std::regex(Regex)); }
    catch (std::exception& e) { throw(e); }
}
bool RegexNone_Nothrow(const std::string& Str, const std::string& Regex) throw()
{
    try { return !std::regex_search(Str, std::regex(Regex)); }
    catch (std::exception& e) { (void)e; return false; }
}
bool RegexNone_Throw(const std::string& Str, const std::string& Regex)
{
    try { return !std::regex_search(Str, std::regex(Regex)); }
    catch (std::exception& e) { throw(e); }
}
bool RegexNotFull_Nothrow(const std::string& Str, const std::string& Regex) throw()
{
    try { return !std::regex_match(Str, std::regex(Regex)); }
    catch (std::exception& e) { (void)e; return false; }
}
bool RegexNotFull_Throw(const std::string& Str, const std::string& Regex)
{
    try { return !std::regex_match(Str, std::regex(Regex)); }
    catch (std::exception& e) { throw(e); }
}
bool RegexNotNone_Nothrow(const std::string& Str, const std::string& Regex) throw()
{
    try { return std::regex_search(Str, std::regex(Regex)); }
    catch (std::exception& e) { (void)e; return false; }
}
bool RegexNotNone_Throw(const std::string& Str, const std::string& Regex)
{
    try { return std::regex_search(Str, std::regex(Regex)); }
    catch (std::exception& e) { throw(e); }
}


bool IBG_SuspendThread(_FAKE_DWORD ThreadID)
{
    HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, (DWORD)ThreadID);
    SuspendThread(hThread);
    CloseHandle(hThread);
    return true;
}
bool IBG_ResumeThread(_FAKE_DWORD ThreadID)
{
    HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, (DWORD)ThreadID);
    ResumeThread(hThread);
    CloseHandle(hThread);
    return true;
}
/*
int GetProcessState(DWORD dwProcessID) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwProcessID);

    if (hSnapshot != INVALID_HANDLE_VALUE) {
        DWORD state = 1;//����1��һ�����̻߳������о���0
        THREADENTRY32 te = { sizeof(te) };
        BOOL fOk = Thread32First(hSnapshot, &te);
        for (; fOk; fOk = Thread32Next(hSnapshot, &te)) {
            if (te.th32OwnerProcessID == dwProcessID) {
                HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
                DWORD suspendCount = SuspendThread(hThread);//����֮ǰ�Ĺ�����������0��ʾ�ѹ���
                ResumeThread(hThread);//���ϻָ������������Ŀ��������Ӱ��
                CloseHandle(hThread);
                if (suspendCount == 0) state = 0; //�Ǹ��ж������̶߳�����ĺ÷���
            }
        }
        CloseHandle(hSnapshot);
        return state;
    }
    return -1;
}
*/
