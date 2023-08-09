
#include "FromEngine/Include.h"
#include "FromEngine/global_tool_func.h"
#include "IBBack.h"
#include "Global.h"

const char* LinkGroup_IniName = "_LINKGROUP_INI_FILE";


template<typename Str>
bool IBB_TDIndex<Str>::Load(JsonObject FromJson)
{
    auto Item = FromJson.GetObjectItem(u8"Index");
    if (Item.IsTypeString())
    {
        UseIndex = false;
        Name = Item.GetString();
        return true;
    }
    else if (Item.IsTypeNumber())
    {
        UseIndex = true;
        Index = Item.GetInt();
        return true;
    }
    else
    {
        return false;
    }
}

template<typename Str> template<typename T>
typename std::vector<T>::iterator IBB_TDIndex<Str>::Search(std::vector<T>& Source, bool CanUseByName, bool Update, const std::function<Str(const T&)>& GetName)
{
    if (!UseIndex)
    {
        if (CanUseByName)
        {
            int i = 0;
            for (const auto& It : Source)
            {
                if (GetName(It) == Name)break;
                i++;
            }
            if (Update)Index = i;
            return Source.begin() + i;
        }
        else
        {
            Index = Source.size();
            return Source.end();
        }
    }
    else
    {
        typename std::vector<T>::iterator Data = Source.begin() + Index;
        if (Update)Name = GetName(*Data);
        return Data;
    }
}
template<typename Str> template<typename T>
typename std::unordered_map<Str, T>::iterator IBB_TDIndex<Str>::Search(std::unordered_map<Str, T>& Source, bool CanUseByName, bool Update)
{
    if (!UseIndex)
    {
        if (CanUseByName)
        {
            auto Ret = Source.find(Name);
            if (Update)
            {
                if (Ret != Source.end())
                {
                    int i = 0;
                    for (const auto& It : Source)
                    {
                        if (It.first == Name)break;
                        i++;
                    }
                    Index = i;
                }
                else Index = Source.size();
            }
            return Ret;
        }
        else
        {
            Index = Source.size();
            return Source.end();
        }
    }
    else
    {
        int i = 0;
        Str Nr{};
        for (const auto& It : Source)
        {
            if (Index == i)
            {
                Nr = It.first;
                break;
            }
            i++;
        }
        if (Update && !Nr.empty())Name = Nr;
        return Source.find(Nr);
    }
}

template<typename Str> template<typename T>
typename std::vector<T>::iterator IBB_TDIndex<Str>::Search(std::vector<T>& Source, bool CanUseByName, const std::function<Str(const T&)>& GetName) const
{
    if (!UseIndex)
    {
        if (CanUseByName)
        {
            int i = 0;
            for (const auto& It : Source)
            {
                if (GetName(It) == Name)break;
                i++;
            }
            return Source.begin() + i;
        }
        else return Source.end();
    }
    else
    {
        typename std::vector<T>::iterator Data = Source.begin() + Index;
        return Data;
    }
}
template<typename Str> template<typename T>
typename std::unordered_map<Str, T>::iterator IBB_TDIndex<Str>::Search(std::unordered_map<Str, T>& Source, bool CanUseByName) const
{
    if (!UseIndex)
    {
        if (CanUseByName)
        {
            auto Ret = Source.find(Name);
            return Ret;
        }
        else return Source.end();
    }
    else
    {
        int i = 0;
        Str Nr{};
        for (const auto& It : Source)
        {
            if (Index == i)
            {
                Nr = It.first;
                break;
            }
            i++;
        }
        return Source.find(Nr);
    }
}

template<typename Str> template<typename T>
typename std::vector<T>::const_iterator IBB_TDIndex<Str>::Search(const std::vector<T>& Source, bool CanUseByName, bool Update, const std::function<Str(const T&)>& GetName)
{
    if (!UseIndex)
    {
        if (CanUseByName)
        {
            int i = 0;
            for (const auto& It : Source)
            {
                if (GetName(It) == Name)break;
                i++;
            }
            if (Update)Index = i;
            return Source.cbegin() + i;
        }
        else
        {
            Index = Source.size();
            return Source.cend();
        }
    }
    else
    {
        typename std::vector<T>::const_iterator Data = Source.cbegin() + Index;
        if (Update)Name = GetName(*Data);
        return Data;
    }
}
template<typename Str> template<typename T>
typename std::unordered_map<Str, T>::const_iterator IBB_TDIndex<Str>::Search(const std::unordered_map<Str, T>& Source, bool CanUseByName, bool Update)
{
    if (!UseIndex)
    {
        if (CanUseByName)
        {
            auto Ret = Source.find(Name);
            if (Update)
            {
                if (Ret != Source.cend())
                {
                    int i = 0;
                    for (const auto& It : Source)
                    {
                        if (It.first == Name)break;
                        i++;
                    }
                    Index = i;
                }
                else Index = Source.size();
            }
            return Ret;
        }
        else
        {
            Index = Source.size();
            return Source.cend();
        }
    }
    else
    {
        int i = 0;
        Str Nr{};
        for (const auto& It : Source)
        {
            if (Index == i)
            {
                Nr = It.first;
                break;
            }
            i++;
        }
        if (Update && !Nr.empty())Name = Nr;
        return Source.find(Nr);
    }
}

template<typename Str> template<typename T>
typename std::vector<T>::const_iterator IBB_TDIndex<Str>::Search(const std::vector<T>& Source, bool CanUseByName, const std::function<Str(const T&)>& GetName) const
{
    if (!UseIndex)
    {
        if (CanUseByName)
        {
            int i = 0;
            for (const auto& It : Source)
            {
                if (GetName(It) == Name)break;
                i++;
            }
            return Source.cbegin() + i;
        }
        else return Source.cend();
    }
    else
    {
        typename std::vector<T>::const_iterator Data = Source.cbegin() + Index;
        return Data;
    }
}
template<typename Str> template<typename T>
typename std::unordered_map<Str, T>::const_iterator IBB_TDIndex<Str>::Search(const std::unordered_map<Str, T>& Source, bool CanUseByName) const
{
    if (!UseIndex)
    {
        if (CanUseByName)
        {
            auto Ret = Source.find(Name);
            return Ret;
        }
        else return Source.cend();
    }
    else
    {
        int i = 0;
        Str Nr{};
        for (const auto& It : Source)
        {
            if (Index == i)
            {
                Nr = It.first;
                break;
            }
            i++;
        }
        return Source.find(Nr);
    }
}

std::string IBB_DIndex::GetText() const
{
    return UseIndex ? ("<IDX:" + std::to_string(Index) + ">") : Name;
}










IBG_SettingPack GlobalSettingPack;


bool ReadSettingFileGen(const ExtFileClass& File, int Order);
bool WriteSettingFileGen(const ExtFileClass& File, int Order);

ReadFileHeader IBB_RSetting
{
    "IBB_Setting_F_200" ,
     [](const ExtFileClass& File,int FVersion,int Length)-> bool
    {
        if (EnableLog)
        {
            GlobalLogB.AddLog_CurTime(false);
            GlobalLogB.AddLog("调用了IBB_RSetting.Action来读入设置。");
        }
        (void)Length;
        if (FVersion > 200)
        {
            return ReadSettingFileGen(File, 0);
        }
        return false;
    }
};

WriteFileHeader IBB_WSetting
{
    "IBB_Setting_F_200" ,
     [](const ExtFileClass& File)-> bool
     {
        /*
        if (EnableLog)
        {
            GlobalLogB.AddLog_CurTime(false);
            GlobalLogB.AddLog("调用了IBB_WSetting.Action。");
        }*/
        return WriteSettingFileGen(File, 0);
     }
};

const std::unordered_map<IBB_SettingType::_Type, std::function<bool(const IBB_SettingType&, const ExtFileClass&)>> ReadTypeMap =
{
    {IBB_SettingType::None,[](const IBB_SettingType& Ty, const ExtFileClass& File) -> bool
    {
        (void)Ty; (void)File; return true;
    }},
    {IBB_SettingType::IntA,[](const IBB_SettingType& Ty, const ExtFileClass& File) -> bool
    {
        return File.ReadData(*((int32_t*)Ty.Data));
    }},
};

const std::unordered_map<IBB_SettingType::_Type, std::function<bool(const IBB_SettingType&, const ExtFileClass&)>> WriteTypeMap =
{
    {IBB_SettingType::None,[](const IBB_SettingType& Ty, const ExtFileClass& File) -> bool
    {
        (void)Ty; (void)File; return true;
    }},
    {IBB_SettingType::IntA,[](const IBB_SettingType& Ty, const ExtFileClass& File) -> bool
    {
        return File.WriteData(*((int32_t*)Ty.Data));
    }},
};

std::vector<int> RW_ReadOrder[1] =
{
    {0,1,2},
};
std::vector<int> RW_WriteOrder[1] =
{
    {0,1,2},
};


void IBB_SettingRegisterRW(SaveFile& Save)
{
    Save.ReadBlockProcess[IBB_RSetting.UsingID] = IBB_RSetting;
    Save.WriteBlockProcess.push_back(IBB_WSetting);
}

bool ReadSettingFileGen(const ExtFileClass& File, int Order)
{
    for (int Tg : RW_ReadOrder[Order])
    {
        auto& Ty = IBF_Inst_Setting.List.Types[Tg];
        if (!ReadTypeMap.at(Ty.Type)(Ty, File))
        {
            return false;
        }
    }return true;
}
bool WriteSettingFileGen(const ExtFileClass& File, int Order)
{
    for (int Tg : RW_WriteOrder[Order])
    {
        auto& Ty = IBF_Inst_Setting.List.Types[Tg];
        if (!WriteTypeMap.at(Ty.Type)(Ty, File))
        {
            return false;
        }
    }return true;
}

IBB_SettingTypeList::IBB_SettingTypeList()
{
    Types = {
        {
            IBB_SettingType::IntA,
                u8"帧率限制", u8"限制使用的帧率以节约CPU\n范围15~2000\n输入-1则不限制\n默认25",
                (void*)&Pack.FrameRateLimit,
            {
                (const void*)&IBG_SettingPack::____FrameRateLimit_Def,
                (const void*)&IBG_SettingPack::____FrameRateLimit_Min,(const void*)&IBG_SettingPack::____FrameRateLimit_Max,
                (const void*)&IBG_SettingPack::____FrameRateLimit_SpV,(const void*)&IBG_SettingPack::____FrameRateLimit_SpV,
            }
        },
        {
            IBB_SettingType::IntA,
                u8"字体大小",u8"使用的字号\n范围12~28\n默认16",
                (void*)&Pack.FontSize,
            {
                (const void*)&IBG_SettingPack::____FontSize_Def,
                (const void*)&IBG_SettingPack::____FontSize_Min,(const void*)&IBG_SettingPack::____FontSize_Max,
            }
        },
        {
            IBB_SettingType::IntA,
                u8"菜单每页条目",u8"翻页菜单每页的条目数\n范围5~∞\n默认10",
                (void*)&Pack.MenuLinePerPage,
            {
                (const void*)&IBG_SettingPack::____MenuLinePerPage_Def,
                (const void*)&IBG_SettingPack::____MenuLinePerPage_Min,(const void*)&IBG_SettingPack::____MenuLinePerPage_Max,
            }
        },
    };
}

void IBB_SettingTypeList::PackSetDefault()
{
    Pack.SetDefault();
}

const IBG_SettingPack& IBG_GetSetting()
{
    return GlobalSettingPack;
}

void IBB_SetGlobalSetting(const IBG_SettingPack& Pack)
{
    GlobalSettingPack = Pack;
}

/*
版本拓展：

ReadFileHeader IBB_RSetting2
{
    "IBB_Setting_F_201" ,
     [](const ExtFileClass& File,int Version,int Length)-> bool
    {
        if (Version > 201)
        {
            return ReadSettingFileGen(File, 1);
        }
        return false;
    }
};

WriteFileHeader IBB_WSetting2
{
    "IBB_Setting_F_201" ,
     [](const ExtFileClass& File)-> bool
    {
        return ReadSettingFileGen(File, 1);
    }
};

void IBB_SettingRegisterRW(SaveFile& Save)
{
    Save.ReadBlockProcess[IBB_RSetting.UsingID] = IBB_RSetting;
    Save.WriteBlockProcess.push_back(IBB_WSetting);
    Save.ReadBlockProcess[IBB_RSetting2.UsingID] = IBB_RSetting;
    Save.WriteBlockProcess.push_back(IBB_WSetting2);
}

*/















struct IBB_IniLine_Data_Base
{
    bool _Empty{ true };
    bool Empty() { return _Empty; }

    virtual bool SetValue(const std::string&) = 0;
    virtual bool MergeValue(const std::string&) = 0;
    virtual std::string GetString() = 0;

    virtual const char* GetName() = 0;

    IBB_IniLine_Data_Base() {}
    ~IBB_IniLine_Data_Base() {}
};
struct IBB_IniLine_Data_Int : public IBB_IniLine_Data_Base
{
    int Value{};

    IBB_IniLine_Data_Int() {}

    bool SetValue(const std::string& Val)
    {
        if (Val.empty())
        {
            _Empty = true;
            return true;
        }
        try
        {
            Value = std::stoi(Val);
            _Empty = false;
            return true;
        }
        catch (std::invalid_argument const& e)
        {
            (void)e;
            if (EnableLog)
            {
                GlobalLogB.AddLog_CurTime(false);
                GlobalLogB.AddLog("IBB_IniLine_Data_Int::SetValue ： std::stoi : invalid_argument");
            }
            _Empty = true;
            return false;
        }
        catch(std::out_of_range const& e)
        {
            (void)e;
            if (EnableLog)
            {
                GlobalLogB.AddLog_CurTime(false);
                GlobalLogB.AddLog("IBB_IniLine_Data_Int::SetValue ： std::stoi : out_of_range");
            }
            _Empty = true;
            return false;
        }
    }
    bool MergeValue(const std::string& Val){ return SetValue(Val); }
    bool MergeData(const IBB_IniLine_Data_Int* Data)
    {
        if (Data == nullptr)return false;
        if (Data->_Empty)return true;
        Value = Data->Value;
        return true;
    }
    bool Clear()
    {
        _Empty = true;
        return true;
    }

    std::string GetString() { return _Empty ? "" : std::to_string(Value); }

    typedef int type;
    typedef int alt_type;
    type GetValue() { return _Empty ? 0 : Value; }
    alt_type GetAltValue() { return _Empty ? 0 : Value; }
    const char* GetName() { return "Int"; }

    ~IBB_IniLine_Data_Int() {}
};
struct IBB_IniLine_Data_Double : public IBB_IniLine_Data_Base
{
    double Value{};

    IBB_IniLine_Data_Double() {}

