
#include "IBSave.h"
#include "IBRender.h"
#include "Global.h"
#include ".\FromEngine\global_tool_func.h"

namespace InsertLoad
{
    SelectFileRet SelectFileName(HWND Root, const SelectFileType& Type, BOOL(_stdcall* Proc)(LPOPENFILENAMEW))
    {
        OPENFILENAMEW ofn;
        WCHAR szFile[296]{};

        std::wstring wptitle = Type.Title;

        memset(&ofn, 0, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = Root;
        ofn.lpstrFilter = Type.Filter.c_str(); //"导入数据(*_pack.dat)\0*_pack.dat\0所有文件 (*.*)\0*.*\0\0"
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFileTitle = wptitle.data();//"选择要导入的文件"
        ofn.nMaxFileTitle = wptitle.size();
        ofn.lpstrInitialDir = Type.InitialPath.c_str();
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;

        SelectFileRet Ret;
        Ret.Success = (Proc(&ofn));
        if (Ret.Success)
        {
            if (szFile[0])
            {
                Ret.RetBuf = szFile;
                Ret.PathOffset = ofn.nFileOffset;
            }
        }
        else
        {
            Ret.RetBuf = L"";
            Ret.PathOffset = 0;
        }
        if (EnableLog)
        {
            GlobalLog.AddLog_CurTime(false);
            GlobalLog.AddLog("选择文件。");
        }
        return Ret;
    }

}
/*

namespace InsertLoad
{
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


}


*/

std::atomic<bool> IBS_Suspended{ false };
InfoStack<StdMessage> SStack;
DWORD SThreadID;
SaveFile ProjSL;//Save & Load of a project
/*
std::string ExtName(const std::string& ss)//拓展名，无'.' 
{
    using namespace std;
    auto p = ss.find_last_of('.');
    return p == ss.npos ? "" : string(ss.begin() + min(p + 1, ss.length()), ss.end());
}*/
std::string FileName(const std::string& ss)//文件名
{
    using namespace std;
    auto p = ss.find_last_of('\\');
    return p == ss.npos ? "" : string(ss.begin() + min(p + 1, ss.length()), ss.end());
}
std::wstring FileName(const std::wstring& ss)//文件名
{
    using namespace std;
    auto p = ss.find_last_of(L'\\');
    return p == ss.npos ? L"" : wstring(ss.begin() + min(p + 1, ss.length()), ss.end());
}


ReadFileHeader IBS_LoadFProject
{
    "IBF_Project" ,
     [](const ExtFileClass& File,int FVersion,int Length)-> bool
    {
        if (EnableLog)
        {
            GlobalLogB.AddLog_CurTime(false);
            GlobalLogB.AddLog("调用了IBS_LoadFProject.Action。");
        }
        (void)Length;
        if (FVersion < 200)
        {
            //TODO
            return false;
        }
        else if (FVersion > VersionN)
        {
            //TODO
            return false;
        }
        File.ReadData(IBS_Inst_Project.Front.CreateTime);
        File.ReadData(IBS_Inst_Project.Front.CreateVersionMajor);
        File.ReadData(IBS_Inst_Project.Front.CreateVersionMinor);
        File.ReadData(IBS_Inst_Project.Front.CreateVersionRelease);
        IBS_Inst_Project.Front.ProjectSecs.Read(File);
        return true;
    }
};
ReadFileHeader IBS_LoadRProject
{
    "IBR_Project" ,
     [](const ExtFileClass& File,int FVersion,int Length)-> bool
    {
        if (EnableLog)
        {
            GlobalLogB.AddLog_CurTime(false);
            GlobalLogB.AddLog("调用了IBS_LoadRProject.Action。");
        }
        (void)Length;
        (void)FVersion;
        //Checked Version Before
        File.ReadData(IBS_Inst_Project.Render.FullView_EqCenter);
        File.ReadData(IBS_Inst_Project.Render.FullView_Ratio);
        size_t NDesc;
        File.ReadData(NDesc);
        IBS_Inst_Project.Render.Project_SecData.resize(NDesc);
        for (size_t i = 0; i < NDesc; i++)
            IBS_Inst_Project.Render.Project_SecData.at(i).Read(File);
        return true;
    }
};

WriteFileHeader IBS_SaveFProject
{
    "IBF_Project" ,
     [](const ExtFileClass& File)-> bool
     {
        if (EnableLog)
        {
            GlobalLogB.AddLog_CurTime(false);
            GlobalLogB.AddLog("调用了IBS_SaveFProject.Action。");
        }
        File.WriteData(IBS_Inst_Project.Front.CreateTime);
        File.WriteData(IBS_Inst_Project.Front.CreateVersionMajor);
        File.WriteData(IBS_Inst_Project.Front.CreateVersionMinor);
        File.WriteData(IBS_Inst_Project.Front.CreateVersionRelease);
        //File.WriteLabel("<IBS_Inst_Project.Front>");
        IBS_Inst_Project.Front.ProjectSecs.Write(File);
        //File.WriteLabel("</IBS_Inst_Project.Front>");
        //::MessageBoxA(NULL, "66C", "Title", MB_OK);
        return true;
     }
};
WriteFileHeader IBS_SaveRProject
{
    "IBR_Project" ,
     [](const ExtFileClass& File)-> bool
     {
        if (EnableLog)
        {
            GlobalLogB.AddLog_CurTime(false);
            GlobalLogB.AddLog("调用了IBS_SaveRProject.Action。");
        }
        File.WriteData(IBS_Inst_Project.Render.FullView_EqCenter);
        File.WriteData(IBS_Inst_Project.Render.FullView_Ratio);
        File.WriteData(IBS_Inst_Project.Render.Project_SecData.size());
        for (const auto& Desc : IBS_Inst_Project.Render.Project_SecData)
            Desc.Write(File);
        return true;
     }
};

void IBS_Thr_SaveLoop()
{
    SThreadID = GetCurrentThreadId();

    ProjSL.ReadBlockProcess[IBS_LoadFProject.UsingID] = IBS_LoadFProject;
    ProjSL.ReadBlockProcess[IBS_LoadRProject.UsingID] = IBS_LoadRProject;
    ProjSL.WriteBlockProcess.push_back(IBS_SaveFProject);
    ProjSL.WriteBlockProcess.push_back(IBS_SaveRProject);

    IBS_Complete();
    while (1)
    {
        for (const auto& msg : SStack.GetCopyAndClear())
        {
            msg();
        }
        IBS_Complete();
    }
}

void IBS_Complete()
{
    if (!IBS_Suspended.load())
    {
        IBS_Suspended.store(true);
        IBG_SuspendThread(SThreadID);
    }
}

void IBS_Push(const StdMessage& Msg)
{
    SStack.Push(Msg);
    if (IBS_Suspended.load())
    {
        IBS_Suspended.store(false);
        IBG_ResumeThread(SThreadID);
    }
}

bool IBS_Project::Save()
{
    return ProjSL.Write(Front.Path.c_str());
}
bool IBS_Project::Load()
{
    Front.ProjName = FileName(Front.Path);
    return ProjSL.Read(Front.Path.c_str());
}

bool IBS_Project::Save(const std::wstring& _Path)
{
    Front.Path = _Path;
    return ProjSL.Write(Front.Path.c_str());
}
bool IBS_Project::Load(const std::wstring& _Path)
{
    Front.Path = _Path;
    Front.ProjName = FileName(Front.Path);
    return ProjSL.Read(Front.Path.c_str());
}

void IBS_Project::_Render::_SecData::Read(const ExtFileClass& File)
{
    File.ReadData(EqPos);
    File.ReadData(EqSize);
    File.ReadData(Desc.Ini);
    File.ReadData(Desc.Sec);
}

void IBS_Project::_Render::_SecData::Write(const ExtFileClass& File)const
{
    File.WriteData(EqPos);
    File.WriteData(EqSize);
    File.WriteData(Desc.Ini);
    File.WriteData(Desc.Sec);
}
