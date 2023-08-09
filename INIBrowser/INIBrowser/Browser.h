#pragma once


#include"FromEngine/Include.h"
#include"FromEngine/global_timer.h"
#include<thread>
#include<chrono>
#include<atomic>
#include<mutex>

#include"TextEditor.h"

typedef void (*callback_t)();
typedef void (*filterinit_t)(int);

const std::string Version = u8"0.1b3";
const int VersionMajor = 0;
const int VersionMinor = 1;
const int VersionRelease = 3;
const int VersionN = VersionMajor * 10000 + VersionMinor * 100 + VersionRelease;
const std::string VersionNStr = u8"103";


LogClass GlobalLog{"browser.log"};
BufString LogBuf;

std::string FontPath = ".\\Resources\\";//全过程不变
std::string Defaultpath{ "C:\\" };
std::string Pathbuf,Desktop,TextEditAboutU8;
std::wstring PathbufW;

HMODULE TElib;

BufString debugbuf;//非测试情况里面啥也没有
Ini::IniFile HoverHintInfo;
Ini::IniSection StringSec;
bool StringSecIterAvailable = false;
bool IsInExitProcess = false;

int GetWindowRetry = 5, GetWindowInitialDelay = 1000, GetWindowRetryDelay = 500;//从INI来
int KeyPerPage = 10, FontHeight = 16;
int GetINITextDelay = 50, GetINITimeOut = 1000;
bool WindowExceptionHint = true, WindowConditionHint = true;//是否在解析窗口时弹出异常

bool AnalysisShowINILine = true;

int FnInfoDelay = 1000, FnInfoEndDelay = 5000;
int WaitTimeMsec = 10;//这个不导出INI，是无INI时的等待时间

int RScrX, RScrY, ScrX, ScrY, ScrMode;
int RInvisibleCol = 0xFF00FF, InvisibleCol;

int ActiveINI = -1;//运行时可变

std::vector<int8_t> IsTicked;
bool SelectMode,WindowMustSync,IsFileChanging;

wchar_t BufW1[5000], BufW2[5000], BufW3[5000], BufW4[5000];
char BufA1[5000], BufA2[5000], BufA3[5000];
wchar_t CurrentDirW[5000];//调用当前目录时必须调用这两个，不要临时求！
char CurrentDirA[5000];//因为OpenFileDialog会改变当前目录，所以必须提前加载

std::vector<std::string> Hint;//UTF-8字符串！
int HintChangeSecs = 5000;
int FPSLimit = -1;

float InitOfsX = -1, TreeOfsX = -1;

bool EnableLog = true;
std::atomic<bool> FontLoaded = false;
std::atomic<bool> InMainLoop = false;

HWND MainWindowHandle = 0;

int ExitCloseAllFile();

enum class SPCond
{
	Waiting,
	Success,
	Quit
}SelPathCond;

namespace PreLink
{
    GLFWwindow* window;
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
    //HICON ProgIcon;
    HINSTANCE hInst;
    ImFont* font;

    void CleanUp();
}

#include"sese.h"


// MBCS字符集转换成UTF-8
std::string MBCStoUTF8(const std::string& MBCS)
{
	// 方法：先转换成CP_ACP再转换成CP_UTF8
	int nLength = MultiByteToWideChar(CP_ACP, 0, MBCS.c_str(), -1, NULL, NULL);   // 获取缓冲区长度，再分配内存
	WCHAR* tch = new WCHAR[nLength];
	nLength = MultiByteToWideChar(CP_ACP, 0, MBCS.c_str(), -1, tch, nLength);     // 将MBCS转换成Unicode
	int nUTF8len = WideCharToMultiByte(CP_UTF8, 0, tch, nLength, 0, 0, 0, 0);   // 获取UTF-8编码长度
	char* UTF8 = new char[nUTF8len + 4];
	WideCharToMultiByte(CP_UTF8, 0, tch, nLength, UTF8, nUTF8len, 0, 0); //转换成UTF-8编码
	delete[] tch;
	std::string ret = UTF8;
	delete[] UTF8;
	return ret;
}
// MBCS字符集转换成Unicode
std::wstring MBCStoUnicode(const std::string& MBCS)
{
	int nLength = MultiByteToWideChar(CP_ACP, 0, MBCS.c_str(), -1, NULL, NULL);   // 获取缓冲区长度，再分配内存
	WCHAR* tch = new WCHAR[nLength];
	nLength = MultiByteToWideChar(CP_ACP, 0, MBCS.c_str(), -1, tch, nLength);     // 将MBCS转换成Unicode
	std::wstring ret = tch;
	delete[] tch;
	return ret;
}
// Unicode字符集转换成UTF-8
std::string UnicodetoUTF8(const std::wstring& Unicode)
{
	int UTF8len = WideCharToMultiByte(CP_UTF8, 0, Unicode.c_str(), -1, 0, 0, 0, 0);// 获取UTF-8编码长度
	char* UTF8 = new char[UTF8len + 4];
	WideCharToMultiByte(CP_UTF8, 0, Unicode.c_str(), -1, UTF8, UTF8len, 0, 0); //转换成UTF-8编码
	std::string ret = UTF8;
	delete[] UTF8;
	return ret;
}




int GetPage(int u)
{
	return (u - 1) / KeyPerPage + 1;
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
	// 从1970年1月1日0:0:0:000到现在的微秒数(UTC时间)
	tt = (li.QuadPart - EpochFIleTime) / 10;
	return tt;
}
bool SelectFilePath(HWND father, std::string& strFilePath, const std::string& FromPath);




















//BROWSE INI FILE
namespace Browse
{
	std::atomic<bool> InitGo！{ false };
	int LoadProgress;
	const int ProgressComplete = 1000;
	int CallbackSelect;
	int TotFile;
	std::atomic<int> CurFile;
	BufString Buf{}, Br{}, Buf3{};

	std::mutex UseBuf;

}










namespace FilterInit
{
	void SimpleStringInit(int);
	void SortSliceInit(int);
	void RegexMatchInit(int);
}
const int FilterInitListLen = 3;
filterinit_t FilterInitList[] =
{
	FilterInit::SimpleStringInit,
	FilterInit::SortSliceInit,
	FilterInit::RegexMatchInit
};
//提前定义
const int FilterListLen = 5;

namespace Analyze
{
	std::string DecryptString(const std::string&);
}
namespace Browse
{
	int GetSliceFromName(int INI, const std::string& Name);
	int GetININum(const std::string&);
	int GetSliceFromName(int, const std::string&);
}

namespace GlobalAnalyze
{
	typedef int (*cmddeal_t)(int, const std::vector<std::string>&);
	bool HasOutput{ false };

	std::vector<std::string>OutputText;
	std::mutex OTMutex;
	std::default_random_engine rnd;

	std::string CutSpace(const std::string& ss)//REPLACE ORIG
	{
		auto fp = ss.find_first_not_of(" \011\r\n\t"), bp = ss.find_last_not_of(" \011\r\n\t");
		return std::string(ss.begin() + (fp == ss.npos ? 0 : fp),
			ss.begin() + (bp == ss.npos ? 0 : bp + 1));
	}
	std::string CutComment(const std::string& ss)//ORIG
	{
		auto fp = ss.find_first_of(';');
		return std::string(ss.begin(), ss.begin() + (fp == ss.npos ? ss.length() : fp));
	}
	std::vector<std::string>LoadFileStr(const std::string& File)
	{
		std::vector<std::string>Ret;
		FILE* GetStr = fopen(File.c_str(), "r");
		if (GetStr != NULL)
		{
			char str[MAX_STRING_LENGTH];
			while (!feof(GetStr))
			{
				fgets(str, MAX_STRING_LENGTH, GetStr);
				Ret.push_back(CutSpace(CutComment(str)));
			}
			fclose(GetStr);
		}
		if (Ret.empty())return Ret;
		Ret.erase(Ret.begin());
		return Ret;
	}
	std::vector<std::string> SplitParam(const std::string& Text)//ORIG
	{
		if (Text.empty())return {};
		size_t cur = 0, crl;
		std::vector<std::string> ret;
		while ((crl = Text.find_first_of(',', cur)) != Text.npos)
		{
			ret.push_back(CutSpace(std::string(Text.begin() + cur, Text.begin() + crl)));
			cur = crl + 1;
		}
		ret.push_back(CutSpace(std::string(Text.begin() + cur, Text.end())));
		return ret;
	}

