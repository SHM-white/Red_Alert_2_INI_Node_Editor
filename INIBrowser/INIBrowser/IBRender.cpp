
#include "IBRender.h"
#include "IBFront.h"
#include "Global.h"
#include "FromEngine/RFBump.h"
#include "FromEngine/global_timer.h"


int HintStayTimeMillis = 3000;

namespace PreLink
{
    void CleanUp();
}
namespace ImGui
{
    ImVec2 GetLineEndPos();
    ImVec2 GetLineBeginPos();
    bool IsWindowClicked(ImGuiMouseButton Button);
}
extern const char* LinkGroup_IniName;
bool InRectangle(ImVec2 P, ImVec2 UL, ImVec2 DR)
{
    std::tie(UL.x, DR.x) = std::minmax(UL.x, DR.x);
    std::tie(UL.y, DR.y) = std::minmax(UL.y, DR.y);
    return UL.x <= P.x && UL.y <= P.y && DR.x >= P.x && DR.y >= P.y;
}
std::tuple<bool, ImVec2, ImVec2> RectangleCross(ImVec2 UL1, ImVec2 DR1, ImVec2 UL2, ImVec2 DR2)//TODO:Test
{
    std::tie(UL1.x, DR1.x) = std::minmax(UL1.x, DR1.x);
    std::tie(UL1.y, DR1.y) = std::minmax(UL1.y, DR1.y);
    std::tie(UL2.x, DR2.x) = std::minmax(UL2.x, DR2.x);
    std::tie(UL2.y, DR2.y) = std::minmax(UL2.y, DR2.y);
    ImVec2 NU{ std::max(UL1.x, UL2.x), std::max(UL1.y, UL2.y) }, ND{ std::min(DR1.x, DR2.x),std::min(DR1.y, DR2.y) };
    return { NU.x < ND.x&& NU.y < ND.y, NU, ND };
}



int IBG_ExitProcess()
{
    if (EnableLog)
    {
        GlobalLog.AddLog_CurTime(false);
        GlobalLog.AddLog("程序正在结束运行。");
    }

    IBF_Inst_Setting.SaveSetting(IBR_Inst_Setting.SettingName);

    IBR_DynamicData::Save();
    IBR_DynamicData::Close();
    IBB_IniLine_AllocationMap_Destroy();

    if (EnableLog)
    {
        GlobalLog.AddLog_CurTime(false);
        GlobalLog.AddLog("程序已经结束运行。");
    }

    return 0;
}

dImVec2 operator+(const dImVec2 a, const dImVec2 b) { return { a.x + b.x,a.y + b.y }; }
dImVec2 operator-(const dImVec2 a, const dImVec2 b) { return { a.x - b.x,a.y - b.y }; }
ImVec4 operator+(const ImVec4 a, const ImVec4 b) { return { a.x + b.x,a.y + b.y,a.z + b.z,a.w + b.w }; }
dImVec2 operator/(const dImVec2 a, const double b) { return { a.x / b,a.y / b }; }
dImVec2 operator*(const dImVec2 a, const double b) { return { a.x * b,a.y * b }; }

bool IBR_Setting::IsReadSettingComplete()
{
    return SettingLoadComplete.load();
}

bool IBR_Setting::IsSaveSettingComplete()
{
    return SettingSaveComplete.load();
}

void IBR_Setting::SetSettingName(const wchar_t* Name)
{
    SettingName = Name;
}

void IBR_Setting::CallReadSetting()
{
    SettingLoadComplete.store(false);
    IBRF_CoreBump.IBR_SendToF({ [=]() {IBF_Inst_Setting.ReadSetting(SettingName); } });
}
void IBR_Setting::CallSaveSetting()
{
    SettingSaveComplete.store(false);
    IBRF_CoreBump.IBR_SendToF({ [=]() {IBF_Inst_Setting.SaveSetting(SettingName); } });
}

std::vector<IBF_SettingType> IBR_Inst_Setting_UIList;


void IBR_Setting::RenderUI()
{
    static bool Called = false;
    static bool First = true;
    static std::string DescLong = "";
    static ETimer::TimerClass Timer;

    if (Timer.GetClock() <= ETimer::__DurationZero || First)
    {
        First = false;
        CallSaveSetting();
        Timer.Set(ETimer::SecondToDuration(5));
    }

    if (IBR_Inst_Setting_UIList.empty())
    {
        if (!Called)
        {
            IBRF_CoreBump.IBR_SendToF({ [this]()
            {
                IBF_Inst_Setting.UploadSettingBoard([](const std::vector<IBF_SettingType>& Vec)
                    {
                        IBR_Inst_Setting_UIList = Vec;
                    });
            }});
            Called = true;
        }
        ImGui::TextDisabled(u8"加载设置列表中……");
    }
    else
    {
        auto CRgMax = ImGui::GetWindowContentRegionMax(), CRgMin = ImGui::GetWindowContentRegionMin();

        ImGui::Text(u8"所有修改将在重启后生效");

        ImGui::BeginChild(113007, { CRgMax.x - CRgMin.x , CRgMax.y - CRgMin.y - FontHeight * 10.0f },
            false,ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding);
        bool Appear = false;
        for (auto& Li : IBR_Inst_Setting_UIList)
        {
            if (Li.Action())
            {
                DescLong = Li.DescLong;
                Appear = true;
            }
        }

        ImGui::NewLine();
        ImGui::NewLine();
        if (ImGui::Button(u8"打开Config.ini"))
            ::ShellExecuteA(nullptr, "open", ".\\Resources\\Config.ini", NULL, NULL, SW_SHOWNORMAL);
        if (ImGui::IsItemHovered())
        {
            DescLong = u8"一部分设置错误会导致无法启动程序\n不建议随意修改\n这部分设置放在Config.ini中\n改动将在重启后生效";
            Appear = true;
        }

        if (!Appear)DescLong = "";

        ImGui::EndChild();

        ImGui::BeginChildFrame(113003, { CRgMax.x - CRgMin.x , FontHeight * 6.0f });
        ImGui::TextWrapped(DescLong.c_str());
        ImGui::EndChildFrame();
    }
}







const IBB_Section_Desc IBB_Section_DescNull{"",""};


void IBR_Debug::AddMsgCycle(const StdMessage& Msg)
{
    DebugVec.push_back(Msg);
}
void IBR_Debug::AddMsgOnce(const StdMessage& Msg)
{
    DebugVecOnce.push_back(Msg);
}
void IBR_Debug::ClearMsgCycle()
{
    DebugVec.clear();
}

void IBR_Debug::RenderUI()
{
    //_CALL_CMD IBR_Inst_Project.GetText(true);
    static bool Ext = false;
    ImGui::Checkbox(u8"输出额外信息", &Ext);
    ImGui::SameLine();
    if (ImGui::Button(u8"复制导出文本"))
    {
        ImGui::LogToClipboard();
        ImGui::LogText(_CALL_CMD IBR_Inst_Project.GetText(Ext).c_str());
        ImGui::LogFinish();
    }


    if (UICond.LoopShow){ if (ImGui::ArrowButton("loopc", ImGuiDir_Down))UICond.LoopShow = false; }
    else { if (ImGui::ArrowButton("loopa", ImGuiDir_Right))UICond.LoopShow = true; }
    if (UICond.LoopShow)for (auto x : DebugVec)x();

    if (UICond.OnceShow){ if (ImGui::ArrowButton("loopd", ImGuiDir_Down))UICond.OnceShow = false; }
    else { if (ImGui::ArrowButton("loopb", ImGuiDir_Right))UICond.OnceShow = true; }
    ImGui::SameLine();
    if (ImGui::Button(u8"CLEAR"))DebugVecOnce.clear();
    if (UICond.OnceShow)for (auto x : DebugVecOnce)x();
}

void IBR_Debug::RenderUIOnce()
{
    /*
    _CALL_CMD IBR_Inst_Project.AddModule(u8"GenUnit1", { u8"LBWNB" }, IBB_Section_DescNull);
    _CALL_CMD IBR_Inst_Project.AddModule(u8"Weapons", { u8"FuckPrimary",u8"FuckSecondary" }, IBB_Section_Desc{ "Rule","LBWNB" });

    GlobalLog.AddLog_CurTime(false); GlobalLog.AddLog("DEBUG_DATA");

    bool RA = _CALL_CMD IBR_Inst_Project.CreateSection(IBB_Section_Desc{ "Rule","LBWNB" });
    bool RB = _CALL_CMD IBR_Inst_Project.CreateSection(IBB_Section_Desc{ "Rule","LZNB" });
    GlobalLog.AddLog_CurTime(false); GlobalLog.AddLog(IBD_BoolStr(RA));
    GlobalLog.AddLog_CurTime(false); GlobalLog.AddLog(IBD_BoolStr(RB));

    {
        IBD_RInterruptF(x);
        IBR_Section SK = _CALL_CMD IBR_Inst_Project.GetSection(IBB_Section_Desc{ "Rule","LZNB" });
        GlobalLog.AddLog_CurTime(false); GlobalLog.AddLog(SK.GetBack());
        SK.GetBack()->VarList.Value["_Local_Category"] = "Infantry";
        SK.GetBack()->VarList.Value["_Local_AtFile"] = "Rule";
    }

    _CALL_CMD IBR_Inst_Project.AddModule(u8"Weapons", { u8"FuckPrimary",u8"FuckSecondary" }, IBB_Section_Desc{ "Rule","LZNB" });


    IBR_Section SC = _CALL_CMD IBR_Inst_Project.GetSection(IBB_Section_Desc{ "Rule","LZNB" });
    bool RSC = _CALL_CMD SC.Register("InfantryTypes", "Rule");

    bool RC = _CALL_CMD SC.DuplicateSection(IBB_Section_Desc{ "Rule","LZNB_II" });
    IBR_Section SD = _CALL_CMD IBR_Inst_Project.GetSection(IBB_Section_Desc{ "Rule","LZNB_II" });

    GlobalLog.AddLog_CurTime(false); GlobalLog.AddLog(SC.GetBack());
    GlobalLog.AddLog_CurTime(false); GlobalLog.AddLog(SD.GetBack());
    GlobalLog.AddLog_CurTime(false); GlobalLog.AddLog(IBD_BoolStr(RSC));
    GlobalLog.AddLog_CurTime(false); GlobalLog.AddLog(IBD_BoolStr(RC));

    {
        IBD_RInterruptF(x);
        auto sdp = SD.GetBack();
        GlobalLog.AddLog_CurTime(false); GlobalLog.AddLog(sdp->SubSecs.size());
        if (!sdp->SubSecs.empty())
        {
            auto GLine = sdp->SubSecs.at(0).Lines.begin()->second;
            GlobalLog.AddLog_CurTime(false); GlobalLog.AddLog(GLine.Default);
            GlobalLog.AddLog_CurTime(false); GlobalLog.AddLog(GLine.Default->Name.c_str());
            GlobalLog.AddLog_CurTime(false); GlobalLog.AddLog(GLine.Default->Property.Type.c_str());
            auto proc = GLine.Default->Property.Proc;
            GlobalLog.AddLog_CurTime(false); GlobalLog.AddLog(proc);
            GlobalLog.AddLog_CurTime(false); GlobalLog.AddLog(proc->GetString(GLine.Data).c_str());
            proc->SetValue(GLine.Data, "WTF_AHHH");
            _CALL_CMD IBR_Inst_Project.UpdateAll();
            GlobalLog.AddLog_CurTime(false); GlobalLog.AddLog(proc->GetString(GLine.Data).c_str());
        }
    }

    IBR_Section SKA = _CALL_CMD IBR_Inst_Project.GetSection(IBB_Section_Desc{ "Rule","FuckSecondary" });
    SKA.Rename("FuckSecondary_NewName");

    //_CALL_CMD IBR_Inst_Project.ForceUpdate();


    std::string s = _CALL_CMD IBR_Inst_Project.GetText(true);

    if (EnableLog)
    {
        GlobalLog.AddLog_CurTime(false);
        GlobalLog.AddLog("导出文本：\"");
        GlobalLog.AddLog(UTF8toMBCS(s).c_str(),false);
        GlobalLog.AddLog("\"");
    }

    //*
    _CALL_CMD IBR_Inst_Project.DeleteSection(SKA.GetSectionDesc());
    _CALL_CMD IBR_Inst_Project.UpdateAll();

    s = _CALL_CMD IBR_Inst_Project.GetText(true);

    if (EnableLog)
    {
        GlobalLog.AddLog_CurTime(false);
        GlobalLog.AddLog("导出文本II：\"");
        GlobalLog.AddLog(UTF8toMBCS(s).c_str(), false);
        GlobalLog.AddLog("\"");
    }
    //*/
}