    bool SetValue(const std::string& Val)
    {
        if (Val.empty())
        {
            _Empty = true;
            return true;
        }
        try
        {
            Value = std::stod(Val);
            _Empty = false;
            return true;
        }
        catch (std::invalid_argument const& e)
        {
            (void)e;
            if (EnableLog)
            {
                GlobalLogB.AddLog_CurTime(false);
                GlobalLogB.AddLog("IBB_IniLine_Data_Double::SetValue ： std::stod : invalid_argument");
            }
            _Empty = true;
            return false;
        }
        catch (std::out_of_range const& e)
        {
            (void)e;
            if (EnableLog)
            {
                GlobalLogB.AddLog_CurTime(false);
                GlobalLogB.AddLog("IBB_IniLine_Data_Double::SetValue ： std::stod : out_of_range");
            }
            _Empty = true;
            return false;
        }
    }
    bool MergeValue(const std::string& Val) { return SetValue(Val); }
    bool MergeData(const IBB_IniLine_Data_Double* Data)
    {
        if (Data == nullptr)return false;
        if (Data->_Empty)return true;
        Value = Data->Value;
        return true;
    }
    bool Clear()
    {
        _Empty = true;
        return true;
    }

    std::string GetString() { return _Empty ? "" : std::to_string(Value); }

    typedef double type;
    typedef double alt_type;
    type GetValue() { return _Empty ? 0.0 : Value; }
    alt_type GetAltValue() { return _Empty ? 0.0 : Value; }
    const char* GetName() { return "Double"; }

    ~IBB_IniLine_Data_Double() {}
};
struct IBB_IniLine_Data_String : public IBB_IniLine_Data_Base
{
    std::string Value{};

    IBB_IniLine_Data_String() {}

    bool SetValue(const std::string& Val)
    {
        Value = Val;
        _Empty = Val.empty();
        return true;
    }
    bool MergeValue(const std::string& Val) { return SetValue(Val); }
    bool MergeData(const IBB_IniLine_Data_String* Data)
    {
        if (Data == nullptr)return false;
        if (Data->_Empty)return true;
        Value = Data->Value;
        return true;
    }
    bool Clear()
    {
        _Empty = true;
        Value.clear();
        return true;
    }

    std::string GetString() { return Value; }

    typedef std::string type;
    typedef std::string alt_type;
    type GetValue() { return Value; }
    alt_type GetAltValue() { return Value; }
    const char* GetName() { return "String"; }

    ~IBB_IniLine_Data_String() {}
};

std::unordered_map<const void*, int> IBB_IniLine_AllocationMap;
void IBB_IniLine_AllocationMap_Destroy()
{
    int i = 0;
    for (auto& p : IBB_IniLine_AllocationMap)
        if (p.second) { ++i; delete p.first; }
    if (EnableLog)
    {
        GlobalLogB.AddLog_CurTime(false);
        GlobalLogB.AddLog(("IBB_IniLine_AllocationMap_Destroy ： 捕获了" + std::to_string(i) + "个未回收IniLine。").c_str());
    }
};

template<typename T>
IBB_IniLine_Process IBB_GetDefaultIniLineProcess()
{
    IBB_IniLine_Process Ret;
    Ret.Allocate = [](void)->void* { void* x = (void*)(new T); ++IBB_IniLine_AllocationMap[x]; return x; };
    Ret.Duplicate = [](const void* Data)->void* { T* ano = new T; if (Data != nullptr)*ano = *(T*)Data; return (void*)ano;  };
    Ret.SetValue = [](const void* Data, const std::string& Str)->bool { return ((T*)Data)->SetValue(Str); };
    Ret.MergeValue = [](const void* Data, const std::string& Str)->bool { return ((T*)Data)->MergeValue(Str); };
    Ret.MergeData = [](const void* Data, const void* From)->bool { return ((T*)Data)->MergeData((const T*)From); };
    Ret.Empty = [](const void* Data)->bool { return ((T*)Data)->Empty(); };
    Ret.Clear = [](const void* Data)->bool { return ((T*)Data)->Clear(); };
    Ret.GetString = [](const void* Data)->std::string { return ((T*)Data)->GetString(); };
    Ret.Destroy = [](const void* Data)->void {
        int& i = --IBB_IniLine_AllocationMap[Data];
        if (i <= 0) { IBB_IniLine_AllocationMap.erase(Data); delete ((T*)Data); } };
    return Ret;
}
//TODO: More Types
const std::unordered_map<std::string, IBB_IniLine_Process> IBB_IniLine_ProcessMap
{
    {
        "Int",
        IBB_GetDefaultIniLineProcess<IBB_IniLine_Data_Int>()
    },
    {
        "String",
        IBB_GetDefaultIniLineProcess<IBB_IniLine_Data_String>()
    },
    {
        "Link",
        IBB_GetDefaultIniLineProcess<IBB_IniLine_Data_String>()
    },
};

bool IBB_IniLine_Default::Load(JsonObject FromJson)
{
    bool Ret = true;

    Platform = FromJson.GetObjectItem(u8"Platform").GetArrayString();
    Name = FromJson.GetObjectItem(u8"Name").GetString();
    DescShort = FromJson.GetObjectItem(u8"DescShort").GetString();
    DescLong = FromJson.GetObjectItem(u8"DescLong").GetString();

    auto JLimit = FromJson.GetObjectItem(u8"Limit");
    Limit.Type = JLimit.GetObjectItem(u8"Type").GetString();
    Limit.Lim = JLimit.GetObjectItem(u8"Data").GetString();

    auto JProp = FromJson.GetObjectItem(u8"Prop");
    Property.Type = JProp.GetObjectItem(u8"Type").GetString();
    auto It = IBB_IniLine_ProcessMap.find(Property.Type);
    if (It == IBB_IniLine_ProcessMap.end())
    {
        if (EnableLog)
        {
            GlobalLogB.AddLog_CurTime(false);
            sprintf_s(LogBufB, "IBB_IniLine_Default::Load ： IniLine \"%s\" 的Property.Type使用了不存在的类型 \"%s\"。"
                , Name.c_str(), Property.Type.c_str());
            GlobalLogB.AddLog(LogBufB);
        }
        It = IBB_IniLine_ProcessMap.find("String");//string does exist
        Ret = false;
        Property.Proc = nullptr;
    }
    else Property.Proc = (IBB_IniLine_Process*)std::addressof(It->second);
    Property.Lim= JProp.GetObjectItem(u8"Restriction");

    auto JRequire = FromJson.GetObjectItem(u8"Require");
    Require.RequiredNames = JRequire.GetObjectItem(u8"RequiredNames").GetArrayString();
    Require.ForbiddenNames = JRequire.GetObjectItem(u8"ForbiddenNames").GetArrayString();
    Require.RequiredValues = JRequire.GetObjectItem(u8"RequiredValues").GetArrayObject();
    Require.ForbiddenValues = JRequire.GetObjectItem(u8"ForbiddenValues").GetArrayObject();

    return Ret;
}

bool IBB_SubSec_Default::Load(JsonObject FromJson, const std::unordered_map<std::string, IBB_IniLine_Default>& LineMap)
{
    Platform = FromJson.GetObjectItem(u8"Platform").GetArrayString();
    Name = FromJson.GetObjectItem(u8"Name").GetString();
    DescShort = FromJson.GetObjectItem(u8"DescShort").GetString();
    DescLong = FromJson.GetObjectItem(u8"DescLong").GetString();

    Require.RequiredValues = FromJson.GetObjectItem(u8"RequiredValues").GetArrayObject();
    Require.ForbiddenValues = FromJson.GetObjectItem(u8"ForbiddenValues").GetArrayObject();
    Lines_ByName = FromJson.GetObjectItem(u8"Lines").GetArrayString();
    for (const auto& s : Lines_ByName)Lines.insert({ s,LineMap.at(s) });

    return true;
}

bool IBB_Link_Default::Load(JsonObject FromJson)
{
    LinkFromRequired = FromJson.GetObjectItem(u8"LinkFromRequired").GetArrayObject();
    LinkFromForbidden = FromJson.GetObjectItem(u8"LinkFromForbidden").GetArrayObject();
    LinkToRequired = FromJson.GetObjectItem(u8"LinkToRequired").GetArrayObject();
    LinkToForbidden = FromJson.GetObjectItem(u8"LinkToForbidden").GetArrayObject();
    Name = FromJson.GetObjectItem(u8"Name").GetString();
    return true;
}

bool IBB_DefaultTypeList::LoadAfterParse()
{
    if (!RootJson.Available())
    {
        if (EnableLog)
        {
            GlobalLogB.AddLog_CurTime(false);
            GlobalLogB.AddLog("IBB_DefaultTypeList::LoadAfterParse ： JSON读取故障。");
        }
        return false;
    }
    auto Restr_List = RootJson.GetObj().GetObjectItem(u8"Require_List");
    auto IniLine_List = RootJson.GetObj().GetObjectItem(u8"IniLine_List");
    auto SubSec_List = RootJson.GetObj().GetObjectItem(u8"SubSec_List");
    auto Link_List = RootJson.GetObj().GetObjectItem(u8"Link_List");

    bool Ret = true;

    for (auto Obj : Restr_List.GetArrayObject())
    {
        auto M = Require_Default.insert({ Obj.GetObjectItem(u8"Name").GetString(),Obj.GetObjectItem(u8"Require") });
    }
    for (auto Obj : IniLine_List.GetArrayObject())
    {
        auto M = IniLine_Default.insert({ Obj.GetObjectItem(u8"Name").GetString(),{} });
        if (!M.first->second.Load(Obj))Ret = false;
    }
    for (auto Obj : SubSec_List.GetArrayObject())
    {
        auto M = SubSec_Default.insert({ Obj.GetObjectItem(u8"Name").GetString(),{} });
        if (!M.first->second.Load(Obj, IniLine_Default))Ret = false;
    }
    for (auto Obj : Link_List.GetArrayObject())
    {
        auto M = Link_Default.insert({ Obj.GetObjectItem(u8"Name").GetString(),{} });
        if (!M.first->second.Load(Obj))Ret = false;
    }

    if (EnableLog)
    {
        GlobalLogB.AddLog_CurTime(false);
        GlobalLogB.AddLog("IBB_DefaultTypeList::LoadAfterParse ： JSON读取完毕。");
        GlobalLogB.AddLog_CurTime(false);
        GlobalLogB.AddLog("IBB_DefaultTypeList::LoadAfterParse ： 返回", false);
        GlobalLogB.AddLog(IBD_BoolStr(Ret));
    }

    return Ret;
}

bool IBB_DefaultTypeList::LoadFromString(const std::string& FromString)
{
    if (EnableLog)
    {
        GlobalLogB.AddLog_CurTime(false);
        GlobalLogB.AddLog("IBB_DefaultTypeList::LoadFromString ： JSON开始读取。");
    }

    RootJson.Clear();
    RootJson.Parse(FromString);

    return LoadAfterParse();
}

bool IBB_DefaultTypeList::LoadFromFile(const std::string& FromFile)
{
    if (EnableLog)
    {
        GlobalLogB.AddLog_CurTime(false);
        GlobalLogB.AddLog("IBB_DefaultTypeList::LoadFromFile ： JSON开始读取。");
    }

    RootJson.Clear();
    RootJson.ParseFromFile(FromFile.c_str());

    return LoadAfterParse();
}
bool IBB_VariableList::Load(JsonObject FromJson)
{
    Value = FromJson.GetMapString();
    return true;
}


bool operator<(const IBB_Section_Desc& A, const IBB_Section_Desc& B) { return (A.Ini < B.Ini) ? true : (A.Sec < B.Sec); }

IBB_Ini* IBB_Project_Index::GetIni(IBB_Project& Proj)
{
    if (EnableLogEx) { GlobalLogB.AddLog_CurTime(false); GlobalLogB.AddLog("IBB_Project_Index::GetIni ：Func I Idx=", false); GlobalLogB.AddLog(GetText().c_str()); }
    auto Iter = Ini.Search<IBB_Ini>(Proj.Inis, true, true, [](const IBB_Ini& F) {return F.Name; });
    return (Iter == Proj.Inis.end()) ? nullptr : std::addressof(*Iter);
}
IBB_Section* IBB_Project_Index::GetSec(IBB_Project& Proj)
{
    if (EnableLogEx) { GlobalLogB.AddLog_CurTime(false); GlobalLogB.AddLog("IBB_Project_Index::GetSec ：Func I Idx=", false); GlobalLogB.AddLog(GetText().c_str()); }
    auto Iter = Ini.Search<IBB_Ini>(Proj.Inis, true, true, [](const IBB_Ini& F) {return F.Name; });
    if (Iter == Proj.Inis.end())return nullptr;
    auto Iter1 = Section.Search(Iter->Secs, true, false);
    return (Iter1 == Iter->Secs.end()) ? nullptr : std::addressof(Iter1->second);
}
IBB_Ini* IBB_Project_Index::GetIni(IBB_Project& Proj) const
{
    if (EnableLogEx) { GlobalLogB.AddLog_CurTime(false); GlobalLogB.AddLog("IBB_Project_Index::GetIni ：Func II Idx=", false); GlobalLogB.AddLog(GetText().c_str()); }
    auto Iter = Ini.Search<IBB_Ini>(Proj.Inis, true, [](const IBB_Ini& F) {return F.Name; });
    return (Iter == Proj.Inis.end()) ? nullptr : std::addressof(*Iter);
}
IBB_Section* IBB_Project_Index::GetSec(IBB_Project& Proj) const
{
    if (EnableLogEx) { GlobalLogB.AddLog_CurTime(false); GlobalLogB.AddLog("IBB_Project_Index::GetSec ：Func II Idx=", false); GlobalLogB.AddLog(GetText().c_str()); }
    auto Iter = Ini.Search<IBB_Ini>(Proj.Inis, true, [](const IBB_Ini& F) {return F.Name; });
    if (Iter == Proj.Inis.end())return nullptr;
    auto Iter1 = Section.Search(Iter->Secs, true);
    return (Iter1 == Iter->Secs.end()) ? nullptr : std::addressof(Iter1->second);
}
const IBB_Ini* IBB_Project_Index::GetIni(const IBB_Project& Proj)
{
    if (EnableLogEx) { GlobalLogB.AddLog_CurTime(false); GlobalLogB.AddLog("IBB_Project_Index::GetIni ：Func III Idx=", false); GlobalLogB.AddLog(GetText().c_str()); }
    auto Iter = Ini.Search<IBB_Ini>(Proj.Inis, true, true, [](const IBB_Ini& F) {return F.Name; });
    return (Iter == Proj.Inis.end()) ? nullptr : std::addressof(*Iter);
}
const IBB_Section* IBB_Project_Index::GetSec(const IBB_Project& Proj)
{
    if (EnableLogEx) { GlobalLogB.AddLog_CurTime(false); GlobalLogB.AddLog("IBB_Project_Index::GetSec ：Func III Idx=", false); GlobalLogB.AddLog(GetText().c_str()); }
    auto Iter = Ini.Search<IBB_Ini>(Proj.Inis, true, true, [](const IBB_Ini& F) {return F.Name; });
    if (Iter == Proj.Inis.end())return nullptr;
    auto Iter1 = Section.Search(Iter->Secs, true, false);
    return (Iter1 == Iter->Secs.end()) ? nullptr : std::addressof(Iter1->second);
}
const IBB_Ini* IBB_Project_Index::GetIni(const IBB_Project& Proj) const
{
    if (EnableLogEx) { GlobalLogB.AddLog_CurTime(false); GlobalLogB.AddLog("IBB_Project_Index::GetIni ：Func IV Idx=", false); GlobalLogB.AddLog(GetText().c_str()); }
    auto Iter = Ini.Search<IBB_Ini>(Proj.Inis, true, [](const IBB_Ini& F) {return F.Name; });
    return (Iter == Proj.Inis.end()) ? nullptr : std::addressof(*Iter);
}
const IBB_Section* IBB_Project_Index::GetSec(const IBB_Project& Proj) const
{
    if (EnableLogEx) { GlobalLogB.AddLog_CurTime(false); GlobalLogB.AddLog("IBB_Project_Index::GetSec ：Func IV Idx=", false); GlobalLogB.AddLog(GetText().c_str()); }
    auto Iter = Ini.Search<IBB_Ini>(Proj.Inis, true, [](const IBB_Ini& F) {return F.Name; });
    if (Iter == Proj.Inis.end())return nullptr;
    auto Iter1 = Section.Search(Iter->Secs, true);
    return (Iter1 == Iter->Secs.end()) ? nullptr : std::addressof(Iter1->second);
}
const IBB_Ini* IBB_Project::GetIni(const IBB_Project_Index& Index) const
{
    return Index.GetIni(*this);
}
const IBB_Section* IBB_Project::GetSec(const IBB_Project_Index& Index)const
{
    return Index.GetSec(*this);
}
IBB_Ini* IBB_Project::GetIni(IBB_Project_Index& Index) const
{
    return Index.GetIni(*(IBB_Project*)this);
}
IBB_Section* IBB_Project::GetSec(IBB_Project_Index& Index)const
{
    return Index.GetSec(*(IBB_Project*)this);
}

