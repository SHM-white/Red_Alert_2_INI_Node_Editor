#include "FromEngine/ini.h"
#include "IBBack.h"
#include "FromEngine/global_tool_func.h"

void IBB_TransformSection(IBB_Section_NameType& Dest, const Ini::IniSection& Src)
{
    //Dest.IniType
    Dest.IsLinkGroup = false;
    Dest.Name = Src.SecName.str;
    Dest.VarList.Value.clear();
    for (const auto& pLine : Src.SecStr)
    {
        Dest.Lines.Value.insert({ pLine.second.Key.str,pLine.second.Value.str });
    }
}
IBB_Section_NameType IBB_TransformSection(const Ini::IniSection& Src)
{
    IBB_Section_NameType ds;
    IBB_TransformSection(ds, Src);
    return ds;
}