void IBR_Debug::DebugLoad()
{
    IBF_Inst_DefaultTypeList.ReadSetting(".\\Global\\DefaultTypeList.json");
    IBF_Inst_DefaultModuleList.ReadSetting(".\\Global\\DefaultModuleList.json");
}

IBR_SectionData* _PROJ_CMD_READ _PROJ_CMD_NOINTERRUPT IBR_Section::GetSectionData() const
{
    auto It = Root->IBR_SectionMap.find(ID);
    return (It == Root->IBR_SectionMap.end()) ? nullptr : std::addressof(It->second);
}

IBB_Section_Desc* _PROJ_CMD_NOINTERRUPT IBR_Section::GetDesc() const
{
    auto It = Root->IBR_SectionMap.find(ID);
    return (It == Root->IBR_SectionMap.end()) ? nullptr : std::addressof(It->second.Desc);
}
IBB_Section* IBR_Section::GetBack_Inl() const
{
    auto pDesc = GetDesc();
    if (pDesc == nullptr)return nullptr;
    else return const_cast<IBB_Section*>(IBF_Inst_Project.Project.GetSec(IBB_Project_Index{ *pDesc }));
}

IBB_Section* _PROJ_CMD_READ IBR_Section::GetBack()
{
    IBD_RInterruptF(x);
    return GetBack_Inl();
}
const IBB_Section* _PROJ_CMD_READ IBR_Section::GetBack() const
{
    IBD_RInterruptF(x);
    return GetBack_Inl();
}
const IBB_Section* _PROJ_CMD_NOINTERRUPT _PROJ_CMD_READ IBR_Section::GetBack_Unsafe() const
{
    return GetBack_Inl();
}
template<typename T>
T _PROJ_CMD_READ _PROJ_CMD_WRITE IBR_Section::OperateBackData(const std::function<T(IBB_Section*)>& Function)
{
    IBD_RInterruptF(x);
    return Function(GetBack_Inl());
}
const IBB_Section_Desc& _PROJ_CMD_NOINTERRUPT IBR_Section::GetSectionDesc() _PROJ_CMD_BACK_CONST const
{
    auto psd = GetDesc();
    return psd == nullptr ? IBB_Section_DescNull : *psd;
}
bool _PROJ_CMD_READ IBR_Section::HasBack() const
{
    IBD_RInterruptF(x);
    return GetBack_Inl() != nullptr;
}
_RETURN_BACK_DATA IBB_VariableList* _PROJ_CMD_READ IBR_Section::GetVarList() _PROJ_CMD_BACK_CONST const
{
    IBD_RInterruptF(x);
    auto pSec = GetBack_Inl();
    if (pSec == nullptr)return nullptr;
    return &(pSec->VarList);
}
IBB_VariableList _PROJ_CMD_READ IBR_Section::GetVarListCopy() _PROJ_CMD_BACK_CONST const
{
    IBD_RInterruptF(x);
    auto pSec = GetBack_Inl();
    if (pSec == nullptr)return {};
    return pSec->VarList;
}
IBB_VariableList _PROJ_CMD_READ IBR_Section::GetVarListFullCopy(bool PrintExtraData) _PROJ_CMD_BACK_CONST const
{
    IBD_RInterruptF(x);
    auto pSec = GetBack_Inl();
    if (pSec == nullptr)return {};
    return pSec->GetLineList(PrintExtraData);
}
bool _PROJ_CMD_WRITE _PROJ_CMD_CAN_UNDO _PROJ_CMD_UPDATE IBR_Section::SetVarList(const IBB_VariableList& NewList)
{
    IBD_RInterruptF(x);
    auto pSec = GetBack_Inl();
    if (pSec == nullptr)return false;
    pSec->VarList = NewList;
    return true;
}
bool _PROJ_CMD_WRITE _PROJ_CMD_CAN_UNDO _PROJ_CMD_UPDATE IBR_Section::DuplicateSection(const IBB_Section_Desc& NewDesc) const
{
    IBD_RInterruptF(x);
    auto pSec = GetBack_Inl();
    if (pSec == nullptr)return false;
    if (!IBF_Inst_Project.Project.CreateNewSection(NewDesc))return false;

    IBF_Inst_Project.UpdateCreateSection(NewDesc);

    auto pNSec = const_cast<IBB_Section*>(IBF_Inst_Project.Project.GetSec(IBB_Project_Index{ NewDesc }));
    if (pNSec == nullptr)return false;
    bool Ret = pNSec->GenerateAsDuplicate(*pSec);

    //TODO: Check if Update Works
    //TODO: Undo Action
    return Ret;
}
IBR_Section  _PROJ_CMD_READ _PROJ_CMD_WRITE _PROJ_CMD_CAN_UNDO _PROJ_CMD_UPDATE IBR_Section::DuplicateSectionAndBack(const IBB_Section_Desc& NewDesc) _PROJ_CMD_BACK_CONST const
{
    DuplicateSection(NewDesc);
    return Root->GetSection(NewDesc);
}

bool _PROJ_CMD_WRITE _PROJ_CMD_CAN_UNDO _PROJ_CMD_UPDATE IBR_Section::Rename(const std::string& NewName)
{
    bool Ret = true;
    auto pDesc = GetDesc();
    if (pDesc == nullptr)return false;

    {
        IBD_RInterruptF(x);
        auto pSec = const_cast<IBB_Section*>(IBF_Inst_Project.Project.GetSec(IBB_Project_Index{ *pDesc }));
        IBB_Section_Desc desc = *pDesc; desc.Sec = NewName;
        auto npSec = const_cast<IBB_Section*>(IBF_Inst_Project.Project.GetSec(IBB_Project_Index{ desc }));
        if (pSec == nullptr || npSec != nullptr)Ret = false;
        else
        {
            auto OldName = pSec->Name;
            Ret = pSec->Rename(NewName);

            auto pIni = pSec->Root;
            auto Node = pIni->Secs.extract(OldName);
            Node.key() = NewName;
            auto sp = pIni->Secs.insert(std::move(Node));
            for (auto& s : pIni->Secs_ByName)if (s == OldName)s = NewName;
        }
    }

    pDesc->Sec = NewName;
    //Update Contained in Rename so no Extra
    //TODO: Undo Action
    return Ret;
}

bool _PROJ_CMD_WRITE _PROJ_CMD_CAN_UNDO IBR_Section::Register(const std::string& Name, const std::string& IniName) _PROJ_CMD_BACK_CONST const
{
    IBD_RInterruptF(x);
    return IBF_Inst_Project.Project.RegisterSection(Name, IniName, *GetBack_Inl());
}


bool _PROJ_CMD_WRITE _PROJ_CMD_CAN_UNDO _PROJ_CMD_UPDATE IBR_Project::AddModule(
    const std::string& ModuleName,
    const std::vector<std::string>& ParamList,
    const IBB_Section_Desc& MergeDesc)
{
    bool Ret;
    {
        IBD_RInterruptF(x);
        Ret = IBF_Inst_Project.AddModule(IBF_DefaultModuleList_Index{ ModuleName }, IBF_DefaultModuleList_Parameter{ ParamList ,MergeDesc });
    }
    //Update Contained in AddModule so no Extra
    //TODO: Undo Action
    return Ret;
}
bool _PROJ_CMD_WRITE _PROJ_CMD_CAN_UNDO _PROJ_CMD_UPDATE IBR_Project::AddModule(
    int ModuleID,
    const std::vector<std::string>& ParamList,
    const IBB_Section_Desc& MergeDesc)
{
    bool Ret;
    {
        IBD_RInterruptF(x);
        Ret = IBF_Inst_Project.AddModule(IBF_DefaultModuleList_Index{ ModuleID }, IBF_DefaultModuleList_Parameter{ ParamList ,MergeDesc });
    }
    //Update Contained in AddModule so no Extra
    //TODO: Undo Action 
    return Ret;
}

bool _PROJ_CMD_WRITE _PROJ_CMD_UPDATE IBR_Project::UpdateAll()
{
    IBD_RInterruptF(x);
    return IBF_Inst_Project.UpdateAll();
}

bool _PROJ_CMD_WRITE _PROJ_CMD_UPDATE IBR_Project::ForceUpdate()
{
    IBD_RInterruptF(x);
    if (BackThreadID == INT_MAX)return false;
    IBG_SuspendThread(BackThreadID);
    IBRF_CoreBump.IBF_ForceProc();
    IBG_ResumeThread(BackThreadID);
    return IBF_Inst_Project.UpdateAll();
}

template<typename T>
T _PROJ_CMD_READ _PROJ_CMD_WRITE IBR_Project::OperateBackData(const std::function<T(void)>& Function)
{
    IBD_RInterruptF(x);
    return Function();
}

_TEXT_UTF8 std::string _PROJ_CMD_READ IBR_Project::GetText(bool PrintExtraData)
{
    IBD_RInterruptF(x);
    return IBF_Inst_Project.GetText(PrintExtraData);
}

IBR_Section _PROJ_CMD_READ IBR_Project::GetSection(const IBB_Section_Desc& Desc)
{
    auto rit = IBR_Rev_SectionMap.find(Desc);
    if (rit == IBR_Rev_SectionMap.end())
    {
        rit = IBR_Rev_SectionMap.insert({ Desc,MaxID }).first;
        IBR_SectionMap.insert({ MaxID,IBR_SectionData{Desc} });
        ++MaxID;
    }
    return IBR_Section{ this,rit->second };
}

void _PROJ_CMD_NOINTERRUPT _PROJ_CMD_READ IBR_Project::EnsureSection(const IBB_Section_Desc& Desc) _PROJ_CMD_BACK_CONST
{
    auto rit = IBR_Rev_SectionMap.find(Desc);
    if (rit == IBR_Rev_SectionMap.end())
    {
        rit = IBR_Rev_SectionMap.insert({ Desc,MaxID }).first;
        IBR_SectionMap.insert({ MaxID,IBR_SectionData{Desc} });
        ++MaxID;
    }
}

std::optional<IBR_Project::id_t> _PROJ_CMD_NOINTERRUPT _PROJ_CMD_READ IBR_Project::GetSectionID(const IBB_Section_Desc& Desc) _PROJ_CMD_BACK_CONST
{
    auto rit = IBR_Rev_SectionMap.find(Desc);
    if (rit == IBR_Rev_SectionMap.end())return std::nullopt;
    else return rit->second;
}

bool _PROJ_CMD_WRITE _PROJ_CMD_CAN_UNDO _PROJ_CMD_UPDATE IBR_Project::CreateSection(const IBB_Section_Desc& Desc)
{
    bool Ret;
    {
        IBD_RInterruptF(x);
        Ret = IBF_Inst_Project.Project.CreateNewSection(Desc);
        IBF_Inst_Project.UpdateCreateSection(Desc);
    }
    //TODO: Undo Action 
    return Ret;
}

IBR_Section _PROJ_CMD_READ _PROJ_CMD_WRITE _PROJ_CMD_CAN_UNDO _PROJ_CMD_UPDATE IBR_Project::CreateSectionAndBack(const IBB_Section_Desc& Desc)
{
    CreateSection(Desc);
    return GetSection(Desc);
}

bool _PROJ_CMD_READ IBR_Project::HasSection(const IBB_Section_Desc& Desc)
{
    IBD_RInterruptF(x);
    return IBF_Inst_Project.Project.GetSec(IBB_Project_Index{ Desc }) != nullptr;
}

bool _PROJ_CMD_WRITE _PROJ_CMD_CAN_UNDO _PROJ_CMD_UPDATE IBR_Project::DeleteSection(const IBB_Section_Desc& Desc)
{
    bool Ret;
    {
        IBD_RInterruptF(x);
        if (IBF_Inst_Project.Project.GetSec(IBB_Project_Index{ Desc }) == nullptr)return false;
        auto ci = const_cast<IBB_Ini*>(IBF_Inst_Project.Project.GetIni(IBB_Project_Index{ Desc }));
        Ret = ci->DeleteSection(Desc.Sec);
        IBF_Inst_Project.UpdateAll();
    }
    auto RS = GetSection(Desc);
    IBR_SectionMap.erase(RS.ID);
    IBR_Rev_SectionMap.erase(Desc);
    return Ret;
}//TODO
/*
bool _PROJ_CMD_WRITE _PROJ_CMD_CAN_UNDO _PROJ_CMD_UPDATE IBR_Project::DeleteSection(const IBB_Section_Desc& Desc)
{
    bool Ret;
    {
        IBD_RInterruptF(x);
        if (IBF_Inst_Project.Project.GetSec(IBB_Project_Index{ Desc }) == nullptr)return false;
        auto ci = const_cast<IBB_Ini*>(IBF_Inst_Project.Project.GetIni(IBB_Project_Index{ Desc }));
        Ret = ci->DeleteSection(Desc.Sec);
    }
    auto RS = GetSection(Desc);
    IBR_SectionMap.erase(RS.ID);
    IBR_Rev_SectionMap.erase(Desc);
    return Ret;
}
*/