	void Warning(const char* Str)
	{
		OTMutex.lock();
		OutputText.push_back(Str);
		OTMutex.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(FnInfoDelay));
		HasOutput = true;
	}
	std::string RandStr(int i)
	{
		static const char* Pool = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";
		std::string ss; ss.reserve(i + 1);
		for (int p = 0; p < i; p++)ss.push_back(Pool[rnd() % 62]);
		return ss;
	}

	std::vector<std::string>FileName;//文件结构 
	std::vector<int>FileCmdLOffset;
	std::vector<int>FileRealLOffset;
	std::vector<int>CmdLBelongFile;
	std::vector<int>RealLBelongFile;
	int GlobalCursor;
	std::vector<std::string>GlobalFileStr;




	int Begin(int, const std::vector<std::string>&);//函数列表 
	int End(int, const std::vector<std::string>&);
	int Debug(int, const std::vector<std::string>&);//#EXPR #DEFAULT
	int SetValue(int, const std::vector<std::string>&);
	int Label(int, const std::vector<std::string>&);
	int LabelJump(int, const std::vector<std::string>&);//#ALL #THIS
	int ImportFile(int, const std::vector<std::string>&);
	int LongCall(int, const std::vector<std::string>&);//#ALL #THIS
	int CmdLineJump(int, const std::vector<std::string>&);
	int Nothing(int, const std::vector<std::string>&);
	int PrintTo(int, const std::vector<std::string>&);//#EXPR 拆序列 
	int LPrintTo(int, const std::vector<std::string>&);//#EXPR 拆序列 + @CO
	int Run(int, const std::vector<std::string>&);//#EXPR 拆序列 
	int Expression(int, const std::vector<std::string>&);//#PAR<n> #EXPR (内)#PARAM__ 拆序列 
	int RunExpr(int, const std::vector<std::string>&);//#EXPR #EXPC #EXPCI #EXPCR #DEFAULT #MAKEEXPR #EXPREND
	int Bind(int, const std::vector<std::string>&);//#PAR<n> #SAME:复制Iter #FULL:不复制Iter #AFTER:[Cur,End) #BEFORE:[Begin,Cur) (内)#PARAM__
	int SetExprIter(int, const std::vector<std::string>&);
	int LabelJumpIf(int, const std::vector<std::string>&);//#ALL #THIS #CONTINUE
	int Link(int, const std::vector<std::string>&);//#ON #OFF 
	int Raw(int, const std::vector<std::string>&);

	std::unordered_map<std::string, cmddeal_t> CommandMap
	{
		{u8"Function",Begin},{u8"Return",End},{u8"Debug",Debug},
		{u8"SetValue",SetValue},{u8"Call",LongCall},{u8"SetExprIter",SetExprIter},
		{u8"LabelJump",LabelJump},{u8"CmdLineJump",CmdLineJump},{u8"Label",Label},
		{u8"Import",ImportFile},{u8"Nop",Nothing},{u8"Bind",Bind},
		{u8"PrintTo",PrintTo},{"LPrintTo",LPrintTo},{u8"Run",Run},
		{u8"Expression",Expression},{u8"RunExpr",RunExpr},{u8"LabelJumpIf",LabelJumpIf},
		{u8"Link",Link},
		//{"Delete",DeleteData}

		{u8"Expr",Expression},{u8"Ret",End},{u8"LblJmp",LabelJump},
		{u8"Func",Begin},{u8"SetV",SetValue},{u8"ExprIt",SetExprIter},
		{u8"CmdLJmp",CmdLineJump},{"PrTo",PrintTo},{u8"LPrTo",LPrintTo},
		{u8"BindExpr",Bind},{u8"LblJmpIf",LabelJumpIf},{u8"LblJIf",LabelJumpIf}
	};



	struct CmdLineInfo//标签、指令管理 
	{
		cmddeal_t Func;
		std::vector<std::string> Param;
		int RealLineNo;
		bool Same(const CmdLineInfo& r)const { return RealLineNo == r.RealLineNo && Param == r.Param && Func == r.Func; }
		bool EqualEffect(const CmdLineInfo& r)const { return Func == r.Func && Param == r.Param; }
		bool EqualFirst(const CmdLineInfo& r)const
		{
			return Func == r.Func && ((Param.empty() && r.Param.empty()) || Param[0] == r.Param[0]);
		}
		bool EqualFunc(const CmdLineInfo& r)const { return Func == r.Func; }
		bool LineLess(const CmdLineInfo& r)const { return RealLineNo < r.RealLineNo; }
	};
	std::vector<CmdLineInfo>GlobalCommands;
	std::vector<CmdLineInfo>GetValidLines(const std::vector<std::string>& vec)
	{
		static char Fname[MAX_STRING_LENGTH], Param[MAX_STRING_LENGTH];
		std::vector<CmdLineInfo> ret;
		int lp = 1;
		for (auto& str : vec)
		{
			std::string ts = CutSpace(str);
			++lp;
			if (ts == "")continue;
			memset(Fname, 0, sizeof(Fname));
			memset(Param, 0, sizeof(Param));
			std::string Tmp1 = CutSpace(CutComment(ts));
			sscanf(Tmp1.c_str(), "%[^ (] (%[^)])%*s", Fname, Param);
			std::string FnN = CutSpace(Fname), Par = CutSpace(Param);
			if (FnN == "Raw")
			{
				auto fp = Tmp1.find_first_of('(');
				auto fl = Tmp1.find_last_of(')');
				auto fs = Tmp1.find_first_of(',',fp);
				if (fp == Tmp1.npos || fl == Tmp1.npos || fs == Tmp1.npos)continue;
				ret.push_back({ Raw,{
					CutSpace(std::string(Tmp1.begin() + fp + 1, Tmp1.begin() + fs)),
					CutSpace(std::string(Tmp1.begin() + fs + 1, Tmp1.begin() + fl))},lp });
				continue;
			}
			auto it = CommandMap.find(FnN);
			if (it != CommandMap.end())
			{
				ret.push_back({ it->second,SplitParam(Par),lp });
			}
			else
			{
				sprintf(Fname, u8"语法错误：第%d行 未知函数\"%s\"", lp, FnN.c_str());
				Warning(Fname);
			}
		}
		return ret;
	}
	int FindLabelPos(const CmdLineInfo& line)
	{
		auto it = find_if(GlobalCommands.begin(), GlobalCommands.end(),
			[&line](decltype(line)& fl) {return fl.EqualFirst(line); });
		if (it == GlobalCommands.end())return -1;
		return std::distance(GlobalCommands.begin(), it);
	}
	int FindLabelPos(int from, int to, const CmdLineInfo& line)
	{
		auto Final = ((to == -1) ? GlobalCommands.end() : GlobalCommands.begin() + to);
		auto it = find_if(GlobalCommands.begin() + from, Final,
			[&line](decltype(line)& fl) {return fl.EqualFirst(line); });
		if (it == Final)return -1;
		return std::distance(GlobalCommands.begin(), it);
	}
	int CmdLine()
	{
		return GlobalCursor - FileCmdLOffset[CmdLBelongFile[GlobalCursor]];
	}
	int RealLine()
	{
		return GlobalCommands[GlobalCursor].RealLineNo;
	}

	struct CallSubInfo//调用栈 
	{
		std::string CallName;
		std::unordered_map<std::string, std::string>ParamMap;
	};
	std::stack<CallSubInfo>CallStack;
	std::stack<int>RetStack;
	std::stack<std::vector<std::string> >LocalStack;
	std::vector<std::string> CallOrder;


	//变量列表 
	//最后加u8 
	std::unordered_map < std::string, std::string > GlobalEnviron =
	{
        {u8"，",u8","},{u8"CO",u8","},{u8"LN",u8"\n"},{u8"\\n",u8"\n"},{u8"AT",u8"@"},{u8"VERSION",VersionNStr},
	};//@FUNCTION @FILE @CMDLINE @FILELINE R@x:"@x" R~@x:"~@x" P@x引用形参 V@x:传值x L@x：局部变量x
	std::string GlobalDecryptString(const std::string& Key)
	{
		//printf("GlobalDecryptString Key=\"%s\"\n",Key.c_str());//OUT
		if (Key.empty())return "";
		if (Key == "@FILE")
		{
			return FileName[CmdLBelongFile[GlobalCursor]];
		}
		if (Key == "@CMDLINE")
		{
			return std::to_string(CmdLine());
		}
		if (Key == "@FILELINE")
		{
			return std::to_string(RealLine());
		}
		if (Key.size() > 1 && Key[0] == 'R' && Key[1] == '@')
		{
			return std::string(Key.begin() + 1, Key.end());
		}
		if (Key.size() > 2 && Key[0] == 'R' && Key[2] == '@')
		{
			return std::string(Key.begin() + 1, Key.end());
		}
		if (Key.size() > 1 && Key[0] == 'P' && Key[1] == '@')
		{
			auto it = CallStack.top().ParamMap.find(std::string(Key.begin() + 2, Key.end()));
			if (it == CallStack.top().ParamMap.end())return "";
			else return GlobalDecryptString(it->second);
		}
		if (Key.size() > 1 && Key[0] == 'L' && Key[1] == '@')
		{
			std::string ns = std::to_string(RetStack.top()) + CallStack.top().CallName + std::string(Key.begin() + 2, Key.end());
			if (GlobalEnviron.find(ns) == GlobalEnviron.end())LocalStack.top().push_back(ns);
			return GlobalDecryptString("@" + ns);
		}
		if (Key[0] == '@')
		{
			std::string su(Key.begin() + 1, Key.end());
			if (Key.size() > 1)
			{
				if (Key[1] == '@')su = GlobalDecryptString(su);
			}
			if (su == "")return"";

			auto It = GlobalEnviron.find(su);
			if (It != GlobalEnviron.end())return It->second;

			return GlobalEnviron[su];
		}
		return Key;
	}
	bool GlobalDecryptStringHasThis(const std::string& Key)
	{
		//printf("GlobalDecryptString Key=\"%s\"\n",Key.c_str());//OUT
		if (Key.empty())return false;
		if (Key[0] == '@')
		{
			std::string su(Key.begin() + 1, Key.end());
			if (Key.size() > 1)
			{
				if (Key[1] == '@')
				{
					if (!GlobalDecryptStringHasThis(su))return false;
					su = GlobalDecryptString(su);
				}
			}
			if (su.empty())return false;

			auto It = GlobalEnviron.find(su);
			if (It != GlobalEnviron.end())return true;

			return false;
		}
		else return false;
	}
	std::string GlobalEncryptStringTarget(const std::string& Key)
	{
		if (Key.empty())return "";
		if (Key[0] == '@')
		{
			std::string su(Key.begin() + 1, Key.end());
			if (Key.size() > 1)
			{
				if (Key[1] == '@')su = GlobalDecryptString(su);
			}
			return su;
		}
		else if (Key.size() >= 2 && Key[1] == '@')
		{
			if (Key.size() > 1 && Key[0] == 'P' && Key[1] == '@')
			{
				auto it = CallStack.top().ParamMap.find(std::string(Key.begin() + 2, Key.end()));
				if (it != CallStack.top().ParamMap.end())GlobalEncryptStringTarget(it->second);
			}
			if (Key.size() > 1 && Key[0] == 'L' && Key[1] == '@')
			{
				std::string ns = std::to_string(RetStack.top()) + CallStack.top().CallName + std::string(Key.begin() + 2, Key.end());
				if (GlobalEnviron.find(ns) == GlobalEnviron.end())LocalStack.top().push_back(ns);
				GlobalEncryptStringTarget("@" + ns);
			}
		}
		return "";
	}
	void GlobalEncryptRawString(const std::string& Key, const std::string& Val)
	{
		std::string su = GlobalEncryptStringTarget(Key);
		if (su.empty())return;
		GlobalEnviron[su] = Val;
	}
	void GlobalEncryptString(const std::string& Key, const std::string& Val)
	{
		GlobalEncryptRawString(Key,GlobalDecryptString(Val));
	}
	

	bool IsTrueString(const std::string& s1)
	{
		return s1 == "true" || s1 == "yes" || s1 == "1" || s1 == "T" || s1 == "True" || s1 == "Yes" || s1 == "t" || s1 == "Y" || s1 == "y";
	}

	struct ExprInfo
	{
		std::vector<std::string> Expr;
		std::vector<int> Replace;
		int ExprIter;
		int MaxParam;
	};
	std::unordered_map<std::string, ExprInfo> ExprMap;
	bool HasExpr(const std::string& Name)
	{
		return ExprMap.find(Name) != ExprMap.end();
	}
	typedef std::vector<std::string>::const_iterator __vsit;
	std::unordered_map<std::string, int> CalcNum
	{
		//零元 
		{"#T",0},{"#F",0},{"#FRI",0},
		//一元 
		{"#RI",1},{"#RS",1},{"#!",1},{"#NOT",1},
		{"#SB",1},{"#IB",1},{"#-I",1},{"#NI",1},
		{"#~I",1},{"#CI",1},
		{"#SL",1},{"#SR",1},{"#NOP",1},
		{"#ELEN",1},{"#EMAXP",1},{"#EIT",1},
		{"#STR",1},
		//二元 
		{"#S=",2},{"#SE",2},{"#I=",2},{"#IE",2},
		{"#S!=",2},{"#SNE",2},{"#I!=",2},{"#INE",2},
		{"#S>",2},{"#SG",2},{"#I>",2},{"#IG",2},
		{"#S>=",2},{"#SGE",2},{"#I>=",2},{"#IGE",2},
		{"#S<",2},{"#SL",2},{"#I<",2},{"#IL",2},
		{"#S<=",2},{"#SLE",2},{"#I<=",2},{"#ILE",2},
		{"#I+",2},{"#I-",2},{"#I*",2},{"#I/",2},{"#I%",2},
		{"#IA",2},{"#IS",2},{"#IM",2},{"#ID",2},{"#IU",2},
		{"#I&",2},{"#I|",2},{"#I^",2},
		{"#IN",2},{"#IO",2},{"#IX",2},
		{"#S+",2},{"#SA",2},{"#SCF",2},{"#SCB",2},
		{"#SRF",2},{"#SRP",2},
		{"#ININI",2},
		//三元特判（就这一个） 
		{"#?:",3},{"#CT",3},
	};
	std::unordered_map<std::string, std::function<std::string(const std::string&, const std::string&)> > CalcMap
	{
		//零元 
		{"#T",[](const std::string&,const std::string&)->std::string {return "1"; }},
		{"#F",[](const std::string&,const std::string&)->std::string {return "0"; }},
		{"#FRI",[](const std::string&,const std::string&)->std::string {return std::to_string(rnd()); }},

		//一元 
		{"#RI",[](const std::string& s1,const std::string&)->std::string
			{
				int md = atoi(s1.c_str());
				if (!md) { Warning((u8"警告 : RunExpr() 第" + std::to_string(RealLine()) + u8"行 #RI：试图对0取模").c_str()); return "0"; }
				return std::to_string(rnd() % md);
			}},
		{"#RS",[](const std::string& s1,const std::string&)->std::string {return RandStr(atoi(s1.c_str())); }},
		{"#NOT",[](const std::string& s1,const std::string&)->std::string {return std::to_string(!atoi(s1.c_str())); }},
		{"#!",[](const std::string& s1,const std::string&)->std::string {return std::to_string(!atoi(s1.c_str())); }},
		{"#SB",[](const std::string& s1,const std::string&)->std::string{return std::to_string((int)IsTrueString(s1)); }},
		{"#IB",[](const std::string& s1,const std::string&)->std::string {return std::to_string((int)(atoi(s1.c_str()) != 0)); }},
		{"#NI",[](const std::string& s1,const std::string&)->std::string {return std::to_string((int)(-atoi(s1.c_str()))); }},
		{"#-I",[](const std::string& s1,const std::string&)->std::string {return std::to_string((int)(-atoi(s1.c_str()))); }},
		{"#CI",[](const std::string& s1,const std::string&)->std::string {return std::to_string((int)(~atoi(s1.c_str()))); }},
		{"#~I",[](const std::string& s1,const std::string&)->std::string {return std::to_string((int)(~atoi(s1.c_str()))); }},
		{"#SL",[](const std::string& s1,const std::string&)->std::string {return std::to_string((int)(s1.length())); }},
		{"#SR",[](const std::string& s1,const std::string&)->std::string {auto s2 = s1; reverse(s2.begin(),s2.end()); return s2; }},
		{"#ELEN",[](const std::string& s1,const std::string&)->std::string {return HasExpr(s1) ? std::to_string(ExprMap[s1].Expr.size()) : "0"; }},
		{"#EEND",[](const std::string& s1,const std::string&)->std::string {return HasExpr(s1) ? std::to_string(ExprMap[s1].Expr.size() + 1) : "0"; }},
		{"#EMAXP",[](const std::string& s1,const std::string&)->std::string {return HasExpr(s1) ? std::to_string(ExprMap[s1].MaxParam) : "0"; }},
		{"#EIT",[](const std::string& s1,const std::string&)->std::string {return HasExpr(s1) ? std::to_string(ExprMap[s1].ExprIter + 1) : "0"; }},
		{"#NOP",[](const std::string& s1,const std::string&)->std::string {return s1; }},
		{"#STR",[](const std::string& s1,const std::string&)->std::string 
			{
				if (!StringSecIterAvailable)return "";
				auto Lin = StringSec.GetLine(s1);
				if (Lin != StringSec.NotFound())return Lin->second.Value.str;
				else return "";
			}},
				//二元 
				{"#S=",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(s1 == s2)); }},
				{"#SE",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(s1 == s2)); }},
				{"#I=",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(atoi(s1.c_str()) == atoi(s2.c_str()))); }},
				{"#IE",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(atoi(s1.c_str()) == atoi(s2.c_str()))); }},
				//
				{"#S!=",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(s1 != s2)); }},
				{"#SNE",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(s1 != s2)); }},
				{"#I!=",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(atoi(s1.c_str()) != atoi(s2.c_str()))); }},
				{"#INE",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(atoi(s1.c_str()) != atoi(s2.c_str()))); }},
				//
				{"#S>",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(s1 > s2)); }},
				{"#SG",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(s1 > s2)); }},
				{"#I>",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(atoi(s1.c_str()) > atoi(s2.c_str()))); }},
				{"#IG",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(atoi(s1.c_str()) > atoi(s2.c_str()))); }},
				//
				{"#S<",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(s1 < s2)); }},
				{"#SL",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(s1 < s2)); }},
				{"#I<",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(atoi(s1.c_str()) < atoi(s2.c_str()))); }},
				{"#IL",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(atoi(s1.c_str()) < atoi(s2.c_str()))); }},
				//
				{"#S>=",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(s1 >= s2)); }},
				{"#SGE",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(s1 >= s2)); }},
				{"#I>=",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(atoi(s1.c_str()) >= atoi(s2.c_str()))); }},
				{"#IGE",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(atoi(s1.c_str()) >= atoi(s2.c_str()))); }},
				//
				{"#S<=",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(s1 <= s2)); }},
				{"#SLE",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(s1 <= s2)); }},
				{"#I<=",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(atoi(s1.c_str()) <= atoi(s2.c_str()))); }},
				{"#ILE",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((int)(atoi(s1.c_str()) <= atoi(s2.c_str()))); }},
				//
				{"#I+",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) + atoi(s2.c_str()))); }},
				{"#IA",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) + atoi(s2.c_str()))); }},
				{"#I-",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) - atoi(s2.c_str()))); }},
				{"#IS",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) - atoi(s2.c_str()))); }},
				{"#I*",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) * atoi(s2.c_str()))); }},
				{"#IM",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) * atoi(s2.c_str()))); }},
				{"#I/",[](const std::string& s1,const std::string& s2)->std::string
					{
						int md = atoi(s2.c_str());
						if (!md) { Warning((u8"警告 : RunExpr() 第" + std::to_string(RealLine()) + u8"行 #I/：试图除以0").c_str()); return "0"; }
						return std::to_string((atoi(s1.c_str()) / md));
					}},
				{"#ID",[](const std::string& s1,const std::string& s2)->std::string
					{
						int md = atoi(s2.c_str());
						if (!md) { Warning((u8"警告 : RunExpr() 第" + std::to_string(RealLine()) + u8"行 #ID：试图除以0").c_str()); return "0"; }
						return std::to_string((atoi(s1.c_str()) / md));
					}},
				{"#I%",[](const std::string& s1,const std::string& s2)->std::string
					{
						int md = atoi(s2.c_str());
						if (!md) { Warning((u8"警告 : RunExpr() 第" + std::to_string(RealLine()) + u8"行 #I%：试图对0取模").c_str()); return "0"; }
						return std::to_string((atoi(s1.c_str()) % md));
					}},
				{"#IU",[](const std::string& s1,const std::string& s2)->std::string
					{
						int md = atoi(s2.c_str());
						if (!md) { Warning((u8"警告 : RunExpr() 第" + std::to_string(RealLine()) + u8"行 #IU：试图对0取模").c_str()); return "0"; }
						return std::to_string((atoi(s1.c_str()) % md));
					}},
						//
						{"#I&",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) & atoi(s2.c_str()))); }},
						{"#IN",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) & atoi(s2.c_str()))); }},
						{"#I|",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) | atoi(s2.c_str()))); }},
						{"#IO",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) | atoi(s2.c_str()))); }},
						{"#I^",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) ^ atoi(s2.c_str()))); }},
						{"#IX",[](const std::string& s1,const std::string& s2)->std::string {return std::to_string((atoi(s1.c_str()) ^ atoi(s2.c_str()))); }},
						//
						{"#S+",[](const std::string& s1,const std::string& s2)->std::string {return s1 + s2; }},
						{"#SA",[](const std::string& s1,const std::string& s2)->std::string {return s1 + s2; }},
						{"#SCF",[](const std::string& s1,const std::string& s2)->std::string
						{
							int at = atoi(s2.c_str());
							if (at >= (int)s1.length()) { Warning((u8"警告 : RunExpr() 第" + std::to_string(RealLine()) + u8"行 #SCF：字符串长度上溢").c_str()); return ""; }
							if (at < 0) { Warning((u8"警告 : RunExpr() 第" + std::to_string(RealLine()) + u8"行 #SCF：字符串长度下溢").c_str()); return ""; }
							return std::string(s1.begin() + at,s1.end());
						}},
						{"#SCB",[](const std::string& s1,const std::string& s2)->std::string
						{
							int at = atoi(s2.c_str());
							if (at >= (int)s1.length()) { Warning((u8"警告 : RunExpr() 第" + std::to_string(RealLine()) + u8"行 #SCB：字符串长度上溢").c_str()); return ""; }
							if (at < 0) { Warning((u8"警告 : RunExpr() 第" + std::to_string(RealLine()) + u8"行 #SCB：字符串长度下溢").c_str()); return ""; }
							return std::string(s1.begin(),s1.begin() + s1.length() - at);
						}},
						{"#SRF",[](const std::string& s1,const std::string& s2)->std::string
							{
								int res;
								//printf("Regex Match: Module=\"%s\" std::string=\"%s\"\n",s2.c_str(),s1.c_str());
								try { res = (int)std::regex_match(s1, std::regex(s2)); }
								catch (std::exception& e) { (void)e; Warning((u8"警告 : RunExpr() 第" + std::to_string(RealLine()) + u8"行 #SRF：正则表达式非法").c_str()); return "0"; }
								return std::to_string(res);
							}},
						{"#SRP",[](const std::string& s1,const std::string& s2)->std::string
							{
								int res;
								//printf("Regex Search: Module=\"%s\" std::string=\"%s\"\n",s2.c_str(),s1.c_str());
								try { res = (int)std::regex_search(s1, std::regex(s2)); }
								catch (std::exception& e) { (void)e; Warning((u8"警告 : RunExpr() 第" + std::to_string(RealLine()) + u8"行 #SRP：正则表达式非法").c_str()); return "0"; }
								return std::to_string(res);
							} },
						{ "#ININI",[](const std::string& s1,const std::string& s2)->std::string
							{
								if (Browse::LoadProgress < Browse::ProgressComplete)
								Warning((u8"警告 : RunExpr() 第" + std::to_string(RealLine()) + u8"行 #ININI：仅支持在Link中调用").c_str());
								int rg = Browse::GetININum(s1);
								if (rg == -1)return "0";
								int Info = Browse::GetSliceFromName(rg, s2);
								if (Info == -1)return "0";
								return "1"; 
							} },

						{"#?:",[](const std::string& s1,const std::string&)->std::string {return s1; }},//三元特判
						{"#CT",[](const std::string& s1,const std::string&)->std::string {return s1; }},//三元特判
	};

	std::string CallCalculateExpr(const std::vector<std::string>& Param);
	std::pair<std::string, __vsit> CalculateForward(__vsit Cur, __vsit End)
	{
		if (Cur == End)return { "",End };
		std::string opr = *Cur;
		//printf("CUR = \"%s\"\n",opr.c_str());
		Cur++;
		if (opr == "#EXPC" || opr == "#EXPCI" || opr == "#EXPCR")
		{
			auto af = CalculateForward(Cur, End);
			Cur = af.second;
			std::string ExprName = af.first;
			af = CalculateForward(Cur, End);
			Cur = af.second;
			int Rx;
			if (af.first == "#DEFAULT")Rx = HasExpr(ExprName) ? ExprMap[ExprName].MaxParam : 0;
			else Rx = atoi(af.first.c_str());

			if (HasExpr(ExprName))
			{
				auto& info = ExprMap[ExprName];
				if (opr == "#EXPCR")info.ExprIter = 0;
				else if (opr == "#EXPCI")
				{
					auto ag = CalculateForward(Cur, End);
					Cur = ag.second;
					info.ExprIter = atoi(ag.first.c_str()) - 1;
				}
			}

			std::vector<std::string> Param{ "",ExprName };
			for (int i = 0; i < Rx; i++)
			{
				auto ap = CalculateForward(Cur, End);
				Cur = ap.second;
				Param.push_back(ap.first);
			}
			return { CallCalculateExpr(Param),Cur };
		}
		if (opr == "#MAKEEXPR")
		{
			std::string Name = RandStr(16);
			std::vector<std::string> Par{ Name };
			--Cur;
			while(1)
			{
				++Cur;
				if (Cur == End)break;
				//Warning(("AP.FIRST : \"" + *Cur + "\"").c_str());
				//ImGui::TextWrapped(("AP.FIRST : \"" + *Cur + "\"").c_str());
				if (*Cur != "#EXPREND")
				{
					//for (auto& ss : SplitParam(*Cur))Par.push_back(ss);
					Par.push_back(*Cur);
				}
				else break;
			}
			//*
			for (size_t i = 0; i < Par.size(); i++)
			{
				//ImGui::TextWrapped(("##PAR THIS : " + std::to_string(i) + " \"" + Par[i] + "\"").c_str());
			}//*/
			Expression(0, Par);
			return { Name,Cur };
		}
		std::string p1, p2;
		auto nit = CalcNum.find(opr);
		auto ncm = CalcMap.find(opr);
		if (nit == CalcNum.end())//数据
		{
			//printf("IS DATA\n");
			return { opr,Cur };
		}
		//printf("IS OPR PARAM N=%d\n",nit->second);
		if (nit->second == 0)//零元 
		{
			std::string al = ncm->second("","");
			//printf("RESULT=%s\n",al.c_str());
			return { al,Cur };
		}
		if (nit->second == 1)//一元 
		{
			auto as = CalculateForward(Cur, End);
			//printf("PARAM 1=%s\n",as.first.c_str());
			std::string al = ncm->second(as.first, "");
			//printf("RESULT=%s\n",al.c_str());
			return { al,as.second };
		}
		if (nit->second == 2)//二元 
		{
			auto af = CalculateForward(Cur, End);
			Cur = af.second;
			auto as = CalculateForward(Cur, End);
			//printf("PARAM 1=%s PARAM 2=%s\n",af.first.c_str(),as.first.c_str());
			std::string al = ncm->second(af.first, as.first);
			//printf("RESULT=%s\n",al.c_str());
			return { al,as.second };
		}
		if (nit->second == 3)//三元特判 
		{
			auto af = CalculateForward(Cur, End);
			Cur = af.second;
			auto as = CalculateForward(Cur, End);
			Cur = as.second;
			auto au = CalculateForward(Cur, End);
			//printf("PARAM 1=%s PARAM 2=%s PARAM 3=%s\n",af.first.c_str(),as.first.c_str(),au.first.c_str());
			std::string al = ncm->second((atoi(af.first.c_str()) ? as.first : au.first), "");
			//printf("RESULT=%s\n",al.c_str());
			return { al,au.second };
		}
		//算完，后面忽略 
		return { "",End };
	}
	std::string CalculateExpr(int& Cursor, const std::vector<std::string>& Expr)
	{
		if (Cursor < 0)Cursor = 0;
		if (Cursor >= (int)Expr.size())Cursor = 0;
		auto It = Expr.begin() + Cursor;
		auto pr = CalculateForward(It, Expr.end());
		Cursor = std::distance(Expr.begin(), pr.second);
		return pr.first;
	}
	std::string CallCalculateExpr(const std::vector<std::string>& Param)
	{
		std::string st = GlobalDecryptString(Param.at(1));
		auto& info = ExprMap[st];
		std::vector<std::string> DParam = info.Expr;
		for (size_t i = 0; i < DParam.size(); i++)
		{
			if (DParam.at(i) == "#PARAM__")
			{
				int id = info.Replace.at(i);
				if (id + 1 < (int)Param.size())
					DParam.at(i) = GlobalDecryptString(Param.at(id + 1));
				else DParam.at(i) = "";
			}
		}
		return CalculateExpr(info.ExprIter, DParam);
	}
	

	struct LinkInfo// cond expr(lim 1) from to hint
	{
		bool On;
		std::string From, Req, ToINI, ToSec, Button, Hint;
	};
	std::vector<LinkInfo> Links;
	std::unordered_map < std::string, int > LinkName;

	int Debug(int Cursor, const std::vector<std::string>& Param)
	{
		std::string Oup = u8"调试数据 : { ";
		bool isexpr = false;
		for (auto s : Param)
		{
			std::string sr = GlobalDecryptString(s);
			if (sr == "#EXPR")
			{
				isexpr = true;
			}
			else if (isexpr)
			{
				Oup += ("\"" + s + "\"=\"" + sr + "\"( ");
				if (HasExpr(sr))
				{
					auto& info = ExprMap[sr];
					for (size_t j = 0; j < info.Expr.size(); j++)
					{
						if ((int)j == info.ExprIter)Oup += u8"**";
						if (info.Expr[j] == u8"#PARAM__")
						{
							Oup += (u8"参数(" + std::to_string(info.Replace[j]) + ") ");
						}
						else Oup += ("\"" + info.Expr[j] + "\" ");
					}
					Oup += u8"运行位置=" + ((int)info.Expr.size() == info.ExprIter ? u8"结尾" : std::to_string(info.ExprIter + 1)) + " ";
				}
				else Oup += u8"找不到 ) ";
				isexpr = false;
			}
			else
			{
				auto p = s.find_first_of('@');
				if (s == sr && p != 0 && p != 1)Oup += ("\"" + s + "\" ");
				else Oup += ("\"" + s + "\"=\"" + sr + "\" ");
			}
		}
		Oup.push_back('}');

		OTMutex.lock();
		OutputText.push_back(Oup);
		OTMutex.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(FnInfoDelay));
		HasOutput = true;

		return Cursor + 1;
	}
	int Begin(int Cursor, const std::vector<std::string>& Param)
	{
		//printf("Function");//OUT
		//PrintVec(Param);//OUT
		//cout<<endl;//OUT
		CallStack.push({ (Param.empty() ? "" : GlobalDecryptString(Param.at(0))),{} });
		auto& Par = CallStack.top().ParamMap;
		int LoadPar = std::min(Param.size() - 1, CallOrder.size());
		for (int i = 0; i < LoadPar; i++)
		{
			Par[Param.at(i + 1)] = CallOrder[i];
		}
		GlobalEnviron["FUNCTION"] = Param.at(0);
		return Cursor + 1;
	}
	int End(int Cursor, const std::vector<std::string>& Param)
	{
		//printf("Return");//OUT 
		//PrintVec(Param);//OUT
		//cout<<endl;//OUT
		int ToCur = RetStack.top();
		for (const auto& ss : LocalStack.top())
		{
			GlobalEnviron.erase(ss);
		}
		RetStack.pop();
		CallStack.pop();
		LocalStack.pop();
		GlobalEnviron["FUNCTION"] = CallStack.top().CallName;
		return ToCur + 1;
	}
	int SetValue(int Cursor, const std::vector<std::string>& Param)
	{
		if (Param.size() < 2)return Cursor + 1;
		GlobalEncryptString(Param.at(0), Param.at(1));
		return Cursor + 1;
	}
	int Raw(int Cursor, const std::vector<std::string>& Param)
	{
		if (Param.size() < 2)return Cursor + 1;
		GlobalEncryptRawString(Param.at(0), Param.at(1));
		return Cursor + 1;
	}
	int LongCall(int Cursor, const std::vector<std::string>& Param)
	{
		static char Fname[MAX_STRING_LENGTH];
		if (Param.size() < 2)return Cursor + 1;
		int JmpTo;
		if (Param.at(1) == "#ALL")
		{
			JmpTo = FindLabelPos({ Begin,{Param.at(0)},0 });
		}
		else if (Param.at(1) == "#THIS")
		{
			int CurFile = CmdLBelongFile[GlobalCursor];
			int Nxt;
			if (CurFile == (int)FileName.size() - 1)Nxt = -1;
			else Nxt = FileCmdLOffset[CurFile + 1];
			JmpTo = FindLabelPos(FileCmdLOffset[CurFile], Nxt, { Begin,{Param.at(0)},0 });
		}
		else
		{
			auto it = std::find_if(FileName.begin(), FileName.end(), [&Param](const std::string& op) {return op == Param.at(1); });
			if (it == FileName.end())
			{
				sprintf(Fname, u8"未载入名为\"%s\"的文件", Param.at(1).c_str());
				Warning(Fname);
				return Cursor + 1;
			}
			int CurFile = std::distance(FileName.begin(), it);
			int Nxt;
			if (CurFile == (int)FileName.size() - 1)Nxt = -1;
			else Nxt = FileCmdLOffset[CurFile + 1];
			JmpTo = FindLabelPos(FileCmdLOffset[CurFile], Nxt, { Begin,{Param.at(0)},0 });
		}

		//cout<<"LongJmpTo:"<<JmpTo<<endl;//OUT
		if (JmpTo == -1)
		{
			sprintf(Fname, u8"找不到名为\"%s\"的调用点", Param.at(0).c_str());
			Warning(Fname);
			return Cursor + 1;
		}
		CallOrder.clear();
		CallOrder.insert(CallOrder.end(), Param.begin() + 2, Param.end());
		LocalStack.push({});
		for (auto& it : CallOrder)
		{
			if (it.length() >= 2 && it[0] == 'V' && it[1] == '@')
			{
				std::string ns = std::to_string(rand()) + std::to_string(rand()) + std::string(it.begin() + 2, it.end());
				GlobalEnviron[ns] = GlobalDecryptString(std::string(it.begin() + 1, it.end()));
				LocalStack.top().push_back(ns);
				it = "@" + ns;
			}
		}
		//cout<<"CallOrder:";//OUT
		//PrintVec(CallOrder);//OUT
		//cout<<endl;//OUT
		RetStack.push(Cursor);
		return JmpTo;
	}
	int Label(int Cursor, const std::vector<std::string>& Param)
	{
		return Cursor + 1;
	}
	int Nothing(int Cursor, const std::vector<std::string>& Param)
	{
		return Cursor + 1;
	}
	int ImportFile(int Cursor, const std::vector<std::string>& Param)
	{
		if (Param.empty())return Cursor + 1;
		auto str = LoadFileStr(Param.at(0));
		auto cmd = GetValidLines(str);
		FileName.push_back(Param.at(0));
		FileCmdLOffset.push_back(GlobalCommands.size());
		FileRealLOffset.push_back(GlobalFileStr.size());
		GlobalFileStr.insert(GlobalFileStr.end(), str.begin(), str.end());
		GlobalCommands.insert(GlobalCommands.end(), cmd.begin(), cmd.end());
		CmdLBelongFile.insert(CmdLBelongFile.end(), cmd.size(), FileName.size() - 1);
		RealLBelongFile.insert(RealLBelongFile.end(), str.size(), FileName.size() - 1);
		return Cursor + 1;
	}
	int LabelJump(int Cursor, const std::vector<std::string>& Param)
	{
		static char Fname[MAX_STRING_LENGTH];
		if (Param.size() < 2)return Cursor + 1;
		int JmpTo;
		if (Param.at(1) == "#ALL")
		{
			JmpTo = FindLabelPos({ Label,{Param.at(0)},0 });
		}
		else if (Param.at(1) == "#THIS")
		{
			int CurFile = CmdLBelongFile[GlobalCursor];
			int Nxt;
			if (CurFile == (int)FileName.size() - 1)Nxt = -1;
			else Nxt = FileCmdLOffset[CurFile + 1];
			JmpTo = FindLabelPos(FileCmdLOffset[CurFile], Nxt, { Label,{Param.at(0)},0 });
		}
		else
		{
			auto it = std::find_if(FileName.begin(), FileName.end(), [&Param](const std::string& op) {return op == Param.at(1); });
			if (it == FileName.end())
			{
				sprintf(Fname, u8"未载入名为\"%s\"的文件", Param.at(1).c_str());
				Warning(Fname);
				return Cursor + 1;
			}
			int CurFile = std::distance(FileName.begin(), it);
			int Nxt;
			if (CurFile == (int)FileName.size() - 1)Nxt = -1;
			else Nxt = FileCmdLOffset[CurFile + 1];
			JmpTo = FindLabelPos(FileCmdLOffset[CurFile], Nxt, { Label,{Param.at(0)},0 });
		}
		//cout<<"JmpTo:"<<JmpTo<<endl;//OUT
		if (JmpTo == -1)
		{
			sprintf(Fname, u8"找不到名为\"%s\"的标签", Param.at(0).c_str());
			Warning(Fname);
			return Cursor + 1;
		}
		return JmpTo + 1;
	}
	int LabelJumpIf(int Cursor, const std::vector<std::string>& Param)
	{
		static char Fname[MAX_STRING_LENGTH];
		if (Param.size() < 2)return Cursor + 1;
		if (!HasExpr(GlobalDecryptString(Param.at(1))))return Cursor + 1;
		std::string ans = CallCalculateExpr(Param);
		if (ans == "#CONTINUE")return Cursor + 1;
		int JmpTo;
		if (Param.at(0) == "#ALL")
		{
			JmpTo = FindLabelPos({ Label,{ans},0 });
		}
		else if (Param.at(0) == "#THIS")
		{
			int CurFile = CmdLBelongFile[GlobalCursor];
			int Nxt;
			if (CurFile == (int)FileName.size() - 1)Nxt = -1;
			else Nxt = FileCmdLOffset[CurFile + 1];
			JmpTo = FindLabelPos(FileCmdLOffset[CurFile], Nxt, { Label,{ans},0 });
		}
		else
		{
			auto it = std::find_if(FileName.begin(), FileName.end(), [&Param](const std::string& op) {return op == Param.at(0); });
			if (it == FileName.end())
			{
				sprintf(Fname, u8"未载入名为\"%s\"的文件", Param.at(0).c_str());
				Warning(Fname);
				return Cursor + 1;
			}
			int CurFile = std::distance(FileName.begin(), it);
			int Nxt;
			if (CurFile == (int)FileName.size() - 1)Nxt = -1;
			else Nxt = FileCmdLOffset[CurFile + 1];
			JmpTo = FindLabelPos(FileCmdLOffset[CurFile], Nxt, { Label,{ans},0 });
		}
		//cout<<"JmpTo:"<<JmpTo<<endl;//OUT
		if (JmpTo == -1)
		{
			sprintf(Fname, u8"找不到名为\"%s\"的标签", ans.c_str());
			Warning(Fname);
			return Cursor + 1;
		}
		return JmpTo + 1;
	}
	int CmdLineJump(int Cursor, const std::vector<std::string>& Param)
	{
		int T;
		if (Param.empty())T = 0;
		else T = atoi(GlobalDecryptString(Param.at(0)).c_str());
		return Cursor + (T ? T : 1);
	}
	int PrintTo(int Cursor, const std::vector<std::string>& Param)
	{
		if (Param.empty())return Cursor + 1;
		std::string sp;
		std::vector<std::string> TParam;
		bool isExpr = false;
		for (auto st = Param.begin() + 1; st != Param.end(); ++st)
			for (const auto& ss : SplitParam(GlobalDecryptString(*st)))
			{
				if (GlobalDecryptString(ss) == "#EXPR")isExpr = true;
				else if (isExpr)
				{
					std::string sv = GlobalDecryptString(ss);
					if (HasExpr(sv))
					{
						auto& in = ExprMap[sv];
						for (size_t i = 0; i < in.Expr.size(); i++)
						{
							if (in.Expr[i] == "#PARAM__")TParam.push_back("#PAR<" + std::to_string(in.Replace[i]) + ">");
							else TParam.push_back(in.Expr[i]);
							TParam.push_back("\n");
						}
						TParam.pop_back();
					}
					isExpr = false;
				}
				else TParam.push_back(ss);
			}
		//PrintVec(TParam);//OUT
		//cout << endl;//OUT
		if (TParam.empty())return Cursor + 1;
		for (auto si : TParam) { sp += GlobalDecryptString(si); sp.push_back('\n'); }
		GlobalEncryptString(TParam.at(0), sp);
		return Cursor + 1;
	}
	int LPrintTo(int Cursor, const std::vector<std::string>& Param)
	{
		if (Param.size() < 2)return Cursor + 1;
		std::string sp;
		std::vector<std::string> TParam;
		bool isExpr = false;
		for (auto st = Param.begin() + 1; st != Param.end(); ++st)
			for (const auto& ss : SplitParam(GlobalDecryptString(*st)))
			{
				if (GlobalDecryptString(ss) == "#EXPR")isExpr = true;
				else if (isExpr)
				{
					std::string sv = GlobalDecryptString(ss);
					if (HasExpr(sv))
					{
						auto& in = ExprMap[sv];
						for (size_t i = 0; i < in.Expr.size(); i++)
						{
							if (in.Expr[i] == "#PARAM__")TParam.push_back("#PAR<" + std::to_string(in.Replace[i]) + ">");
							else TParam.push_back(in.Expr[i]);
							TParam.push_back(",");
						}
						TParam.pop_back();
					}
					isExpr = false;
				}
				else TParam.push_back(ss);
			}
		//PrintVec(TParam);//OUT
		//cout<<endl;//OUT
		if (TParam.empty())return Cursor + 1;
		for (auto si : TParam) { sp += GlobalDecryptString(si); }
		GlobalEncryptString(Param.at(0), sp);
		return Cursor + 1;
	}
	int Run(int Cursor, const std::vector<std::string>& Param)
	{
		static char Fcs[MAX_STRING_LENGTH];
		if (Param.empty())return Cursor + 1;
		std::vector<std::string> TParam;
		bool isExpr = false;
		for (const auto& st : Param)
			for (const auto& ss : SplitParam(GlobalDecryptString(st)))
			{
				if (GlobalDecryptString(ss) == "#EXPR")isExpr = true;
				else if (isExpr)
				{
					std::string sv = GlobalDecryptString(ss);
					if (HasExpr(sv))
					{
						auto& in = ExprMap[sv];
						for (size_t i = 0; i < in.Expr.size(); i++)
						{
							if (in.Expr[i] == "#PARAM__")TParam.push_back("#PAR<" + std::to_string(in.Replace[i]) + ">");
							else TParam.push_back(in.Expr[i]);
						}
					}
					isExpr = false;
				}
				else TParam.push_back(ss);
			}

		if (TParam.empty())return Cursor + 1;
		auto FnS = GlobalDecryptString(TParam.at(0));
		auto FnIter = CommandMap.find(FnS);
		TParam.erase(TParam.begin());
		if (FnIter != CommandMap.end())
		{
			//std::string tot;
			//for (const auto& ss : TParam)tot += ('\"' + ss + '\"');
			//sprintf(Fcs,"Run() 解析内容： Fn=\"%s\",Param={%s}", FnS.c_str(), tot.c_str());
			//Warning(Fcs);
			return FnIter->second(Cursor, TParam);
		}
		else
		{
			std::string tot;
			for (const auto& ss : TParam)tot += ('\"' + ss + '\"');
			sprintf(Fcs, u8"Run() 解析错误： Fn=\"%s\",Param={%s}", FnS.c_str(), tot.c_str());
			Warning(Fcs);
			return Cursor + 1;
		}

	}
	int Expression(int Cursor, const std::vector<std::string>& Param)
	{
		if (Param.empty())return Cursor + 1;
		std::vector<std::string> TParam;
		bool isExpr = false;
		/*
		for (size_t i = 0; i < Param.size(); i++)
		{
			Warning(("##PARAM : " + std::to_string(i) + " \"" + Param[i] + "\"").c_str());
		}*/
		for (const auto& st : Param)
			for (const auto& ss : SplitParam(GlobalDecryptString(st)))
			{
				if (GlobalDecryptString(ss) == "#EXPR")isExpr = true;
				else if (isExpr)
				{
					std::string sv = GlobalDecryptString(ss);
					if (HasExpr(sv))
					{
						auto& in = ExprMap[sv];
						for (size_t i = 0; i < in.Expr.size(); i++)
						{
							if (in.Expr[i] == "#PARAM__")TParam.push_back("#PAR<" + std::to_string(in.Replace[i]) + ">");
							else TParam.push_back(in.Expr[i]);
						}
					}
					isExpr = false;
				}
				else TParam.push_back(ss);
			}
		/*v
		for (size_t i = 0; i < TParam.size(); i++)
		{
			Warning(("##TPARAM : " + std::to_string(i) + " \"" + TParam[i] + "\"").c_str());
		}*/
		if (TParam.empty())return Cursor + 1;
		std::string st = GlobalDecryptString(TParam.at(0));
		auto& info = ExprMap[st];
		//Warning(("EXPR NAME = " + st).c_str());
		if (TParam.size() == 1)return Cursor + 1;
		//Warning(("EXPR NAME = " + st).c_str());

		info.Expr.clear();
		info.Replace.resize(TParam.size() - 1);
		info.ExprIter = 0;

		for (size_t i = 1; i < TParam.size(); i++)
		{
			std::string sr = GlobalDecryptString(TParam.at(i));
			//Warning(("TPARAM : " + std::to_string(i) + " \"" + sr + "\"").c_str());
			if (sr.length() > 5 && std::string(sr.begin(), sr.begin() + 5) == "#PAR<")
			{
				int Rx;
				sscanf(sr.c_str(), "#PAR<%d>", &Rx);
				sr = "#PARAM__";
				info.Replace[i - 1] = Rx;
				info.MaxParam = std::max(Rx, info.MaxParam);
			}
			else if (sr.length() > 6 && std::string(sr.begin(), sr.begin() + 5) == "R#PAR<")
			{
				info.Expr.push_back(std::string(sr.begin() + 1, sr.end()));
			}
			info.Expr.push_back(sr);
		}
		return Cursor + 1;
	}
	int RunExpr(int Cursor, const std::vector<std::string>& Param)
	{
		if (Param.size() < 2)return Cursor + 1;
		if (!HasExpr(GlobalDecryptString(Param.at(1))))return Cursor + 1;
		GlobalEncryptString(Param.at(0), CallCalculateExpr(Param));
		return Cursor + 1;
	}
	int Bind(int Cursor, const std::vector<std::string>& Param)
	{
		if (Param.size() < 3)return Cursor + 1;
		std::string sf = GlobalDecryptString(Param.at(1));
		if (!HasExpr(sf))return Cursor + 1;
		auto& infof = ExprMap[sf];
		auto& infot = ExprMap[GlobalDecryptString(Param.at(0))];
		if (Param.at(2) == "#SAME")
		{
			infot = infof;
		}
		else if (Param.at(2) == "#FULL")
		{
			infot = infof;
			infot.ExprIter = 0;
		}
		else if (Param.at(2) == "#BEFORE")
		{
			infot = ExprInfo{};
			infot.Expr.insert(infot.Expr.end(), infof.Expr.begin(), infof.Expr.begin() + infof.ExprIter);
			infot.Replace.insert(infot.Replace.end(), infof.Replace.begin(), infof.Replace.begin() + infof.ExprIter);
			infot.ExprIter = 0;
			infot.MaxParam = 0;
			for (int p : infot.Replace)infot.MaxParam = std::max(infot.MaxParam, p);
		}
		else if (Param.at(2) == "#AFTER")
		{
			infot = ExprInfo{};
			infot.Expr.insert(infot.Expr.end(), infof.Expr.begin() + infof.ExprIter, infof.Expr.end());
			infot.Replace.insert(infot.Replace.end(), infof.Replace.begin() + infof.ExprIter, infof.Replace.end());
			infot.ExprIter = 0;
			infot.MaxParam = 0;
			for (int p : infot.Replace)infot.MaxParam = std::max(infot.MaxParam, p);
		}
		else
		{
			ExprMap.erase(GlobalDecryptString(Param.at(0)));
			return Cursor + 1;
		}
		if (Param.size() == 3)return Cursor + 1;
		for (size_t i = 0; i < infot.Expr.size(); i++)
		{
			if (infot.Expr.at(i) == "#PARAM__")
			{
				int id = infot.Replace.at(i);
				if (id + 2 < (int)Param.size())
				{
					sf = GlobalDecryptString(Param.at(id + 2));
					if (sf.length() > 5 && std::string(sf.begin(), sf.begin() + 5) == "#PAR<")
					{
						int Rx;
						sscanf(sf.c_str(), "#PAR<%d>", &Rx);
						infot.Expr.at(i) = "#PARAM__";
						infot.Replace[i] = Rx;
						infot.MaxParam = std::max(Rx, infot.MaxParam);
					}
					else if (sf.length() > 6 && std::string(sf.begin(), sf.begin() + 5) == "R#PAR<")
					{
						infot.Expr.at(i) = std::string(sf.begin() + 1, sf.end());
						infot.Replace.at(i) = 0; 
					}
					else
					{
						infot.Expr.at(i) = sf;
						infot.Replace.at(i) = 0;
					}
				}
				else infot.Expr.at(i) = "";
			}
		}
		return Cursor + 1;
	}
	int SetExprIter(int Cursor, const std::vector<std::string>& Param)
	{
		if (Param.size() < 3)return Cursor + 1;
		std::string ss = GlobalDecryptString(Param.at(0));
		if (!HasExpr(ss))return Cursor + 1;
		auto& info = ExprMap[ss];
		int Rx = atoi(Param.at(2).c_str());
		if (Param.at(1) == "#CUR")info.ExprIter += Rx;
		if (Param.at(1) == "#BEGIN")info.ExprIter = Rx - 1;
		if (Param.at(1) == "#END")info.ExprIter = info.Expr.size() - Rx;
		return Cursor + 1;
	}
	int Link(int Cursor, const std::vector<std::string>& Param)// cond expr(lim 1) button hint
	{
		if (Param.size() < 8)return Cursor + 1;
		LinkInfo inf{ false,
			GlobalDecryptString(Param.at(2)),
			GlobalDecryptString(Param.at(3)),
			GlobalDecryptString(Param.at(4)),
			GlobalDecryptString(Param.at(5)),
			GlobalDecryptString(Param.at(6)),
			GlobalDecryptString(Param.at(7)) };

		if (GlobalDecryptString(Param.at(1)) == "#ON")inf.On = true;
		else if (GlobalDecryptString(Param.at(1)) == "#OFF")inf.On = false;
		Links.push_back(inf);
		LinkName[GlobalDecryptString(Param.at(1))] = Links.size() - 1;
		return Cursor + 1;
	}
	void thrLoadGlobal(const std::string& File)//Force UTF8
	{
		HasOutput = false;
		
		GlobalFileStr = LoadFileStr(File);
		GlobalCommands = GetValidLines(GlobalFileStr);
		RealLBelongFile.push_back(0);

		FileName.push_back(File);
		FileCmdLOffset.push_back(0);
		FileRealLOffset.push_back(1);
		CmdLBelongFile.insert(CmdLBelongFile.end(), GlobalCommands.size(), FileName.size() - 1);
		RealLBelongFile.insert(RealLBelongFile.end(), GlobalFileStr.size(), FileName.size() - 1);

		GlobalCursor = FindLabelPos({ Begin,{"main"},0 });
		RetStack.push(GlobalCursor);
		CallStack.push({ "main",{} });
		LocalStack.push({});
		//cout<<"Cursor:"<<Cursor<<" AT "<<GlobalCommands[Cursor].RealLineNo<<endl;
		while (GlobalCursor >= 0 && GlobalCursor < (int)GlobalCommands.size())
		{
			GlobalCursor = GlobalCommands[GlobalCursor].Func(GlobalCursor, GlobalCommands[GlobalCursor].Param);
			//printf("Cursor=%d LineNo=%d\n",Cursor,GlobalCommands[Cursor].RealLineNo);
			//cout<<"Environ:";
			//PrintUM(GlobalEnviron);
			//cout<<endl;
			if (CallStack.size() <= 1)break;
		}

		if (HasOutput)std::this_thread::sleep_for(std::chrono::milliseconds(FnInfoEndDelay));
	}

	struct LinkResult
	{
		std::string JumpINI, JumpSec , Button, Hint;
	};
	/*
		const std::string& ININame,
		const std::string& INIActive,
		const std::string& CurSec,
		const std::string& TgKey,
		const std::string& TgValue
		*/
	std::vector<LinkResult> ProcessLink(std::vector<std::string>& Params)
	{
		std::vector<LinkResult> Result;
		for (auto& lnk : Links)
		{
			if (lnk.On)
			{
				ExprMap[lnk.From].ExprIter = 0;
				Params[1] = lnk.From;
				std::string froms = CallCalculateExpr(Params);
				Params[1] = froms;
				ExprMap[froms].ExprIter = 0;
				//if (ExprMap[froms].Expr.size() != 1)MessageBoxA(nullptr, std::to_string(ExprMap[froms].Expr.size()).c_str(), "BOOP", MB_OK);
				if (ExprMap[froms].Expr.empty())continue;
				while (ExprMap[froms].ExprIter != ExprMap[froms].Expr.size())
				{
					std::string spls = CallCalculateExpr(Params);
					Params[1] = lnk.Req;
					ExprMap[lnk.Req].ExprIter = 0;
					if (Params.size() < 8)Params.resize(9);
					Params[7] = spls;
					if (lnk.Req != "#IGNORE")
					{
						std::string reqs = CallCalculateExpr(Params);
						if (!IsTrueString(reqs))
						{
							Params[1] = froms;
							Params[7] = "";
							continue;
						}
					}
					Params[1] = lnk.ToINI;
					ExprMap[lnk.ToINI].ExprIter = 0;
					std::string toi = CallCalculateExpr(Params);
					Params[1] = lnk.ToSec;
					ExprMap[lnk.ToSec].ExprIter = 0;
					std::string tos = CallCalculateExpr(Params);
					Params[1] = lnk.Button;
					ExprMap[lnk.Button].ExprIter = 0;
					std::string bns = CallCalculateExpr(Params);
					Params[1] = lnk.Hint;
					ExprMap[lnk.Hint].ExprIter = 0;
					std::string his = CallCalculateExpr(Params);
					//ImGui::TextWrapped(u8"to=%s btn=%s hint=%s", tos.c_str(), bns.c_str(), his.c_str());
					Result.push_back({ toi,tos,bns,his });
					Params[1] = froms;
					Params[7] = "";
				}
				ExprMap.erase(froms);
			}
		}
		return Result;
	}

	/*
	int main()
	{
		//rnd = std::default_random_engine{ (unsigned)time(NULL) };
		//srand(time(NULL));
		//thrLoadGlobal("load.in");
		system("pause");
		return 0;
	}
	*/
}