bool IBB_Project_Index::SameTarget(const IBB_Project& Proj, const IBB_Project_Index& A) const
{
    return GetSec(Proj) == A.GetSec(Proj);
}
std::string IBB_Project_Index::GetText() const
{
    return Ini.GetText() + "->" + Section.GetText();
}

std::string IBB_Section_Desc::GetText() const
{
    return Ini + "->" + Sec;
}













bool IBB_Project::CreateIni(const std::string& Name)
{
    IBB_Project_Index IniIndex(Name,"");
    auto Ini=GetIni(IniIndex);
    if (Ini == nullptr)
    {
        Inis.emplace_back();
        Inis.back().Name = Name;
        Inis.back().Root = this;
        return true;
    }
    else return false;
}
bool IBB_Project::AddIni(const IBB_Ini& Ini, bool IsDuplicate)
{
    IBB_Project_Index IniIndex(Ini.Name, "");
    auto IniF = GetIni(IniIndex);
    if (IniF == nullptr)
    {
        Inis.emplace_back(Ini);
        Inis.back().Root = this;
        return true;
    }
    else return IniF->Merge(Ini, IsDuplicate);
}

const char* __WTF__ = "_KENOSIS_SB_";

bool IBB_Project::CreateRegisterList(const std::string& Name,const std::string& IniName)
{
    IBB_DIndex Index(Name + __WTF__ + IniName);
    auto It = Index.Search<IBB_RegisterList>(RegisterLists, true, [](const IBB_RegisterList& L) {return L.Type + __WTF__ + L.IniType; });
    if (It == RegisterLists.end())
    {
        RegisterLists.emplace_back();
        RegisterLists.back().Type = Name;
        RegisterLists.back().IniType = IniName;
        RegisterLists.back().Root = this;
        return true;
    }
    else return false;
}
bool IBB_Project::AddRegisterList(const IBB_RegisterList& List)
{
    IBB_DIndex Index(List.Type + __WTF__ + List.IniType);
    auto It = Index.Search<IBB_RegisterList>(RegisterLists, true, [](const IBB_RegisterList& L) {return L.Type + __WTF__ + L.IniType; });
    if (It == RegisterLists.end())
    {
        RegisterLists.emplace_back(List);
        RegisterLists.back().Root = this;
        return true;
    }
    else return It->Merge(List);
}
IBB_RegisterList& IBB_Project::GetRegisterList(const std::string& Name, const std::string& IniName)
{
    IBB_DIndex Index(Name + __WTF__ + IniName);
    auto It = Index.Search<IBB_RegisterList>(RegisterLists, true, [](const IBB_RegisterList& L) {return L.Type + __WTF__ + L.IniType; });
    if (It == RegisterLists.end())
    {
        RegisterLists.emplace_back();
        RegisterLists.back().Type = Name;
        RegisterLists.back().IniType = IniName;
        RegisterLists.back().Root = this;
        return RegisterLists.back();
    }
    else return *It;
}
bool IBB_Project::RegisterSection(const std::string& Name, const std::string& IniName, const IBB_Section& Section)
{
    IBB_RegisterList& Li = GetRegisterList(Name, IniName);
    Li.List.push_back(const_cast<IBB_Section*>(&Section));
    return true;
}
bool IBB_Project::RegisterSection(size_t RegListID, const IBB_Section& Section)
{
    if (RegListID >= RegisterLists.size())return false;
    RegisterLists.at(RegListID).List.push_back(const_cast<IBB_Section*>(&Section));
    return true;
}
IBB_Section* IBB_Project::AddNewSection(const IBB_Section_NameType& Paragraph)
{
    if (EnableLogEx)
    {
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "IBB_Project::AddNewSection <- IBB_Section_NameType Paragraph=%p(Name=%s)", &Paragraph, Paragraph.Name.c_str()); GlobalLogB.AddLog(LogBufB);
        auto Ret = AddNewSectionEx(Paragraph);
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "IBB_Project::AddNewSection -> IBB_Section* Ret=%p", Ret); GlobalLogB.AddLog(LogBufB);
        return Ret;
    }
    else return AddNewSectionEx(Paragraph);
}
IBB_Section* IBB_Project::AddNewSectionEx(const IBB_Section_NameType& Paragraph)
{
    IBB_Project_Index Tg(Paragraph.IniType, Paragraph.Name);
    auto Sc = Tg.GetSec(*this);
    if (Sc != nullptr)return nullptr;//this is AddNewSection plz don't give me an existing paragraph plz
    auto SIni = Tg.GetIni(*this);
    if (SIni == nullptr)
    {
        CreateIni(Paragraph.IniType);
        SIni = Tg.GetIni(*this);
        if (SIni == nullptr)
        {
            if (EnableLog)
            {
                GlobalLogB.AddLog_CurTime(false);
                GlobalLogB.AddLog("IBB_Project::AddNewSection ：无法添加INI。");
            }
            return nullptr;
        }
    }
    if (!SIni->CreateSection(Paragraph.Name))
    {
        if (EnableLog)
        {
            GlobalLogB.AddLog_CurTime(false);
            GlobalLogB.AddLog("IBB_Project::AddNewSection ：CreateSection创建失败。");
        }
        return nullptr;
    }
    Sc = Tg.GetSec(*this);

    if (Sc == nullptr)
    {
        if (EnableLog)
        {
            GlobalLogB.AddLog_CurTime(false);
            GlobalLogB.AddLog("IBB_Project::AddNewSection ：无法添加字段。");
        }
        return nullptr;
    }
    if(Sc->Generate(Paragraph))return Sc;
    else return nullptr;
}
IBB_Section* IBB_Project::CreateNewSection(const IBB_Section_Desc& Desc)
{
    IBB_Project_Index Tg(Desc.Ini, Desc.Sec);
    auto Sc = Tg.GetSec(*this);
    if (Sc != nullptr)return nullptr;//this is AddNewSection plz don't give me an existing paragraph plz
    auto SIni = Tg.GetIni(*this);
    if (SIni == nullptr)
    {
        CreateIni(Desc.Ini);
        SIni = Tg.GetIni(*this);
        if (SIni == nullptr)
        {
            if (EnableLog)
            {
                GlobalLogB.AddLog_CurTime(false);
                GlobalLogB.AddLog("IBB_Project::CreateNewSection ：无法添加INI。");
            }
            return nullptr;
        }
    }
    if (!SIni->CreateSection(Desc.Sec))
    {
        if (EnableLog)
        {
            GlobalLogB.AddLog_CurTime(false);
            GlobalLogB.AddLog("IBB_Project::CreateNewSection ：CreateSection创建失败。");
        }
        return nullptr;
    }
    Sc = Tg.GetSec(*this);
    if (Sc == nullptr)
    {
        if (EnableLog)
        {
            GlobalLogB.AddLog_CurTime(false);
            GlobalLogB.AddLog("IBB_Project::CreateNewSection ：无法添加字段。");
        }
        return nullptr;
    }
    return Sc;
}

bool IBB_Project::AddNewLinkToLinkGroup(const IBB_Section_Desc& From, const IBB_Section_Desc& To)
{
    IBB_Project_Index FIn(From),TIn(To);
    auto FromPtr = FIn.GetSec(*this);
    if (FromPtr == nullptr)
    {
        FromPtr = CreateNewSection(From);
        if (FromPtr == nullptr)
        {
            if (EnableLog)
            {
                GlobalLogB.AddLog_CurTime(false);
                GlobalLogB.AddLog("IBB_Project::AddNewLinkToLinkGroup ：无法添加From字段。");
            }
            return false;
        }
    }
    if (!FromPtr->IsLinkGroup)return false;
    auto ToPtr = TIn.GetSec(*this);
    if (ToPtr == nullptr)
    {
        ToPtr = CreateNewSection(To);
        if (ToPtr == nullptr)
        {
            if (EnableLog)
            {
                GlobalLogB.AddLog_CurTime(false);
                GlobalLogB.AddLog("IBB_Project::AddNewLinkToLinkGroup ：无法添加From字段。");
            }
            return false;
        }
    }
    FromPtr->LinkGroup_LinkTo.push_back({ nullptr,FIn,TIn });
    //ToPtr->LinkedBy.push_back({ nullptr,FIn,TIn });
    //auto& a = FromPtr->LinkGroup_LinkTo.back(), b = ToPtr->LinkedBy.back();
    //a.FillData(&b, "");
    //b.FillData(&a, "");
    return true;
}

bool IBB_Project::AddModule(const IBB_Module& Module)
{
    if (EnableLogEx)
    {
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "IBB_Project::AddModule -> IBB_Module Module=%p(Name=%s)", &Module,Module.Name.c_str()); GlobalLogB.AddLog(LogBufB);
    }

    bool Ret = true;
    if (Module.MergeToSection)
    {
        IBB_Project_Index Tg(Module.MergeParagraph.IniType, Module.MergeParagraph.Name);
        auto Sc = Tg.GetSec(*this);
        if (Sc == nullptr) { if (!AddNewSection(Module.MergeParagraph))Ret = false; }
        else Sc->Merge(IBB_Section(Module.MergeParagraph, Tg.GetIni(*this)), Module.MergeType, false);
    }
    for (const auto& Paragraph : Module.AttachedParagraph)
        { if (!AddNewSection(Paragraph))Ret = false; }
    for (const auto& Link : Module.LinkGraph)
    {
        AddNewLinkToLinkGroup({ Link.FromIni, Link.FromSec }, { Link.ToIni, Link.ToSec });
    }
    for (const auto& Reg : Module.AttachedRegister)
    {
        auto& Li = GetRegisterList(Reg.Type, Reg.IniType);
        if (Reg.UseTargetIniTypeList)
        {

        }
        for (size_t i = 0; i < Reg.List.size(); i++)
        {
            const auto& Name = Reg.List.at(i);
            const auto& TgType = Reg.UseTargetIniTypeList ? Reg.TargetIniTypeList.at(i) : Reg.TargetIniType;
            IBB_Project_Index Ind(TgType, Name);
            auto Ptr = GetSec(Ind);
            if (Ptr == nullptr)
            {
                if (EnableLog)
                {
                    GlobalLogB.AddLog_CurTime(false);
                    sprintf_s(LogBufB, "IBB_Project::AddModule ：字段 %s -> %s 不存在。", TgType.c_str(), Name.c_str());
                    GlobalLogB.AddLog(LogBufB);
                }
                Ret = false;
            }
            else Li.List.push_back(Ptr);
        }
    }
    if (EnableLogEx)
    {
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "IBB_Project::AddModule -> bool Ret=%s", IBD_BoolStr(Ret)); GlobalLogB.AddLog(LogBufB);
    }
    return Ret;
}

//我也不太确定就这吗
bool IBB_Project::UpdateAll()
{
    if (EnableLogEx)
    {
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "IBB_Project::UpdateAll <- void"); GlobalLogB.AddLog(LogBufB);
    }
    bool Ret = true;
    for (auto& Ini : Inis)for (auto& sp : Ini.Secs)
    {
        sp.second.LinkedBy.clear();
        for (auto& ss : sp.second.SubSecs)ss.LinkTo.clear();
    }
    for (auto& Ini : Inis)if (!Ini.UpdateAll())Ret = false;
    GlobalLogB.AddLog_CurTime(false); GlobalLogB.AddLog("——IBB_Project::UpdateAll——");//BREAKPOINT
    for (auto& Ini : Inis)for (auto& sp : Ini.Secs)
    {
        if (sp.second.IsLinkGroup)
        {
            int iOrd = 0;
            for (auto& L : sp.second.LinkGroup_LinkTo)
            {
                L.DynamicCheck_Legal(*this);
                L.DynamicCheck_UpdateNewLink(*this);
                L.Order = iOrd;
                L.OrderEx = INT_MAX;
                ++iOrd;
            }
        }
        else
        {
            int iOrdEx = 0;
            for (auto& ss : sp.second.SubSecs)
            {
                int iOrd = 0;
                for (auto& L : ss.LinkTo)
                {
                    L.DynamicCheck_Legal(*this);
                    L.DynamicCheck_UpdateNewLink(*this);
                    L.Order = iOrd;
                    L.OrderEx = iOrdEx;
                    ++iOrd;
                    GlobalLogB.AddLog_CurTime(false); GlobalLogB.AddLog(L.GetText(*this).c_str(), false);//BREAKPOINT
                }
                ++iOrdEx;
            }
        }
    }
    if (EnableLogEx)
    {
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "IBB_Project::UpdateAll -> bool Ret=%s",IBD_BoolStr(Ret)); GlobalLogB.AddLog(LogBufB);
    }
    return Ret;
}