IBR_SectionData::IBR_SectionData() :
    EqPos(IBR_FullView::EqCenter), EqSize({ (float)IBG_GetSetting().FontSize * 12,(float)IBG_GetSetting().FontSize * 20 }) {}
IBR_SectionData::IBR_SectionData(const IBB_Section_Desc& D) :
    Desc(D), EqPos(IBR_FullView::EqCenter), EqSize({ (float)IBG_GetSetting().FontSize * 12,(float)IBG_GetSetting().FontSize * 20 }) {}

bool _PROJ_CMD_UPDATE IBR_Project::DataCheck()//TODO:确认有没有用
{
    for (auto& p : IBR_SectionMap)p.second.Exists = false;
    bool Ret = true;
    {
        IBD_RInterruptF(x);
        IBB_Section_Desc dsc;
        for (auto& ini : IBF_Inst_Project.Project.Inis)
        {
            dsc.Ini = ini.Name;
            for (auto& sp : ini.Secs)
            {
                auto& sec = sp.second;
                dsc.Sec = sec.Name;
                auto opt = GetSectionID(dsc);
                if (opt.has_value())
                {
                    IBR_SectionMap[opt.value()].Exists = true;
                }
                else
                {
                    auto sc = GetSection(dsc);
                    IBR_SectionMap[sc.ID].Exists = true;
                }
            }
        }
    }
    for (auto& p : IBR_SectionMap)
    {
        if (p.second.Exists == false)DeleteSection(p.second.Desc);
    }
    return Ret;
}

/*
WRITE:
    ret=Interrupt(Action);
    SendToF(Update);
    Push(UndoAction);
    return ret;

READ:
    Interrupt(Action);
*/

void IBR_Project::Load(const IBS_Project& Proj)
{
    for (const auto& dp : Proj.Render.Project_SecData)
    {
        ++MaxID;
        IBR_Rev_SectionMap[dp.Desc] = MaxID;
        auto& sd = IBR_SectionMap[MaxID];
        sd.Desc = dp.Desc;
        sd.EqPos = dp.EqPos;
        sd.EqSize = dp.EqSize;
    }
    IBR_WorkSpace::EqCenterPrev = IBR_FullView::EqCenter = IBR_FullView::EqCenter;
    IBR_WorkSpace::RatioPrev = IBR_FullView::Ratio = Proj.Render.FullView_Ratio;
    IBR_EditFrame::Empty = true;
}

void IBR_Project::Save(IBS_Project& Proj)
{
    Proj.Render.FullView_EqCenter = IBR_FullView::EqCenter;
    Proj.Render.FullView_Ratio = IBR_FullView::Ratio;
    Proj.Render.Project_SecData.reserve(IBR_SectionMap.size());
    for (const auto& dp : IBR_SectionMap)
    {
        Proj.Render.Project_SecData.push_back({ dp.second.Desc, dp.second.EqPos, dp.second.EqSize });
    }
}

namespace IBR_RealCenter
{
    ImVec2 Center;
    dImVec2 WorkSpaceUL, WorkSpaceDR;
    bool Update()
    {
        double LWidth = ImGui::IsWindowCollapsed() ? 0.0f : ImGui::GetWindowWidth();
        WorkSpaceUL = dImVec2{ LWidth,FontHeight * 2.0 - WindowSizeAdjustY };
        WorkSpaceDR = dImVec2{ (double)IBR_UICondition::CurrentScreenWidth ,(double)IBR_UICondition::CurrentScreenHeight - FontHeight * 1.5 };
        Center = ImVec2((WorkSpaceUL + WorkSpaceDR) / 2.0);
        return true;
    }
}

namespace IBR_UICondition
{
    int CurrentScreenWidth, CurrentScreenHeight;
    bool MenuChangeShow = true;
    std::wstring WindowTitle;
    bool UpdateWindowTitle()
    {
        std::wstring nt = AppNameW;
        auto& proj = IBF_Inst_Project.Project;
        if (IsProjectOpen)nt += L" - " + proj.ProjName;
        if (proj.ChangeAfterSave)nt += L"[*]";
        if (nt == WindowTitle)return true;
        WindowTitle = nt;
        return (bool)SetWindowTextW(MainWindowHandle, nt.c_str());
    }
}


void IBR_MainMenu::RenderUIBar()
{
    ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
    for (size_t i = 0; i < ItemList.size(); i++)
    {
        if (ItemList[i].Button())
        {
            Choice = i;
            IBR_UICondition::MenuChangeShow = true;
            if (EnableLog)
            {
                GlobalLog.AddLog_CurTime(false);
                GlobalLog.AddLog("切换了主菜单");
            }
        }
        ImGui::SameLine();
    }
    ImGui::PopStyleColor(1);
}
void IBR_MainMenu::RenderUIMenu()
{
    if (Choice < ItemList.size())ItemList.at(Choice).Menu();
}

void ControlPanel_Debug()
{
    ImGui::Text(u8"调试信息：");

    IBR_Inst_Debug.RenderUI();

    int ii = 0;
    for (auto& s : IBG_Undo.Stack)
    {
        ImGui::Text(s.Id.c_str());
        if (ii == IBG_Undo.Cursor)ImGui::Separator();
        ++ii;
    }

}

void ControlPanel_About();

void ControlPanel_WaitOpen()
{
    ImGui::TextDisabled(u8"请等待打开项目");
}
//TODO * 4
void ControlPanel_File()
{
    if (!IsProjectOpen)
    {
        if (ImGui::Button(u8"新建"))IBR_ProjectManager::CreateAction();
        if (ImGui::Button(u8"打开"))IBR_ProjectManager::OpenAction();
        ImGui::NewLine();
        IBR_RecentManager::RenderUI();
    }
    else
    {
        //TODO:导出
        if (ImGui::Button(u8"保存"))IBR_ProjectManager::SaveOptAction();
        if (ImGui::Button(u8"另存为"))IBR_ProjectManager::SaveAsAction();
        ImGui::NewLine();
        if (ImGui::Button(u8"关闭"))IBR_ProjectManager::CloseAction();
    }
}
void ControlPanel_Module()
{
    if (!IsProjectOpen)
    {
        ControlPanel_WaitOpen();
        return;
    }
    auto& ListTypes = IBF_Inst_DefaultModuleList.List.ListTypes;
    if (ListTypes.empty())
    {
        ImGui::Text(u8"无可用模块");
    }
    else
    {
        static _TEXT_UTF8 std::string CurrentType;
        static bool ShowAll = true;
        if (ImGui::BeginCombo(u8"模块类型", ShowAll?u8"所有":CurrentType.c_str()))
        {
            if (ImGui::Selectable(u8"所有", ShowAll))
            {
                ShowAll = true;
                CurrentType.clear();
            }
            for (auto& str: ListTypes)
            {
                if (ImGui::Selectable(str.c_str(), (!ShowAll) && (str == CurrentType)))
                {
                    CurrentType = str;
                    ShowAll = false;
                }
            }
            ImGui::EndCombo();
        }

        auto CRgMax = ImGui::GetWindowContentRegionMax(), CRgMin = ImGui::GetWindowContentRegionMin();
        static std::string DescLong;
        bool Appear = false;

        ImGui::BeginChild(113009, { CRgMax.x - CRgMin.x , CRgMax.y - CRgMin.y - FontHeight * 10.5f },
            false, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding);
        int NModule = 0;
        for (auto& Module : IBF_Inst_DefaultModuleList.List.Module_Default)
        {
            if ((!ShowAll) && (Module.ListType != CurrentType))continue;
            ImGui::Text(Module.DescShort.c_str());
            if (ImGui::IsItemHovered())
            {
                DescLong = Module.DescLong;
                Appear = true;
            }
            ImGui::SameLine();
            ImGui::SetCursorPosX(std::max(ImGui::GetCursorPosX(), ImGui::GetWindowWidth() - FontHeight * 4.5f));//4.5个字符是右侧内容的预留空间
            if (ImGui::SmallButton((u8"属性##ub_" + Module.Name).c_str()))
            {
                //IBR_HintManager::SetHintCustom([](std::string& st) ->bool {static int i{}; i++; st = RandStr(50); if (i >= 100) { i = 0; return false; } else return true; });
                IBR_PopupManager::SetCurrentPopup(IBR_PopupManager::Popup{}.CreateModal(Module.DescShort, true).SetFlag(ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize).PushMsgBack([=]()
                    {
                        //ImGui::SetWindowSize({ FontHeight * 20.0f,ImGui::GetWindowHeight()});
                        ImGui::Text(Module.DescLong.c_str());
                        ImGui::Text(u8"%d个新增字段 %s", Module.AttachedParagraph.size(), (Module.MergeToSection ? u8"需和已有字段合并" : u8"不和已有字段合并"));
                        ImGui::Text(u8"%d个参数", Module.ParamN);

                        ImGui::Text(u8"参数：");
                        for (int i = 0; i < Module.ParamN; i++)
                        {
                            ImGui::Text(u8"参数#%d ： %s", i, Module.ParamDescShort.at(i).c_str());
                            if (ImGui::IsItemHovered())ImGui::SetTooltip(Module.ParamDescLong.at(i).c_str());
                        }
                    }));
            }
            if (!(IBR_Inst_Project.IBR_SectionMap.empty() && Module.MergeToSection))
            {
                ImGui::SameLine();
                if (ImGui::SmallButton((u8"+##ub_" + Module.Name).c_str()))
                {
                    IBR_HintManager::SetHint(u8"左键单击选择，右键取消");
                    bool mg = Module.MergeToSection;
                    IBR_SelectMode::EnterSelectMode(IBR_SelectMode::Mode{ Module.MergeToSection,[mg, NModule](IBR_Section Section, ImVec2 ClickRePos)
                        {
                            IBR_HintManager::Clear();
                            bool CanGo = true;
                            IBB_Section_Desc desc{};
                            if (mg)
                            {
                                auto sdata = Section.GetSectionData();
                                if (sdata != nullptr)
                                {
                                    IBR_HintManager::SetHint((u8"点选了(" + std::to_string((int)ClickRePos.x) + u8", " + std::to_string((int)ClickRePos.y) +
                                        u8")，窗口\"" + sdata->Desc.GetText() + u8"\"id=" + std::to_string(Section.ID)).c_str(), HintStayTimeMillis);
                                    desc = sdata->Desc;
                                }
                                else
                                {
                                    IBR_HintManager::SetHint(u8"错误：点选了不存在的字段", HintStayTimeMillis);
                                    CanGo = false;
                                    //TODO : Log the error
                                }
                            }
                            else IBR_HintManager::SetHint((u8"点选了(" + std::to_string((int)ClickRePos.x) + u8", " + std::to_string((int)ClickRePos.y) + u8")").c_str(), HintStayTimeMillis);
                            if (CanGo)
                            {
                                //Text MaxLength=1000
                                auto TextBuf = new std::array<char, 1001>[IBF_Inst_DefaultModuleList.List.Module_Default.at(NModule).ParamN]();
                                IBR_PopupManager::SetCurrentPopup(IBR_PopupManager::Popup{}.CreateModal(u8"添加模块", true,
                                    [=]() {IBR_HintManager::SetHint(u8"取消了添加模块", HintStayTimeMillis); delete[]TextBuf; })
                                    .SetFlag(ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)
                                    .PushMsgBack([=]()
                                        {
                                            auto& md = IBF_Inst_DefaultModuleList.List.Module_Default.at(NModule);
                                            ImGui::Text(md.DescLong.c_str());
                                            ImGui::Text(md.MergeToSection ? u8"需和已有字段合并" : u8"不和已有字段合并");
                                            ImGui::Text(u8"%d个参数", md.ParamN);
                                            ImGui::Separator();
                                            ImGui::Text(u8"填写参数：");
                                            bool Full = true;
                                            for (int i = 0; i < md.ParamN; i++)
                                            {
                                                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
                                                ImGui::InputText((u8"##" + std::to_string(i)).c_str(), TextBuf[i].data(), 1000);
                                                ImGui::PopStyleColor(1);

                                                ImGui::SameLine();
                                                ImGui::Text(u8"#%d : %s", i, md.ParamDescShort.at(i).c_str());
                                                if (ImGui::IsItemHovered())ImGui::SetTooltip(md.ParamDescLong.at(i).c_str());

                                                if (strnlen(TextBuf[i].data(), 1000) == 0)Full = false;
                                            }

                                            if (Full)
                                            {
                                                if (ImGui::Button(u8"确定"))
                                                {
                                                    //GlobalLog.AddLog("--1--");

                                                    std::vector<std::string> Params;
                                                    Params.resize(md.ParamN);
                                                    for (int i = 0; i < md.ParamN; i++) //Params.at(i) = RandStr(10);
                                                    {
                                                        TextBuf[i][1000] = '\0';
                                                        Params.at(i).assign(TextBuf[i].data());
                                                    }
                                                    delete[]TextBuf;
                                                    IBR_Inst_Project.AddModule(NModule, Params, desc);
                                                    IBR_HintManager::SetHint(u8"添加模块成功", HintStayTimeMillis);

                                                    IBRF_CoreBump.IBF_SendToR(
                                                        {
                                                            []()
                                                            {
                                                                IBR_PopupManager::ClearCurrentPopup();
                                                            }, nullptr
                                                        });
                                                    //重要：需要延迟触发：否则对CanClose=true的Popup会导致HasPopup=false而触发Close的执行，比如此处会导致TextBuf释放两次
                                                    //IHS 23/3/25
                                                }
                                            }
                                            else
                                            {
                                                ImGui::TextDisabled(u8"确定");
                                                ImGui::SameLine();
                                                ImGui::PushStyleColor(ImGuiCol_Text, IBR_Color::ErrorTextColor.Value);
                                                ImGui::Text(u8"模板参数均不可为空");
                                                ImGui::PopStyleColor(1);
                                            }
                                        })
                                );
                            }
                        },[]() {IBR_HintManager::Clear(); } });
                }
            }
            
            NModule++;
        }
        ImGui::EndChild();

        ImGui::BeginChildFrame(113005, { CRgMax.x - CRgMin.x , FontHeight * 6.0f });
        if (Appear)ImGui::TextWrapped(DescLong.c_str());
        ImGui::EndChildFrame();

        
    }
    
}
void ControlPanel_View()
{
    if (!IsProjectOpen)
    {
        ControlPanel_WaitOpen();
        return;
    }
    IBR_FullView::RenderUI();
}



