
#include"external_file.h"
#include"external_log.h"
#include"Csf.h"
#include<algorithm>

char CToLower(char c) { return (char)std::tolower(c); }

bool CSFClass::Load(const char* name, bool IgLetterCase)
{
    IgnoreLetterCase = IgLetterCase;

    static char gbuf[10000];
    ExtFileClass fp;
    if (!fp.Open(name, "rb"))return Loaded = false;
    gbuf[4] = 0;
    fp.Read(gbuf, 1, 4);
    if (std::string(gbuf) != " FSC")return Loaded = false;
    fp.Read(&Version, 4, 1);
    fp.Read(&LblN, 4, 1);
    fp.Read(&StrN, 4, 1);
    fp.Read(&Language, 4, 1);
    fp.Read(&Reserved, 4, 1);
    //cout<<Version<<endl<<LblN<<endl<<StrN<<endl<<Language<<endl;
    uint32_t NLblStr, StrLen;
    std::string Key, Ext;
    std::wstring Value;
    bool HasExt;
    for (size_t i = 0; i < LblN; i++)
    {
        gbuf[4] = 0;
        fp.Read(gbuf, 1, 4);
        if (std::string(gbuf) != " LBL")return Loaded = false;
        fp.Read(&NLblStr, 4, 1);
        fp.Read(&StrLen, 4, 1);
        gbuf[StrLen] = 0;
        fp.Read(gbuf, 1, StrLen);
        Key = gbuf;
        //cout<<Key<<endl;

        gbuf[4] = 0;
        fp.Read(gbuf, 1, 4);
        if (std::string(gbuf) != " RTS" && std::string(gbuf) != "WRTS")return Loaded = false;
        HasExt = (std::string(gbuf) == "WRTS");
        fp.Read(&StrLen, 4, 1);
        gbuf[StrLen << 1] = gbuf[StrLen << 1 | 1] = 0;
        fp.Read(gbuf, 1, StrLen << 1);
        for (size_t j = 0; j < (StrLen << 1); j++)gbuf[j] = ~gbuf[j];
        Value = (const wchar_t*)gbuf;
        //cout<<UnicodetoMBCS(Value)<<endl;

        if (HasExt)
        {
            fp.Read(&StrLen, 4, 1);
            gbuf[StrLen] = 0;
            fp.Read(gbuf, 1, StrLen);
            Ext = gbuf;
            //cout<<Ext<<endl;
        }

        if(IgnoreLetterCase)std::transform(Key.begin(), Key.end(), Key.begin(), CToLower);
        CsfMap[Key] = { Value,Ext };
    }
    fp.Close();
    return Loaded = true;
}


CSFClass::sdata CSFClass::GetStr(const std::string& _Key)
{
    auto Key = _Key;
    if (IgnoreLetterCase)std::transform(Key.begin(), Key.end(), Key.begin(), CToLower);
    auto it = CsfMap.find(Key);
    if (it == CsfMap.end())return { L"","" };
    else return it->second;
}