namespace BrowserCallback
{
	void InitialLoad()
	{
		ImGui::Text(u8"载入中……");
	}
	void GetFileList()
	{
		ImGui::Text(u8"获取文件列表……");
	}
	void ProjDataNotFound()
	{
		ImGui::Text(u8"错误：未检测到 __ProjectData 子目录");
		ImGui::Text(u8"程序将在5秒后退出");
	}
	void LoadINI()
	{
		Browse::UseBuf.lock();
		ImGui::Text(u8"载入INI %s（%d/%d）", MBCStoUTF8(Browse::Buf).c_str(), Browse::CurFile.load()+1, Browse::TotFile);
		Browse::UseBuf.unlock();
	}
	void Finish()
	{
		ImGui::Text(u8"加载完成！");
	}
	void FilterInit()
	{
		ImGui::Text(u8"初始化筛选器……");
	}
	void LinkJumpInit()
	{
		ImGui::Text(u8"初始化跳转列表……");
		GlobalAnalyze::OTMutex.lock();
		for(auto& ss: GlobalAnalyze::OutputText)
			ImGui::TextWrapped(ss.c_str());
		GlobalAnalyze::OTMutex.unlock();
	}
}
callback_t BrowserCallbackList[] =
{
	BrowserCallback::InitialLoad,
	BrowserCallback::ProjDataNotFound,
	BrowserCallback::LoadINI,
	BrowserCallback::Finish,
	BrowserCallback::GetFileList,
	BrowserCallback::FilterInit,
	BrowserCallback::LinkJumpInit
};