void ControlPanel_ListView()
{
    if (!IsProjectOpen)
    {
        ControlPanel_WaitOpen();
        return;
    }
    {
        int SelectN = 0;
        bool FullSelected = true;

        IBD_RInterruptF(x);

        for (auto& ini : IBF_Inst_Project.Project.Inis)
        {
            if (ini.Secs_ByName.empty())continue;
            for (auto& sec : ini.Secs)
            {
                if (sec.second.Dynamic.Selected)++SelectN;
                else FullSelected = false;
            }
        }

        bool SelectAll{ false }, SelectNone{ false }, Delete{ false }, Duplicate{ false };
        if (FullSelected)
        {
            if (SelectN == 0)ImGui::TextDisabled(u8"全选");
            else if (ImGui::Button(u8"全不选"))SelectNone = true;
        }
        else if (ImGui::Button(u8"全选"))SelectAll = true;
        ImGui::SameLine();
        if (SelectN == 0)
        {
            ImGui::TextDisabled(u8"删除"); ImGui::SameLine();
            ImGui::TextDisabled(u8"复制");
        }
        else
        {
            if (ImGui::Button(u8"删除"))Delete = true; ImGui::SameLine();
            if (ImGui::Button(u8"复制"))Duplicate = true;
        }

        if (SelectAll || SelectNone || Delete || Duplicate)
            for (auto& ini : IBF_Inst_Project.Project.Inis)
            {
                if (ini.Secs_ByName.empty())continue;
                for (auto& sec : ini.Secs)
                {
                    if (SelectAll)sec.second.Dynamic.Selected = true;
                    if (SelectNone)sec.second.Dynamic.Selected = false;
                    if (sec.second.Dynamic.Selected && Delete)IBRF_CoreBump.IBF_SendToR({ [=]() {IBR_Inst_Project.DeleteSection({ ini.Name,sec.second.Name }); },nullptr });
                    if (sec.second.Dynamic.Selected && Duplicate)IBRF_CoreBump.IBF_SendToR({ [=]() {IBR_Inst_Project.GetSection({ ini.Name,sec.second.Name }).DuplicateSection({ ini.Name,sec.second.Name + u8"_Copy"}); },nullptr });
                    //见V0.2.0任务清单（四）第75条“涉及字段数目变化的指令应借由IBF_SendToR等提至主循环开头”
                }
            }

        for (auto& ini : IBF_Inst_Project.Project.Inis)
        {
            if (ini.Secs_ByName.empty())continue;//屏蔽空INI
            if (ImGui::TreeNode(ini.Name == LinkGroup_IniName ? u8"链接组" : (MBCStoUTF8(ini.Name).c_str())))
            {
                for (auto& sec : ini.Secs)
                {
                    ImGui::Checkbox((u8"##" + sec.second.Name).c_str(), &sec.second.Dynamic.Selected);
                    ImGui::SameLine();
                    ImGui::Text(/*MBCStoUTF8(*/sec.second.Name.c_str());
                    ImGui::SameLine();
                    ImGui::SetCursorPosX(std::max(ImGui::GetCursorPosX(), ImGui::GetWindowWidth() - FontHeight * 2.0f));//4.5个字符是右侧内容的预留空间
                    if (ImGui::ArrowButton((sec.second.Name + "_ub_arr").c_str(), ImGuiDir_Right))
                    {
                        auto rsc = IBR_Inst_Project.GetSection(IBB_Section_Desc{ ini.Name,sec.second.Name });
                        auto dat = rsc.GetSectionData();
                        if (dat != nullptr)
                        {
                            IBR_EditFrame::SetActive(rsc.ID);
                            IBR_FullView::EqCenter = dat->EqPos + (dat->EqSize / 2.0);
                        }
                    }
                }
                ImGui::TreePop();
            }
        }
    }
}



void ControlPanel_Edit()
{
    if (!IsProjectOpen)
    {
        ControlPanel_WaitOpen();
        return;
    }
    if (IBR_EditFrame::CurSection.ID != IBR_EditFrame::PrevId)
    {
        IBR_EditFrame::PrevId = IBR_EditFrame::CurSection.ID;
        IBR_EditFrame::UpdateSection();
    }
    if (!IBR_EditFrame::Empty)if(!IBR_EditFrame::CurSection.HasBack())
    {
        IBR_EditFrame::Empty = true;
    }
    IBR_EditFrame::RenderUI();
}

bool ImGui_TextDisabled_Helper(const char* Text)
{
    ImGui::TextDisabled(Text); return false;
}
bool SmallButton_Disabled_Helper(bool cond, const char* Text)
{
    return cond ? ImGui::SmallButton(Text) : ImGui_TextDisabled_Helper(Text);
}


IBR_MainMenu IBR_Inst_Menu
{
{
    {[]() {return ImGui::SmallButton(u8"文件"); },ControlPanel_File},
    {[]() {return SmallButton_Disabled_Helper(IsProjectOpen, u8"视图"); },ControlPanel_View},
    {[]() {return SmallButton_Disabled_Helper(IsProjectOpen, u8"列表"); },ControlPanel_ListView},
    {[]() {return SmallButton_Disabled_Helper(IsProjectOpen, u8"预设"); },ControlPanel_Module},
    {[]() {return SmallButton_Disabled_Helper(IsProjectOpen, u8"编辑"); },ControlPanel_Edit},
    {[]() {return ImGui::SmallButton(u8"设置"); },[]() {IBR_Inst_Setting.RenderUI(); }},
    {[]() {return ImGui::SmallButton(u8"关于"); },ControlPanel_About},
    {[]() {return ImGui::SmallButton(u8"调试"); },ControlPanel_Debug},
}
};


void ControlPanel_About()
{
    ImGui::TextWrapped(u8"INI浏览器 V%s", Version.c_str());
    ImGui::TextWrapped(u8"最近更新于 2022年11月");
    ImGui::Separator();
    ImGui::TextWrapped(u8"作者：std::iron_hammer");
    ImGui::TextWrapped(u8"QQ：钢铁之锤（2482911962）");
    ImGui::TextWrapped(u8"贴吧：笨030504");
    ImGui::TextWrapped(u8"邮箱：2482911962@qq.com");
    ImGui::Separator();
    ImGui::TextWrapped(u8"更新链接：");
    ImGui::TextWrapped(u8"更新帖（红色警戒吧）"); //ImGui::SameLine();
    if (ImGui::Button(u8"复制链接##A"))
    {
        ImGui::LogToClipboard();
        ImGui::LogText("https://tieba.baidu.com/p/8005920823");
        ImGui::LogFinish();
    }ImGui::SameLine();
    if (ImGui::Button(u8"打开链接##A"))
    {
        ::ShellExecuteA(nullptr, "open", "https://tieba.baidu.com/p/8005920823", NULL, NULL, SW_SHOWNORMAL);
    }
    ImGui::TextWrapped(u8"更新帖（心灵终结吧）"); //ImGui::SameLine();
    if (ImGui::Button(u8"复制链接##B"))
    {
        ImGui::LogToClipboard();
        ImGui::LogText("https://tieba.baidu.com/p/8005924464");
        ImGui::LogFinish();
    }ImGui::SameLine();
    if (ImGui::Button(u8"打开链接##B"))
    {
        ::ShellExecuteA(nullptr, "open", "https://tieba.baidu.com/p/8005924464", NULL, NULL, SW_SHOWNORMAL);
    }
    ImGui::TextWrapped(u8"更新帖（心灵终结3ini吧）"); //ImGui::SameLine();
    if (ImGui::Button(u8"复制链接##C"))
    {
        ImGui::LogToClipboard();
        ImGui::LogText("https://tieba.baidu.com/p/8133473361");
        ImGui::LogFinish();
    }ImGui::SameLine();
    if (ImGui::Button(u8"打开链接##C"))
    {
        ::ShellExecuteA(nullptr, "open", "https://tieba.baidu.com/p/8133473361", NULL, NULL, SW_SHOWNORMAL);
    }
    ImGui::TextWrapped(u8"注：三个更新帖同步更新");
    ImGui::TextWrapped(u8"全部版本下载（百度网盘）"); //ImGui::SameLine();
    if (ImGui::Button(u8"复制链接##D"))
    {
        ImGui::LogToClipboard();
        ImGui::LogText("https://pan.baidu.com/s/1EpzAuIQfbU1-7sjb2YJocg?pwd=EASB");
        ImGui::LogFinish();
    }ImGui::SameLine();
    if (ImGui::Button(u8"打开链接##D"))
    {
        ::ShellExecuteA(nullptr, "open", "https://pan.baidu.com/s/1EpzAuIQfbU1-7sjb2YJocg?pwd=EASB", NULL, NULL, SW_SHOWNORMAL);
    }
    ImGui::TextWrapped(u8"提取码：EASB");
}

bool IBG_UndoStack::Undo()
{
    if (Cursor == -1)return false;
    Stack[Cursor].UndoAction();
    --Cursor;
    IBF_Inst_Project.Project.ChangeAfterSave = true;
    return true;
}
bool IBG_UndoStack::Redo()
{
    if (Cursor == (int)Stack.size() - 1)return false;
    ++Cursor;
    Stack[Cursor].RedoAction();
    IBF_Inst_Project.Project.ChangeAfterSave = true;
    return true;
}
bool IBG_UndoStack::CanUndo() const
{
    return Cursor > -1;
}
bool IBG_UndoStack::CanRedo() const
{
    return Cursor < (int)Stack.size() - 1;
}
void IBG_UndoStack::Release()
{
    while (Cursor < (int)Stack.size() - 1)
        Stack.pop_back();
}
void IBG_UndoStack::Push(const _Item& a)
{
    Release();
    Stack.push_back(a);
    ++Cursor;
    IBF_Inst_Project.Project.ChangeAfterSave = true;
}
void IBG_UndoStack::RenderUI()
{
    if (SmallButton_Disabled_Helper(CanUndo(), u8"撤销"))IBRF_CoreBump.IBF_SendToR({ [this]() {IBR_Inst_Debug.AddMsgOnce([=]() {ImGui::Text("Undo"); }); Undo(); } });
    ImGui::SameLine();
    if (SmallButton_Disabled_Helper(CanRedo(), u8"重做"))IBRF_CoreBump.IBF_SendToR({ [this]() {IBR_Inst_Debug.AddMsgOnce([=]() {ImGui::Text("Redo"); }); Redo(); } });
}
void IBG_UndoStack::Clear()
{
    Stack.clear();
}
IBG_UndoStack::_Item* IBG_UndoStack::Top()
{
    if (Cursor <= -1)return nullptr;
    else return &Stack.at(Cursor);
}
IBG_UndoStack IBG_Undo;

extern wchar_t CurrentDirW[];

namespace IBR_ProjectManager
{
#define _IN_SAVE_THREAD
#define _IN_FRONT_THREAD
#define _IN_RENDER_THREAD
#define _IN_ANY_THREAD

