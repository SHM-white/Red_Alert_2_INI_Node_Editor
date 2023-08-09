
#include"SaveFile.h"
#include"Global.h"


int32_t IntBuf, IntBuf2, IntBuf3, IntBuf4;
char ChrBuf, ChrBuf2, ChrBuf3, ChrBuf4;
std::string StrBuf, StrBuf2, StrBuf3, StrBuf4;

bool SaveFile::Read(const wchar_t* Name)
{
    int32_t RealVMajor, RealVMinor, RealVRelease, RealVersion, NBlock, BlockBeginOffset;

    if (!File.Open(Name, L"rb"))return false;

    File.ReadData(IntBuf);
    if (IntBuf != SaveFileHeaderSign)return false;
    File.ReadData(BlockBeginOffset);
    File.ReadData(RealVMajor);
    File.ReadData(RealVMinor);
    File.ReadData(RealVRelease);
    RealVersion = RealVMajor * 10000 + RealVMinor * 100 + RealVRelease;
    File.ReadData(NBlock);
    //for (int i = 0; i < 2048; i++)File.ReadData(ChrBuf);//Blank Of 2048 Bytes
    File.Seek(BlockBeginOffset, SEEK_SET);

    for (int i = 0; i < NBlock; i++)
    {
        File.ReadData(IntBuf);
        File.ReadData(IntBuf2);
        if (IntBuf == 0)
        {
            for (int j = 0; j < IntBuf2; j++)File.ReadData(ChrBuf);//Blank Of Many Bytes
        }
        else
        {
            File.ReadData(StrBuf);
            auto Iter = ReadBlockProcess.find(StrBuf);
            if (Iter == ReadBlockProcess.end())
            {
                for (int j = 0; j < IntBuf2; j++)File.ReadData(ChrBuf);//Cannot Analyze Block So I Skip It
            }
            else
            {
                Iter->second.Process(File, RealVersion, IntBuf2);
            }
        }
    }

    File.Close();

    return true;
}

bool SaveFile::Write(const wchar_t* Name)
{
    const int32_t FileHeaderReserve = 128;//这就是文件头给预留的空间（单位：字节），文件头超过这个大小就扩容一下啦，方便随不同版本改变文件头大小

    if (!File.Open(Name, L"wb"))return false;

    File.WriteData(SaveFileHeaderSign);
    File.WriteData(FileHeaderReserve);
    
    File.WriteData((int32_t)VersionMajor);
    File.WriteData((int32_t)VersionMinor);
    File.WriteData((int32_t)VersionRelease);
    File.WriteData((int32_t)WriteBlockProcess.size());
    while (File.Position() < FileHeaderReserve)File.WriteData((char)0);

    for (auto Block : WriteBlockProcess)
    {
        if (Block.UsingID.empty())File.WriteData((int32_t)0);
        else File.WriteData((int32_t)1);
        int TempPos = File.Position();
        File.WriteData((int32_t)0);

        if (!Block.UsingID.empty())File.WriteData(Block.UsingID);
        int TPos1= File.Position();
        Block.Process(File);
        int TPos2 = File.Position();

        File.Seek(TempPos, SEEK_SET);
        File.WriteData((int32_t)(TPos2 - TPos1));
        File.Seek(0, SEEK_END);
        
    }

    File.Close();

    return true;
}