namespace Browse
{
	struct INISliceInfo
	{
		TE_FileID MainWindow;
		bool IsOpen{ false };
		int INI;
		int Sec;
		int ID;
	};
	std::map<int, INISliceInfo*> InfoMap;
	int MaxID = 0;
	struct INIGraph
	{
		std::vector<std::string> SecName;
		std::vector<hash_t> SecHash;
		std::vector<INISliceInfo> SectionSlice;
		std::string IniName;
		std::vector<int> FilterSec;//SortedSec的索引 每次增删字段、改筛选器、排序时启动
		std::vector<int> SortedSec;//SecHash的索引 每次增删字段、排序时启动
		std::vector<int> SecOrder; //SecHash的存储序索引
		hash_t IniHash;

		INIGraph() = default;
		INIGraph(const std::string& Name, hash_t Hs) :IniName(Name), IniHash(Hs) {}
		
		//RUNTIME:
		int CurrentPage{ 0 };
		int ResetPage{ -1 };
		int CurrentFilter{ 0 };
		std::vector<int> CurrentSubFilter;
		int CurrentSort{ 0 };
		int CurrentSortPage{ 0 };

		std::unordered_map<std::string, int>NameSec;
	};
	std::vector<INIGraph> INIFiles;

	void thrLoadINI(const char* Name, hash_t NHash, const std::string& Path)
	{
		INIFiles.push_back(INIGraph( std::string(Name),NHash ));
		auto& ThisFile = INIFiles.back();

		ExtFileClass DatFile;
		sprintf(Br, "%s\\%08X.dat", Path.c_str(), NHash);
		DatFile.Open(Br, "rb", nullptr);
		if (EnableLog)
		{
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog(DatFile.Available()?"dat文件打开成功。":"dat文件打开失败。");
		}
		int32_t SecN,Len; 
		DatFile.Read(&SecN, sizeof(int), 1);
		ThisFile.SecHash.resize(SecN);
		ThisFile.SecOrder.resize(SecN);
		ThisFile.SectionSlice.resize(SecN);
		ThisFile.FilterSec.resize(SecN);
		ThisFile.SortedSec.resize(SecN);
		ThisFile.SecName.reserve(SecN);
		ThisFile.CurrentSubFilter.resize(FilterListLen + 2);
		DatFile.Read(ThisFile.SecHash.data(), sizeof(hash_t), SecN);//vector复制
		DatFile.Read(ThisFile.SecOrder.data(), sizeof(int), SecN);//vector复制
		if (EnableLog)
		{
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog("初始化INI数据结构。");
		}

		for (auto& slice : ThisFile.SectionSlice)
		{
			TE_Initialize(&slice.MainWindow);
		}
		if (EnableLog)
		{
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog("命令TextEdit.dll使用TE_Initialize开辟空间");
		}

		ThisFile.SecName.reserve(SecN + 10);
		for (int i = 0; i < SecN; i++)
		{
			DatFile.Read(&Len, sizeof(int32_t), 1);
			DatFile.Read(Br, 1, Len);
			Br[Len] = 0;
			ThisFile.SecName.push_back(std::string{ Br });
			ThisFile.FilterSec[i] = i;
			ThisFile.SortedSec[i] = i;
			ThisFile.NameSec[ThisFile.SecName.back()] = i;
		}
		if (EnableLog)
		{
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog("初始化INI字段名。");
		}

		DatFile.Close();


	}
	void thrInit()
	{
		if (EnableLog)
		{
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog("初始化线程启动成功。");
		}
		std::string& Path = Pathbuf;
		while (!InitGo！.load())std::this_thread::sleep_for(std::chrono::milliseconds(10));

		if (EnableLog)
		{
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog("初始化线程开始初始化。");
		}

		std::string dir_n = Path + "\\__ProjectData";
		CallbackSelect = 0;
		if (_access(dir_n.c_str(), 0) != 0)
		{
			CallbackSelect = 1;
			if (EnableLog)
			{
				GlobalLog.AddLog_CurTime(false);
				GlobalLog.AddLog("初始化失败 — 未找到__ProjectData目录。");
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(5000));
            PreLink::CleanUp();
            exit(0);
		}
		LoadProgress = ProgressComplete / 5;
		int RestP = ProgressComplete - LoadProgress - ProgressComplete / 4;
		CurFile.store(0);
		CallbackSelect = 4;
		ExtFileClass Full;
		Full.Open((dir_n + "\\Files.dat").c_str(), "rb", nullptr);//if cannot self terminate
		if (EnableLog)
		{
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog("尝试打开Files.dat。");
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog((std::string("Files.dat ") + (Full.Available() ? "打开成功" : "打开失败")).c_str());
		}
		int NameHash, Len;
		Full.Read(&TotFile, sizeof(int), 1);
		CallbackSelect = 2;
		for (int i=0; i < TotFile; i++)
		{
			CurFile.store(i);
			Full.Read(&NameHash, sizeof(int), 1);
			Full.Read(&Len, sizeof(int), 1);
			Full.Read(Buf, 1, Len);
			Buf[Len] = 0;
			std::this_thread::sleep_for(std::chrono::milliseconds(std::min(300 / TotFile, 25)));//优化要加钱（大雾）
			UseBuf.lock();
			if (EnableLog)
			{
				GlobalLog.AddLog_CurTime(false);
				GlobalLog.AddLog(("开始载入ini — " + std::string(Buf)).c_str());
			}
			thrLoadINI(Buf, NameHash, dir_n);
			UseBuf.unlock();
			LoadProgress += RestP / TotFile;
		}
		Full.Close();
		CallbackSelect = 6;
		LoadProgress = ProgressComplete - ProgressComplete / 4;
		
		Ini::IniFile str;
		if (str.Load((Path + "\\string.ini").c_str()))
		{
			auto Sec = str.GetSection("String");
			if (Sec != str.NotFound())
			{
				if (StringSecIterAvailable)for (auto& sv : Sec->second.SecStr)StringSec.SecStr[sv.first] = sv.second;
				else StringSec = Sec->second;
			}
			if (EnableLog)
			{
				GlobalLog.AddLog_CurTime(false);
				GlobalLog.AddLog("载入项目目录下的string.ini成功。");
			}
		}
		else
		{
			if (EnableLog)
			{
				GlobalLog.AddLog_CurTime(false);
				GlobalLog.AddLog("载入项目目录下的string.ini失败。");
			}
		}
		if (_access((Path + "\\link.tbl").c_str(), 0) == 0)
		{
			GlobalAnalyze::thrLoadGlobal((Path + "\\link.tbl").c_str());
			if (EnableLog)
			{
				GlobalLog.AddLog_CurTime(false);
				GlobalLog.AddLog("成功载入项目目录下的link.tbl。");
			}
		}
		else
		{
			GlobalAnalyze::thrLoadGlobal(".\\Resources\\link.tbl");
			if (EnableLog)
			{
				GlobalLog.AddLog_CurTime(false);
				GlobalLog.AddLog("未能载入项目目录下的link.tbl — 载入默认的link.tbl。");
			}
		}
		CallbackSelect = 5;
		LoadProgress = ProgressComplete - ProgressComplete / 8;
		for (int sr = 0; sr < FilterInitListLen; sr++)
		{
			FilterInitList[sr]((int)INIFiles.size());
		}
		if (EnableLog)
		{
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog("筛选器初始化成功。");
		}

		CallbackSelect = 3;
		LoadProgress = ProgressComplete-2;
		std::this_thread::sleep_for(std::chrono::milliseconds(1500)); 
		if (EnableLog)
		{
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog("初始化完成。");
		}
		LoadProgress = ProgressComplete;

		if (EnableLog)
		{
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog("初始化线程结束运行。");
		}

        onexit(ExitCloseAllFile);
	}
	void Initialize()
	{
		static bool First = false;
		if (First)return;
		First = true;
		if (EnableLog)
		{
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog("转告初始化线程开始初始化。");
		}
		InitGo！.store(true);
		if (EnableLog)
		{
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog("向初始化线程转告完毕。");
		}
        while (!FontLoaded.load());
        glfwShowWindow(PreLink::window);
        SetClassLong(MainWindowHandle, GCL_HICON, (LONG)LoadIcon(PreLink::hInst, MAKEINTRESOURCE(IDI_ICON1)));
		if (EnableLog)
		{
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog("显示渲染窗口。");
		}
		while(GetForegroundWindow()!= MainWindowHandle)SetForegroundWindow(MainWindowHandle);
		if (EnableLog)
		{
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog("将渲染窗口设置为活动窗口。");
		}
		return;
	}
	