    //ClearCurrentPopup的调用起始点没有线程要求
    inline void _IN_RENDER_THREAD SetWaitingPopup()
    {
        IBR_PopupManager::SetCurrentPopup(
            IBR_PopupManager::Popup{}
            .CreateModal(u8"请稍候", false)
            .SetFlag(
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoNav |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoResize)
            .SetSize({ FontHeight * 10.0F,FontHeight * 6.0F })
            .PushTextBack(u8"请稍候……"));
    }


    bool _IN_RENDER_THREAD Create()
    {
        auto& proj = IBF_Inst_Project.Project;
        proj.ProjName = L"未命名";
        proj.Path = L"";
        proj.IsNewlyCreated = true;
        proj.ChangeAfterSave = false;
        proj.LastUpdate = GetSysTimeMicros();
        proj.CreateTime = GetSysTimeMicros();
        proj.CreateVersionMajor = VersionMajor;
        proj.CreateVersionMinor = VersionMinor;
        proj.CreateVersionRelease = VersionRelease;
        IBF_Inst_Project.CurrentProjectRID = GlobalRnd();
        IsProjectOpen = true;
        return true;
    }
    bool _IN_RENDER_THREAD Close() {
        IsProjectOpen = false;
        {
            IBD_RInterruptF(x);
            IBF_Inst_Project.CurrentProjectRID = 0;
            auto& proj = IBF_Inst_Project.Project;
            proj.Clear();

            IBR_Inst_Project.MaxID = 0;
            IBR_Inst_Project.IBR_SectionMap.clear();
            IBR_Inst_Project.IBR_Rev_SectionMap.clear();
            IBG_Undo.Clear();
            IBR_WorkSpace::EqCenterPrev = IBR_FullView::EqCenter = { 0.0f,0.0f };
            IBR_WorkSpace::RatioPrev = IBR_FullView::Ratio = 1.0F;
            IBR_SelectMode::CancelSelectMode();
            IBR_EditFrame::Clear();
        }
        return true;
    }
    bool _IN_ANY_THREAD IsOpen() { return IsProjectOpen; }

    /*
    void _IN_RENDER_THREAD AskFilePath(const InsertLoad::SelectFileType& Type, BOOL(_stdcall* Proc)(LPOPENFILENAMEW), const std::function<void(const std::optional<std::wstring>&)>& _IN_SAVE_THREAD Next)
    {
        IBS_Push([=]()
            {
                auto Ret = InsertLoad::SelectFileName(MainWindowHandle, Type, Proc);
                if (Ret.Success)Next(Ret.RetBuf);
                else Next(std::nullopt);
            });
    }*/
    void _IN_RENDER_THREAD AskFilePath(const InsertLoad::SelectFileType& Type, BOOL(_stdcall* Proc)(LPOPENFILENAMEW), const std::function<void(const std::optional<std::wstring>&)>& _IN_SAVE_THREAD Next)
    {
        IBS_Push([=]()
            {
                ::MessageBoxA(NULL, "Save!", "Title", MB_OK);
                Next(L"./Test/Temp.proj");
            });
    }
    void _IN_RENDER_THREAD AskLoadPath(const std::function<void(const std::optional<std::wstring>&)>& _IN_SAVE_THREAD Next)
    {
        AskFilePath(InsertLoad::SelectFileType{ CurrentDirW ,L"打开项目",L"项目文件(.proj)\0*.proj\0所有文件 (*.*)\0*.*\0\0" }, ::GetOpenFileNameW, Next);
    }
    void _IN_RENDER_THREAD AskSavePath(const std::function<void(const std::optional<std::wstring>&)>& _IN_SAVE_THREAD Next)
    {
        AskFilePath(InsertLoad::SelectFileType{ CurrentDirW ,L"保存项目",L"项目文件(.proj)\0*.proj\0所有文件 (*.*)\0*.*\0\0" }, ::GetSaveFileNameW, Next);
    }

    void _IN_SAVE_THREAD Save(const std::wstring& Path, const std::function<void(bool)>& _IN_SAVE_THREAD Next)
    {
        std::shared_ptr<bool> SigF = std::make_shared<bool>(false), SigR = std::make_shared<bool>(false), Sent = std::make_shared<bool>(false);
        IBRF_CoreBump.IBF_SendToR({ [=]()
            {
                IBR_Inst_Project.Save(IBS_Inst_Project);
                *SigR = true;
                if (*SigF && (!*Sent)) { *Sent = true; IBS_Push([=]() {Next(IBS_Inst_Project.Save(Path)); });  }
            },nullptr });
        IBRF_CoreBump.IBR_SendToF({ [=]()
            {
                IBF_Inst_Project.Save(IBS_Inst_Project);
                *SigF = true;
                if (*SigR && (!*Sent)) { *Sent = true; IBS_Push([=]() {Next(IBS_Inst_Project.Save(Path)); }); }
            } });
    }
    void _IN_SAVE_THREAD Load(const std::wstring& Path, const std::function<void(bool)>& _IN_SAVE_THREAD Next)
    {
        if (IBS_Inst_Project.Load(Path))
        {
            std::shared_ptr<bool> SigF = std::make_shared<bool>(false), SigR = std::make_shared<bool>(false), Sent = std::make_shared<bool>(false);
            IBRF_CoreBump.IBF_SendToR({ [=]()
                {
                    IBR_Inst_Project.Load(IBS_Inst_Project);
                    *SigR = true;
                    if (*SigF && (!*Sent)) { IBS_Push([=]() {Next(true); }); *Sent = true; }
                },nullptr });
            IBRF_CoreBump.IBR_SendToF({ [=]()
                {
                    IBF_Inst_Project.Load(IBS_Inst_Project);
                    *SigF = true;
                    if (*SigR && (!*Sent)) { IBS_Push([=]() {Next(true); }); *Sent = true; }
                } });
        }
        else Next(false);
    }
    void _IN_RENDER_THREAD AskIfSave(const std::function<void(bool)>& _IN_RENDER_THREAD Next)
    {
        IBR_PopupManager::SetCurrentPopup(
            IBR_PopupManager::Popup{}
            .CreateModal(u8"请稍候", true, []() {IBR_HintManager::SetHint(u8"已取消", HintStayTimeMillis); })
            .SetFlag(
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoNav |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoResize)
            .SetSize({ FontHeight * 10.0F,FontHeight * 6.0F })
            .PushTextBack(u8"你是否要保存当前文件？")
            .PushMsgBack([=]()
                {
                    if (ImGui::Button(u8"确定##AskIfSave"))
                    {
                        IBRF_CoreBump.IBF_SendToR({ [=]() {IBR_PopupManager::ClearCurrentPopup(); Next(true); }, nullptr });
                    }ImGui::SameLine();
                    if (ImGui::Button(u8"取消##AskIfSave"))
                    {
                        IBRF_CoreBump.IBF_SendToR({ [=]() {IBR_PopupManager::ClearCurrentPopup(); Next(false); }, nullptr });
                    }
                }));
    }




    void _IN_RENDER_THREAD OpenAction()
    {
        SetWaitingPopup();
        AskLoadPath([](auto ws)
            {
                if (ws)
                {
                    IBR_RecentManager::Push(ws.value());
                    IBR_RecentManager::Save();
                    IBS_Push([=]()
                        {
                            Load(ws.value(), [](bool OK) {IBRF_CoreBump.IBF_SendToR({ [=]()
                            {
                                IBR_PopupManager::ClearCurrentPopup();
                                if (OK)
                                {
                                    IBR_HintManager::SetHint(u8"载入成功！", HintStayTimeMillis);
                                    IBF_Inst_Project.CurrentProjectRID = GlobalRnd();
                                    IsProjectOpen = true;
                                }
                                else IBR_HintManager::SetHint(u8"载入失败！", HintStayTimeMillis);
                            }, nullptr }); });
                        });
                }
                else { IBR_HintManager::SetHint(u8"已取消", HintStayTimeMillis); IBR_PopupManager::ClearCurrentPopup(); }
            });
    }
    void _IN_RENDER_THREAD OpenRecentAction(const std::wstring& Path)
    {
        SetWaitingPopup();

        IBR_RecentManager::Push(Path);
        IBR_RecentManager::Save();
        IBS_Push([=]()
            {
                Load(Path, [](bool OK) {IBRF_CoreBump.IBF_SendToR({ [=]()
                    {
                        IBR_PopupManager::ClearCurrentPopup();
                        if (OK)
                            {
                                IBR_HintManager::SetHint(u8"载入成功！", HintStayTimeMillis);
                                IBF_Inst_Project.CurrentProjectRID = GlobalRnd();
                                IsProjectOpen = true;
                            }
                        else IBR_HintManager::SetHint(u8"载入失败！", HintStayTimeMillis);
                    }, nullptr }); });
            });
    }
    void _IN_RENDER_THREAD CreateAction()
    {
        Create();
    }
    void _IN_RENDER_THREAD CloseAction()
    {
        if (!IBF_Inst_Project.Project.ChangeAfterSave)
        {
            Close();
            return;
        }
        AskIfSave([](bool OK)
            {
                if (OK)
                {
                    if (IBF_Inst_Project.Project.IsNewlyCreated)
                    {
                        SetWaitingPopup();
                        IBS_Push([]()
                            {
                                Save(IBF_Inst_Project.Project.Path, [](bool OK2) {IBRF_CoreBump.IBF_SendToR({ [=]()
                                    {
                                        IBR_PopupManager::ClearCurrentPopup();
                                        if (OK2)
                                        {
                                            Close();
                                            IBR_HintManager::SetHint(u8"关闭成功！", HintStayTimeMillis);
                                        }
                                        else IBR_HintManager::SetHint(u8"保存失败！关闭失败！", HintStayTimeMillis);
                                    }, nullptr }); });
                            });
                    }
                    else
                    {
                        SetWaitingPopup();
                        AskSavePath([](auto ws)
                            {
                                if (ws)Save(ws.value(), [](bool OK2) {IBRF_CoreBump.IBF_SendToR({ [=]()
                                    {
                                        IBR_PopupManager::ClearCurrentPopup();
                                        if (OK2)
                                        {
                                            Close();
                                            IBR_HintManager::SetHint(u8"关闭成功！", HintStayTimeMillis);
                                        }
                                        else IBR_HintManager::SetHint(u8"保存失败！关闭失败！", HintStayTimeMillis);
                                    }, nullptr }); });
                                else { IBR_HintManager::SetHint(u8"已取消", HintStayTimeMillis); IBR_PopupManager::ClearCurrentPopup(); }
                            });
                    }
                }
                else IBR_HintManager::SetHint(u8"已取消", HintStayTimeMillis);
            });
    }
    void _IN_RENDER_THREAD SaveAction()
    {

        if (!IBF_Inst_Project.Project.ChangeAfterSave)return;
        SetWaitingPopup();
        IBS_Push([=]()
            {
                Save(IBF_Inst_Project.Project.Path, [](bool OK) {IBRF_CoreBump.IBF_SendToR({ [=]()
                {
                    IBR_PopupManager::ClearCurrentPopup();
                    if (OK)IBR_HintManager::SetHint(u8"保存成功！", HintStayTimeMillis);
                    else IBR_HintManager::SetHint(u8"保存失败！", HintStayTimeMillis);
                }, nullptr }); });
            });
        
    }
    void _IN_RENDER_THREAD SaveAsAction()
    {
        SetWaitingPopup();
        AskSavePath([](auto ws)
            {
                if (ws)Save(ws.value(), [](bool OK) {IBRF_CoreBump.IBF_SendToR({ [=]()
                    {
                        IBR_PopupManager::ClearCurrentPopup();
                        if (OK)IBR_HintManager::SetHint(u8"保存成功！", HintStayTimeMillis);
                        else IBR_HintManager::SetHint(u8"保存失败！", HintStayTimeMillis);
                    }, nullptr }); });
                else { IBR_HintManager::SetHint(u8"已取消", HintStayTimeMillis); IBR_PopupManager::ClearCurrentPopup(); }
            });
    }
    void _IN_RENDER_THREAD SaveOptAction()
    {
        if (IBF_Inst_Project.Project.IsNewlyCreated)SaveAsAction();
        else SaveAction();
    }
};