_TEXT_UTF8 std::string IBB_Project::GetText(bool PrintExtraData) const
{
    std::string Text{ ";Project::GetText\n\n" };
    {
        Text = "[ProjectBasicInfo]\n";
        Text += "ProjName=" + UnicodetoMBCS(ProjName); Text.push_back('\n');
        Text += "CreateTime=" + std::to_string(CreateTime); Text.push_back('\n');
        Text += "LastUpdate=" + std::to_string(LastUpdate); Text.push_back('\n');
        Text += "CreateVersion=" + std::to_string(CreateVersionMajor) + "." +
            std::to_string(CreateVersionMinor) + "." + std::to_string(CreateVersionRelease);
        Text.push_back('\n');
    }
    Text += "\n;INI Files\n";
    for (const auto& ini : Inis)
    {
        Text += ini.GetText(PrintExtraData);
        Text.push_back('\n');
    }
    Text += "\n;Register Lists\n";
    for (const auto& list : RegisterLists)
    {
        Text += list.GetText(PrintExtraData);
        Text.push_back('\n');
    }
    return Text;
}

void IBB_Project::PackModule(IBB_Module& Module)const
{
    Module.MergeToSection = false;
    for (const auto& Reg : RegisterLists)
    {
        if (EnableLog)
        {
            GlobalLogB.AddLog_CurTime(false);
            GlobalLogB.AddLog(("IBB_Project::PackModule ： " + Reg.IniType + " -> " + Reg.Type).c_str());
        }
        Module.AttachedRegister.push_back(/*std::move*/(Reg.GetNameType()));
    }
    for (const auto& Ini : Inis)
    {
        for (const auto& Sec : Ini.Secs)
        {
            if (Sec.second.IsLinkGroup)
                for (const auto& Link : Sec.second.LinkGroup_LinkTo)
                    Module.LinkGraph.push_back(std::move(Link.GetNameType()));
            Module.AttachedParagraph.push_back(std::move(Sec.second.GetNameType()));
        }
    }
}

void IBB_Project::Clear()
{
    ProjName.clear();
    Path.clear();
    IsNewlyCreated = true;
    ChangeAfterSave = false;
    RegisterLists.clear();
    Inis.clear();
    LastUpdate = GetSysTimeMicros();
    CreateTime = GetSysTimeMicros();
    CreateVersionMajor = VersionMajor;
    CreateVersionMinor = VersionMinor;
    CreateVersionRelease = VersionRelease;

    IBB_IniLine_AllocationMap_Destroy();
}


bool IBB_RegisterList::Merge(const IBB_RegisterList& Another)
{
    List.insert(List.end(), Another.List.begin(), Another.List.end());
    return true;//return !Another.List.empty();
}
std::string IBB_RegisterList::GetText(bool PrintExtraData) const
{
    (void)PrintExtraData;
    std::string Text;
    Text += "[" + Type + "]\n";
    Text += "Type=" + Type; Text.push_back('\n');
    Text += "IniType=" + IniType; Text.push_back('\n');
    int i = 1;
    for (auto ptr : List)
    {
        //if(ptr==nullptr) 暂不需检查
        Text += std::to_string(i) + "=" + ptr->Name; Text.push_back('\n');
        i++;
    }
    return Text;
}
IBB_RegisterList_NameType IBB_RegisterList::GetNameType() const
{
    IBB_RegisterList_NameType Ret;
    Ret.UseTargetIniTypeList = true;
    Ret.List.reserve(List.size());
    Ret.TargetIniTypeList.reserve(List.size());
    Ret.IniType = IniType;
    Ret.Type = Type;
    for (const auto& Sec : List)
    {
        if (Sec == nullptr)continue;
        Ret.List.push_back(Sec->Name);
        Ret.TargetIniTypeList.push_back(Sec->Root->Name);
    }
    return Ret;
}



bool IBB_Ini::Merge(const IBB_Ini& Another, bool IsDuplicate)
{
    bool Ret = true;
    for (const auto& fs : Another.Secs_ByName)
    {
        auto It = Another.Secs.find(fs);
        if (It == Another.Secs.end())
        {
            if (EnableLog)
            {
                GlobalLogB.AddLog_CurTime(false);
                GlobalLogB.AddLog("IBB_Ini::Merge ：有名字的字段理应存在。");
            }
            Ret = false;
        }
        else
        {
            if (!AddSection(It->second, IsDuplicate))Ret = false;
        }
    }
    return Ret;
}




bool IBB_IniLine::Merge(const IBB_IniLine& Another, const std::string& Mode)
{
    if (Another.Default == nullptr)return false;
    if (Another.Default->Property.Proc == nullptr)return false;
    if (Another.Data == nullptr)return false;
    return Merge(Another.Default->Property.Proc->GetString(Another.Data), Mode);
}
bool IBB_IniLine::Merge(const std::string& Another, const std::string& Mode)
{
    if (Default == nullptr)return false;
    if (Default->Property.Proc == nullptr)return false;
    if (Data == nullptr)
    {
        Data = Default->Property.Proc->Allocate();
        if (Data == nullptr)
        {
            if (EnableLog)
            {
                GlobalLogB.AddLog_CurTime(false);
                GlobalLogB.AddLog("IBB_IniLine::Merge ：Data不能被分配。");
            }
            return false;
        }
        return Default->Property.Proc->SetValue(Data, Another);
    }
    if (Mode == u8"Reserve")
    {
        if (Default->Property.Proc->Empty(Data))return Default->Property.Proc->SetValue(Data, Another);
        else return true;
    }
    else if (Mode == u8"Replace")return Default->Property.Proc->SetValue(Data, Another);
    else if (Mode == u8"Merge")
    {
        if (Default->Property.Proc->Empty(Data))Default->Property.Proc->SetValue(Data, Another);
        else return Default->Property.Proc->MergeValue(Data, Another);
    }
    else
    {
        if (EnableLog)
        {
            GlobalLogB.AddLog_CurTime(false);
            sprintf_s(LogBufB, "IBB_IniLine::Merge ：Line \"%s\"合并模式\"%s\"不存在。", Default->Name.c_str(), Mode.c_str());
            GlobalLogB.AddLog(LogBufB);
        }
        return false;
    }
    return true;
}
bool IBB_IniLine::Generate(const std::string& Value, IBB_IniLine_Default* Def)
{
    if (EnableLogEx)
    {
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "IBB_IniLine::Generate <- std::string Value=%s, IBB_IniLine_Default* Def=%p(Name=%s)",
            Value.c_str(), Def, (Def == nullptr) ? "_ERROR" : Def->Name.c_str()); GlobalLogB.AddLog(LogBufB);
    }
    if (Def != nullptr)Default = Def;
    if (Default == nullptr)return false;
    if (Default->Property.Proc == nullptr)return false;
    if (Data == nullptr)Data = Default->Property.Proc->Allocate();
    if (Data == nullptr)
    {
        if (EnableLog)
        {
            GlobalLogB.AddLog_CurTime(false);
            GlobalLogB.AddLog("IBB_IniLine::Generate ：Data不能被分配。");
        }
        return false;
    }
    bool Ret = Default->Property.Proc->SetValue(Data, Value);
    if (EnableLogEx)
    {
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "IBB_IniLine::Generate -> std::string Default->Name=%s, std::string Default->Property.Type=%s",
            Default->Name.c_str(), Default->Property.Type.c_str()); GlobalLogB.AddLog(LogBufB);
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "IBB_IniLine::Generate -> bool Ret=%s",
            IBD_BoolStr(Ret)); GlobalLogB.AddLog(LogBufB);
    }
    return Ret;
}
//IBB_IniLine(IBB_IniLine&& F) { Default = F.Default; Data = F.Data; if (F.ShouldDestroy) { F.ShouldDestroy = false; ShouldDestroy = true; } }
IBB_IniLine::IBB_IniLine(IBB_IniLine&& F)
{
    if (EnableLogEx) {GlobalLogB.AddLog_CurTime(false); GlobalLogB.AddLog("IBB_IniLine : Move Ctor");}
    Default = F.Default; Data = F.Data;
    if (F.ShouldDestroy) { F.ShouldDestroy = false; ShouldDestroy = true; }
}

IBB_IniLine IBB_IniLine::Duplicate() const
{
    IBB_IniLine L;
    L.Default = Default;
    L.ShouldDestroy = false;
    L.Data = nullptr;
    if (Data != nullptr)
    {
        if (Default == nullptr)return L;
        if (Default->Property.Proc == nullptr)return L;
        L.Data = Default->Property.Proc->Duplicate(Data);
        L.ShouldDestroy = true;
    }
    return L;
}
IBB_IniLine::~IBB_IniLine()
{
    if (ShouldDestroy && Data != nullptr && Default != nullptr && Default->Property.Proc != nullptr)
    {
        //Default->Property.Proc->Destroy(Data);
        //TODO:确认这种实现没问题
        Data = nullptr;
    }
}

IBB_SubSec::IBB_SubSec(IBB_SubSec&& A):
    Root(A.Root),Default(A.Default), Lines_ByName(std::move(A.Lines_ByName)), Lines(std::move(A.Lines)),LinkTo(std::move(A.LinkTo))
{
    for (auto& p : A.Lines)
    {
        p.second.ShouldDestroy = false;
    }
}

bool IBB_SubSec::Merge(const IBB_SubSec& Another, const IBB_VariableList& MergeType, bool IsDuplicate)
{
    bool Ret = true;
    for (auto p : Another.Lines)
    {
        auto it = Lines.find(p.first);
        if (it == Lines.end())
        {
            Lines_ByName.push_back(p.first);
            if (IsDuplicate)
            {
                IBB_IniLine np = p.second.Duplicate();
                Lines.insert({ p.first,np });
            }
            else Lines.insert(p);
        }
        else
        {
            const auto& Mode = MergeType.GetVariable(p.first);
            if (!it->second.Merge(p.second, Mode.empty() ? "Replace" : Mode))Ret = false;
        }
    }
    if (Root != nullptr)
    {
        //auto pproj = Root->Root->Root;
        auto ti = Root->GetThisIndex();
        for (const auto& p : Another.LinkTo)
        {
            LinkTo.push_back(p);
            LinkTo.back().From = ti;
        }
    }
    return Ret;
}
bool IBB_SubSec::Merge(const IBB_SubSec& Another, const std::string& Mode,bool IsDuplicate)
{
    bool Ret = true;
    for (auto p : Another.Lines)
    {
        auto it = Lines.find(p.first);
        if (it == Lines.end())
        {
            Lines_ByName.push_back(p.first);
            if (IsDuplicate)
            {
                IBB_IniLine np = p.second.Duplicate();
                Lines.insert({ p.first,np });
            }
            else Lines.insert(p);
        }
        else
        {
            if (!it->second.Merge(p.second, Mode))Ret = false;
        }
    }
    if (Root != nullptr)
    {
        //auto pproj = Root->Root->Root;
        auto ti = Root->GetThisIndex();
        for (const auto& p : Another.LinkTo)
        {
            LinkTo.push_back(p);
            LinkTo.back().From = ti;

        }
    }
    return Ret;
}
bool IBB_SubSec::AddLine(const std::pair<std::string, std::string>& Line)
{
    auto it = Lines.find(Line.first);
    if (it == Lines.end())
    {
        IBB_IniLine_Default* Def = IBF_Inst_DefaultTypeList.List.KeyBelongToLine(Line.first);
        if (Def == nullptr)return false;
        Lines_ByName.push_back(Line.first);
        auto rp = Lines.insert({ Line.first,IBB_IniLine(Line.second, Def) });
        rp.first->second.ShouldDestroy = true;
        return true;
    }
    else
    {
        return it->second.Merge(Line.second, "Merge");
    }
}

bool IBB_SubSec::ChangeRoot(IBB_Section* NewRoot)
{
    if (NewRoot != nullptr)
    {
        auto ti = NewRoot->GetThisIndex();
        for (auto& L : LinkTo)L.From = ti;
    }
    Root = NewRoot;
    return true;
}

std::string IBB_SubSec::GetText(bool PrintExtraData) const
{
    std::string Text;
    if (PrintExtraData)
    {
        Text.push_back(';'); Text += (Default == nullptr ? std::string{ "MISSING SubSec Default" } : Default->Name); Text.push_back('\n');
    }
    for (const auto& sn : Lines_ByName)
    {
        auto It = Lines.find(sn);
        if (It == Lines.end())
            if (EnableLog)
            {
                GlobalLogB.AddLog_CurTime(false);
                GlobalLogB.AddLog(("IBB_SubSec::GetText ：SubSec" + sn + "有名字而无对应的内容。").c_str());
            }
        auto& L = It->second;
        Text += sn;Text.push_back('=');
        if (L.Default == nullptr)
            Text += "MISSING Line Default\n";
        else
        {
            if (L.Default->Property.Proc == nullptr)Text += "MISSING Property Process";
            else Text += L.Default->Property.Proc->GetString(L.Data);
            Text.push_back('\n');
        }
    }
    if (PrintExtraData)
    {
        Text += ";Extra Data - Link To\n";
        //for (const auto& to : LinkTo)
        //    Text += to.GetText(*(Root->Root->Root));//SubSec->Sec->Ini->Project
            
    }
    return Text;
}
std::vector<std::string> IBB_SubSec::GetKeys(bool PrintExtraData) const
{
    std::vector<std::string> Ret;
    if (PrintExtraData)Ret.push_back("_DEFAULT_NAME");
    for (const auto& sn : Lines_ByName)
    {
        if (Lines.find(sn) == Lines.end())
            if (EnableLog)
            {
                GlobalLogB.AddLog_CurTime(false);
                GlobalLogB.AddLog(("IBB_SubSec::GetKeys ：SubSec" + sn + "有名字而无对应的内容。").c_str());
            }
        Ret.push_back(sn);
    }
    if (PrintExtraData)for (const auto& L : LinkTo)
    {
        auto pf = L.From.GetSec(*(Root->Root->Root));
        if (pf != nullptr)Ret.push_back("_LINK_FROM_" + pf->Name);
    }
    return Ret;
}
IBB_VariableList IBB_SubSec::GetLineList(bool PrintExtraData) const
{
    IBB_VariableList Ret;
    if (PrintExtraData)Ret.Value["_DEFAULT_NAME"] = (Default == nullptr ? std::string{ "MISSING SubSec Default" } : Default->Name);
    for (const auto& sn : Lines_ByName)
    {
        auto It = Lines.find(sn);
        if (It == Lines.end())
            if (EnableLog)
            {
                GlobalLogB.AddLog_CurTime(false);
                GlobalLogB.AddLog(("IBB_SubSec::GetLineList ：SubSec" + sn + "有名字而无对应的内容。").c_str());
            }
        auto& L = It->second;
        if (L.Default == nullptr)Ret.Value[sn] = "MISSING Line Default";
        else
        {
            if (L.Default->Property.Proc == nullptr)Ret.Value[sn] = "MISSING Property Process";
            else Ret.Value[sn] = L.Default->Property.Proc->GetString(L.Data);
        }
    }
    if (PrintExtraData)for (const auto& L : LinkTo)
    {
        auto pf = L.From.GetSec(*(Root->Root->Root)), pt = L.To.GetSec(*(Root->Root->Root));
        if (pf != nullptr && pt != nullptr)
            Ret.Value["_LINK_FROM_" + pf->Name] = "_LINK_TO_" + pt->Name;
    }
    return Ret;
}

