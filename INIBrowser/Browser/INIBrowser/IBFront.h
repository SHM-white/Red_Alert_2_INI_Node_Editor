#pragma once

#include "IBBack.h"
#include "SaveFile.h"

void IBF_Thr_FrontLoop();

struct IBF_SettingType
{
    std::string DescLong;
    std::function<bool()> Action;
};

class IBF_Setting
{
    SaveFile SettingFile;
public:
    IBB_SettingTypeList List;
    bool ReadSetting(const wchar_t* Name);
    bool SaveSetting(const wchar_t* Name);
    void UploadSettingBoard(std::function<void(const std::vector<IBF_SettingType>&)> Callback);
};









struct IBF_DefaultTypeList
{
    IBB_DefaultTypeList List;
    bool ReadSetting(const char* Name);
};

struct IBF_DefaultModuleList_Index
{
    bool UseIndex;
    int Index;
    std::string Name;

    IBF_DefaultModuleList_Index() {}
    IBF_DefaultModuleList_Index(int i) : UseIndex(true),Index(i) {}
    IBF_DefaultModuleList_Index(const std::string& N) : UseIndex(false), Name(N) {}
};

struct IBF_DefaultModuleList_Parameter
{
    std::vector<std::string> Param;
    IBB_Section_Desc MergeDesc;
};

struct IBF_DefaultModuleList
{
    IBB_DefaultModuleList List;
    bool ReadSetting(const char* Name);
    IBB_Module_Default* GetModule(const IBF_DefaultModuleList_Index& Module);
};

struct IBS_Project;

struct IBF_Project
{
    IBB_Project Project;
    uint32_t CurrentProjectRID;

    bool AddModule(const IBF_DefaultModuleList_Index& Module, const IBF_DefaultModuleList_Parameter& Param);
    bool AddModule(const IBB_Module_Default* Module, const IBF_DefaultModuleList_Parameter& Param);

    bool UpdateAll();
    bool UpdateCreateSection(const IBB_Section_Desc& Desc);

    void Load(const IBS_Project&);
    void Save(IBS_Project&);

    _TEXT_UTF8 std::string GetText(bool PrintExtraData) const;
};