namespace IBR_WorkSpace
{
    ImVec2 EqCenterPrev, ReCenterPrev;
    float RatioPrev;
    int UpdatePrevResult;
    bool InWorkSpace(ImVec2 RePos)
    {
        return InRectangle(RePos, IBR_RealCenter::WorkSpaceUL, IBR_RealCenter::WorkSpaceDR);
    }
    void UpdatePrev()
    {
        RatioPrev = IBR_FullView::Ratio;
        EqCenterPrev = IBR_FullView::EqCenter;
        ReCenterPrev = IBR_RealCenter::Center;
    }
    void UpdatePrevII()
    {
        UpdatePrevResult = _UpdatePrev_None;
        if (abs(IBR_FullView::EqCenter - EqCenterPrev).max() >= 1)
        {
            auto EqCCopy = IBR_FullView::EqCenter;
            auto EqCPCopy = EqCenterPrev;
            UpdatePrevResult |= _UpdatePrev_EqCenter;
            IBG_Undo.Release();
            auto top = IBG_Undo.Top();
            if (top != nullptr && top->Id == "IBR_FullView::EqCenter")//MERGE
            {
                EqCPCopy = std::any_cast<std::pair<ImVec2, ImVec2>>(top->Extra()).first;
                top->UndoAction = [=]() {EqCenterPrev = IBR_FullView::EqCenter = EqCPCopy; };
                top->RedoAction = [=]() {EqCenterPrev = IBR_FullView::EqCenter = EqCCopy; };
                top->Extra = [=]()->std::any {return std::any{ std::make_pair(EqCPCopy,EqCCopy) }; };
            }
            else
            {
                IBG_UndoStack::_Item it;
                it.Id = "IBR_FullView::EqCenter";
                it.UndoAction = [=]() {EqCenterPrev = IBR_FullView::EqCenter = EqCPCopy; };
                it.RedoAction = [=]() {EqCenterPrev = IBR_FullView::EqCenter = EqCCopy; };
                it.Extra = [=]()->std::any {return std::any{ std::make_pair(EqCPCopy,EqCCopy) }; };
                IBG_Undo.Push(it);
            }
        }
        else if (abs(IBR_FullView::Ratio - RatioPrev) >= 0.01)
        {
            auto EqCCopy = IBR_FullView::Ratio;
            auto EqCPCopy = RatioPrev;
            UpdatePrevResult |= _UpdatePrev_Ratio;
            IBG_Undo.Release();
            auto top = IBG_Undo.Top();
            if (top != nullptr && top->Id == "IBR_FullView::Ratio")//MERGE
            {
                EqCPCopy = std::any_cast<std::pair<float, float>>(top->Extra()).first;
                top->UndoAction = [=]() {RatioPrev = IBR_FullView::Ratio = EqCPCopy; };
                top->RedoAction = [=]() {RatioPrev = IBR_FullView::Ratio = EqCCopy; };
                top->Extra = [=]()->std::any {return std::any{ std::make_pair(EqCPCopy,EqCCopy) }; };
            }
            else
            {
                IBG_UndoStack::_Item it;
                it.Id = "IBR_FullView::Ratio";
                it.UndoAction = [=]() {RatioPrev = IBR_FullView::Ratio = EqCPCopy; };
                it.RedoAction = [=]() {RatioPrev = IBR_FullView::Ratio = EqCCopy; };
                it.Extra = [=]()->std::any {return std::any{ std::make_pair(EqCPCopy,EqCCopy) }; };
                IBG_Undo.Push(it);
            }
            //IBR_Inst_Debug.AddMsgOnce([=]() {ImGui::Text("CCopy=%f,CPCopy=%f", EqCCopy, EqCPCopy); });
        }
    }
    
    InfoStack<StdMessage> ExtSetPos;
    IBR_SectionData* CurOnRender;
    struct PosHelper { ImVec2 eq, re; };
    void RenderUI()
    {
        IBB_Section_Desc _RenderUI_OnRender;
        if (!IsProjectOpen)return;
        auto vu = ExtSetPos.GetCopyAndClear();

        IBR_Inst_Project.LinkList.clear();

        for (auto& sp : IBR_Inst_Project.IBR_SectionMap)
        {
            if (!IBR_Inst_Project.GetSectionFromID(sp.first).HasBack())continue;

            auto& sd = sp.second;
            CurOnRender = &sd;
            _RenderUI_OnRender = sd.Desc;
            auto TA = EqPosToRePos(sd.EqPos);

            if (sd.First)
            {
                ImGui::SetNextWindowPos(TA);
                ImGui::SetNextWindowSize(sd.EqSize * IBR_FullView::Ratio);
                if (sp.first == IBR_EditFrame::CurSection.ID)ImGui::SetNextWindowFocus();
                sd.First = false;
            }

            for (auto& p : vu)p();

            

            ImGui::Begin((sd.Desc.Ini + u8" - " + sd.Desc.Sec).c_str(), &sd.IsOpen, ImGuiWindowFlags_NoClamping);
            ImGui::SetWindowFontScale(IBR_FullView::Ratio);
            ImGui::PushClipRect(IBR_RealCenter::WorkSpaceUL, IBR_RealCenter::WorkSpaceDR, true);

            if (ImGui::IsWindowFocused())IBR_EditFrame::SetActive(sp.first);
            auto PCopy = ImGui::GetWindowPos();
            auto SCopy = ImGui::GetWindowSize();
            sd.Hovered = ImGui::IsWindowHovered();

            if (UpdatePrevResult & _UpdatePrev_EqCenter)
            {
                ImGui::SetWindowPos(TA);
                //RePos
            }
            else if (UpdatePrevResult & _UpdatePrev_Ratio)
            {
                ImGui::SetWindowSize(sd.EqSize * IBR_FullView::Ratio);
                ImGui::SetWindowPos(TA);
                //ReSize|RePos
            }
            else if (abs(IBR_WorkSpace::ReCenterPrev - IBR_RealCenter::Center).max() >= 1.0)
            {
                ImGui::SetWindowPos(TA);
                //RePos
            }
            else if (abs(PCopy - TA).max() >= 1.0)
            {
                ImVec2 NP = sd.EqPos + (PCopy - TA) / IBR_FullView::Ratio;
                PosHelper un = { sd.EqPos,TA }, re = { NP,PCopy };
                sd.EqPos = NP;
                
                IBG_Undo.Release();
                auto top = IBG_Undo.Top();
                auto sn = sd.Desc.Ini + "[" + sd.Desc.Sec + "].EqPos";
                if (top != nullptr && top->Id == sn)//MERGE
                {
                    un = std::any_cast<std::pair<PosHelper, PosHelper>>(top->Extra()).first;
                    top->UndoAction = [=]() {IBRF_CoreBump.IBF_SendToR({ [=]() {
                        if (_RenderUI_OnRender == CurOnRender->Desc) { CurOnRender->EqPos = un.eq; ImGui::SetNextWindowPos(un.re); }},
                        &ExtSetPos }); };
                    top->RedoAction = [=]() {IBRF_CoreBump.IBF_SendToR({ [=]() {
                        if (_RenderUI_OnRender == CurOnRender->Desc) { CurOnRender->EqPos = re.eq; ImGui::SetNextWindowPos(re.re); }},
                        &ExtSetPos }); };
                    top->Extra = [=]()->std::any {return std::any{ std::make_pair(un,re) }; };
                }
                else
                {
                    IBG_UndoStack::_Item it;
                    it.Id = sn;
                    it.UndoAction = [=]() {IBRF_CoreBump.IBF_SendToR({ [=]() {
                        if (_RenderUI_OnRender == CurOnRender->Desc) { CurOnRender->EqPos = un.eq; ImGui::SetNextWindowPos(un.re); }},
                        &ExtSetPos }); };
                    it.RedoAction = [=]() {IBRF_CoreBump.IBF_SendToR({ [=]() {
                        if (_RenderUI_OnRender == CurOnRender->Desc) { CurOnRender->EqPos = re.eq; ImGui::SetNextWindowPos(re.re); }},
                        &ExtSetPos }); };
                    it.Extra = [=]()->std::any {return std::any{ std::make_pair(un,re) }; };
                    IBG_Undo.Push(it);
                }
                //EqPos
            }
            else if (abs(SCopy - sd.EqSize * IBR_FullView::Ratio).max() >= 1.0)
            {
                ImVec2 NP = SCopy / IBR_FullView::Ratio;
                PosHelper un = { sd.EqSize,sd.EqSize * IBR_FullView::Ratio }, re = { NP,SCopy };
                sd.EqSize = NP;

                IBG_Undo.Release();
                auto top = IBG_Undo.Top();
                auto sn = sd.Desc.Ini + "[" + sd.Desc.Sec + "].EqSize";
                if (top != nullptr && top->Id == sn)//MERGE
                {
                    un = std::any_cast<std::pair<PosHelper, PosHelper>>(top->Extra()).first;
                    top->UndoAction = [=]() {IBRF_CoreBump.IBF_SendToR({ [=]() {
                        if (_RenderUI_OnRender == CurOnRender->Desc) { CurOnRender->EqSize = un.eq; ImGui::SetNextWindowSize(un.re); }},
                        &ExtSetPos }); };
                    top->RedoAction = [=]() {IBRF_CoreBump.IBF_SendToR({ [=]() {
                        if (_RenderUI_OnRender == CurOnRender->Desc) { CurOnRender->EqSize = re.eq; ImGui::SetNextWindowSize(re.re); }},
                        &ExtSetPos }); };
                    top->Extra = [=]()->std::any {return std::any{ std::make_pair(un,re) }; };
                }
                else
                {
                    IBG_UndoStack::_Item it;
                    it.Id = sn;
                    it.UndoAction = [=]() {IBRF_CoreBump.IBF_SendToR({ [=]() {
                        if (_RenderUI_OnRender == CurOnRender->Desc) { CurOnRender->EqSize = un.eq; ImGui::SetNextWindowSize(un.re); }},
                        &ExtSetPos }); };
                    it.RedoAction = [=]() {IBRF_CoreBump.IBF_SendToR({ [=]() {
                        if (_RenderUI_OnRender == CurOnRender->Desc) { CurOnRender->EqSize = re.eq; ImGui::SetNextWindowSize(re.re); }},
                        &ExtSetPos }); };
                    it.Extra = [=]()->std::any {return std::any{ std::make_pair(un,re) }; };
                    IBG_Undo.Push(it);
                }
                //EqSize
            }

            sd.RenderUI();

            ImGui::PopClipRect();
            ImGui::End();
            if (!sd.IsOpen)IBRF_CoreBump.IBF_SendToR({ [=]() {IBR_Inst_Project.DeleteSection(sd.Desc); },nullptr });
        }

        auto BList = ImGui::GetBackgroundDrawList();
        for (const auto& Link : IBR_Inst_Project.LinkList)
        {
            auto Rsec = IBR_Inst_Project.GetSection(Link.Dest);
            auto RSD = Rsec.GetSectionData();
            if (RSD == nullptr || !Rsec.HasBack())
            {
                //::MessageBoxA(NULL, "Fuck!", "Title", MB_OK);
                auto par = Link.BeginR;
                par.x -= FontHeight * 0.5f;
                BList->AddLine(par, dImVec2{ par.x + FontHeight * 3.0, par.y },
                    IBR_Color::IllegalLineColor, FontHeight / 3.0f);
            }
            else
            {
                ImVec2 ReUL = EqPosToRePos(RSD->EqPos);//Test
                ImVec2 EqUR = RSD->EqPos;
                EqUR.x += RSD->EqSize.x;
                ImVec2 ReUR = EqPosToRePos(EqUR);
                {
                    auto pal = Link.BeginL;
                    auto par = Link.BeginR;
                    auto pbl = ReUL;
                    auto pbr = ReUR;
                    pal.x += FontHeight * 0.5f;
                    par.x -= FontHeight * 0.5f;
                    pbl.x += FontHeight * 0.5f;
                    pbl.y += FontHeight * 0.5f;
                    pbr.x -= FontHeight * 0.5f;
                    pbr.y += FontHeight * 0.5f;
                    ImVec2 pa, pb;
                    pa = pal.x >= pbr.x ? pal : par;
                    pb = pbl.x >= par.x ? pbl : pbr;

                    pa.y += FontHeight / 6.0f;
                    BList->AddBezierCurve(pa, { (pa.x + 4 * pb.x) / 5,pa.y }, { (4 * pa.x + pb.x) / 5,pb.y }, pb,
                        IBR_Color::LegalLineColor, FontHeight / 5.0f);
                    pa.y -= FontHeight / 6.0f;
                    BList->AddBezierCurve(pa, { (pa.x + 4 * pb.x) / 5,pa.y }, { (4 * pa.x + pb.x) / 5,pb.y }, pb,
                        IBR_Color::LegalLineColor, FontHeight / 5.0f);
                    pa.y -= FontHeight / 6.0f;
                    BList->AddBezierCurve(pa, { (pa.x + 4 * pb.x) / 5,pa.y }, { (4 * pa.x + pb.x) / 5,pb.y }, pb,
                        IBR_Color::LegalLineColor, FontHeight / 5.0f);
                }
            }
        }
    }
}