std::string IBB_SubSec::GetFullVariable(const std::string& Name) const
{
    auto it = Lines.find(Name);
    if (it == Lines.end())return "";
    if (it->second.Data == nullptr)return "";
    if (it->second.Default==nullptr)return ""; 
    if (it->second.Default->Property.Proc == nullptr)return "";
    return it->second.Default->Property.Proc->GetString(it->second.Data);
}

IBB_SubSec IBB_SubSec::Duplicate() const
{
    IBB_SubSec Ret;
    Ret.Root = Root;
    Ret.Default = Default;
    Ret.Lines_ByName = Lines_ByName;
    Ret.LinkTo = LinkTo;
    Ret.Lines.reserve(Lines.size());
    for (const auto& p : Lines)Ret.Lines.insert({ p.first,p.second.Duplicate() });
    return Ret;
}

void IBB_SubSec::GenerateAsDuplicate(const IBB_SubSec& Src)
{
    Root = Src.Root;
    Default = Src.Default;
    Lines_ByName = Src.Lines_ByName;
    LinkTo = Src.LinkTo;
    Lines.reserve(Src.Lines.size());
    for (const auto& p : Src.Lines)Lines.insert({ p.first,p.second.Duplicate() });
}

bool IBB_SubSec::UpdateAll()
{
    bool Ret = true;
    if (Default == nullptr)Ret = false;
    std::vector<IBB_Link> NewLT;
    NewLT.reserve(LinkTo.size());
    auto pproj = Root->Root->Root; (void)pproj;
    for (auto& L : Lines)
    {
        GlobalLogB.AddLog_CurTime(false); GlobalLogB.AddLog("IBB_SubSec::UpdateAll Line : ",false); GlobalLogB.AddLog(L.first.c_str());//BREAKPOINT
        auto def = L.second.Default;
        if(def == nullptr)Ret = false;
        else
        {

            GlobalLogB.AddLog_CurTime(false); GlobalLogB.AddLog("IBB_SubSec::UpdateAll Line II Type : ",false);//BREAKPOINT
            GlobalLogB.AddLog(def->Property.Type.c_str());//BREAKPOINT

            if (def->Property.Type == "Link")
            {
                auto prop = def->Property;
                if (prop.Proc == nullptr)Ret = false;
                else
                {
                    auto It = IBF_Inst_DefaultTypeList.List.Link_Default.find(prop.Lim.GetObjectItem(u8"LinkType").GetString());
                    NewLT.emplace_back(
                        (It == IBF_Inst_DefaultTypeList.List.Link_Default.end()) ? nullptr : (&(It->second)),
                        Root->GetThisIndex(),
                        IBB_Project_Index{ prop.Lim.GetObjectItem(u8"TargetIniType").GetString(), prop.Proc->GetString(L.second.Data) }
                    );
                    NewLT.back().FromKey = L.first;
                }
            }
            //TODO: LinkList
        }
    }
    LinkTo = NewLT;
    return Ret;
}


bool IBB_Ini::CreateSection(const std::string& _Name)
{
    auto Is = Secs.find(_Name);
    if (Is == Secs.end())
    {
        Secs_ByName.push_back(_Name);
        Secs.insert({ _Name,IBB_Section(_Name, this) });
        return true;
    }
    else return false;
}
bool IBB_Ini::AddSection(const IBB_Section& Section, bool IsDuplicate)
{
    auto Is = Secs.find(Section.Name);
    if (Is == Secs.end())
    {
        Secs_ByName.push_back(Section.Name);
        auto It = Secs.insert({ Section.Name,Section });
        It.first->second.ChangeRootAndBack(this);
        return true;
    }
    else
    {
        return Is->second.Merge(Section, "Merge", IsDuplicate);
    }
}
//TODO:刷新Link，不过不着急，毕竟AddSection实际只会在AddModule时调用，而不会导致原有Link的改动

/*
bool IBB_Ini::DeleteSection(const std::string& Tg)
{
    bool Ret = true;
    auto it = Secs.find(Tg);
    if (it == Secs.end())return false;
    else
    {
        for (auto ij = Secs_ByName.begin(); ij != Secs_ByName.end(); ++ij)
            if (*ij == Tg){ Secs_ByName.erase(ij); break; }
        if(it->second.Isolate())Ret = false;
        Secs.erase(it);
    }
    return Ret;
}
*/
//TODO
bool IBB_Ini::DeleteSection(const std::string& Tg)
{
    bool Ret = true;
    auto it = Secs.find(Tg);
    if (it == Secs.end())return false;
    else
    {
        for (auto ij = Secs_ByName.begin(); ij != Secs_ByName.end(); ++ij)
            if (*ij == Tg){ Secs_ByName.erase(ij); break; }
        //if(it->second.Isolate())Ret = false;
        auto psc = &it->second;
        for (auto& rl : Root->RegisterLists)
        {
            std::vector<IBB_Section*> pv; pv.reserve(rl.List.size());
            for (auto ps : rl.List)
            {
                if (ps != psc)pv.push_back(ps);
            }
            rl.List = pv;
        }
        Secs.erase(it);
    }
    return Ret;
}

std::string IBB_Ini::GetText(bool PrintExtraData) const
{
    std::string Text;
    Text.push_back(';'); Text += Name; Text.push_back('\n');
    for (const auto& sn : Secs_ByName)
    {
        auto It = Secs.find(sn);
        if (It == Secs.end())
            if (EnableLog)
            {
                GlobalLogB.AddLog_CurTime(false);
                GlobalLogB.AddLog(("IBB_Ini::GetText ：字段"+ sn +"有名字而无对应的内容。").c_str());
            }
        auto& Sec = It->second;
        Text += "[" + Sec.Name + "]\n" + Sec.GetText(PrintExtraData);
        Text.push_back('\n');
    }
    return Text;
}

bool IBB_Ini::UpdateAll()
{
    bool Ret = true;
    for (auto& P : Secs)if (!P.second.UpdateAll())Ret = false;
    return Ret;
}



void IBB_VariableList::FillKeys(const std::vector<std::string>& List, const std::string& Val)
{
    for (const auto& s : List)Value[s] = Val;
}
void IBB_VariableList::Merge(const IBB_VariableList& Another, bool MergeUpValue)
{
    for (const auto& p : Another.Value)
        Value[p.first] = p.second;
    if (MergeUpValue && UpValue != nullptr && Another.UpValue != nullptr && UpValue != Another.UpValue)
        UpValue->Merge(*Another.UpValue, true);
}
const std::string& IBB_VariableList::GetVariable(const std::string& Name) const
{
    static std::string Null = "";
    auto It = Value.find(Name);
    if (It != Value.end())return It->second;
    else if (UpValue == nullptr)return Null;
    else return UpValue->GetVariable(Name);
}
bool IBB_VariableList::HasValue(const std::string& Name) const
{
    if ((Value.find(Name) != Value.end()))return true;
    else if (UpValue == nullptr)return false;
    else return UpValue->HasValue(Name);
}
bool IBB_VariableList::CoverUpValue(const std::string& Name) const
{
    if (UpValue == nullptr)return false;
    else return (Value.find(Name) != Value.end()) && UpValue->HasValue(Name);
}
std::string IBB_VariableList::GetText(bool ConsiderUpValue) const
{
    if (!ConsiderUpValue)
    {
        std::string Ret;
        for (const auto& p : Value)
        {
            Ret += p.first;
            Ret.push_back('=');
            Ret += p.second;
            Ret.push_back('\n');
        }
        return Ret;
    }
    else
    {
        IBB_VariableList List;
        Flatten(List);
        return List.GetText(false);
    }
}
void IBB_VariableList::Flatten(IBB_VariableList& Target) const
{
    if (UpValue != nullptr)UpValue->Flatten(Target);
    for (const auto& p : Value)
        Target.Value[p.first]=p.second;
}



void IBB_Link::DynamicCheck_Legal(const IBB_Project& Proj)
{
    auto pf = Proj.GetSec(From), pt = Proj.GetSec(To);
    if (pf == nullptr || pt == nullptr){ Dynamic.Legal = IBB_Link::_Dynamic::Incomplete; return; }
    else { Dynamic.Legal = IBB_Link::_Dynamic::Correct; return; }
    /*
    std::vector<IBB_Link*> ToChange;
    while(pt->IsLinkGroup)
    {
        if(pt->LinkGroup_LinkTo.empty()) { Dynamic.Legal = IBB_Link::_Dynamic::Incomplete; return; }
        pt = nullptr; auto it = pt->LinkGroup_LinkTo.begin();
        IBB_Link* pl;
        while (pt == nullptr && it != pt->LinkGroup_LinkTo.end()) { pl = &(*it); pt = Proj.GetSec(it->To); ++it; }
        if(pt == nullptr) { Dynamic.Legal = IBB_Link::_Dynamic::Incomplete; return; }
        //暂且这么干
        ToChange.push_back(pl);
        for (auto l : pt->LinkedBy)if (l == *pl)ToChange.push_back(&l);
    }
    while (pf->IsLinkGroup)
    {
        if(pf->LinkedBy.empty()) { Dynamic.Legal = IBB_Link::_Dynamic::Incomplete; return; }
        pf = nullptr; auto it = pf->LinkedBy.begin();
        IBB_Link* pl;
        while (pf == nullptr && it != pf->LinkedBy.end()) { pl = &(*it); pf = Proj.GetSec(it->From); ++it; }
        if (pf == nullptr) { Dynamic.Legal = IBB_Link::_Dynamic::Incomplete; return; }
        //暂且这么干
        ToChange.push_back(pl);
        for (auto l : pt->GetLinkTo())if (l == *pl)ToChange.push_back(&l);
    }
    //TODO: Correct or Wrong
    //TODO: Mixed Cond Check
    */
}
void IBB_Link::DynamicCheck_UpdateNewLink(const IBB_Project& Proj)
{
    auto sp = Proj.GetSec(To);
    if (sp != nullptr)
    {
        //sp->Dynamic.NewLinkedBy.push_back(*this);
        //auto& b = sp->Dynamic.NewLinkedBy.back();
        //b.FillData(this, FromKey);
        //Another = &b;
        sp->LinkedBy.push_back(*this);
        auto& b = sp->LinkedBy.back();
        b.Order = sp->LinkedBy.size() - 1;
        b.Another = this;
        Another = &b;
    }
}
bool IBB_Link::ChangeAddress()
{
    if (Another != nullptr)
    {
        Another->Another = this;
    }
    return true;
}



std::string IBB_Link::GetText(const IBB_Project& Proj) const
{
    std::string Text;
    auto pf = From.GetSec(Proj), pt = To.GetSec(Proj);

    Text += ((pf != nullptr) ? ("_LINK_FROM:" + pf->Root->Name + "->" + pf->Name) : ("_MISSING:" + From.GetText()));
    Text.push_back('=');
    Text += ((pt != nullptr) ? ("_LINK_TO:" + pf->Root->Name + "->" + pt->Name) : ("_MISSING:" + To.GetText()));
    Text.push_back('\n');

    Text += "_ADDRESS=" + std::to_string(this); Text.push_back('\n');
    Text += "_ANO_ADDR=" + std::to_string(Another); Text.push_back('\n');
    Text += "_FROM_KEY=" + FromKey; Text.push_back('\n');
    return Text;
}

IBB_Link_NameType IBB_Link::GetNameType() const
{
    return IBB_Link_NameType{ From.Ini.GetText(),From.Section.GetText(),To.Ini.GetText(),To.Section.GetText() };
}



std::vector<IBB_Link> IBB_Section::GetLinkTo() const
{
    if (IsLinkGroup)
    {
        return LinkGroup_LinkTo;
    }
    else
    {
        std::vector<IBB_Link> Ret;
        for (const auto& Sub : SubSecs)
            Ret.insert(Ret.end(), Sub.LinkTo.begin(), Sub.LinkTo.end());
        return Ret;
    }
}
std::vector<std::string> IBB_Section::GetKeys(bool PrintExtraData) const
{
    std::vector<std::string> Ret;
    if (IsLinkGroup)
    {
        for (const auto& L : LinkGroup_LinkTo)
        {
            auto pf = L.From.GetSec(*(Root->Root)), pt = L.To.GetSec(*(Root->Root));
            if (pf != nullptr && pt != nullptr)
                Ret.push_back("_LINK_FROM_" + pf->Name);
        }
        if (PrintExtraData)
        {
            Ret.push_back("_SECTION_NAME");
            Ret.push_back("_IS_LINKGROUP");
            IBB_VariableList VL;
            VarList.Flatten(VL);
            for (const auto& V : VL.Value)
                Ret.push_back(V.first);
        }
    }
    else
    {
        for (const auto& Sub : SubSecs)
        {
            auto K = Sub.GetKeys(PrintExtraData);
            Ret.insert(Ret.end(), K.begin(), K.end());
        }
        for (const auto& V : UnknownLines.Value)
            Ret.push_back(V.first);
        if (PrintExtraData)
        {
            Ret.push_back("_SECTION_NAME");
            Ret.push_back("_IS_LINKGROUP");
            IBB_VariableList VL;
            VarList.Flatten(VL);
            for (const auto& V : VL.Value)
                Ret.push_back(V.first);
            for (const auto& L : LinkedBy)
            {
                auto pf = L.From.GetSec(*(Root->Root)), pt = L.To.GetSec(*(Root->Root));
                if (pf != nullptr && pt != nullptr)
                    Ret.push_back("_LINK_FROM_" + pf->Name);
            }
        }
    }
    return Ret;
}
IBB_VariableList IBB_Section::GetLineList(bool PrintExtraData) const
{
    IBB_VariableList Ret;
    if (IsLinkGroup)
    {
        for (const auto& L : LinkGroup_LinkTo)
        {
            auto pf = L.From.GetSec(*(Root->Root)), pt = L.To.GetSec(*(Root->Root));
            if (pf != nullptr && pt != nullptr)
                Ret.Value["_LINK_FROM_" + pf->Name] = "_LINK_TO_" + pt->Name;
        }
        if (PrintExtraData)
        {
            Ret.Value["_SECTION_NAME"] = Name;
            Ret.Value["_IS_LINKGROUP"] = "true";
            IBB_VariableList VL;
            VarList.Flatten(VL);
            Ret.Merge(VL, false);
        }
    }
    else
    {
        for (const auto& Sub : SubSecs)
            Ret.Merge(Sub.GetLineList(PrintExtraData), false);
        Ret.Merge(UnknownLines, false);
        if (PrintExtraData)
        {
            Ret.Value["_SECTION_NAME"] = Name;
            Ret.Value["_IS_LINKGROUP"] = "false";
            IBB_VariableList VL;
            VarList.Flatten(VL);
            Ret.Merge(VL, false);
            for (auto L : LinkedBy)
            {
                auto pf = L.From.GetSec(*(Root->Root)), pt = L.To.GetSec(*(Root->Root));
                if (pf != nullptr && pt != nullptr)
                    Ret.Value["_LINK_FROM_" + pf->Name] = "_LINK_TO_" + pt->Name;
            }
        }
    }
    return Ret;
}
IBB_VariableList IBB_Section::GetSimpleLines() const
{
    IBB_VariableList Ret;
    if (!IsLinkGroup)
    {
        for (const auto& Sub : SubSecs)
            Ret.Merge(Sub.GetLineList(false), false);
        Ret.Merge(UnknownLines, false);
    }
    return Ret;
}
std::string IBB_Section::GetText(bool PrintExtraData) const
{
    std::string Text;
    if (IsLinkGroup)
    {
        Text += ";LinkGroup - Links\n";
        for (const auto& L : LinkGroup_LinkTo)
        {
            auto pf = L.From.GetSec(*(Root->Root)), pt = L.To.GetSec(*(Root->Root));
            if (pf != nullptr && pt != nullptr)
            {
                Text += pf->Name;
                Text.push_back('=');
                Text += pt->Name;
                Text.push_back('\n');
            }
            else Text += "_ERROR=\n";
        }
        if (PrintExtraData)
        {
            Text += "\n;ExtraData - Name\n";
            Text += "_SECTION_NAME=" + Name;
            Text += "\n\n;ExtraData - IsLinkGroup\n_IS_LINKGROUP=true\n;ExtraData - Variables\n";
            Text += VarList.GetText(true);
        }
    }
    else
    {
        for (const auto& Sub : SubSecs)
            Text += Sub.GetText(PrintExtraData);
        Text.push_back('\n');
        Text += UnknownLines.GetText(false);
        if (PrintExtraData)
        {
            Text += "\n;ExtraData - Name\n";
            Text += "_SECTION_NAME=" + Name;
            Text += "\n\n;ExtraData - IsLinkGroup\n_IS_LINKGROUP=false\n;ExtraData - Variables\n";
            Text += VarList.GetText(true);
            Text += "\n;ExtraData - Linked By\n";
            for (const auto& L : LinkedBy)
                Text += L.GetText(*(Root->Root));
        }
    }
    return Text;
}

