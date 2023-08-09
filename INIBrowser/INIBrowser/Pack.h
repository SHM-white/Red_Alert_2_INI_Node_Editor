#pragma once

#include".\FromEngine\Include.h"
#include".\FromEngine\SpIni.h"


#define FILE_ATTRIB_DIR 16
#define FILE_ATTRIB_FILE 32
#define GLOBAL_FPS 12

std::atomic<bool> InFileProcess{ false };

//std::vector<std::string> FileBuf;
std::mutex FileBufLock;
std::vector<std::function<void()>> FileBuf;
void WritePackUIAction()
{
    FileBufLock.lock();
    for (auto fc : FileBuf)fc();
    FileBufLock.unlock();
}


std::string ExtName(const std::string& ss)//拓展名，无'.' 
{
    using namespace std;
    auto p = ss.find_last_of('.');
    return p == ss.npos ? "" : string(ss.begin() + min(p + 1, ss.length()), ss.end());
}
std::string FileTag(const std::string& ss)//文件去掉拓展名和最后一个'.' 
{
    using namespace std;
    return string(ss.begin(), ss.begin() + min(ss.find_last_of('.'), ss.length()));
}

void TravelFiles(const std::string& search, std::vector<std::pair<std::string, unsigned> >& finfo)
{
    using namespace std;
    long handle;                                                //用于查找的句柄
    _finddata_t fileinfo;                       //文件信息的结构体
    handle = _findfirst(search.c_str(), &fileinfo);         //第一次查找
    if (-1 == handle)return;
    finfo.push_back({ fileinfo.name,fileinfo.attrib });                         //打印出找到的文件的文件名
    while (!_findnext(handle, &fileinfo))               //循环查找其他符合的文件，知道找不到其他的为止
    {
        finfo.push_back({ fileinfo.name,fileinfo.attrib });
    }
    _findclose(handle);
}


BOOL IsDirectory(const char* pDir)
{
    char szCurPath[500];
    ZeroMemory(szCurPath, 500);
    sprintf_s(szCurPath, 500, "%s//*", pDir);
    WIN32_FIND_DATAA FindFileData;
    ZeroMemory(&FindFileData, sizeof(WIN32_FIND_DATAA));

    HANDLE hFile = FindFirstFileA(szCurPath, &FindFileData); /**< find first file by given path. */

    if (hFile == INVALID_HANDLE_VALUE)
    {
        FindClose(hFile);
        return FALSE; /** 如果不能找到第一个文件，那么没有目录 */
    }
    else
    {
        FindClose(hFile);
        return TRUE;
    }

}

bool DeleteDirectory(const char* DirName, bool DelSelf)
{
    //	CFileFind tempFind;		//声明一个CFileFind类变量，以用来搜索
    char szCurPath[MAX_PATH];		//用于定义搜索格式
    _snprintf(szCurPath, MAX_PATH, "%s//*.*", DirName);	//匹配格式为*.*,即该目录下的所有文件
    WIN32_FIND_DATAA FindFileData;
    ZeroMemory(&FindFileData, sizeof(WIN32_FIND_DATAA));
    HANDLE hFile = FindFirstFileA(szCurPath, &FindFileData);
    BOOL IsFinded = TRUE;
    while (IsFinded)
    {
        IsFinded = FindNextFileA(hFile, &FindFileData);	//递归搜索其他的文件
        if (strcmp(FindFileData.cFileName, ".") && strcmp(FindFileData.cFileName, "..")) //如果不是"." ".."目录
        {
            std::string strFileName{};
            strFileName = strFileName + DirName + "//" + FindFileData.cFileName;
            std::string strTemp{};
            strTemp = strFileName;
            if (IsDirectory(strFileName.c_str()))DeleteDirectory(strTemp.c_str(), DelSelf);
            else DeleteFileA(strTemp.c_str());
        }
    }
    FindClose(hFile);
    if (DelSelf)RemoveDirectoryA(DirName);
    return true;
}


