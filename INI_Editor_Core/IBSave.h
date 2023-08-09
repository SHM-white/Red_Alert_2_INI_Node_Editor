#pragma once
#include "FromEngine/Include.h"
#include "FromEngine/RFBump.h"
#include "IBBack.h"

namespace InsertLoad
{
    struct SelectFileType
    {
        std::wstring InitialPath;
        std::wstring Title;
        std::wstring Filter;
    };
    struct SelectFileRet
    {
        std::wstring RetBuf;
        int PathOffset;
        bool Success;
    };

    SelectFileRet SelectFileName(HWND Root, const SelectFileType& Type, BOOL(_stdcall* Proc)(LPOPENFILENAMEW));

    inline SelectFileRet SelectOpenFileName(HWND Root, const SelectFileType& Type)
    {
        return SelectFileName(Root, Type, ::GetOpenFileNameW);
    }

    inline SelectFileRet SelectSaveFileName(HWND Root, const SelectFileType& Type)
    {
        return SelectFileName(Root, Type, ::GetSaveFileNameW);
    }

}

inline std::string MinorDir(const std::string& ss)
{
    return std::string(ss.begin() + std::min(ss.find_last_of('\\') + 1, ss.length() - 1), ss.end());
}


void IBS_Push(const StdMessage& Msg);

void IBS_Complete();

void IBS_Thr_SaveLoop();


struct IBS_Project
{
    struct _Front
    {
        IBB_Module ProjectSecs;
        std::wstring ProjName;
        uint64_t CreateTime;
        int CreateVersionMajor, CreateVersionMinor, CreateVersionRelease;
        std::wstring Path;
    }Front;
    struct _Render
    {
        struct _SecData
        {
            IBB_Section_Desc Desc;
            ImVec2 EqPos, EqSize;
            void Read(const ExtFileClass& File);
            void Write(const ExtFileClass& File)const;
        };

        float FullView_Ratio;
        ImVec2 FullView_EqCenter;
        std::vector<_SecData> Project_SecData;
    }Render;

    bool Save();
    bool Load();
    bool Save(const std::wstring& _Path);
    bool Load(const std::wstring& _Path);
};