std::vector<size_t> IBB_Section::GetRegisteredPosition() const
{
    std::vector<size_t> Ret;
    auto& proj = *(Root->Root);
    for (size_t i = 0; i < proj.RegisterLists.size(); i++)
    {
        auto& li = proj.RegisterLists.at(i);
        for (size_t j = 0; j < li.List.size(); j++)
            if (li.List.at(j) == this){ Ret.push_back(i); break; }
    }
    return Ret;
}
std::vector<std::pair<size_t, size_t>> IBB_Section::GetRegisteredPositionAlt() const
{
    std::vector<std::pair<size_t, size_t>> Ret;
    auto& proj = *(Root->Root);
    for (size_t i = 0; i < proj.RegisterLists.size(); i++)
    {
        auto& li = proj.RegisterLists.at(i);
        for (size_t j = 0; j < li.List.size(); j++)
            if (li.List.at(j) == this) { Ret.push_back({i,j}); break; }
    }
    return Ret;
}

IBB_Section_NameType IBB_Section::GetNameType() const
{
    IBB_Section_NameType Ret;
    Ret.IsLinkGroup = IsLinkGroup;
    Ret.IniType = Root->Name;
    Ret.Name = Name;
    Ret.VarList = VarList;
    Ret.Lines = GetSimpleLines();
    return Ret;
}

std::string IBB_Section::GetFullVariable(const std::string& _Name) const
{
    if (VarList.HasValue(_Name))return VarList.GetVariable(_Name);
    if (_Name == "_SECTION_NAME")return Name;
    if (UnknownLines.HasValue(_Name))return UnknownLines.GetVariable(_Name);
    for (const auto& ss : SubSecs)
    {
        auto u=ss.GetFullVariable(_Name);
        if (!u.empty())return u;
    }
    return "";
}

IBB_Project_Index IBB_Section::GetThisIndex() const
{
    return IBB_Project_Index{ Root->Name,Name };
}

bool IBB_Section::Generate(const IBB_Section_NameType& Par)
{
    Name = Par.Name;
    IsLinkGroup = Par.IsLinkGroup;
    VarList = Par.VarList;
    if (IsLinkGroup)
    {
        if (!Par.Lines.Value.empty())
        {
            if (EnableLog)
            {
                GlobalLogB.AddLog_CurTime(false);
                GlobalLogB.AddLog("IBB_Section::Generate ： 错误：Section作为一个LinkGroup却具有非空的Lines。");
            }
            return false;
        }
        else return true;
    }
    else
    {
        bool Ret = true;
        SubSecs.clear();
        std::unordered_map<IBB_SubSec_Default*, int> SubSecList;
        for (const auto& L : Par.Lines.Value)
        {
            auto ptr = IBF_Inst_DefaultTypeList.List.KeyBelongToSubSec(L.first);
            if (ptr == nullptr)UnknownLines.Value[L.first] = L.second;
            else
            {
                auto It = SubSecList.find(ptr);
                if (It == SubSecList.end())
                {
                    It = SubSecList.insert({ ptr,SubSecs.size() }).first;
                    SubSecs.emplace_back(ptr, this);
                }
                auto& Sub = SubSecs.at(It->second);
                if (!Sub.AddLine(L))Ret = false;
            }
        }
        return Ret;
    }
}

bool IBB_Section::Merge(const IBB_Section& Another, const IBB_VariableList& MergeType, bool IsDuplicate)
{
    auto pproj = Root->Root; (void)pproj;
    if (IsLinkGroup)
    {
        LinkGroup_LinkTo.insert(LinkGroup_LinkTo.end(), Another.LinkGroup_LinkTo.begin(), Another.LinkGroup_LinkTo.end());
        return true;
    }
    else
    {
        UnknownLines.Merge(Another.UnknownLines, false);
        VarList.Merge(Another.VarList, false);
        for (const auto& ss:Another.SubSecs)
        {
            if (ss.Default == nullptr)continue;
            IBB_TDIndex<IBB_SubSec_Default*> idx(ss.Default);
            auto It = idx.Search<IBB_SubSec>(SubSecs, true, [](const IBB_SubSec& su)->IBB_SubSec_Default* {return su.Default; });
            if (It == SubSecs.end())
            {
                if (IsDuplicate)
                {
                    SubSecs.emplace_back(); SubSecs.back().GenerateAsDuplicate(ss); SubSecs.back().ChangeRoot(this);
                }
                else { SubSecs.push_back(ss); SubSecs.back().ChangeRoot(this); }
            }
            else It->Merge(ss, MergeType, IsDuplicate);
        }
        return true;
    }
    //LinkedBy : Refresh from Update !
}
bool IBB_Section::Merge(const IBB_Section& Another, const std::string& MergeType, bool IsDuplicate)
{
    auto pproj = Root->Root; (void)pproj;
    if (IsLinkGroup)
    {
        LinkGroup_LinkTo.insert(LinkGroup_LinkTo.end(), Another.LinkGroup_LinkTo.begin(), Another.LinkGroup_LinkTo.end());
        return true;
    }
    else
    {
        UnknownLines.Merge(Another.UnknownLines, false);
        VarList.Merge(Another.VarList, false);
        for (const auto& ss : Another.SubSecs)
        {
            if (ss.Default == nullptr)continue;
            IBB_TDIndex<IBB_SubSec_Default*> idx(ss.Default);
            auto It = idx.Search<IBB_SubSec>(SubSecs, true, [](const IBB_SubSec& su)->IBB_SubSec_Default* {return su.Default; });
            if (It == SubSecs.end())
            {
                if (IsDuplicate)
                {
                    SubSecs.emplace_back(); SubSecs.back().GenerateAsDuplicate(ss); SubSecs.back().ChangeRoot(this);
                }
                else { SubSecs.push_back(ss); SubSecs.back().ChangeRoot(this); }
            }
            else It->Merge(ss, MergeType, IsDuplicate);
        }
        return true;
    }
    //LinkedBy : Refresh from Update !
}

bool IBB_Section::GenerateAsDuplicate(const IBB_Section& Src)
{
    auto pproj = Root->Root;
    Merge(Src, "Replace", true);
    for (auto s : Src.GetRegisteredPosition())
    {
        pproj->RegisterSection(s, *this);
    }
    return true;
}

IBB_Section::IBB_Section(IBB_Section&& S):
    Root(S.Root),
    IsLinkGroup(S.IsLinkGroup),
    Name(std::move(S.Name)),
    SubSecs(std::move(S.SubSecs)),
    LinkedBy(std::move(S.LinkedBy)),
    VarList(std::move(S.VarList)),
    UnknownLines(std::move(S.UnknownLines)),
    LinkGroup_LinkTo(std::move(S.LinkGroup_LinkTo))
{
    if (EnableLogEx)
    {
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "IBB_Section::IBB_Section MOVE CTOR <- IBB_Section&& Src=%p(Name=%s)", &S, Name.c_str()); GlobalLogB.AddLog(LogBufB);
    }
    ChangeAddress();
}

bool IBB_Section::ChangeRoot(const IBB_Ini* NewRoot)
{
    Root = const_cast<IBB_Ini*>(NewRoot);
}

bool IBB_Section::Rename(const std::string& NewName)
{
    if (EnableLogEx)
    {
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "IBB_Section::Rename (OldName=%s)<- std::string NewName=%s",Name.c_str(), NewName.c_str()); GlobalLogB.AddLog(LogBufB);
    }
    //auto& proj = *(Root->Root);
    Name = NewName;
    if (IsLinkGroup)
    {
        for (auto& p : LinkGroup_LinkTo)
        {
            if (p.Another != nullptr)p.Another->From.Section.Assign(NewName);
            p.From.Section.Assign(NewName);
        }
            
        for (auto& p : LinkedBy)
        {
            if (p.Another != nullptr)p.Another->To.Section.Assign(NewName);
            p.To.Section.Assign(NewName);
        }
    }
    else
    {
        for (auto& s : SubSecs)
        {
            for (auto& p : s.LinkTo)
            {
                auto it = s.Lines.find(p.FromKey);
                if (it != s.Lines.end())
                {
                    if (it->second.Default != nullptr)if (it->second.Default->Property.Proc != nullptr)
                    {
                        if (it->second.Default->Property.Type == "Link")
                            it->second.Default->Property.Proc->SetValue(it->second.Data, NewName);
                        //TODO:LinkList
                    }
                }
                if (p.Another != nullptr)p.Another->From.Section.Assign(NewName);
                p.From.Section.Assign(NewName);
            }
        }
            
        for (auto& p : LinkedBy)
        {
            if (p.Another != nullptr)p.Another->To.Section.Assign(NewName);
            p.To.Section.Assign(NewName);
        }
    }
    return true;
}

bool IBB_Section::ChangeAddress()
{
    bool Ret = true;
    for (auto& L : LinkedBy) if (!L.ChangeAddress())Ret = false;
    if (IsLinkGroup)
    {
        for (auto& L : LinkGroup_LinkTo)if (!L.ChangeAddress())Ret = false;
    }
    else
    {
        for (auto& ss : SubSecs) if (!ss.ChangeRoot(this))Ret = false;
    }
    return Ret;
}

bool IBB_Section::Isolate()
{
    auto pproj = Root->Root;
    //*
    for (auto& L : LinkedBy)
    {
        auto ps = L.From.GetSec(*pproj);
        if (L.Another != nullptr && ps != nullptr)
        {
            if (ps->IsLinkGroup)
            {
                if (L.Another->Order < ps->LinkGroup_LinkTo.size() - 1)
                {
                    ps->LinkGroup_LinkTo.at(L.Another->Order) = ps->LinkGroup_LinkTo.back();
                    ps->LinkGroup_LinkTo.pop_back();
                }
                if (L.Another->Order == ps->LinkGroup_LinkTo.size() - 1)
                {
                    ps->LinkGroup_LinkTo.pop_back();
                }
            }
            else
            {
                if (L.Another->OrderEx < ps->SubSecs.size())
                {
                    auto& ss = ps->SubSecs.at(L.Another->OrderEx);
                    auto it = ss.Lines.find(L.Another->FromKey);
                    if (it != ss.Lines.end())if (it->second.Default != nullptr && it->second.Default->Property.Proc != nullptr)
                        it->second.Default->Property.Proc->Clear(it->second.Data);
                    if (L.Another->Order < ss.LinkTo.size() - 1)
                    {
                        ss.LinkTo.at(L.Another->Order) = ss.LinkTo.back();
                        ss.LinkTo.pop_back();
                    }
                    if (L.Another->Order == ss.LinkTo.size() - 1)
                    {
                        ss.LinkTo.pop_back();
                    }
                }

            }
        }
    }
    
    if (IsLinkGroup)
    {
        for (auto& L : LinkGroup_LinkTo)
        {
            auto ps = L.To.GetSec(*pproj);
            if (L.Another != nullptr && ps != nullptr)
            {
                if (L.Another->Order < ps->LinkedBy.size() - 1)
                {
                    ps->LinkedBy.back().Order = L.Another->Order;
                    ps->LinkedBy.at(L.Another->Order) = ps->LinkedBy.back();
                    ps->LinkedBy.pop_back();
                }
                else if (L.Another->Order == ps->LinkedBy.size() - 1)
                {
                    ps->LinkedBy.pop_back();
                }
            }
        }
    }
    else
    {
        //TODO
        for (auto& ss : SubSecs)
        {
            for (auto& L : ss.LinkTo)
            {
                auto ps = L.To.GetSec(*pproj);
                if (L.Another != nullptr && ps != nullptr)
                {
                    if (L.Another->Order < ps->LinkedBy.size() - 1)
                    {
                        ps->LinkedBy.back().Order = L.Another->Order;
                        ps->LinkedBy.at(L.Another->Order) = ps->LinkedBy.back();
                        ps->LinkedBy.pop_back();
                    }
                    else if (L.Another->Order == ps->LinkedBy.size() - 1)
                    {
                        ps->LinkedBy.pop_back();
                    }
                }
            }
        }
    }
    //*/
    return true;
}