	template<typename Cont>
	void ShowList(const std::string& suffix,std::vector<Cont>& Ser, int* Page, void (*Callback)(Cont&,int,int))
	{
		int RenderF = (*Page) * KeyPerPage;
		int RenderN = (1 + (*Page)) * KeyPerPage;
		int Sz = Ser.size();
		bool HasPrev = ((*Page) != 0);
		bool HasNext = (RenderN < Sz);
		int RealRF = std::max(RenderF, 0);
		int RealNF = std::min(RenderN, Sz);
		int PageN = GetPage(Sz);
		for (int On = RealRF; On < RealNF; On++)
		{
			Callback(Ser.at(On),On-RealRF+1,On);
		}
		if (HasPrev||HasNext)
		{
			if (HasPrev)
			{
				if (ImGui::ArrowButton(("prev_" + suffix).c_str(), ImGuiDir_Left))
				{
					(*Page)--;
                    if (EnableLog)
                    {
                        GlobalLog.AddLog_CurTime(false);
                        GlobalLog.AddLog("点击了上一页按钮。");
                    }
				}
				ImGui::SameLine();
				ImGui::Text(u8"上一页");
				ImGui::SameLine();
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Text, { 0,0,0,0 });
				ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0,0,0,0 });
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0,0,0,0 });
				if (ImGui::ArrowButton(("prev_" + suffix).c_str(), ImGuiDir_Left))
#ifdef SESE
				SeseCheck()
#endif
				;
				ImGui::SameLine();
				ImGui::PopStyleColor(4);
			}
			if ((*Page) + 1 >= 1000)ImGui::SetCursorPosX(FontHeight * 5.5f);
			else ImGui::SetCursorPosX(FontHeight * 6.0f);
			ImGui::Text(u8"第（%d/%d）页", (*Page) + 1, PageN);
			ImGui::SameLine();
			if ((*Page) + 1 >= 1000)ImGui::SetCursorPosX(FontHeight * 13.0f);
			if ((*Page) + 1 >= 100)ImGui::SetCursorPosX(FontHeight * 12.5f);
			else ImGui::SetCursorPosX(FontHeight * 12.0f);
			if (HasNext)
			{
				ImGui::Text(u8"下一页");
				ImGui::SameLine();
				if (ImGui::ArrowButton(("next_" + suffix).c_str(), ImGuiDir_Right))
				{
					(*Page)++;
                    if (EnableLog)
                    {
                        GlobalLog.AddLog_CurTime(false);
                        GlobalLog.AddLog("点击了下一页按钮。");
                    }
				}
				ImGui::SameLine();
				
			}
			ImGui::NewLine();
		}
	}
	
	struct CurrentWindow
	{
		INISliceInfo* Info{ nullptr };
		bool valid{ false };
	}CurWindow;
	
	bool IsActiveWindow(Browse::INISliceInfo Info)
	{
		if (!CurWindow.valid)return false;
		return CurWindow.Info->ID == Info.ID;
	}
	void InfoMapDel(int ID)
	{
		InfoMap[ID]->IsOpen = false;
		InfoMap.erase(ID);
	}
	bool IsINISliceOpen(INISliceInfo* Info)
	{
		if ((!TE_IsFileOpen(Info->MainWindow)) && Info->IsOpen)//发现不可用
		{
			Info->IsOpen = false;//确认是否打开切片时确认窗口在不在
			if (Info->INI == CurWindow.Info->INI && Info->Sec == CurWindow.Info->Sec)
			{
				CurWindow.valid = false;
			}
			InfoMapDel(Info->ID);
		}
		return Info->IsOpen;
	}
	bool IsINISliceOpen(int INIAbsoluteOrder, int SectionAbsoluteOrder)
	{
		return IsINISliceOpen(&INIFiles[INIAbsoluteOrder].SectionSlice[SectionAbsoluteOrder]);
	}

	void SelectINISlice(int INIAbsoluteOrder, int SectionAbsoluteOrder)//仅在确认切片可用时调用
	{
		CurWindow.Info = &(INIFiles[INIAbsoluteOrder].SectionSlice[SectionAbsoluteOrder]);
		CurWindow.valid = true;
		//ChangeJumpList();
		TE_SetActive(CurWindow.Info->MainWindow);//假定开启
		IsFileChanging = true;
		WindowMustSync = true;
		if (EnableLog)
		{
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog("将一个字段置为活动。");
		}
	}
	void OpenINISlice(int INIAbsoluteOrder, int SectionAbsoluteOrder)
	{
		static wchar_t  ConDir[500];
		wsprintf(ConDir, L"%s\\__ProjectData\\%08X_%08X_%08X.ini", MBCStoUnicode(Pathbuf).c_str(),
			INIFiles[INIAbsoluteOrder].SecOrder[SectionAbsoluteOrder],
			INIFiles[INIAbsoluteOrder].IniHash, INIFiles[INIAbsoluteOrder].SecHash[SectionAbsoluteOrder]);
		//::MessageBoxW(nullptr, ConDir, L"Ding!", MB_ICONINFORMATION);
		auto& Info = INIFiles[INIAbsoluteOrder].SectionSlice[SectionAbsoluteOrder];
		if (TE_OpenFile(Info.MainWindow, ConDir, GetWindowRetry, []() { Sleep(GetWindowInitialDelay); }, []() {Sleep(GetWindowRetryDelay); }))
		{
			Info.IsOpen = true;
			Info.INI = INIAbsoluteOrder;
			Info.Sec = SectionAbsoluteOrder;
			++MaxID;
			Info.ID = MaxID;
			InfoMap[MaxID] = &Info;
			SelectINISlice(INIAbsoluteOrder, SectionAbsoluteOrder);
			if (EnableLog)
			{
				GlobalLog.AddLog_CurTime(false);
				GlobalLog.AddLog("成功打开了一个字段。");
			}
		}
		else
		{
			if (EnableLog)
			{
				GlobalLog.AddLog_CurTime(false);
				GlobalLog.AddLog("未能打开一个字段。");
			}
			::MessageBoxW(nullptr, ConDir, L"打开文件失败！", MB_ICONINFORMATION);
		}
		IsFileChanging = true;
		WindowMustSync = true;
	}
	void InfoMapRewriteVec(std::vector<INISliceInfo*>& vec)
	{
		vec.clear();
		std::vector<INISliceInfo*> tmp;
		for (auto sptr : InfoMap)
		{
			tmp.push_back(sptr.second);
		}
		for (auto sptr : tmp)
		{
			if (IsINISliceOpen(sptr))
			{
				vec.push_back(sptr);
			}
		}
	}

	void CloseFile(INISliceInfo Slice,bool Wait)//如果点了取消就没关掉
	{
		if (IsINISliceOpen(&Slice))
		{
			TE_TellClose(Slice.MainWindow, Wait);
			if (CurWindow.Info->ID == Slice.ID)IsFileChanging = true;
		}
		IsINISliceOpen(&Slice);//只是做关闭后的处理&善后，不判断
	}
	void CloseAll(bool Wait)
	{
		std::vector<INISliceInfo*> tmp;
		for (auto sptr : InfoMap)
		{
			tmp.push_back(sptr.second);
		}
		for (auto sptr : tmp)
		{
			CloseFile(*sptr, Wait);
		}
	}

	int GetININum(const std::string& Name)
	{
		for (size_t i = 0; i < INIFiles.size(); i++)
		{
			if (INIFiles[i].IniName == Name)return i;
		}
		return -1;
	}
	int GetSliceFromName(int INI, const std::string& Name)
	{
		//ImGui::Text("GSS INI=%d FindTg=\"%s\" ", INI, Name.c_str());
		//for (size_t i = 0; i < INIFiles[INI].SecName.size(); i++)if (INIFiles[INI].SecName[i] == Name)return i;
		//ThisFile.NameSec[ThisFile.SecName.back()] = i;
		auto it = INIFiles[INI].NameSec.find(Name);
		if (it != INIFiles[INI].NameSec.end())return it->second;
		return -1;
	}

}






















typedef void (*filter_t)(int,bool);
namespace Filter
{
	void ResetFilter_Reset(int Active)
	{
		auto& file = Browse::INIFiles[Active];
		file.FilterSec.clear();
		for (size_t i = 0; i < file.SecHash.size(); i++)
		{
			file.FilterSec.push_back(file.SortedSec[i]);
		}
		Browse::INIFiles[Active].ResetPage = 0;
		WindowMustSync = true;
	}
	void ResetFilter(int Active, bool Change)
	{
		if (Change)ResetFilter_Reset(Active);
		ImGui::Text(u8"选择全部");
	}

	std::vector<char> CurChar;
	std::vector<std::array<char, MAX_STRING_LENGTH> > Strings[3];
	void SimpleString_CheckFirst(int Active,char ch)
	{
		auto& file = Browse::INIFiles[Active];
		file.FilterSec.clear();
		if (ch == 0)for (size_t i = 0; i < file.SecHash.size(); i++)file.FilterSec.push_back(file.SortedSec[i]);
		else for (size_t i = 0; i < file.SecHash.size(); i++)
		{
			if (file.SecName[file.SortedSec[i]][0] == ch || file.SecName[file.SortedSec[i]][0] == (ch ^ 32))
			{
				file.FilterSec.push_back(file.SortedSec[i]);
			}
			Browse::INIFiles[Active].ResetPage = 0;
			WindowMustSync = true;
		}
	}
	void SimpleString_CheckFull(int Active, const std::array<char, MAX_STRING_LENGTH>& arr)
	{
		auto& file = Browse::INIFiles[Active];
		file.FilterSec.clear();
		if (!strcmp(arr.data(), ""))for (size_t i = 0; i < file.SecHash.size(); i++)file.FilterSec.push_back(file.SortedSec[i]);
		else for (size_t i = 0; i < file.SecHash.size(); i++)
			if (file.SecName[file.SortedSec[i]] == arr.data())file.FilterSec.push_back(file.SortedSec[i]);
		Browse::INIFiles[Active].ResetPage = 0;
		WindowMustSync = true;
	}
	void SimpleString_CheckIncludes(int Active, const std::array<char, MAX_STRING_LENGTH>& arr)
	{
		auto& file = Browse::INIFiles[Active];
		file.FilterSec.clear();
		if (!strcmp(arr.data(), ""))for (size_t i = 0; i < file.SecHash.size(); i++)file.FilterSec.push_back(file.SortedSec[i]);
		else for (size_t i = 0; i < file.SecHash.size(); i++)
			if (strstr(file.SecName[file.SortedSec[i]].c_str(), arr.data()) != NULL)file.FilterSec.push_back(file.SortedSec[i]);
		Browse::INIFiles[Active].ResetPage = 0;
		WindowMustSync = true;
	}
	void SimpleString_CheckIncluded(int Active, const std::array<char, MAX_STRING_LENGTH>& arr)
	{
		auto& file = Browse::INIFiles[Active];
		file.FilterSec.clear();
		if (!strcmp(arr.data(), ""))for (size_t i = 0; i < file.SecHash.size(); i++)file.FilterSec.push_back(file.SortedSec[i]);
		else for (size_t i = 0; i < file.SecHash.size(); i++)
			if (strstr(arr.data(), file.SecName[file.SortedSec[i]].c_str()) != NULL)file.FilterSec.push_back(file.SortedSec[i]);
		Browse::INIFiles[Active].ResetPage = 0;
		WindowMustSync = true;
	}
	void SimpleString(int Active, bool Change)
	{
		int& choice = Browse::INIFiles[Active].CurrentSubFilter[1];
		ImGui::Text(u8"筛选符合一定条件的字段名");
		if (ImGui::RadioButton(u8"首字母匹配", choice == 1)) { if (choice != 1)Change = true; choice = 1; }
		ImGui::SameLine();
		ImGui::SetCursorPosX(FontHeight * 6.5f);
		if (ImGui::RadioButton(u8"全串匹配", choice == 2)) { if (choice != 2)Change = true; choice = 2; }

		if (ImGui::RadioButton(u8"字段包含了", choice == 3)) { if (choice != 3)Change = true; choice = 3; }
		ImGui::SameLine();
		ImGui::SetCursorPosX(FontHeight * 6.5f);
		if (ImGui::RadioButton(u8"包含了字段", choice == 4)) { if (choice != 4)Change = true; choice = 4; }

		switch (choice)
		{
		case 1:
		{
			char Curs[] = u8"\0##CHAR";
			if (CurChar[Active] != 0 && Change)
			{
				SimpleString_CheckFirst(Active, CurChar[Active]);
			}
			for (char i = 'A'; i <= 'Z'; i++)
			{
				Curs[0] = i;
				if (ImGui::Button(Curs, { 1.0f * FontHeight,1.0f * FontHeight }))
				{
					CurChar[Active] = i;
					SimpleString_CheckFirst(Active, CurChar[Active]);
				}
				if (i != 'J' && i != 'T')ImGui::SameLine();
			}
			ImGui::Text(u8"当前选择：%c", (CurChar[Active]) ? CurChar[Active] : ' ');
		}break;
		case 2:
		{
			if (Change)SimpleString_CheckFull(Active, Strings[0][Active]);
			if (ImGui::InputText(u8"匹配文本", Strings[0][Active].data(), MAX_STRING_LENGTH))
			{
				SimpleString_CheckFull(Active, Strings[0][Active]);
			}
			//ImGui::Text("size=%d str=\"%s\"", strlen(Strings[0][Active].data()), Strings[0][Active].data());
		}break;
		case 3:
		{
			if (Change)SimpleString_CheckIncludes(Active, Strings[1][Active]);
			if (ImGui::InputText(u8"匹配文本", Strings[1][Active].data(), MAX_STRING_LENGTH))
			{
				SimpleString_CheckIncludes(Active, Strings[1][Active]);
			}
		}break;
		case 4:
		{
			if (Change)SimpleString_CheckIncluded(Active, Strings[2][Active]);
			if (ImGui::InputText(u8"匹配文本", Strings[2][Active].data(), MAX_STRING_LENGTH))
			{
				SimpleString_CheckIncluded(Active, Strings[2][Active]);
			}
		}break;
		default:break;
		}
	}