void IBR_SectionData::RenderUI()
{
    auto Rsec = IBR_Inst_Project.GetSection(Desc);
    {
        IBD_RInterruptF(x);
        auto Bsec = Rsec.GetBack_Unsafe();
        if (Bsec == nullptr)
        {
            ImGui::TextColored(IBR_Color::ErrorTextColor, u8"字段错误链接或无链接");
        }
        else
        {
            for (const auto& sub : Bsec->SubSecs)
            {
                for (const auto& lt : sub.LinkTo)
                {
                    ImGui::TextWrapped(u8"%s=%s   ", lt.FromKey.c_str(),lt.To.Section.GetText().c_str());
                    ImGui::SameLine();
                    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - FontHeight * 2.0f);
                    IBR_Inst_Project.LinkList.push_back({ ImGui::GetLineBeginPos(),ImGui::GetLineEndPos(),{lt.To.Ini.GetText(),lt.To.Section.GetText()} });
                    if (ImGui::ArrowButton((lt.FromKey + u8"_WTF_" + lt.To.Section.GetText()).c_str(), ImGuiDir_Right))
                    {
                        auto rsc = IBR_Inst_Project.GetSection(IBB_Section_Desc{ lt.To.Ini.GetText(),lt.To.Section.GetText() });
                        auto dat = rsc.GetSectionData();
                        if (dat != nullptr)
                        {
                            IBR_EditFrame::SetActive(rsc.ID);
                            IBRF_CoreBump.IBF_SendToR({ [=]() {IBR_FullView::EqCenter = dat->EqPos + (dat->EqSize / 2.0); } });
                            
                        }
                    }
                }
            }
        }
    }
    //TODO
}

namespace IBR_FullView
{
    

    ImVec2 ViewSize;//Set in INIBrowser.cpp
    double ViewScale = 20;

    void DrawView(ImDrawList* dl, ImVec2 Pos)
    {
        ImVec2 CPos = Pos + (ViewSize * 0.5);
        dImVec2 VOffset = dImVec2(IBR_FullView::EqCenter) / ViewScale;
        dl->AddRectFilled(Pos, Pos + ViewSize, IBR_Color::BackgroundColor);
        dImVec2 CC = Pos + (ViewSize / 2);//中心
        /*
        for (int i = 0; i < CurrentNSec; i++)
        {
            ImVec2 VCI = WindowZoomDirection[i] / (double)IBR_FullView::Ratio / ViewScale ;
            for (int j : Linkto[i])
            {
                if (j == -1)continue;
                ImVec2 VCJ = WindowZoomDirection[j] / (double)IBR_FullView::Ratio / ViewScale ;
                dl->AddBezierCurve(
                    VCI + CPos + VOffset,
                    ImVec2{ (3 * VCJ.x + VCI.x) / 4 ,VCI.y } + CPos + VOffset,
                    ImVec2{ (3 * VCI.x + VCJ.x) / 4 ,VCJ.y } + CPos + VOffset,
                    VCJ + CPos + VOffset,
                     _TempSelectLink::LegalLineColor,1.0f);
            }
        }*/
        IBR_SectionData* psd = nullptr;
        for (auto& sp : IBR_Inst_Project.IBR_SectionMap)
        {
            auto& sd = sp.second;
            if (sp.first == IBR_EditFrame::CurSection.ID)
            {
                psd = &sp.second; continue;
            }
            ImVec2 WUL = CC + sd.EqPos / ViewScale;
            ImVec2 WDR = CC + (sd.EqPos + sd.EqSize) / ViewScale;
            dl->AddRectFilled(WUL, WDR, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_WindowBg)));
            dl->AddRect(WUL, WDR, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)), 0.0f, 0, 1.0f);

        }
        if (psd != nullptr)
        {
            auto& sd = *psd;
            ImVec2 WUL = CC + sd.EqPos / ViewScale;
            ImVec2 WDR = CC + (sd.EqPos + sd.EqSize) / ViewScale;
            dl->AddRectFilled(WUL, WDR, IBR_Color::FocusWindowColor);
            dl->AddRect(WUL, WDR, ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)), 0.0f, 0, 1.0f);
        }

        dImVec2 ClipSize = (IBR_RealCenter::WorkSpaceDR - IBR_RealCenter::WorkSpaceUL) / (double)IBR_FullView::Ratio;
        dImVec2 ClipVSize = ClipSize / ViewScale;
        dImVec2 ClipVUL = VOffset - (ClipVSize * 0.5);
        dImVec2 CCPS = ClipVUL + (ClipVSize / 2) + CPos;
        dl->AddLine(CCPS - ImVec2{ 3, 0 }, CCPS + ImVec2{ 4, 0 }, IBR_Color::CenterCrossColor, 1.0f);
        dl->AddLine(CCPS - ImVec2{ 0, 3 }, CCPS + ImVec2{ 0, 4 }, IBR_Color::CenterCrossColor, 1.0f);
        dl->AddRect(ClipVUL + CPos, ClipVUL + ClipVSize + CPos, IBR_Color::ClipFrameLineColor, 0.0f, 0, 1.0f);

    }

    void ChangeOffsetPos(ImVec2 ClickRel)
    {
        IBR_FullView::EqCenter = (ClickRel - ViewSize * 0.5) * ViewScale;
    }

    ImVec2 EqCenter = { 0.0f,0.0f };
    float Ratio = 1.0;
    void RenderUI()
    {
        static float TmpScale;
        TmpScale = 100.0f * IBR_FullView::Ratio;
        ImGui::SliderFloat(u8"缩放比例", &TmpScale, 25.0f, 200.0f, "%.0f%%", ImGuiSliderFlags_Logarithmic);
        TmpScale = floor(TmpScale / 5.0f) * 5.0f;
        IBR_FullView::Ratio = TmpScale / 100.0f;

        ImGui::Text(u8"视图");
        ImGui::BeginChildFrame(114514 + 2, ViewSize + ImVec2{ 5, 8 });
        auto CRect = ImGui::GetCursorScreenPos();
        ImGui::Dummy(ViewSize);
        {
            bool CHover = ImGui::IsItemHovered();
            IBR_Inst_Debug.AddMsgCycle([=]() {ImGui::Text("View Hovered = %s", (CHover ? "true" : "false")); });
            IBR_Inst_Debug.AddMsgCycle([=]() {ImGui::Text("View Pos = ( %.2f, %.2f )", CRect.x, CRect.y); });
            IBR_Inst_Debug.AddMsgCycle([=]() {ImGui::Text("Offset Pos = ( %.2f, %.2f )", IBR_FullView::EqCenter.x, IBR_FullView::EqCenter.y); });
        }
        if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            auto MP = ImGui::GetIO().MousePos;
            ChangeOffsetPos(MP - CRect);
        }
        DrawView(ImGui::GetWindowDrawList(), CRect);
        ImGui::EndChildFrame();
    }
}

namespace IBR_DynamicData
{
    ExtFileClass DynamicData;
    void Read(int DefaultResX, int DefaultResY)
    {
        if (DynamicData.Open(".\\Resources\\dynamic.dat", "rb"))
        {
            int64_t ScrXR, ScrYR;
            DynamicData.ReadData(ScrXR);
            DynamicData.ReadData(ScrYR);
            double SectionTextScaleR;
            DynamicData.ReadData(SectionTextScaleR);
            IBR_FullView::Ratio = IBR_WorkSpace::RatioPrev = (float)SectionTextScaleR;
            //DynamicData.ReadData(IBR_FullView::EqCenter.x);
            //DynamicData.ReadData(IBR_FullView::EqCenter.y);
            IBR_WorkSpace::EqCenterPrev = IBR_FullView::EqCenter;

            ScrX = (int)ScrXR;
            ScrY = (int)ScrYR;
            if (ScrX <= 0 || ScrY <= 0)
            {
                ScrX = DefaultResX;
                ScrY = DefaultResY;
            }
            DynamicData.Close();
        }
        else //缺省值
        {
            ScrX = DefaultResX;
            ScrY = DefaultResY;
            IBR_FullView::Ratio = IBR_WorkSpace::RatioPrev = 1.0f;
        }
        IBR_UICondition::CurrentScreenWidth = ScrX;
        IBR_UICondition::CurrentScreenHeight = ScrY;
    }
    void SetRandom()
    {
        ::srand((unsigned)::time(NULL));
        GlobalRnd = std::default_random_engine{ (unsigned)::time(NULL) };
        if (EnableLog)
        {
            GlobalLog.AddLog_CurTime(false);
            GlobalLog.AddLog("设置随机数引擎。");
        }
    }
    void Open()
    {
        DynamicData.Open(L".\\Resources\\dynamic.dat", L"wb");
        if (!DynamicData.Available())
        {
            MessageBoxA(NULL, ("GetLastError()错误码：" + std::to_string(GetLastError())).c_str(), "IBR_DynamicData::Open 发生错误", MB_OK);
        }
    }
    void Save()
    {
        DynamicData.Rewind();
        DynamicData.WriteData((int64_t)(IBR_UICondition::CurrentScreenWidth));
        DynamicData.WriteData((int64_t)(IBR_UICondition::CurrentScreenHeight));
        DynamicData.WriteData((double)IBR_FullView::Ratio);
        //DynamicData.WriteData(IBR_FullView::EqCenter.x);
        //DynamicData.WriteData(IBR_FullView::EqCenter.y);
        DynamicData.Flush();
    }
    void Close()
    {
        DynamicData.Close();
    }
}

namespace IBR_RecentManager
{
    ExtFileClass RecentFile;
    std::vector<_TEXT_UTF8 std::string> RecentName;
    const wchar_t* Path = L".\\Resources\\recent.dat";

    const int MaxRecent = 10;

    IBR_ListMenu<_TEXT_UTF8 std::string> RecentList{ RecentName,u8"Recent",[](_TEXT_UTF8 std::string& Name,int,int)
        {
            if (ImGui::SmallButton((u8"打开##" + Name).c_str()))
                IBR_ProjectManager::OpenRecentAction(UTF8toUnicode(Name));
            ImGui::SameLine();
            ImGui::TextWrapped(Name.c_str());
        } };
    void RenderUI()
    {
        ImGui::Text(u8"最近打开");
        RecentList.RenderUI();
    }
    void Load()
    {
        if (RecentFile.Open(Path, L"rb"))
        {
            if (!RecentFile.Available())
            {
                MessageBoxA(NULL, ("GetLastError()错误码：" + std::to_string(GetLastError())).c_str(), "IBR_RecentManager::Load 发生错误", MB_OK);
            }
            RecentFile.ReadVector(RecentName);
            RecentFile.Close();
        }
    }
    void Push(const std::wstring& _Path)
    {
        _TEXT_UTF8 std::string up = UnicodetoUTF8(_Path);
        if (RecentName.empty())
        {
            RecentName.push_back(up);
        }
        else if (RecentName.front() != up)
        {
            RecentName.insert(RecentName.begin(), up);
            while (RecentName.size() > MaxRecent)RecentName.pop_back();
        }
    }
    void Save()
    {
        if (RecentFile.Open(Path, L"wb"))
        {
            RecentFile.WriteVector(RecentName);
            RecentFile.Close();
        }
    }
}

namespace IBR_EditFrame
{
    IBR_Section CurSection;
    IBR_Project::id_t PrevId;
    bool Empty{ true };
    void SetActive(IBR_Project::id_t id)
    {
        if (Empty)
        {
            PrevId = id;
            Empty = false;
        }
        CurSection = IBR_Inst_Project.GetSectionFromID(id);
    }
    void UpdateSection()
    {
        //TODO
    }
    void RenderUI()
    {
        if (Empty)
        {
            //TODO
            return;
        }
        //TODO
    }
    void Clear()
    {
        Empty = true;
    }
};