bool IBB_Section::UpdateAll()
{
    bool Ret = true;
    auto PProj = Root->Root;
    if (IsLinkGroup)
    {
        std::vector<IBB_Link> NewGroup;
        NewGroup.reserve(LinkGroup_LinkTo.size());
        for (size_t i = 0; i < LinkGroup_LinkTo.size(); i++)
        {
            if (LinkGroup_LinkTo.at(i).To.GetSec(*PProj) != nullptr)NewGroup.push_back(LinkGroup_LinkTo.at(i));
        }
        for (auto& L : NewGroup)
        {
            L.FromKey.clear();
        }
        LinkGroup_LinkTo = NewGroup;
        if (LinkGroup_LinkTo.size() < (LinkGroup_LinkTo.capacity() >> 1))LinkGroup_LinkTo.shrink_to_fit();//TEST
    }
    else
    {
        for (auto& ss : SubSecs)if (!ss.UpdateAll())Ret = false;
    }
    return Ret;
}
























bool IBB_DefaultModuleList::LoadAfterParse()
{
    if (!RootJson.Available())
    {
        if (EnableLog)
        {
            GlobalLogB.AddLog_CurTime(false);
            GlobalLogB.AddLog("IBB_DefaultModuleList::LoadAfterParse ： JSON读取故障。");
        }
        return false;
    }

    bool Ret = true;

    for (auto Obj : RootJson.GetObj().GetObjectItem(u8"Module_List").GetArrayObject())
    {
        Module_Default.push_back({});
        if (!Module_Default.back().Load(Obj))Ret = false;
        ListTypes.insert(Module_Default.back().ListType);
        Module_Default_ByName[Module_Default.back().Name] = Module_Default.size() - 1;
    }

    if (EnableLog)
    {
        GlobalLogB.AddLog_CurTime(false);
        GlobalLogB.AddLog("IBB_DefaultTypeList::Load ： JSON读取完毕。");
        GlobalLogB.AddLog_CurTime(false);
        GlobalLogB.AddLog("IBB_DefaultTypeList::Load ： 返回", false);
        GlobalLogB.AddLog(IBD_BoolStr(Ret));
    }

    return Ret;
}

bool IBB_DefaultModuleList::LoadFromString(const std::string& FromString)
{
    if (EnableLog)
    {
        GlobalLogB.AddLog_CurTime(false);
        GlobalLogB.AddLog("IBB_DefaultModuleList::LoadFromString ： JSON开始读取。");
    }

    RootJson.Clear();
    RootJson.Parse(FromString);

    return LoadAfterParse();
}

bool IBB_DefaultModuleList::LoadFromFile(const std::string& FromFile)
{
    if (EnableLog)
    {
        GlobalLogB.AddLog_CurTime(false);
        GlobalLogB.AddLog("IBB_DefaultModuleList::LoadFromFile ： JSON开始读取。");
    }

    RootJson.Clear();
    RootJson.ParseFromFile(FromFile.c_str());

    return LoadAfterParse();
}

bool IBB_Module_Default::Load(JsonObject FromJson)
{
    Platform = FromJson.GetObjectItem(u8"Platform").GetArrayString();
    Name = FromJson.GetObjectItem(u8"Name").GetString();
    DescShort = FromJson.GetObjectItem(u8"DescShort").GetString();
    DescLong = FromJson.GetObjectItem(u8"DescLong").GetString();
    ListType = FromJson.GetObjectItem(u8"ListType").GetString();

    MergeToSection = FromJson.GetObjectItem(u8"MergeToSection").GetBool();
    ParamN = FromJson.GetObjectItem(u8"NumberOfParams").GetInt();
    ParamDescShort = FromJson.GetObjectItem(u8"ParamDescShort").GetArrayString();
    ParamDescLong = FromJson.GetObjectItem(u8"ParamDescLong").GetArrayString();

    bool Ret = true;

    if(MergeToSection)
        if (!MergeParagraph.Load(FromJson.GetObjectItem(u8"MergeParagraph")))Ret = false;
    for (auto Obj : FromJson.GetObjectItem(u8"AttachedLinkGroup").GetArrayObject())
    {
        AttachedLinkGroup.push_back({});
        if (!AttachedLinkGroup.back().Load(Obj))Ret = false;
    }
    for (auto Obj : FromJson.GetObjectItem(u8"AttachedParagraph").GetArrayObject())
    {
        AttachedParagraph.push_back({});
        if (!AttachedParagraph.back().Load(Obj))Ret = false;
    }
    for (auto Obj : FromJson.GetObjectItem(u8"AttachedRegister").GetArrayObject())
    {
        AttachedRegister.push_back({});
        if (!AttachedRegister.back().Load(Obj))Ret = false;
    }

    return Ret;
}

std::string IBB_Module_Default::_NameType::GetString(IBB_Module_ParagraphList& Param) const
{
    static BufString Buf;
    if (IsList)
    {
        int It = (int)std::min({ UseAutoParameterList.size(),DataList.size(),ParamNList.size() });
        std::string Ret{};
        Ret.reserve(It);
        for (int i = 0; i < It; i++)
        {
            if (ParamNList[i] == -1)Ret += (DataList[i]);
            else
            {
                auto& Par = Param.GetParam(ParamNList[i], UseAutoParameterList[i]);
                if (DataList[i] == "%s")Ret += (Par);
                else
                {
                    sprintf_s(Buf, DataList[i].c_str(), Par.c_str());
                    Ret += (Buf);
                }
            }
            Ret.push_back(',');
        }
        Ret.pop_back();
        return Ret;
    }
    else
    {
        if (ParamN == -1)return Data;
        else
        {
            auto& Par=Param.GetParam(ParamN, UseAutoParameter);
            if (Data == "%s")return Par;
            else
            {
                sprintf_s(Buf, Data.c_str(), Par.c_str());
                return Buf;
            }
        }
    }
}
IBB_Module_Default::_NameType IBB_Module_Default::_NameType::CutMergeName(const std::string& Name) const
{
    IBB_Module_Default::_NameType Ret;
    static BufString Buf;
    Ret.IsList = IsList;
    if (IsList)
    {
        Ret.UseAutoParameterList = UseAutoParameterList;
        Ret.ParamNList = ParamNList;
        Ret.DataList = DataList;
        int It = (int)std::min({ UseAutoParameterList.size(),DataList.size(),ParamNList.size() });
        for (int i = 0; i < It; i++)
        {
            if (!UseAutoParameterList[i] && ParamNList[i] == -2)
            {
                Ret.ParamNList[i] = -1;
                if (DataList[i] == "%s")Ret.DataList[i] = Name;
                else
                {
                    sprintf_s(Buf, DataList[i].c_str(), Name.c_str());
                    Ret.DataList[i] = Buf;
                }
            }
        }
    }
    else
    {
        Ret.UseAutoParameter = UseAutoParameter;
        Ret.ParamN = ParamN;
        Ret.Data = Data;
        if (!UseAutoParameter && ParamN == -2)
        {
            Ret.ParamN = -1;
            if (Data == "%s")Ret.Data = Name;
            else
            {
                sprintf_s(Buf, Data.c_str(), Name.c_str());
                Ret.Data = Buf;
            }
        }
    }
    return Ret;
}
std::string IBB_Module_Default::_NameType::FillIn_NotList(const std::string& Name) const//如果不是List，快速填充参数
{
    static BufString Buf;
    if (IsList)return "";
    else if (ParamN == -1)return Data;
    else
    {
        if (Data == "%s")return Name;
        else
        {
            sprintf_s(Buf, Data.c_str(), Name.c_str());
            return Buf;
        }
    }
}

IBB_Module_Default::_Paragraph IBB_Module_Default::_MergeParagraph::ToParagraph(const std::string& MergeName, const std::string& IniType) const
{
    IBB_Module_Default::_Paragraph Ret;
    Ret.Name = IBB_Module_Default::_NameType(MergeName);
    Ret.IniType = IniType;
    Ret.Variables = Variables;
    IBB_Module_Default::_Paragraph::_Line L;
    for (const auto& Line : Paragraph)
    {
        L.Key = Line.Key.CutMergeName(MergeName);
        L.Value= Line.Value.Text.CutMergeName(MergeName);
        Ret.Paragraph.push_back(L);
    }
    return Ret;
}



IBB_Section_NameType IBB_Module_Default::_Paragraph::GetSectionNameType(IBB_Module_ParagraphList& Param) const
{
    IBB_Section_NameType Ret;
    Ret.Name = Name.GetString(Param);
    Ret.IsLinkGroup = false;
    Ret.VarList = Variables;
    Ret.IniType = IniType;
    for (const auto& L : Paragraph)
    {
        Ret.Lines.Value.insert({ L.Key.GetString(Param),L.Value.GetString(Param) });
    }
    return Ret;
}
std::pair<IBB_Section_NameType, IBB_VariableList> IBB_Module_Default::_MergeParagraph::GetSectionNameType(IBB_Module_ParagraphList& Param) const
{
    std::pair<IBB_Section_NameType, IBB_VariableList> Ret;
    Ret.first.Name = Param.MergeName;
    Ret.first.IsLinkGroup = false;
    Ret.first.VarList = Variables;
    Ret.first.IniType = "";
    for (const auto& L : Paragraph)
    {
        auto s = L.Key.GetString(Param);
        Ret.first.Lines.Value.insert({ s,L.Value.Text.GetString(Param) });
        Ret.second.Value.insert({ s,L.Value.Type });
    }
    return Ret;
}

bool IBB_Module_Default::_NameType::Load(JsonObject FromJson)
{
    if (EnableLogEx)
    {
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "IBB_Module_Default::_NameType::Load <- JsonObject FromJson=%p", FromJson.GetRaw());
        GlobalLogB.AddLog(LogBufB);
    }
    IsList = FromJson.GetObjectItem(u8"IsList").GetBool();
    if (IsList)
    {
        ParamNList = FromJson.GetObjectItem(u8"IndexOfParam").GetArrayInt();
        DataList = FromJson.GetObjectItem(u8"FormatStr").GetArrayString();
        UseAutoParameterList = FromJson.GetObjectItem(u8"UseAutoParameter").GetArrayBool();
    }
    else
    {
        ParamN = FromJson.GetObjectItem(u8"IndexOfParam").GetInt();
        Data = FromJson.GetObjectItem(u8"FormatStr").GetString();
        UseAutoParameter = FromJson.GetObjectItem(u8"UseAutoParameter").GetBool();
    }
    if (EnableLogEx)
    {
        if (IsList)
        {
            GlobalLogB.AddLog_CurTime(false);
            sprintf_s(LogBufB, "IBB_Module_Default::_NameType::Load -> IsList=%s, List Loaded", IBD_BoolStr(IsList)); GlobalLogB.AddLog(LogBufB);//TODO
        }
        else
        {
            GlobalLogB.AddLog_CurTime(false);
            sprintf_s(LogBufB, "IBB_Module_Default::_NameType::Load -> IsList=%s, IndexOfParam=%d, FormatStr=%s, UseAutoParameter=%s",
                IBD_BoolStr(IsList), ParamN, Data.c_str(), IBD_BoolStr(UseAutoParameter)); GlobalLogB.AddLog(LogBufB);
        }
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "IBB_Module_Default::_NameType::Load -> bool Ret=%s", IBD_BoolStr(true)); GlobalLogB.AddLog(LogBufB);
    }
    return true;
}
bool IBB_Module_Default::_NameType::Load(const std::string& FromStr)
{
    if (EnableLogEx)
    {
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "IBB_Module_Default::_NameType::Load <- std::string FromStr=%s", FromStr.c_str());GlobalLogB.AddLog(LogBufB);
    }
    IsList = false;
    ParamN = -1;
    UseAutoParameter = false;
    Data = FromStr;
    if (EnableLogEx)
    {
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "IBB_Module_Default::_NameType::Load -> IsList=false, IndexOfParam=%d, FormatStr=%s, UseAutoParameter=%s",
            ParamN, Data.c_str(), IBD_BoolStr(UseAutoParameter)); GlobalLogB.AddLog(LogBufB);
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "IBB_Module_Default::_NameType::Load -> bool Ret=%s", IBD_BoolStr(true)); GlobalLogB.AddLog(LogBufB);
    }
    return true;
}
bool IBB_Module_Default::_Paragraph::_Line::Load(JsonObject FromJson)
{
    if (EnableLogEx)
    {
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "IBB_Module_Default::_Paragraph::_Line::Load <- JsonObject FromJson=%p", FromJson.GetRaw());GlobalLogB.AddLog(LogBufB);
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "IBB_Module_Default::_Paragraph::_Line::Load -> Key");GlobalLogB.AddLog(LogBufB);
    }
    Key.Load(FromJson.GetObjectItem(u8"Key"));
    if (EnableLogEx)
    {
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "IBB_Module_Default::_Paragraph::_Line::Load -> Value");GlobalLogB.AddLog(LogBufB);
    }
    Value.Load(FromJson.GetObjectItem(u8"Value"));
    if (EnableLogEx)
    {
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "IBB_Module_Default::_NameType::Load -> bool Ret=%s", IBD_BoolStr(true)); GlobalLogB.AddLog(LogBufB);
    }
    return true;
}
bool IBB_Module_Default::_MergeParagraph::_Line::Load(JsonObject FromJson)
{
    Key.Load(FromJson.GetObjectItem(u8"Key"));
    Value.Text.Load(FromJson.GetObjectItem(u8"Value"));
    Value.Type = FromJson.GetObjectItem(u8"Type").GetString();
    return true;
}

bool IBB_Module_Default::_Paragraph::Load(JsonObject FromJson)
{
    Name.Load(FromJson.GetObjectItem(u8"Name"));
    IniType = FromJson.GetObjectItem(u8"IniType").GetString();

    bool Ret = true;
    if (!Variables.Load(FromJson.GetObjectItem(u8"VariableList")))Ret = false;
    for (auto Obj : FromJson.GetObjectItem(u8"Paragraph").GetArrayObject())
    {
        Paragraph.push_back({});
        if (!Paragraph.back().Load(Obj))Ret = false;
    }
    for (auto Pair : FromJson.GetObjectItem(u8"ParagraphConst").GetMapString())
    {
        Paragraph.push_back({});
        Paragraph.back().Key.Load(Pair.first);
        Paragraph.back().Value.Load(Pair.second);
    }
    return Ret;
}
bool IBB_Module_Default::_MergeParagraph::Load(JsonObject FromJson)
{
    bool Ret = true;
    if (!Variables.Load(FromJson.GetObjectItem(u8"VariableList")))Ret = false;
    for (auto Obj : FromJson.GetObjectItem(u8"Paragraph").GetArrayObject())
    {
        Paragraph.push_back({});
        if (!Paragraph.back().Load(Obj))Ret = false;
    }
    for (auto Pair : FromJson.GetObjectItem(u8"ParagraphConst").GetMapString())
    {
        Paragraph.push_back({});
        Paragraph.back().Key.Load(Pair.first);
        if (Pair.second.empty())
        {
            Paragraph.back().Value.Text.Load(Pair.second);
            Paragraph.back().Value.Type = u8"Replace";
        }
        else
        {
            switch (Pair.second.back())
            {
            case 'R':
            case 'r':
                Paragraph.back().Value.Type = u8"Replace"; break;
            case 'M':
            case 'm':
                Paragraph.back().Value.Type = u8"Merge"; break;
            case 'E':
            case 'e':
                Paragraph.back().Value.Type = u8"Reserve"; break;
            default:
                if (EnableLog)
                {
                    GlobalLogB.AddLog_CurTime(false);
                    GlobalLogB.AddLog("IBB_Module_Default::_MergeParagraph::Load ：不应走入 Pair.second.empty() == false && switch(Pair.second.back())->Default 的分支，MergeParagraph的ParagraphConst出现后缀错误。");
                }
                Paragraph.back().Value.Type = u8"Replace"; break;
            }
            Pair.second.pop_back();
            Paragraph.back().Value.Text.Load(Pair.second);
        }
    }
    return Ret;
}
bool IBB_Module_Default::_LinkGroup::Load(JsonObject FromJson)
{
    Name.Load(FromJson.GetObjectItem(u8"Name"));

    bool Ret = true;
    for (auto Obj : FromJson.GetObjectItem(u8"LinkTo").GetArrayObject())
    {
        auto O1 = Obj.GetObjectItem("Ini");
        auto O2 = Obj.GetObjectItem("Section");
        LinkTo.push_back({});
        if (!LinkTo.back()[0].Load(O1))Ret = false;
        if (!LinkTo.back()[1].Load(O2))Ret = false;
    }
    return Ret;
}