	std::vector<std::array<int, 2> >SliceInput[2];
	void SortSlice_ByPage(int Active, int l, int r)
	{
		auto& file = Browse::INIFiles[Active];
		file.FilterSec.clear();
		for (int i = (l - 1) * KeyPerPage; i < std::min((int)file.SecHash.size(),r * KeyPerPage); i++)
		{
			file.FilterSec.push_back(file.SortedSec[i]);
		}
		file.ResetPage = 0;
		WindowMustSync = true;
	}
	void SortSlice_BySection(int Active,int l,int r)
	{
		auto& file = Browse::INIFiles[Active];
		file.FilterSec.clear();
		for (int i = l; i <= r; i++)
		{
			file.FilterSec.push_back(file.SortedSec[i]);
		}
		file.ResetPage = 0;
		WindowMustSync = true;
	}
	void SortSlice(int Active, bool Change)
	{
		auto& file = Browse::INIFiles[Active];
		int& choice = file.CurrentSubFilter[2];
		ImGui::Text(u8"筛选目前顺序下全INI指定区间的字段");
		if (ImGui::RadioButton(u8"页码区间", choice == 1)) { if (choice != 1)Change = true; choice = 1; }
		ImGui::SameLine();
		ImGui::SetCursorPosX(FontHeight * 6.5f);
		if (ImGui::RadioButton(u8"序号区间", choice == 2)) { if (choice != 2)Change = true; choice = 2; }

		switch (choice)
		{
		case 1:
		{
			int sz = file.SecHash.size();
			int PageN = GetPage(sz);
			if (Change)SortSlice_ByPage(Active, std::max(1,SliceInput[0][Active][0]), std::min(PageN,SliceInput[0][Active][1]));
			if(ImGui::InputInt2(u8"输入区间", SliceInput[0][Active].data()))
			{
				if (SliceInput[0][Active][0] < 1)SliceInput[0][Active][0] = 1;
				if (SliceInput[0][Active][1] > PageN)SliceInput[0][Active][1] = PageN;
				SortSlice_ByPage(Active, std::max(1, SliceInput[0][Active][0]), std::min(PageN, SliceInput[0][Active][1]));
			}
		}break;
		case 2:
		{
			int sz = file.SecHash.size();
			if (Change)SortSlice_BySection(Active, std::max(1, SliceInput[1][Active][0]), std::min(sz, SliceInput[1][Active][1]));
			if (ImGui::InputInt2(u8"输入区间", SliceInput[1][Active].data()))
			{
				if (SliceInput[1][Active][0] < 1)SliceInput[1][Active][0] = 1;
				if (SliceInput[1][Active][1] > sz)SliceInput[1][Active][1] = sz;
				SortSlice_BySection(Active, std::max(1, SliceInput[1][Active][0]), std::min(sz, SliceInput[1][Active][1]));
			}
			
		}break;
		default:break;
		}
	}

	void RegexMatch_CheckFull(int Active, const std::array<char, MAX_STRING_LENGTH>& arr)
	{
		auto& file = Browse::INIFiles[Active];
		file.FilterSec.clear();
		if (!strcmp(arr.data(), ""))for (size_t i = 0; i < file.SecHash.size(); i++)file.FilterSec.push_back(file.SortedSec[i]);
		else for (size_t i = 0; i < file.SecHash.size(); i++)
		{
			try
			{
				if (std::regex_match(file.SecName[file.SortedSec[i]], std::regex(arr.data())))file.FilterSec.push_back(file.SortedSec[i]);
			}
			catch (std::exception& e){ (void)e;}
		}
		Browse::INIFiles[Active].ResetPage = 0;
		WindowMustSync = true;
	}
	void RegexMatch_CheckAny(int Active, const std::array<char, MAX_STRING_LENGTH>& arr)
	{
		auto& file = Browse::INIFiles[Active];
		file.FilterSec.clear();
		if (!strcmp(arr.data(), ""))for (size_t i = 0; i < file.SecHash.size(); i++)file.FilterSec.push_back(file.SortedSec[i]);
		else for (size_t i = 0; i < file.SecHash.size(); i++)
		{
			try
			{
				if (std::regex_search(file.SecName[file.SortedSec[i]], std::regex(arr.data())))file.FilterSec.push_back(file.SortedSec[i]);
			}
			catch (std::exception& e) { (void)e; }
		}
		Browse::INIFiles[Active].ResetPage = 0;
		WindowMustSync = true;
	}
	void RegexMatch_CheckAnyNot(int Active, const std::array<char, MAX_STRING_LENGTH>& arr)
	{
		auto& file = Browse::INIFiles[Active];
		file.FilterSec.clear();
		if (!strcmp(arr.data(), ""))for (size_t i = 0; i < file.SecHash.size(); i++)file.FilterSec.push_back(file.SortedSec[i]);
		else for (size_t i = 0; i < file.SecHash.size(); i++)
		{
			try
			{
				if (!std::regex_match(file.SecName[file.SortedSec[i]], std::regex(arr.data())))file.FilterSec.push_back(file.SortedSec[i]);
			}
			catch (std::exception& e) { (void)e; }
		}
		Browse::INIFiles[Active].ResetPage = 0;
		WindowMustSync = true;
	}
	void RegexMatch_CheckNone(int Active, const std::array<char, MAX_STRING_LENGTH>& arr)
	{
		auto& file = Browse::INIFiles[Active];
		file.FilterSec.clear();
		if (!strcmp(arr.data(), ""))for (size_t i = 0; i < file.SecHash.size(); i++)file.FilterSec.push_back(file.SortedSec[i]);
		else for (size_t i = 0; i < file.SecHash.size(); i++)
		{
			try
			{
				if (!std::regex_search(file.SecName[file.SortedSec[i]], std::regex(arr.data())))file.FilterSec.push_back(file.SortedSec[i]);
			}
			catch (std::exception& e) { (void)e; }
		}
		Browse::INIFiles[Active].ResetPage = 0;
		WindowMustSync = true;
	}

	std::vector<std::array<char, MAX_STRING_LENGTH> > RegexStr[4];
	void RegexMatch(int Active, bool Change)
	{
		int& choice = Browse::INIFiles[Active].CurrentSubFilter[3];
		ImGui::Text(u8"筛选符合正则表达式的字段名");
		if (ImGui::RadioButton(u8"完全匹配", choice == 1)) { if (choice != 1)Change = true; choice = 1; }
		ImGui::SameLine();
		ImGui::SetCursorPosX(FontHeight * 6.5f);
		if (ImGui::RadioButton(u8"部分匹配", choice == 2)) { if (choice != 2)Change = true; choice = 2; }

		if (ImGui::RadioButton(u8"部分不匹配", choice == 3)) { if (choice != 3)Change = true; choice = 3; }
		ImGui::SameLine();
		ImGui::SetCursorPosX(FontHeight * 6.5f);
		if (ImGui::RadioButton(u8"完全不匹配", choice == 4)) { if (choice != 3)Change = true; choice = 4; }

		switch (choice)
		{
		case 1:
		{
			if (Change)RegexMatch_CheckFull(Active, RegexStr[0][Active]);
			if (ImGui::InputText(u8"正则表达式", RegexStr[0][Active].data(), MAX_STRING_LENGTH))
			{
				RegexMatch_CheckFull(Active, RegexStr[0][Active]);
			}
		}break;
		case 2:
		{
			if (Change)RegexMatch_CheckAny(Active, RegexStr[1][Active]);
			if (ImGui::InputText(u8"正则表达式", RegexStr[1][Active].data(), MAX_STRING_LENGTH))
			{
				RegexMatch_CheckAny(Active, RegexStr[1][Active]);
			}
		}break;
		case 3:
		{
			if (Change)RegexMatch_CheckAnyNot(Active, RegexStr[2][Active]);
			if (ImGui::InputText(u8"正则表达式", RegexStr[2][Active].data(), MAX_STRING_LENGTH))
			{
				RegexMatch_CheckAnyNot(Active, RegexStr[2][Active]);
			}
		}break;
		case 4:
		{
			if (Change)RegexMatch_CheckNone(Active, RegexStr[3][Active]);
			if (ImGui::InputText(u8"正则表达式", RegexStr[3][Active].data(), MAX_STRING_LENGTH))
			{
				RegexMatch_CheckNone(Active, RegexStr[3][Active]);
			}
		}break;
		default:break;
		}
	}

	void TickedSec_Check(int Active)
	{
		auto& file = Browse::INIFiles[Active];
		file.FilterSec.clear();
		for (size_t i = 0; i < file.SecHash.size(); i++)
		{
			if(SelectMode && IsTicked[file.SortedSec[i]])file.FilterSec.push_back(file.SortedSec[i]);
		}
		Browse::INIFiles[Active].ResetPage = 0;
		WindowMustSync = true;
	}
	void TickedSec(int Active, bool Change)
	{
		if (Change)TickedSec_Check(Active);
		ImGui::Text(u8"选择全部已勾选字段");
	}

	void SilentRefilter(int Active)
	{
		auto& file = Browse::INIFiles[Active];
		WindowMustSync = true;
		switch (file.CurrentFilter)
		{
		case 0:
		{
			ResetFilter_Reset(Active);
		}break;
		case 1:
		{
			switch (file.CurrentSubFilter[file.CurrentFilter])
			{
			case 1:SimpleString_CheckFirst(Active, CurChar[Active]); break;
			case 2:SimpleString_CheckFull(Active, Strings[0][Active]); break;
			case 3:SimpleString_CheckIncludes(Active, Strings[1][Active]); break;
			case 4:SimpleString_CheckIncluded(Active, Strings[2][Active]); break;
			default:break;
			}
		}break;
		case 2:
		{
			int sz = file.SecHash.size();
			int PageN = GetPage(sz);
			switch (file.CurrentSubFilter[file.CurrentFilter])
			{
			case 1:SortSlice_ByPage(Active, std::max(1, SliceInput[0][Active][0]), std::min(PageN, SliceInput[0][Active][1])); break;
			case 2:SortSlice_BySection(Active, std::max(1, SliceInput[1][Active][0]), std::min(sz, SliceInput[1][Active][1])); break;
			default:break;
			}
		}break;
		case 3:
		{
			switch (file.CurrentSubFilter[file.CurrentFilter])
			{
			case 1:RegexMatch_CheckFull(Active, RegexStr[0][Active]); break;
			case 2:RegexMatch_CheckAny(Active, RegexStr[1][Active]); break;
			case 3:RegexMatch_CheckAnyNot(Active, RegexStr[2][Active]); break;
			case 4:RegexMatch_CheckNone(Active, RegexStr[3][Active]); break;
			default:break;
			}
		}break;
		case 4:
		{
			TickedSec_Check(Active);
		}break;
		default:break;
		}
		if (EnableLog)
		{
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog("筛选器成功自动刷新。");
		}
	}
}
//看前面
//const int FilterListLen = 4;
filter_t FilterList[] =
{
	Filter::ResetFilter,
	Filter::SimpleString,
	Filter::SortSlice,
	Filter::RegexMatch,
	Filter::TickedSec
};
const char* FilterName[] =
{
	u8"无",
	u8"字段名筛选",
	u8"当前字段序",
	u8"字段名正则",
	u8"已勾选字段"
};

namespace FilterInit
{
	void SimpleStringInit(int NFile)
	{
		Filter::CurChar.resize(NFile);
		std::fill(Filter::CurChar.begin(), Filter::CurChar.end(), 0);
		for (int i = 0; i < 3; i++)
		{
			Filter::Strings[i].resize(NFile);
		}
	}
	void SortSliceInit(int NFile)
	{
		Filter::SliceInput[0].resize(NFile);
		Filter::SliceInput[1].resize(NFile);
		std::fill(Filter::SliceInput[0].begin(), Filter::SliceInput[0].end(), std::array<int, 2>{ 1, 1 });
		std::fill(Filter::SliceInput[1].begin(), Filter::SliceInput[1].end(), std::array<int, 2>{ 1, 1 });
	}
	void RegexMatchInit(int NFile)
	{
		for (int i = 0; i < 4; i++)
		{
			Filter::RegexStr[i].resize(NFile);
		}
	}
}

























typedef bool (*sort_t)(int,int,int);
namespace Sort
{
	bool File(int INI, int LSec, int RSec)
	{
		return LSec < RSec;
	}
	bool FileRev(int INI, int LSec, int RSec)
	{
		return LSec > RSec;
	}
	bool Dict(int INI, int LSec, int RSec)
	{
		return Browse::INIFiles[INI].SecName[LSec] < Browse::INIFiles[INI].SecName[RSec];
	}
	bool DictRev(int INI, int LSec, int RSec)
	{
		return Browse::INIFiles[INI].SecName[LSec] > Browse::INIFiles[INI].SecName[RSec];
	}
	bool Len(int INI, int LSec, int RSec)
	{
		return Browse::INIFiles[INI].SecName[LSec].length() < Browse::INIFiles[INI].SecName[RSec].length();
	}
	bool LenRev(int INI, int LSec, int RSec)
	{
		return Browse::INIFiles[INI].SecName[LSec].length() > Browse::INIFiles[INI].SecName[RSec].length();
	}
}
const int SortListLen = 6;
sort_t SortList[] =
{
	Sort::File,Sort::FileRev,
	Sort::Dict,Sort::DictRev,
	Sort::Len ,Sort::LenRev
};
const char* SortName[] =
{
	u8"文件序",u8"文件逆序",
	u8"字典序",u8"字典逆序",
	u8"字段长度序",u8"字段长度逆序",
};
namespace Sort
{
	void SilentResort()
	{
		std::sort(Browse::INIFiles[ActiveINI].SortedSec.begin(), Browse::INIFiles[ActiveINI].SortedSec.end(),
			[](int L, int R) { return SortList[Browse::INIFiles[ActiveINI].CurrentSort](ActiveINI, L, R); });
		WindowMustSync = true;
		if (EnableLog)
		{
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog("排序器成功自动刷新。");
		}
	}
}











namespace InsertLoad
{
	struct LoadINIGraph
	{
		std::vector<std::string> SecName;
		std::vector<hash_t> SecHash;
		std::vector<int> SecOrder; //SecHash的存储序索引

		std::string IniName;
		hash_t IniHash;
		uint64_t TimeStamp;

		std::map<int, int> Selection;
		int SelMaxID;
		bool Initial{ true };

		std::string Path;
		int PathSplit;
	};
	LoadINIGraph INIToLoad;

