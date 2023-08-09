
#include "FromEngine/Include.h"
#include "IBFront.h"
#include "FromEngine/RFBump.h"
#include "Global.h"
#include "IBSave.h"

extern int ScrX;//extern everything in Browser.h

void IBF_Thr_FrontLoop()
{
    BackThreadID = GetCurrentThreadId();
    uint64_t TimeWait = GetSysTimeMicros();
    //if (EnableLog)
    //{
    //    GlobalLogB.AddLog_CurTime(false);
    //    GlobalLogB.AddLog("启动了IBF_Thr_FrontLoop。");
    //}
    while (1)
    {
        int FrameRateLim = 50;
        if (SettingLoadComplete.load())FrameRateLim = IBG_GetSetting().FrameRateLimit;
        if (FrameRateLim != -1)
        {
            int Uax = 1000000 / FrameRateLim;//单位：微秒
            while (GetSysTimeMicros() < TimeWait)Sleep(Uax / 1000);
            if (Uax > 3000 && abs((int64_t)GetSysTimeMicros() - (int64_t)ShellLoopLastTime) < 1000ull)Sleep(Uax / 2000);
            TimeWait += Uax;
        }
        
        IBRF_CoreBump.IBF_AutoProc();
    }
}

bool IBF_Setting::ReadSetting(const wchar_t* Name)
{
    //if (EnableLog)
    //{
    //    GlobalLogB.AddLog_CurTime(false);
    //    GlobalLogB.AddLog("调用了IBF_Setting::ReadSetting。");
    //}
    IBB_SettingRegisterRW(SettingFile);
    List.PackSetDefault();
    bool Ret = SettingFile.Read(Name);
    IBB_SetGlobalSetting(List.Pack);
    SettingLoadComplete.store(true);
    return Ret;
}

bool IBF_Setting::SaveSetting(const wchar_t* Name)
{
    bool Ret=SettingFile.Write(Name);
    SettingSaveComplete.store(true);
    return Ret;
}

const std::unordered_map<IBB_SettingType::_Type, std::function<IBF_SettingType(const IBB_SettingType&)>> UploadSettingMap =
{
    {IBB_SettingType::None,[](const IBB_SettingType& BSet)->IBF_SettingType
    {
        IBF_SettingType Ret;
        Ret.DescLong = BSet.DescLong;
        Ret.Action = [=]() -> bool
        {
            ImGui::Separator();
            ImGui::TextDisabled(BSet.DescShort.c_str());
            return ImGui::IsItemHovered();
        };
        return Ret;
    }},
    {IBB_SettingType::IntA,[](const IBB_SettingType& BSet)->IBF_SettingType
    {
        IBF_SettingType Ret;
        Ret.DescLong = BSet.DescLong;
        Ret.Action = [=]() -> bool 
        {
            int TempSet = *((int*)BSet.Data), MedInt;
            ImGui::InputInt(BSet.DescShort.c_str(), &TempSet, 10, 200);
            bool Drt = ImGui::IsItemHovered();
            MedInt = TempSet;
            bool a = false;
            for (int i = 1; i < (int)BSet.Limit.size(); i += 2)
            {
                if (*((const int*)BSet.Limit.at(i)) <= MedInt && MedInt <= *((const int*)BSet.Limit.at(i + 1)))
                {
                    a = true; break;
                }
            }
            if (!a)MedInt = *((const int*)BSet.Limit.at(0));
            *((int*)BSet.Data) = MedInt;
            TempSet = MedInt;
            return Drt;
        };
        return Ret;
    }},
};

void IBF_Setting::UploadSettingBoard(std::function<void(const std::vector<IBF_SettingType>&)> Callback)
{
    std::vector<IBF_SettingType> Ret;

    for (auto Ty : List.Types)
    {
        Ret.push_back(UploadSettingMap.at(Ty.Type)(Ty));
    }

    
    IBRF_CoreBump.IBF_SendToR({ [=]()
        {
            auto CRet = Ret;
            Callback(CRet);
        }
    ,nullptr });
}











bool IBF_DefaultTypeList::ReadSetting(const char* Name)
{
    bool Ret = true;
    if (!List.LoadFromFile(Name))Ret = false;
    if (!List.BuildQuery())Ret = false;
    return Ret;
}

bool IBF_DefaultModuleList::ReadSetting(const char* Name)
{
    return List.LoadFromFile(Name);
}