namespace IBR_PopupManager
{
    Popup CurrentPopup;
    bool HasPopup = false;
    Popup BasicPopupA{
        false,false,"",ImGuiWindowFlags_None,[]() {},[]() {}
    };
    Popup& Popup::Create(const _TEXT_UTF8 std::string& title)
    {
        Title = title;
        CanClose = false;
        Show = []() {};
        Flag = ImGuiWindowFlags_AlwaysAutoResize;
        return *this;
    }
    Popup& Popup::CreateModal(const _TEXT_UTF8 std::string& title, bool canclose, StdMessage close)
    {
        Title = title;
        Modal = true;
        CanClose = canclose;
        Close = close;
        Show = []() {};
        Flag = ImGuiWindowFlags_AlwaysAutoResize;
        return *this;
    }
    Popup& Popup::SetTitle(const _TEXT_UTF8 std::string& title)
    {
        Title = title;
        return *this;
    }
    Popup& Popup::SetFlag(ImGuiWindowFlags flag)
    {
        Flag |= flag;
        return *this;
    }
    Popup& Popup::UnsetFlag(ImGuiWindowFlags flag)
    {
        Flag &= ~flag;
        return *this;
    }
    Popup& Popup::ClearFlag()
    {
        Flag = 0;
        return *this;
    }
    Popup& Popup::PushTextPrev(const _TEXT_UTF8 std::string& Text)//TODO:优化，试图砍掉prev
    {
        StdMessage ShowPrev{ std::move(Show) };
        Show = [=]() {ImGui::TextWrapped(Text.c_str()); ShowPrev(); };
        return *this;
    }
    Popup& Popup::PushTextBack(const _TEXT_UTF8 std::string& Text)
    {
        StdMessage ShowPrev{ std::move(Show) };
        Show = [=]() {ShowPrev(); ImGui::TextWrapped(Text.c_str()); };
        return *this;
    }
    Popup& Popup::PushMsgPrev(StdMessage Msg)
    {
        StdMessage ShowPrev{ std::move(Show) };
        Show = [=]() {Msg(); ShowPrev(); };
        return *this;
    }
    Popup& Popup::PushMsgBack(StdMessage Msg)
    {
        StdMessage ShowPrev{ std::move(Show) };
        Show = [=]() {ShowPrev(); Msg(); };
        return *this;
    }
    Popup& Popup::SetSize(ImVec2 NewSize)
    {
        Size = NewSize;
        return *this;
    }
    Popup SingleText(const _TEXT_UTF8 std::string& StrId, const _TEXT_UTF8 std::string& Text,bool Modal)
    {
        return (Modal ? Popup{}.CreateModal(StrId, false) : Popup{}.Create(StrId)).SetFlag(ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize).PushTextBack(Text);
    }
    void RenderUI()
    {
        if (HasPopup)
        {

            static bool pp = true;
            ImGui::OpenPopup(CurrentPopup.Title.c_str());
            bool HPPrev = HasPopup;
            if (CurrentPopup.Size.x >= 1.0F && CurrentPopup.Size.y >= 1.0F)ImGui::SetNextWindowSize(CurrentPopup.Size);
            if (CurrentPopup.Modal)
            {
                if (ImGui::BeginPopupModal(CurrentPopup.Title.c_str(), CurrentPopup.CanClose ? (&HasPopup) : nullptr), CurrentPopup.Flag)
                {
                    CurrentPopup.Show();
                    ImGui::EndPopup();
                }
            }
            else
            {
                if (ImGui::BeginPopup(CurrentPopup.Title.c_str(), CurrentPopup.Flag))
                {
                    CurrentPopup.Show();
                    ImGui::EndPopup();
                }
            }
            if (CurrentPopup.CanClose && HPPrev && (!HasPopup))CurrentPopup.Close();
            ImGui::CloseCurrentPopup();
        }
    }
}

inline  int GetPage(int u)
{
    return (u - 1) / KeyPerPage + 1;
}


template<typename Cont>
void Browse_ShowList(const std::string& suffix, std::vector<Cont>& Ser, int* Page,const std::function<void(Cont&, int, int)>& Callback)
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
        Callback(Ser.at(On), On - RealRF + 1, On);
    }
    if (HasPrev || HasNext)
    {
        if (HasPrev)
        {
            if (ImGui::ArrowButton(("prev_" + suffix).c_str(), ImGuiDir_Left))
            {
                (*Page)--;
                if (EnableLog)
                {
                    GlobalLog.AddLog_CurTime(false);
                    GlobalLog.AddLog(("点击了上一页（" + suffix + "）按钮。").c_str());
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
            ImGui::ArrowButton(("prev_" + suffix).c_str(), ImGuiDir_Left);
            ImGui::SameLine();
            ImGui::PopStyleColor(4);
        }
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
                    GlobalLog.AddLog(("点击了下一页（" + suffix + "）按钮。").c_str());
                }
            }
            ImGui::SameLine();

        }
        ImGui::NewLine();

        if (*Page != 0)
        {
            if (ImGui::ArrowButton(("fpg_" + suffix).c_str(), ImGuiDir_Left))
            {
                (*Page) = 0;
                if (EnableLog)
                {
                    GlobalLog.AddLog_CurTime(false);
                    GlobalLog.AddLog(("点击了第一页（" + suffix + "）按钮。").c_str());
                }
            }
            ImGui::SameLine();
            ImGui::Text(u8"第一页");
            ImGui::SameLine();
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Text, { 0,0,0,0 });
            ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0,0,0,0 });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0,0,0,0 });
            ImGui::ArrowButton("OBSOLETE_BUTTON", ImGuiDir_Left);
            ImGui::SameLine();
            ImGui::PopStyleColor(4);
        }
        if ((*Page) + 1 >= 1000)ImGui::SetCursorPosX(FontHeight * 5.5f);
        else ImGui::SetCursorPosX(FontHeight * 6.0f);
        auto PosYText = ImGui::GetCursorPosY();
        ImGui::SetCursorPosY(PosYText - FontHeight * 0.5f);
        ImGui::Text(u8"第（%d/%d）页", (*Page) + 1, PageN);
        ImGui::SetCursorPosY(PosYText);
        ImGui::SameLine();
        if ((*Page) + 1 >= 1000)ImGui::SetCursorPosX(FontHeight * 13.0f);
        if ((*Page) + 1 >= 100)ImGui::SetCursorPosX(FontHeight * 12.5f);
        else ImGui::SetCursorPosX(FontHeight * 12.0f);
        if ((*Page) + 1 != PageN)
        {
            ImGui::Text(u8"最后页");
            ImGui::SameLine();
            if (ImGui::ArrowButton(("lpg_" + suffix).c_str(), ImGuiDir_Right))
            {
                (*Page) = PageN - 1;
                if (EnableLog)
                {
                    GlobalLog.AddLog_CurTime(false);
                    GlobalLog.AddLog(("点击了最后页（" + suffix + "）按钮。").c_str());
                }
            }
            ImGui::SameLine();
        }
        ImGui::NewLine();
    }
}

template<typename Cont>
void IBR_ListMenu<Cont>::RenderUI()
{
    Browse_ShowList(Tag, List, &Page, Action);
}

namespace IBR_HintManager
{
    std::vector<_TEXT_UTF8 std::string> Hint;
    int HintChangeMillis = 5000;
    int UseHint = 0;
    ETimer::TimerClass HintTimer;

    _TEXT_UTF8 std::string CustomHint;
    bool HasCustom = false, HasSet = false, IsCountDown = false;
    std::function<bool(_TEXT_UTF8 std::string&)> CustomFn;
    ETimer::TimerClass CountDownTimer;
    int CountDownMillis;

    void Clear()
    {
        HasSet = HasCustom = IsCountDown = false;
    }
    void Load()
    {
        ExtFileClass GetHint;
        if (GetHint.Open(".\\Resources\\hint.txt", "r"))
        {
            char str[MAX_STRING_LENGTH];
            while (!GetHint.Eof())
            {
                GetHint.GetStr(str, MAX_STRING_LENGTH);
                Hint.push_back(str);
            }
            GetHint.Close();
            if (!Hint.empty())Hint.erase(Hint.begin());//第一个不要！
            if (EnableLog)
            {
                GlobalLog.AddLog_CurTime(false);
                GlobalLog.AddLog("成功载入.\\Resources\\hint.txt");
            }
        }
        else
        {
            if (EnableLog)
            {
                GlobalLog.AddLog_CurTime(false);
                GlobalLog.AddLog("未能载入.\\Resources\\hint.txt");
            }
        }
    }
    void RenderUI()
    {
        ImDrawList* DList = ImGui::GetForegroundDrawList();
        if (!Hint.empty())
        {
            if (HintTimer.GetMilli() > HintChangeMillis)
            {
                UseHint = rand() % Hint.size();
                HintTimer.Set();
            }
        }
        if (HasSet && IsCountDown && CountDownTimer.GetMilli() > CountDownMillis)Clear();
        if (HasSet && HasCustom)
            if (!CustomFn(CustomHint))Clear();
        
        DList->AddRectFilled({ 0.0f,(float)IBR_UICondition::CurrentScreenHeight - FontHeight * 1.5f },
            { (float)IBR_UICondition::CurrentScreenWidth ,(float)IBR_UICondition::CurrentScreenHeight },
            ImColor(ImGui::GetStyleColorVec4(ImGuiCol_WindowBg)));
        DList->AddLine({ 0.0f,(float)IBR_UICondition::CurrentScreenHeight - FontHeight * 1.5f },
            { (float)IBR_UICondition::CurrentScreenWidth,(float)IBR_UICondition::CurrentScreenHeight - FontHeight * 1.5f },
            ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)), 1.0f);
        /*
        if (_TempSelectLink::IsInLink())
        {
            const char* Tx = u8"左键单击选择，右键取消";
            DList->AddText({ FontHeight * 0.8f,(float)IBR_UICondition::CurrentScreenHeight - FontHeight * 1.25f },
                ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Text)),
                Tx, Tx + strlen(Tx));
        }
        else*/

        if (!Hint.empty() || HasSet)
        {
            const std::string& HintStr = GetHint();
            DList->AddText({ FontHeight * 0.8f,(float)IBR_UICondition::CurrentScreenHeight - FontHeight * 1.25f },
                ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Text)),
                HintStr.c_str(), HintStr.c_str() + HintStr.size());
        }
    }
    void SetHint(_TEXT_UTF8 const std::string& Str, int TimeLimitMillis)
    {
        HasCustom = false;
        HasSet = true;
        CustomHint = Str;
        if (TimeLimitMillis != -1)
        {
            IsCountDown = true;
            CountDownMillis = TimeLimitMillis;
            CountDownTimer.Set();
        }
        else IsCountDown = false;

    }
    void SetHintCustom(const std::function<bool(_TEXT_UTF8 std::string&)>& Fn)
    {
        HasCustom = true;
        HasSet = true;
        CustomHint = "";
        IsCountDown = false;
        CustomFn = Fn;
    }
    const std::string& GetHint()
    {
        return HasSet ? CustomHint : Hint[UseHint];
    }
}


namespace IBR_SelectMode
{
    Mode CurrentMode;
    bool InSelectProcess;
    void EnterSelectMode(const Mode& Mode)
    {
        if (!InSelectProcess)
        {
            InSelectProcess = true;
            CurrentMode = Mode;
        }
    }
    void ExitSelectMode(IBR_Section Section, ImVec2 ClickRePos)
    {
        if (InSelectProcess)
        {
            InSelectProcess = false;
            CurrentMode.Exit(Section, ClickRePos);
        }
    }
    void CancelSelectMode()
    {
        if (InSelectProcess)
        {
            InSelectProcess = false;
            CurrentMode.Cancel();
        }
    }
    void RenderUI()
    {
        if (InSelectProcess)
        {
            auto& WUL = IBR_RealCenter::WorkSpaceUL, WDR = IBR_RealCenter::WorkSpaceDR;
            ImDrawList* DList = ImGui::GetForegroundDrawList();
            IBR_Project::id_t HoverId = ULLONG_MAX;
            if (CurrentMode.RestrictedToSections)
            {
                for (auto& sp : IBR_Inst_Project.IBR_SectionMap)
                {
                    if (!IBR_Inst_Project.GetSectionFromID(sp.first).HasBack())continue;
                    auto ReUL = IBR_WorkSpace::EqPosToRePos(sp.second.EqPos), ReDR = IBR_WorkSpace::EqPosToRePos(sp.second.EqPos + sp.second.EqSize);
                    auto [ok, NUL, NDR] = RectangleCross(ReUL, ReDR, WUL, WDR);
                    if (ok)DList->AddRectFilled(NUL, NDR, IBR_Color::ForegroundCoverColor);

                    if (sp.second.Hovered)HoverId = sp.first;
                }
            }
            else
            {
                DList->AddRectFilled(WUL, WDR, IBR_Color::ForegroundCoverColor);
            }
            auto& io = ImGui::GetIO();
            if (IBR_WorkSpace::InWorkSpace(io.MousePos))
            {
                DList->AddCircle(io.MousePos, 5.0f, IBR_Color::LinkingLineColor, 0, 2.0f);

                bool RC = ImGui::IsMouseClicked(ImGuiMouseButton_Right), LC = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
                if (RC && (!LC))CancelSelectMode();
                if (LC && (!RC))
                {
                    if (CurrentMode.RestrictedToSections)
                    {
                        if (HoverId != ULLONG_MAX)ExitSelectMode(IBR_Inst_Project.GetSectionFromID(HoverId), io.MousePos);
                    }
                    else ExitSelectMode(IBR_Inst_Project.GetSectionFromID(ULLONG_MAX), io.MousePos);
                }
            }
        }
    }
    bool InSelectMode()
    {
        return InSelectProcess;
    }
    const Mode& CurrentSelectMode()
    {
        return CurrentMode;
    }
}