	void NewSection(int NFile, const char* Name)//仅增加空键
	{
		auto& file = Browse::INIFiles[NFile];
		file.SecHash.push_back(StrHash(Name));
		file.SecName.push_back(Name);
		file.NameSec[Name] = file.SecName.size() - 1;
		file.SectionSlice.push_back({ TE_FileID{},false,NFile,(int)file.SecHash.size() - 1,0 });
		TE_Initialize(&file.SectionSlice.back().MainWindow);
		int TOrder = file.SecOrder.back() + 1;
		file.SecOrder.push_back(TOrder);
		file.SortedSec.push_back(file.SecHash.size() - 1);
		file.FilterSec.push_back(file.SecHash.size() - 1);
		WindowMustSync = true;

	}
	void RewriteData(int NFile)
	{
		auto& file = Browse::INIFiles[NFile];
		ExtFileClass DatFile;
		static BufString RBuf;

		sprintf(RBuf, "%s\\__ProjectData\\%08X.dat",
			Pathbuf.c_str(), file.IniHash);
		DatFile.Open(RBuf, "wb");
		int ToWrite = file.SecHash.size();
		DatFile.Write(&ToWrite, sizeof(int), 1);
		DatFile.Write(file.SecHash.data(), sizeof(hash_t), ToWrite);
		DatFile.Write(file.SecOrder.data(), sizeof(int), ToWrite);
		for (int i = 0; i < ToWrite; i++)
		{
			int ls = file.SecName[i].length();
			DatFile.Write(&ls, sizeof(int), 1);
			DatFile.Write(file.SecName[i].c_str(), 1, ls);
		}
		DatFile.Close();
		if (EnableLog)
		{
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog("向磁盘保存字段表。");
		}
	}
	void CreateFileForSec(int INI, int Sec)//对有键无文件使用（目前仅新建）
	{
		static BufString NBuf;
		auto& file = Browse::INIFiles[INI];
		sprintf(NBuf, "%s\\__ProjectData\\%08X_%08X_%08X.ini", Pathbuf.c_str(),
			file.SecOrder[Sec], file.IniHash, file.SecHash[Sec]);
		ExtFileClass IniFile;
		IniFile.Open(NBuf, "w");
		IniFile.Ln();
		IniFile.PutStr(("[" + file.SecName[Sec] + "]").c_str()); IniFile.Ln();
		IniFile.PutStr(";INI浏览器 by std::iron_hammer"); IniFile.Ln();
		IniFile.PutStr(";请在这里输入所需的内容"); IniFile.Ln();
		IniFile.Close();
	}
	bool SelectFileName(HWND father, std::string& RetBuf, int& PathOffset, const std::string& InitialPath)//true确定false取消
	{
		OPENFILENAMEA ofn;
		CHAR szFile[296]{};
		CHAR szFileTitle[296] = "选择要导入的文件";
		memset(&ofn, 0, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = father;
		ofn.lpstrFilter = "导入数据(*_pack.dat)\0*_pack.dat\0所有文件 (*.*)\0*.*\0\0";
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFileTitle = szFileTitle;
		ofn.nMaxFileTitle = sizeof(szFileTitle);
		ofn.lpstrInitialDir = InitialPath.c_str();
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;
		bool ret = (GetOpenFileNameA(&ofn));
		if (ret)
		{
			if (szFile[0])
			{
				RetBuf = szFile;
				PathOffset = ofn.nFileOffset;
			}
		}
		else
		{
			RetBuf = "";
			PathOffset = 0;
		}
		if (EnableLog)
		{
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog("选择文件。");
		}
		return ret;
	}
	std::pair<std::string, int> GetSelectFile(const std::string& FromFile)
	{
		std::string Ret{};
		int RetI{};
		if (SelectFileName(MainWindowHandle, Ret, RetI, FromFile))return { Ret, RetI };
		else return { "",0 };
	}
	void GetLoadListOfDir(LoadINIGraph& ToLoad, const std::string& From, int SplitAt)
	{
		static BufString ReadBuf;
		ToLoad.SecHash.clear();
		ToLoad.SecOrder.clear();
		ToLoad.SecName.clear();
		ToLoad.Selection.clear();
		ToLoad.SelMaxID = 0;
		ToLoad.Path = From;
		ToLoad.PathSplit = SplitAt;

		ExtFileClass DatFile;
		DatFile.Open(From.c_str(), "rb");
		int NFile, SLen;
		DatFile.Read(&NFile, sizeof(int), 1);
		DatFile.Read(&ToLoad.TimeStamp, sizeof(uint64_t), 1);
		DatFile.Read(&ToLoad.IniHash, sizeof(hash_t), 1);
		DatFile.Read(&SLen, sizeof(int), 1);
		DatFile.Read(&ReadBuf, 1, SLen);
		ToLoad.IniName.assign(ReadBuf, SLen);

		ToLoad.SecHash.resize(NFile);
		ToLoad.SecName.resize(NFile);
		ToLoad.SecOrder.resize(NFile);

		for (int i = 0; i < NFile; i++)
		{
			DatFile.Read(ToLoad.SecOrder.data() + i, sizeof(int), 1);
			DatFile.Read(ToLoad.SecHash.data() + i, sizeof(hash_t), 1);
			DatFile.Read(&SLen, sizeof(int), 1);
			DatFile.Read(&ReadBuf, 1, SLen);
			ToLoad.SecName[i].assign(ReadBuf, SLen);
		}

		DatFile.Close();
		ToLoad.Initial = false;

		if (EnableLog)
		{
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog("成功载入导入列表。");
		}
	}

	std::vector<int> PickedSec;//从SecHash的排布序而非SortedSec避免排序错乱
	std::string WriteOutPath;
	std::atomic<bool> IsWorking{ false };
	std::atomic<bool> IsAdding{ false };
	std::atomic<bool> IsDeleting{ false };
	std::atomic<bool> IsWorkingComplete{ false };
	std::atomic<int> WorkingCond;
	//警告：当前目录会动
	void thrWriteOut()
	{
		static BufString NBuf, NBuf2;
		if (EnableLog)
		{
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog("导入/导出线程启动成功。");
		}
		while (1)
		{
			while (!IsWorking.load())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
			WorkingCond.store(0);

			if (IsAdding.load())
			{
				if (EnableLog)
				{
					GlobalLog.AddLog_CurTime(false);
					GlobalLog.AddLog("导入/导出线程开始添加新字段。");
				}
				auto& file = Browse::INIFiles[ActiveINI];
				WorkingCond.store(Browse::ProgressComplete);
				WriteOutPath.assign(INIToLoad.Path.c_str(), INIToLoad.PathSplit);
				if (WriteOutPath.back() == '\\')WriteOutPath.pop_back();
				//::MessageBoxA(nullptr, WriteOutPath.c_str(), "BEEEEEE!!!!!!", MB_OK);
				int PrevSize = file.SecHash.size();
				int Loop = 0;
				for (auto it : INIToLoad.Selection)
				{
					NewSection(ActiveINI, INIToLoad.SecName[it.second].c_str());//新建空键
					sprintf(NBuf2, "%s\\__ProjectData\\%08X_%08X_%08X.ini",
						Pathbuf.c_str(), file.SecOrder.back(), file.IniHash, file.SecHash.back());
					sprintf(NBuf, "%s\\%016llX_%08X_%08X_%08X.ini",
						WriteOutPath.c_str(), INIToLoad.TimeStamp, INIToLoad.SecOrder[it.second], INIToLoad.IniHash, INIToLoad.SecHash[it.second]);
					//::MessageBoxA(nullptr, NBuf, "FROM!!!!!!", MB_OK);
					//::MessageBoxA(nullptr, NBuf2, "TO!!!!!!", MB_OK);
					CopyFileA(NBuf, NBuf2, false);
					++Loop;
					WorkingCond.store(Browse::ProgressComplete * Loop / INIToLoad.Selection.size());
				}
				RewriteData(ActiveINI);//写入dat
				Sort::SilentResort();//重新排序
				Filter::SilentRefilter(ActiveINI);//重新筛选

				INIToLoad.Selection.clear();
				file.ResetPage = GetPage(PrevSize) - 1;//页码拨到原结尾
				WorkingCond.store(Browse::ProgressComplete);
				std::this_thread::sleep_for(std::chrono::milliseconds(800));

				if (EnableLog)
				{
					GlobalLog.AddLog_CurTime(false);
					GlobalLog.AddLog("导入/导出线程完成添加新字段。");
				}
				WindowMustSync = true;
				IsFileChanging = true;

				IsWorking.store(false);
				IsAdding.store(false);
				IsWorkingComplete.store(true);

			}
			else
			{
				if (EnableLog)GlobalLog.AddLog_CurTime(false);
				if (IsDeleting.load())
				{
					WriteOutPath = Pathbuf + "\\__Deleted";
					if (EnableLog)GlobalLog.AddLog("导入/导出线程开始删除新字段。");
				}
				else
				{
					SelectFilePath(MainWindowHandle, WriteOutPath, Defaultpath);
					if (EnableLog)GlobalLog.AddLog("导入/导出线程开始导出新字段。");
				}

				if (_access(WriteOutPath.c_str(), 0) != 0)
				{
					assert(_mkdir(WriteOutPath.c_str()) != ("不存在，且不能创建目录", 0));
				}

				int ToWrite;
				auto& file = Browse::INIFiles[ActiveINI];
				std::vector<char> ToReserve;
				ToReserve.resize(file.SecHash.size() + 10);
				std::fill(ToReserve.begin(), ToReserve.end(), 1);
				for (int Sec : PickedSec)ToReserve[Sec] = 0;
				uint64_t WriteTimeStamp = GetSysTimeMicros();
				WorkingCond.store(0);

				for (size_t i = 0; i < file.SecHash.size(); i++)if (!ToReserve[i])CloseFile(file.SectionSlice[i], true);

				ExtFileClass DatFile;
				sprintf(NBuf, "%s\\%016llX_pack.dat",
					WriteOutPath.c_str(), WriteTimeStamp);
				DatFile.Open(NBuf, "wb");
				ToWrite = PickedSec.size();
				DatFile.Write(&ToWrite, sizeof(int), 1);
				DatFile.Write(&WriteTimeStamp, sizeof(uint64_t), 1);
				DatFile.Write(&file.IniHash, sizeof(hash_t), 1);
				ToWrite = file.IniName.length();
				DatFile.Write(&ToWrite, sizeof(int), 1);
				DatFile.Write(file.IniName.c_str(), 1, ToWrite);
				int CmLoop = 0;

				for (int Sec : PickedSec)
				{
					DatFile.Write(&Sec, sizeof(int), 1);
					DatFile.Write(&file.SecHash[Sec], sizeof(int), 1);
					ToWrite = file.SecName[Sec].length();
					DatFile.Write(&ToWrite, sizeof(int), 1);
					DatFile.Write(file.SecName[Sec].c_str(), 1, ToWrite);

					sprintf(NBuf, "%s\\__ProjectData\\%08X_%08X_%08X.ini",
						Pathbuf.c_str(), file.SecOrder[Sec], file.IniHash, file.SecHash[Sec]);
					sprintf(NBuf2, "%s\\%016llX_%08X_%08X_%08X.ini",
						WriteOutPath.c_str(), WriteTimeStamp, Sec, file.IniHash, file.SecHash[Sec]);
					CopyFileA(NBuf, NBuf2, false);

					++CmLoop;
					WorkingCond.store(Browse::ProgressComplete * CmLoop / PickedSec.size());
				}
				DatFile.Close();

				if (IsDeleting.load())
				{
					WorkingCond.store(Browse::ProgressComplete - 10);//天杀的99% 

					std::vector<hash_t> TSecHash;
					std::vector<std::string> TSecName;
					std::vector<Browse::INISliceInfo> TSectionSlice;
					std::vector<int> TSecOrder;

					for (size_t i = 0; i < file.SecHash.size(); i++)
					{
						if (ToReserve[i])
						{
							TSecHash.push_back(file.SecHash[i]);
							TSecName.push_back(file.SecName[i]);
							TSecOrder.push_back(file.SecOrder[i]);
							auto& sc = file.SectionSlice[i];
							TSectionSlice.push_back({ sc.MainWindow,sc.IsOpen,sc.INI,(int)TSecHash.size() - 1,sc.ID });
						}
						else
						{
							if (Browse::IsINISliceOpen(&file.SectionSlice[i]))Browse::CloseFile(file.SectionSlice[i], false);
							sprintf(NBuf2, "%s\\__ProjectData\\%08X_%08X_%08X.ini",
								Pathbuf.c_str(), file.SecOrder[i], file.IniHash, file.SecHash[i]);
							if (_access(NBuf2, 0) == 0)remove(NBuf2);
							//Physically Remove
						}
					}

					file.SecHash = TSecHash;
					file.SecName = TSecName;
					file.SectionSlice = TSectionSlice;
					file.SecOrder = TSecOrder;
					file.FilterSec.resize(file.SecHash.size());
					file.SortedSec.resize(file.SecHash.size());
					for (size_t i = 0; i < file.SortedSec.size(); i++) { file.SortedSec[i] = i; file.FilterSec[i] = i; }
					{
						decltype(file.NameSec) EMap;
						file.NameSec.swap(EMap);
						for (size_t i = 0; i < TSecName.size(); i++)
						{
							file.NameSec[TSecName[i]] = i;
						}
					}

					RewriteData(ActiveINI);
					Sort::SilentResort();
					Filter::SilentRefilter(ActiveINI);
					Browse::INIFiles[ActiveINI].ResetPage = 0;
				}

				std::fill(IsTicked.begin(), IsTicked.end(), 0);

				WorkingCond.store(Browse::ProgressComplete);
				std::this_thread::sleep_for(std::chrono::milliseconds(800));

				if (EnableLog)
				{
					GlobalLog.AddLog_CurTime(false);
					if (IsDeleting.load())GlobalLog.AddLog("导入/导出线程完成删除新字段。");
					else GlobalLog.AddLog("导入/导出线程完成导出新字段。");
				}

				WindowMustSync = true;
				IsFileChanging = true;
				IsWorkingComplete.store(true);
				IsWorking.store(false);
				IsDeleting.store(false);

			}
		}
		if (EnableLog)
		{
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog("导入/导出线程结束运行。");
		}
	}
}






























namespace Analyze
{
	std::vector<std::pair<std::pair<std::string, std::string>, bool> >FileStack;
	
	std::vector<WCHAR> GiveBuf, PrevGiveBuf;
	std::string CopyBuf,MainCopyBuf,PrevMBuf;
	LPWSTR CALLBACK GiveBuffer(int64_t Len)
	{
		if(GiveBuf.size()< Len * 2)GiveBuf.resize((size_t)Len + 20000);
		GiveBuf[(size_t)Len] = (WCHAR)0;
		return GiveBuf.data();
	}
	std::atomic<TE_FileID> CopyFileStruct;
	std::atomic<ETimer::timer_t> CheckClock;
	std::mutex WriteLock;//int GetINITextDelay = 50, GetINITimeOut = 1000;
	void thrGetINITextLoop()
	{
		if (EnableLog)
		{
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog("抓取文本线程启动成功。");
		}
		ETimer::TimerClass timer;
		TE_FileID InternalID;
		GiveBuf.resize(100); PrevGiveBuf.resize(100);
		auto WaitTime = std::chrono::milliseconds(WaitTimeMsec);
		while (1)
		{
			while (!Browse::CurWindow.valid)
				std::this_thread::sleep_for(WaitTime);
			CheckClock.store(ETimer::ClockNow());
			timer.Set();
			PrevGiveBuf = GiveBuf;
			InternalID = CopyFileStruct.load();
			if(TE_IsFileOpen(InternalID))
			if (TE_GetWText(InternalID, GiveBuffer, (int64_t)GetINITimeOut) < (int64_t)0)
			{
				GiveBuf = PrevGiveBuf;
				//MessageBoxW(nullptr, L"TIME OUT!", L"BEEP", MB_OK);
			}
			WriteLock.lock();
			CopyBuf = UnicodetoUTF8(GiveBuf.data());
			WriteLock.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(GetINITextDelay - timer.GetMilli()));
		}
		if (EnableLog)
		{
			GlobalLog.AddLog_CurTime(false);
			GlobalLog.AddLog("抓取文本线程结束运行。");
		}
	}

	ETimer::TimerClass MainTimer;
	bool INITimeOut;
	std::string& GetINISliceTextU8()
	{
		PrevMBuf = MainCopyBuf;
		MainTimer.Set(CheckClock.load());
		int Milli = (int)MainTimer.GetMilli();
		INITimeOut = (bool)(Milli > GetINITimeOut * 2);
		if (WriteLock.try_lock())
		{
			MainCopyBuf = CopyBuf;
			WriteLock.unlock();
		}
		IsFileChanging = (PrevMBuf != MainCopyBuf);
		return MainCopyBuf;
	}

	float XBase, YBase, XOffset;
	std::stack<bool> TreeList;

	std::unordered_map<std::string, std::vector<GlobalAnalyze::LinkResult> > LineHintMap;

	bool IsLineShow;

	typedef int (*cmddeal_t)(int, const std::vector<std::string>&);

	int Separator(int, const std::vector<std::string>&);
	int SameLine(int, const std::vector<std::string>&);
	int NewLine(int, const std::vector<std::string>&);
	int Jump(int, const std::vector<std::string>&);
	int LineJump(int, const std::vector<std::string>&);
	int LabelJump(int, const std::vector<std::string>&);
	int Print(int, const std::vector<std::string>&);
	int LPrint(int, const std::vector<std::string>&);
	int Label(int, const std::vector<std::string>&);
	int SetX(int, const std::vector<std::string>&);
	int SetY(int, const std::vector<std::string>&);
	int SetValue(int, const std::vector<std::string>&);
	int SetOffsetX(int, const std::vector<std::string>&);
	int PrintTo(int, const std::vector<std::string>&);
	int LPrintTo(int, const std::vector<std::string>&);
	int Run(int, const std::vector<std::string>&);
	int SetLink(int, const std::vector<std::string>&);
	int SubMenu(int, const std::vector<std::string>&);
	int SubEnd(int, const std::vector<std::string>&);
	int ShowLine(int, const std::vector<std::string>&);

	const std::unordered_map<std::string, cmddeal_t> CommandsToBeDealt
	{
		{u8"Jump",Jump},{u8"Separator",Separator},{u8"SameLine",SameLine},
		{u8"NewLine",NewLine},{u8"LineJump",LineJump},{u8"LabelJump",LabelJump},
		{u8"Label",Label},{u8"SetX",SetX},{u8"SetY",SetY},{u8"Print",Print},
		{u8"LPrint",LPrint},{u8"SetValue",SetValue},{u8"SetOffsetX",SetOffsetX},
		{u8"PrintTo",PrintTo},{u8"LPrintTo",LPrintTo},{u8"Run",Run},{u8"SetLink",SetLink},
		{u8"Sub",SubMenu},{u8"SubEnd",SubEnd},{u8"ShowLine",ShowLine}
	};//Ext : Begin End

	std::string CutSpace(const std::string& ss)
	{
		auto fp = ss.find_first_not_of(" \011"), bp = ss.find_last_not_of(" \011");
		return std::string(ss.begin() + (fp == ss.npos ? 0 : fp),
			ss.begin() + (bp == ss.npos ? 0 : bp + 1));
	}
	std::string CutComment(const std::string& ss)
	{
		auto fp = ss.find_first_of(';');
		return std::string(ss.begin(), ss.begin() + (fp == ss.npos ? ss.length() : fp));
	}
	std::vector<std::string> SplitLines(const std::string& Text)
	{
		size_t cur = 0, crl;
		std::vector<std::string> ret;
		while ((crl = Text.find_first_of('\n', cur)) != Text.npos)
		{
			ret.push_back(std::string(Text.begin() + cur, Text.begin() + crl - 1));
			cur = crl + 1;
		}
		ret.push_back(std::string(Text.begin() + cur, Text.end()));
		return ret;
	}
	std::vector<std::pair<std::pair<std::string, std::string>, bool> > 
	GetValidLines(const std::vector<std::string>& vec)
	{
		static char Fname[5000], Param[5000];
		std::vector<std::pair<std::pair<std::string, std::string>, bool> > ret;
		for (auto& str : vec)
		{
			std::string ts = CutSpace(str);
			if (ts == "")continue;
			if (ts[0] == ';' && ts[1] == '@')
			{
				memset(Fname, 0, sizeof(Fname));
				memset(Param, 0, sizeof(Param));
				sscanf(CutSpace(CutComment(std::string(ts.c_str() + 2))).c_str(), "%[^ (] (%[^)])%*s", Fname, Param);
				std::string FnN = CutSpace(Fname), Par = CutSpace(Param);
				if (FnN == "Begin")ret.clear();
				else if (FnN == "End")return ret;
				else if (CommandsToBeDealt.find(FnN) != CommandsToBeDealt.end())
				{
					ret.push_back({ {FnN,Par},true });
				}
				continue;
			}
			std::string cs = CutComment(ts);
			auto fp = cs.find_first_of('=');
			if (fp != cs.npos)
			{
				ret.push_back({ {CutSpace(std::string(cs.begin(),cs.begin() + fp)),
					CutSpace(std::string(cs.begin() + fp + 1,cs.end()))},false });
			}
		}
		return ret;
	}
	std::vector<std::string> SplitParam(const std::string& Text)
	{
		if (Text.empty())return {};
		size_t cur = 0, crl;
		std::vector<std::string> ret;
		while ((crl = Text.find_first_of(',', cur)) != Text.npos)
		{
			ret.push_back(std::string(Text.begin() + cur, Text.begin() + crl));
			cur = crl + 1;
		}
		ret.push_back(CutSpace(std::string(Text.begin() + cur, Text.end())));
		return ret;
	}