bool IBB_Module_Default::_RegisterList::Load(JsonObject FromJson)
{
    Type = FromJson.GetObjectItem(u8"Type").GetString();
    IniType = FromJson.GetObjectItem(u8"IniType").GetString();
    TargetIniType = FromJson.GetObjectItem(u8"TargetIniType").GetString();

    bool Ret = true;
    for (auto Obj : FromJson.GetObjectItem(u8"Data").GetArrayObject())
    {
        ToRegister.push_back({});
        if (!ToRegister.back().Load(Obj))Ret = false;
    }
    return Ret;
}

std::pair<IBB_Section_NameType, std::vector<IBB_Link_NameType>> IBB_Module_Default::_LinkGroup::GetSectionNameType(IBB_Module_ParagraphList& Param) const
{
    std::pair<IBB_Section_NameType, std::vector<IBB_Link_NameType>> Ret;
    Ret.first.IsLinkGroup = true;
    Ret.first.Name = Name.GetString(Param);
    Ret.first.IniType = LinkGroup_IniName;
    for (const auto& To : LinkTo)
        Ret.second.emplace_back(std::string(LinkGroup_IniName),Ret.first.Name,To[0].GetString(Param),To[1].GetString(Param));
    return Ret;
}

bool IBB_Module::Generate(const IBB_Module_Default& Module, const std::vector<std::string>& Param, const IBB_Section_Desc& MergeDesc)
{
    MergeToSection = Module.MergeToSection;
    Name = Module.Name;

    if (EnableLogEx)
    {
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "IBB_Module::Generate <- IBB_Module_Default Module=%p(Name=%s, Merge=%s)",
            &Module,Module.Name.c_str(), IBD_BoolStr(Module.MergeToSection));//TODO : full params
        GlobalLogB.AddLog(LogBufB);
    }

    IBB_Module_ParagraphList PList;
    PList.Param = Param;
    PList.MergeName = MergeDesc.Sec;

    if (MergeToSection)
    {
        std::tie(MergeParagraph, MergeType) = Module.MergeParagraph.GetSectionNameType(PList);
        MergeParagraph.IniType = MergeDesc.Ini;
    }
        

    for (const auto& Reg : Module.AttachedRegister)
    {
        AttachedRegister.emplace_back();
        auto& Back = AttachedRegister.back();
        Back.IniType = Reg.IniType;
        Back.Type = Reg.Type;
        Back.TargetIniType = Reg.TargetIniType;
        Back.UseTargetIniTypeList = false;
        for (const auto& Na : Reg.ToRegister)
        {
            Back.List.push_back(Na.GetString(PList));
        }
    }
    //AttachedParagraph <- Module.AttachedLinkGroup Module.AttachedParagraph
    {
        IBB_Section_NameType PName;
        std::vector<IBB_Link_NameType> PLink;
        for (const auto& Par : Module.AttachedLinkGroup)
        {
            std::tie(PName, PLink) = Par.GetSectionNameType(PList);
            LinkGraph.insert(LinkGraph.end(), PLink.begin(), PLink.end());
        }
    }
    for (const auto& Par : Module.AttachedParagraph)
    {
        AttachedParagraph.push_back(Par.GetSectionNameType(PList));
    }

    return true;
}






















void IBB_Module::Read(const ExtFileClass& File)
{
    File.ReadData(MergeToSection);
    File.ReadData(Name);
    if (MergeToSection)
    {
        MergeParagraph.Read(File);
        MergeType.Read(File);
    }
    File.ReadVector<IBB_RegisterList_NameType>(AttachedRegister, [](auto file, auto data)->bool {data.Read(file); return true; });
    File.ReadVector<IBB_Link_NameType>(LinkGraph, [](auto file, auto data)->bool {data.Read(file); return true; });
    File.ReadVector<IBB_Section_NameType>(AttachedParagraph, [](auto file, auto data)->bool {data.Read(file); return true; });
}

void IBB_Module::Write(const ExtFileClass& File)const
{
    File.WriteData(MergeToSection);
    File.WriteData(Name);
    if (MergeToSection)
    {
        MergeParagraph.Write(File);
        MergeType.Write(File);
    }
    //蠢到爆炸のbug：WriteVector的lambda参数必须const auto&否则导致资源的不当析构！！
    //File.WriteLabel("<AttachedRegister>");
    //::MessageBoxA(NULL, "66B", "Title", MB_OK);
    ::MessageBoxA(NULL, std::to_string(AttachedRegister.size()).c_str() , "Title SB Size=", MB_OK);
    File.WriteVector<IBB_RegisterList_NameType>(AttachedRegister, [](const auto& file, const auto& data)->bool { data.Write(file);  return true; });
    //File.WriteLabel("</AttachedRegister>");
    //File.WriteLabel("<LinkGraph>");
    //::MessageBoxA(NULL, "66D", "Title", MB_OK);
    File.WriteVector<IBB_Link_NameType>(LinkGraph, [](const auto& file, const auto& data)->bool {data.Write(file); return true; });
    //File.WriteLabel("</LinkGraph>");
    //File.WriteLabel("<AttachedParagraph>");
    //::MessageBoxA(NULL, "66E", "Title", MB_OK);
    File.WriteVector<IBB_Section_NameType>(AttachedParagraph, [](const auto& file, const auto& data)->bool {data.Write(file); return true; });
    //File.WriteLabel("</AttachedParagraph>");
}

void IBB_Section_NameType::Read(const ExtFileClass& File)
{
    File.ReadData(Name);
    File.ReadData(IniType);
    File.ReadData(IsLinkGroup);
    VarList.Read(File);
    Lines.Read(File);
}
void IBB_Section_NameType::Write(const ExtFileClass& File)const
{
    File.WriteData(Name);
    File.WriteData(IniType);
    File.WriteData(IsLinkGroup);
    VarList.Write(File);
    Lines.Write(File);
}


void IBB_VariableList::Read(const ExtFileClass& File)
{
    std::vector<std::string> sv;
    File.ReadVector(sv);
    for (size_t i = 0; i + 1 < sv.size(); i += 2)
        Value.insert({ std::move(sv.at(i)),std::move(sv.at(i + 1)) });
}
void IBB_VariableList::Write(const ExtFileClass& File)const
{
    std::vector<std::string> sv;
    for (const auto& p : Value)
    {
        sv.push_back(p.first);
        sv.push_back(p.second);
    }
    File.WriteVector(sv);
}

void IBB_RegisterList_NameType::Read(const ExtFileClass& File)
{
    File.ReadData(Type);
    File.ReadData(IniType);
    File.ReadData(UseTargetIniTypeList);
    File.ReadVector(List);
    if (UseTargetIniTypeList)File.ReadVector(TargetIniTypeList);
    else File.ReadData(TargetIniType);
}
void IBB_RegisterList_NameType::Write(const ExtFileClass& File)const
{
    //File.WriteLabel("<NameType>");
    File.WriteData(Type);
    File.WriteData(IniType);
    File.WriteData(UseTargetIniTypeList);
    //::MessageBoxA(NULL, "91", "Title", MB_OK);
    //File.WriteLabel("<NameType::List>");
    File.WriteVector(List);
    //File.WriteLabel("</NameType::List>");
    //File.WriteLabel("<NameType::TargetIniType>");
    //::MessageBoxA(NULL, "92", "Title", MB_OK);
    if (UseTargetIniTypeList)File.WriteVector(TargetIniTypeList);
    else File.WriteData(TargetIniType);
    //File.WriteLabel("</NameType::TargetIniType>");
    //::MessageBoxA(NULL, "93", "Title", MB_OK);
    //File.WriteLabel("</NameType>");
}

void IBB_Link_NameType::Read(const ExtFileClass& File)
{
    File.ReadData(FromIni);
    File.ReadData(FromSec);
    File.ReadData(ToIni);
    File.ReadData(ToSec);
}
void IBB_Link_NameType::Write(const ExtFileClass& File)const
{
    File.WriteData(FromIni);
    File.WriteData(FromSec);
    File.WriteData(ToIni);
    File.WriteData(ToSec);
}
















const std::string& IBB_Module_ParagraphList::GetParam(int N, bool Random)
{
    if (EnableLogEx)
    {
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "IBB_Module_ParagraphList::GetParam <- int N=%d, bool Random=%s", N, IBD_BoolStr(Random));GlobalLogB.AddLog(LogBufB);
        const std::string& Ret = GetParamEx(N, Random);
        GlobalLogB.AddLog_CurTime(false);
        sprintf_s(LogBufB, "IBB_Module_ParagraphList::GetParam -> std::string Ret=%s", Ret.c_str());GlobalLogB.AddLog(LogBufB);
        return Ret;
    }
    else return GetParamEx(N, Random);
}
const std::string& IBB_Module_ParagraphList::GetParamEx(int N, bool Random)
{
    static std::string Null = "";
    if (Random)
    {
        std::unordered_map<int, std::string>::iterator Iter = RandomParam.find(N);
        if (Iter == RandomParam.end())
            Iter = RandomParam.insert({ N,RandStr(ModuleRandomParameterLength) }).first;
        return Iter->second;
    }
    else
    {
        if (N >= (int)Param.size())return Null;
        else if (N == -1)
        {
            if (EnableLog)
            {
                GlobalLogB.AddLog_CurTime(false);
                GlobalLogB.AddLog("IBB_Module_ParagraphList::GetParam ：不应走入 Random == false && N == -1 的分支，而应该提前处理。");
            }
            return Null;
        }
        else if (N == -2)return MergeName;
        else return Param.at(N);
    }
}

IBB_DefaultTypeList::_Query::_Query()
{
    IniLine_Default_Special_FunctionList = {};//有什么Special类型再添加。
}

bool IBB_DefaultTypeList::BuildQuery()
{
    bool Ret = true;
    for (const auto& p : IniLine_Default)
    {
        if (p.second.Limit.Type == u8"String") { Query.IniLine_Default_Full.insert({ p.second.Limit.Lim,(IBB_IniLine_Default*)std::addressof(p.second) }); }
        else if (p.second.Limit.Type == u8"Special") { Query.IniLine_Default_Special.push_back({ p.second.Limit.Lim,(IBB_IniLine_Default*)std::addressof(p.second) }); }
        else if (p.second.Limit.Type == u8"RegexFull") { Query.IniLine_Default_RegexFull.push_back({ p.second.Limit.Lim,(IBB_IniLine_Default*)std::addressof(p.second) }); }
        else if (p.second.Limit.Type == u8"RegexNone") { Query.IniLine_Default_RegexNone.push_back({ p.second.Limit.Lim,(IBB_IniLine_Default*)std::addressof(p.second) }); }
        else if (p.second.Limit.Type == u8"RegexNotFull") { Query.IniLine_Default_RegexNotFull.push_back({ p.second.Limit.Lim,(IBB_IniLine_Default*)std::addressof(p.second) }); }
        else if (p.second.Limit.Type == u8"RegexNotNone") { Query.IniLine_Default_RegexNotNone.push_back({ p.second.Limit.Lim,(IBB_IniLine_Default*)std::addressof(p.second) }); }
        else
        {
            if (EnableLog)
            {
                GlobalLogB.AddLog_CurTime(false);
                sprintf_s(LogBufB, "IBB_DefaultTypeList::BuildQuery ：Ini行\"%s\"的Limit.Type=\"%s\"不属于任何已知类型。",
                    p.second.Name.c_str(), p.second.Limit.Type.c_str());
                GlobalLogB.AddLog(LogBufB);
            }
            Ret = false;
        }
    }
    for (const auto& p : SubSec_Default)
    {
        for (const auto& s : p.second.Lines_ByName)
        {
            Query.SubSec_Default_FromLineID.insert({ s,(IBB_SubSec_Default*)std::addressof(p.second) });
        }
    }
    return Ret;
}

IBB_IniLine_Default* IBB_DefaultTypeList::KeyBelongToLine(const std::string& KeyName) const
{
    {
        auto It = Query.IniLine_Default_Full.find(KeyName);
        if (It != Query.IniLine_Default_Full.end())return It->second;
    }
    {
        for (const auto& p : Query.IniLine_Default_Special)
        {
            auto It = Query.IniLine_Default_Special_FunctionList.find(p.first);
            if (It != Query.IniLine_Default_Special_FunctionList.end())
                if (It->second(KeyName))
                    return p.second;
        }
    }
    {
        for (const auto& p : Query.IniLine_Default_RegexFull)if (RegexFull_Nothrow(KeyName, p.first))return p.second;
        for (const auto& p : Query.IniLine_Default_RegexNone)if (RegexNone_Nothrow(KeyName, p.first))return p.second;
        for (const auto& p : Query.IniLine_Default_RegexNotFull)if (RegexNotFull_Nothrow(KeyName, p.first))return p.second;
        for (const auto& p : Query.IniLine_Default_RegexNotNone)if (RegexNotNone_Nothrow(KeyName, p.first))return p.second;
    }
    return nullptr;
}

IBB_SubSec_Default* IBB_DefaultTypeList::KeyBelongToSubSec(const std::string& KeyName) const
{
    auto ptr = KeyBelongToLine(KeyName);
    if (ptr == nullptr)return nullptr;
    auto it = Query.SubSec_Default_FromLineID.find(ptr->Name);
    if (it == Query.SubSec_Default_FromLineID.end())return nullptr;
    return it->second;
}
