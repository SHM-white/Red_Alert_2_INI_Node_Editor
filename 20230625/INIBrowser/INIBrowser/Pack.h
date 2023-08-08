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


std::string ExtName(const std::string& ss)//��չ������'.' 
{
    using namespace std;
    auto p = ss.find_last_of('.');
    return p == ss.npos ? "" : string(ss.begin() + min(p + 1, ss.length()), ss.end());
}
std::string FileTag(const std::string& ss)//�ļ�ȥ����չ�������һ��'.' 
{
    using namespace std;
    return string(ss.begin(), ss.begin() + min(ss.find_last_of('.'), ss.length()));
}

void TravelFiles(const std::string& search, std::vector<std::pair<std::string, unsigned> >& finfo)
{
    using namespace std;
    long handle;                                                //���ڲ��ҵľ��
    _finddata_t fileinfo;                       //�ļ���Ϣ�Ľṹ��
    handle = _findfirst(search.c_str(), &fileinfo);         //��һ�β���
    if (-1 == handle)return;
    finfo.push_back({ fileinfo.name,fileinfo.attrib });                         //��ӡ���ҵ����ļ����ļ���
    while (!_findnext(handle, &fileinfo))               //ѭ�������������ϵ��ļ���֪���Ҳ���������Ϊֹ
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
        return FALSE; /** ��������ҵ���һ���ļ�����ôû��Ŀ¼ */
    }
    else
    {
        FindClose(hFile);
        return TRUE;
    }

}

bool DeleteDirectory(const char* DirName, bool DelSelf)
{
    //	CFileFind tempFind;		//����һ��CFileFind�����������������
    char szCurPath[MAX_PATH];		//���ڶ���������ʽ
    _snprintf(szCurPath, MAX_PATH, "%s//*.*", DirName);	//ƥ���ʽΪ*.*,����Ŀ¼�µ������ļ�
    WIN32_FIND_DATAA FindFileData;
    ZeroMemory(&FindFileData, sizeof(WIN32_FIND_DATAA));
    HANDLE hFile = FindFirstFileA(szCurPath, &FindFileData);
    BOOL IsFinded = TRUE;
    while (IsFinded)
    {
        IsFinded = FindNextFileA(hFile, &FindFileData);	//�ݹ������������ļ�
        if (strcmp(FindFileData.cFileName, ".") && strcmp(FindFileData.cFileName, "..")) //�������"." ".."Ŀ¼
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