	std::unordered_map < std::string, std::string > Environ =
	{
		{"",""}
	};
	//Ext : @ @FILENAME @SECTION @LN=@\n @CO=@， @AT  R@~~="@~~" @RESX @RESY
	std::string DecryptString(const std::string& Key)
	{
		if (Key.empty())return "";
		if (GlobalAnalyze::GlobalDecryptStringHasThis(Key))return GlobalAnalyze::GlobalDecryptString(Key);
		if (Key.size() > 1 && Key[0] == 'R' && Key[1] == '@')
		{
			return std::string(Key.begin() + 1, Key.end());
		}
		if (Key[0] == '@')
		{
			std::string su(Key.begin() + 1, Key.end());
			if (Key.size() > 1)
			{
				if(Key[1] == '@')su = DecryptString(su);
			}
			if (su == "")return"";

			auto It = Environ.find(su);
			if (It != Environ.end())return It->second;

			for (auto& pp : FileStack)
			{
				if (pp.first.first == su)return pp.first.second;
			}

			return Environ[su];
		}
		return Key;
	}
	void EncryptString(const std::string& Key, const std::string& Val)
	{
		if (Key.empty())return;
		if (Key[0] == '@')
		{
			std::string su(Key.begin() + 1, Key.end());
			if (Key.size() > 1)
			{
				if (Key[1] == '@')su = DecryptString(su);
			}
			Environ[su] = DecryptString(Val);
		}
	}

	bool InternalJump(const std::string& INI, const std::string& Sec, const std::string& Hint, const std::string& Jump)
	{
		int rg = Browse::GetININum(INI);
		if (rg == -1)return false;
		int Info = Browse::GetSliceFromName(rg, Sec);
		if (Info == -1)return false;

		ImGui::Text(Hint.c_str());
		ImGui::SameLine();
		if (ImGui::SmallButton((DecryptString(Jump) + "##" + INI + Sec + Hint + Jump).c_str()))
		{
			if (EnableLog)
			{
				GlobalLog.AddLog_CurTime(false);
				BufString bff;
				sprintf(bff, "从%s[%s]跳转到%s[%s]",
					Browse::INIFiles[Browse::CurWindow.Info->INI].IniName.c_str(),
					Browse::INIFiles[Browse::CurWindow.Info->INI].SecName[Browse::CurWindow.Info->Sec].c_str(),
					INI.c_str(), Sec.c_str());
				GlobalLog.AddLog(bff);
			}
			if (Browse::IsINISliceOpen(rg, Info))Browse::SelectINISlice(rg, Info);
			else Browse::OpenINISlice(rg, Info);
		}
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + XBase + XOffset);
		return true;
	}

	int Separator(int Cursor, const std::vector<std::string>& Param)
	{
		if (!TreeList.empty() && !TreeList.top())return Cursor + 1;
		int T;
		if (Param.empty())T = 1;
		else
		{
			auto Par0 = DecryptString(Param.at(0));
			T = std::max(1, std::stoi(Par0));
		}
		while (T--)ImGui::Separator();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + XBase + XOffset);
		return Cursor + 1;
	}
	int NewLine(int Cursor, const std::vector<std::string>& Param)
	{
		if (!TreeList.empty() && !TreeList.top())return Cursor + 1;
		int T;
		if (Param.empty())T = 1;
		else 
		{
			auto Par0 = DecryptString(Param.at(0));
			T = std::max(1, std::stoi(Par0));
		}
		while (T--)ImGui::NewLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + XBase + XOffset);
		return Cursor + 1;
	}
	int SameLine(int Cursor, const std::vector<std::string>& Param)
	{
		if (!TreeList.empty() && !TreeList.top())return Cursor + 1;
		if (Param.empty())ImGui::SameLine();
		else
		{
			auto Par0 = DecryptString(Param.at(0));
			ImGui::SameLine(ImGui::GetCursorPosX() + XOffset + XBase + FontHeight * (float)std::stod(Par0));
		}
		return Cursor + 1;
	}
	int SetX(int Cursor, const std::vector<std::string>& Param)
	{
		if (!TreeList.empty() && !TreeList.top())return Cursor + 1;
		if (Param.empty())ImGui::SetCursorPosX(XBase);
		else
		{
			auto Par0 = DecryptString(Param.at(0));
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + XOffset + XBase + FontHeight * (float)std::stod(Par0));
		}
		return Cursor + 1;
	}
	int SetY(int Cursor, const std::vector<std::string>& Param)
	{
		if (!TreeList.empty() && !TreeList.top())return Cursor + 1;
		if (Param.empty())ImGui::SetCursorPosY(YBase);
		else
		{
			auto Par0 = DecryptString(Param.at(0));
			ImGui::SetCursorPosY(YBase + FontHeight * (float)std::stod(Par0));
		}
		return Cursor + 1;
	}
	int LineJump(int Cursor, const std::vector<std::string>& Param)
	{
		int T;
		if (Param.empty())T = 0;
		else
		{
			auto Par0 = DecryptString(Param.at(0));
			T = std::stoi(Par0);
		}
		return Cursor + 1 + T;
	}
	int Label(int Cursor, const std::vector<std::string>& Param)
	{
		return Cursor + 1;
	}
	int LabelJump(int Cursor, const std::vector<std::string>& Param)
	{
		if (!Param.empty())
		{
			auto Par0 = DecryptString(Param.at(0));
			for (int i = 0; i < (int)FileStack.size(); i++)
			{
				if (FileStack[i].first.second == Par0)return i;
			}
		}
		return Cursor + 1;
	}
	int Print(int Cursor, const std::vector<std::string>& Param)
	{
		if (!TreeList.empty() && !TreeList.top())return Cursor + 1;
		for(auto& st : Param)
		{
			ImGui::TextWrapped(DecryptString(st).c_str());
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + XBase + XOffset);
		}
		return Cursor + 1;
	}
	int LPrint(int Cursor, const std::vector<std::string>& Param)
	{
		if (!TreeList.empty() && !TreeList.top())return Cursor + 1;
		{
			std::string sp;
			for (auto st : Param)sp += DecryptString(st);
			ImGui::TextWrapped(sp.c_str());
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + XBase + XOffset);
		}
		return Cursor + 1;
	}
	int SetValue(int Cursor, const std::vector<std::string>& Param)
	{
		if (Param.size() < 2)return Cursor + 1;
		EncryptString(Param.at(0), Param.at(1));
		return Cursor + 1;
	}
	int SetOffsetX(int Cursor, const std::vector<std::string>& Param)
	{
		if (!TreeList.empty() && !TreeList.top())return Cursor + 1;
		auto XOffsetPrev = XOffset;
		if (Param.empty())XOffset = 0.0f;
		else
		{
			auto Par0 = DecryptString(Param.at(0));
			XOffset = FontHeight * (float)std::stod(Par0);
		}
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - XOffsetPrev + XOffset);
		return Cursor + 1;
	}
	int PrintTo(int Cursor, const std::vector<std::string>& Param)
	{
		if (Param.empty())return Cursor + 1;
		std::string sp;
		for (size_t i = 1; i < Param.size(); i++) { sp += DecryptString(Param.at(i));sp.push_back('\n'); }
		EncryptString(Param.at(0), sp);
		return Cursor + 1;
	}
	int LPrintTo(int Cursor, const std::vector<std::string>& Param)
	{
		if (Param.empty())return Cursor + 1;
		std::string sp;
		for (size_t i = 1; i < Param.size(); i++) { sp += DecryptString(Param.at(i)); }
		EncryptString(Param.at(0), sp);
		return Cursor + 1;
	}
	int Run(int Cursor, const std::vector<std::string>& Param)
	{
		if(Param.empty())return Cursor + 1;
		std::vector<std::string> TParam;
		for (const auto& st : Param)
		{
			for (const auto& ss : SplitParam(DecryptString(st)))
			{
				TParam.push_back(ss);
			}
		}
		if(TParam.empty())return Cursor + 1;
		auto FnS = DecryptString(TParam.at(0));
		auto FnIter = CommandsToBeDealt.find(FnS);
		TParam.erase(TParam.begin());
		if (FnIter != CommandsToBeDealt.end())
		{
			std::string tot;
			for (const auto& ss : TParam)
			{
				tot += ('\"' + ss + '\"');
			}
			ImGui::TextWrapped(u8"Run() 解析内容： Fn=\"%s\",Param={%s}", FnS.c_str(), tot.c_str());
			ImGui::SetCursorPosX(InitOfsX + TreeOfsX * TreeList.size() + XBase + XOffset);
			return FnIter->second(Cursor, TParam);
		}
		else
		{
			std::string tot;
			for (const auto& ss : TParam)
			{
				tot += ('\"' + ss + '\"');
			}
			ImGui::TextWrapped(u8"Run() 解析错误： Fn=\"%s\",Param={%s}", FnS.c_str(), tot.c_str());
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + XBase + XOffset);
			return Cursor + 1;
		}
		
	}
	int ShowLine(int Cursor, const std::vector<std::string>& Param)
	{
		if (Param.empty())return Cursor + 1;
		std::string sr = DecryptString(Param.at(0));
		if (sr == "#ON")IsLineShow = true;
		else if (sr == "#OFF")IsLineShow = false;
		return Cursor + 1;
	}
	int Jump(int Cursor, const std::vector<std::string>& Param)// INI SEC HINT JUMP
	{
		if (!TreeList.empty() && !TreeList.top())return Cursor + 1;
		if (Param.size() < 4)return Cursor + 1;
		InternalJump(DecryptString(Param.at(0)), DecryptString(Param.at(1)), DecryptString(Param.at(2)), DecryptString(Param.at(3)));
		return Cursor + 1;
	}
	int SetLink(int Cursor, const std::vector<std::string>& Param)
	{
		if (Param.size() < 2)return Cursor + 1;
		auto it = GlobalAnalyze::LinkName.find(DecryptString(Param.at(0)));
		if (it == GlobalAnalyze::LinkName.end())return Cursor + 1;
		std::string sr = DecryptString(Param.at(1));
		if (sr == "#ON")GlobalAnalyze::Links[it->second].On = true;
		else if (sr == "#OFF")GlobalAnalyze::Links[it->second].On = false;
		return Cursor + 1;
	}
	int SubMenu(int Cursor, const std::vector<std::string>& Param)
	{
		if (Param.size() < 1)return Cursor + 1;

		if (ImGui::TreeNode(DecryptString(Param.at(0)).c_str()))
		{
			TreeList.push(true);
		}
		else TreeList.push(false);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + XBase + XOffset);
		return Cursor + 1;
	}
	int SubEnd(int Cursor, const std::vector<std::string>& Param)
	{
		if (!TreeList.empty())
		{
			if (TreeList.top())
			{
				ImGui::TreePop();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + XBase + XOffset);
			}
			TreeList.pop();
			
		}
		return Cursor + 1;
	}

	void OperateJumpData(const std::string& Text, bool Sync)
	{
		XBase = 0.0f;// ImGui::GetCursorPosX();
		YBase = ImGui::GetCursorPosY();
		XOffset = 0.0f;
		if (IsFileChanging)FileStack = GetValidLines(SplitLines(Text));
		int Cursor = 0;
		static char Except[2500];
		IsLineShow = true;
		//std::vector<LinkResult>
		std::string ININameS, INIActiveS, CurrentS;
		ININameS=MBCStoUTF8(Browse::INIFiles[Browse::CurWindow.Info->INI].IniName);
		INIActiveS=MBCStoUTF8(Browse::INIFiles[ActiveINI].IniName);
		CurrentS=MBCStoUTF8(Browse::INIFiles[Browse::CurWindow.Info->INI].SecName[Browse::CurWindow.Info->Sec]);

		if (IsFileChanging)
		{
			decltype(LineHintMap) EMap;
			LineHintMap.swap(EMap);
		}

		int LastVisible = 0, ItemNowNum = 0;
		
		{
			std::stack<bool> emp;
			TreeList.swap(emp);
		}

		if(WindowConditionHint) ImGui::Text(u8"%s", (Sync ? ( INITimeOut ? u8"文本同步超时" : u8"保持同步中" ) : u8"已中断同步"));
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + XBase + XOffset);

		std::vector<std::string> VParam;

		while (Cursor >= 0 && Cursor < (int)FileStack.size())
		{
			auto& ss = FileStack[Cursor];
			if (ss.second)
			{
				try
				{
					Cursor = CommandsToBeDealt.find(ss.first.first)->second
						(Cursor, SplitParam(ss.first.second));//不在Map里面的根本不会进循环
				}
				catch (std::exception& e)
				{
					if (Sync && WindowExceptionHint)
					{
						MessageBoxA(nullptr, e.what(), "命令解析异常", MB_OK | MB_ICONINFORMATION);
						std::string tot;
						for (const auto& ss : SplitParam(ss.first.second))
						{
							tot += ('\"' + DecryptString(ss) + '\"');
						}
						sprintf(Except,
R"(异常内容："%s"
stoi，stod之类是由于传参故障引起，
检查传参类型之后点击确定即可。
函数名："%s"，入参："%s",
解析后的参数列表：{%s}，
如果无法解决，发送整个字段文件，
和匹配符号表一并提交作者，作者会尽力解答。
感谢您的配合。)"
, e.what(), ss.first.first.c_str(), ss.first.second.c_str(), tot.c_str());
						MessageBoxA(nullptr, Except, "异常说明", MB_OK|MB_ICONINFORMATION);
					}
					Cursor++;
				}
			}
			else
			{//std::unordered_map<std::string, std::vector<GlobalAnalyze::LinkResult> > LineHintMap;
				std::string Rstr = ss.first.first + "=" + ss.first.second;
				if(IsLineShow)
				{
					if (TreeList.empty() || TreeList.top())
					{
						//auto U = ImGui::GetCursorPosX();
						bool Hover = false;
						//ImGui::Text(u8"键值：\"%s\"=\"%s\"", ss.first.first.c_str(), ss.first.second.c_str());
						if (AnalysisShowINILine)
						{
							ImGui::Text(Rstr.c_str());
							if (ImGui::IsItemHovered())Hover = true;
							//	ImGui::SetTooltip("%s", ss.first.first.c_str());
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + XBase + XOffset);
						}
						if (ImGui::IsItemVisible())LastVisible = ItemNowNum;
						if(ImGui::IsItemVisible()|| LastVisible - ItemNowNum < 6)//应该6需要是大于下面额外三行的行数值即可
						{
							VParam = { "","",ININameS, INIActiveS, CurrentS,ss.first.first,ss.first.second };
							auto it = LineHintMap.find(Rstr);
							if (it == LineHintMap.end())LineHintMap[Rstr] = GlobalAnalyze::ProcessLink(VParam);
							for (const auto& jmpi : LineHintMap[Rstr])
							{
								if (jmpi.Button == "#HOVERTEXT")
								{
									if (Hover)
									{
										if(!jmpi.Hint.empty())ImGui::SetTooltip("%s", jmpi.Hint.c_str());
									}
								}
								else
								{
									InternalJump(jmpi.JumpINI, jmpi.JumpSec, jmpi.Hint, jmpi.Button);
								}
							}
						}
					}
				}
				Cursor++;
			}
			ItemNowNum++;
			//ImGui::Text("Cursor %d/%d", Cursor, FileStack.size());
		}

		while (!TreeList.empty())
		{
			if(TreeList.top())ImGui::TreePop();
			TreeList.pop();
		}
	}
}

























/*
		if (CreateProcess(
			NULL,   //  指向一个NULL结尾的、用来指定可执行模块的宽字节字符串
			ConDir, // 命令行字符串
			NULL, //    指向一个SECURITY_ATTRIBUTES结构体，这个结构体决定是否返回的句柄可以被子进程继承。
			NULL, //    如果lpProcessAttributes参数为空（NULL），那么句柄不能被继承。<同上>
			false,//    指示新进程是否从调用进程处继承了句柄。
			0,   //  指定附加的、用来控制优先类和进程的创建的标
				//  CREATE_NEW_CONSOLE  新控制台打开子进程
				//  CREATE_SUSPENDED    子进程创建后挂起，直到调用ResumeThread函数
			NULL, //    指向一个新进程的环境块。如果此参数为空，新进程使用调用进程的环境
			NULL,//    指定子进程的工作路径
			&si, // 决定新进程的主窗体如何显示的STARTUPINFO结构体
			&pi  // 接收新进程的识别信息的PROCESS_INFORMATION结构体
	*/

/*
///*

VIRTUAL ALT+F4

			//MessageBoxW(nullptr, L"DET!", L"BEEP", MB_OK);
			/*
			INPUT Quit[4];
			Quit[0].type = Quit[1].type = Quit[2].type = Quit[3].type = INPUT_KEYBOARD;
			Quit[2].ki.dwFlags = Quit[3].ki.dwFlags = KEYEVENTF_KEYUP;
			Quit[0].ki.wVk = Quit[3].ki.wVk = VK_LMENU;
			Quit[1].ki.wVk = Quit[2].ki.wVk = VK_F4;*/
			//::SendMessage(Slice.MainWindow, WM_CLOSE, 0, 0);
			/*
			INPUT input[4];
			memset(input, 0, sizeof(input));
			input[0].type = input[1].type = INPUT_KEYBOARD;
			input[2].ki.dwFlags = KEYEVENTF_KEYUP;
			input[0].ki.wVk = input[3].ki.wVk = VK_LMENU;
			input[0].ki.wScan = input[3].ki.wScan = MapVirtualKey(VK_LMENU, 0);

			input[2].type = input[3].type = INPUT_KEYBOARD;
			input[3].ki.dwFlags = KEYEVENTF_KEYUP;
			input[2].ki.wVk = input[1].ki.wVk = VK_F4;
			input[2].ki.wScan = input[1].ki.wScan = MapVirtualKey(VK_F4, 0);

			while (GetForegroundWindow() != Slice.MainWindow)
			{
				SetForegroundWindow(Slice.MainWindow);
				Sleep(0);
			}//*/
			//MessageBoxW(nullptr, L"DETECT!", L"BEEP", MB_OK);
			//SendInput(4, input, sizeof(INPUT));//*/