IBB_Module_Default* IBF_DefaultModuleList::GetModule(const IBF_DefaultModuleList_Index& Module)
{
    if (Module.UseIndex)
    {
        if (Module.Index >= 0 && Module.Index < (int)List.Module_Default.size())
        {
            return std::addressof(List.Module_Default.at(Module.Index));
        }
        else return nullptr;
    }
    else
    {
        auto It = List.Module_Default_ByName.find(Module.Name);
        if (It == List.Module_Default_ByName.end())return nullptr;
        else return std::addressof(List.Module_Default.at(It->second));
    }
}

bool IBF_Project::AddModule(const IBF_DefaultModuleList_Index& Module, const IBF_DefaultModuleList_Parameter& Param)
{
    return AddModule(IBF_Inst_DefaultModuleList.GetModule(Module), Param);
}
bool IBF_Project::AddModule(const IBB_Module_Default* Module, const IBF_DefaultModuleList_Parameter& Par)
{
    bool Ret = true;
    IBB_Module Mod;
    if (!Mod.Generate(*Module, Par.Param, Par.MergeDesc))return false;
    if (!Project.AddModule(Mod))Ret = false;
    if (!Project.UpdateAll())Ret = false;
    return Ret;
}

_TEXT_UTF8 std::string IBF_Project::GetText(bool PrintExtraData) const
{
    return Project.GetText(PrintExtraData);
}

bool IBF_Project::UpdateAll()
{
    return Project.UpdateAll();
}

bool IBF_Project::UpdateCreateSection(const IBB_Section_Desc& Desc)
{
    IBB_Section* pSec = const_cast<IBB_Section*>(Project.GetSec(Desc));
    if (pSec == nullptr)return false;
    bool Ret = true;
    for (auto& Ini : Project.Inis)for (auto& sp : Ini.Secs)
    {
        if (sp.second.IsLinkGroup)
        {
            for(auto& l:sp.second.LinkGroup_LinkTo)//Index不可能用于指向一个尚未存在的Section
                if ((!l.To.Ini.UseIndex && l.To.Ini.Name == Desc.Ini) && (!l.To.Section.UseIndex && l.To.Section.Name == Desc.Sec))
                {
                    l.DynamicCheck_Legal(Project);
                    pSec->LinkedBy.push_back(l);
                    auto& b = pSec->LinkedBy.back();
                    b.FillData(&l, l.FromKey);
                    l.Another = &b;
                }
        }
        else
        {
            for (auto& ss : sp.second.SubSecs)
            {
                for (auto l : ss.LinkTo)
                    if ((!l.To.Ini.UseIndex && l.To.Ini.Name == Desc.Ini) && (!l.To.Section.UseIndex && l.To.Section.Name == Desc.Sec))
                    {
                        l.DynamicCheck_Legal(Project);
                        pSec->LinkedBy.push_back(l);
                        auto& b = pSec->LinkedBy.back();
                        b.FillData(&l, l.FromKey);
                        l.Another = &b;
                    }
            }
        }
    }
    return Ret;
}

void IBF_Project::Load(const IBS_Project& Proj)
{
    Project.CreateVersionMajor = Proj.Front.CreateVersionMajor;
    Project.CreateVersionMinor = Proj.Front.CreateVersionMinor;
    Project.CreateVersionRelease = Proj.Front.CreateVersionRelease;
    Project.CreateTime = Proj.Front.CreateTime;
    Project.LastUpdate = GetSysTimeMicros();
    Project.ProjName = Proj.Front.ProjName;
    Project.Path = Proj.Front.Path;
    Project.IsNewlyCreated = false;
    Project.ChangeAfterSave = false;
    Project.AddModule(Proj.Front.ProjectSecs);
    Project.UpdateAll();
}

void IBF_Project::Save(IBS_Project& Proj)
{
    Proj.Front.CreateVersionMajor = Project.CreateVersionMajor;
    Proj.Front.CreateVersionMinor = Project.CreateVersionMinor;
    Proj.Front.CreateVersionRelease = Project.CreateVersionRelease;
    Proj.Front.CreateTime = Project.CreateTime;
    Project.LastUpdate = GetSysTimeMicros();
    Proj.Front.ProjName = Project.ProjName;
    Proj.Front.Path = Project.Path;
    Project.IsNewlyCreated = false;
    Project.ChangeAfterSave = false;
    Project.PackModule(Proj.Front.ProjectSecs);
    //::MessageBoxA(NULL, "66A", "Title", MB_OK);
}
