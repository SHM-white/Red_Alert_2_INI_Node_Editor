#pragma once

#include<cstdio>
#include<string>
#include<vector>
#include<cstdint>
#include<functional>

class ExtFileClass
{
	FILE* fp;
	bool IsOpen{ false };
public:
	FILE* GetPlain()const
	{
		return fp;
	} 
    bool Open(const char* path, const char* mode, void* Reserved = nullptr)
    {
        (void)Reserved;
        fp = fopen(path, mode);
        return IsOpen = (fp != NULL);
    }
    bool Open(const wchar_t* path, const wchar_t* mode, void* Reserved = nullptr)
    {
        (void)Reserved;
        IsOpen = (_wfopen_s(&fp, path, mode) == 0);
        return IsOpen;
    }
	size_t Read(void* Buf, size_t Size, size_t Count)const
	{
		if (!IsOpen)return 0;
		return fread(Buf, Size, Count, fp);
	}
	size_t Write(const void* Buf, size_t Size, size_t Count)const
	{
		if (!IsOpen)return 0;
		return fwrite(Buf, Size, Count, fp);
	}
	char GetChr()const
	{
		if (!IsOpen)return 0;
		return (char)fgetc(fp);
	}
	bool PutChr(const char Byte)const
	{
		if (!IsOpen)return 0;
		return fputc(Byte, fp)!=EOF;
	}
	bool Ln()const
	{
		return PutChr('\n');
	}
	char* GetStr(char* Buf, size_t Size)const
	{
		if (!IsOpen)return 0;
		return fgets(Buf, Size, fp);
	}
	size_t PutStr(const char* Buf)const
	{
		if (!IsOpen)return 0;
		return fputs(Buf, fp);
	}
	int Close()
	{
		if (!IsOpen)return 0;
		IsOpen = false;
		return fclose(fp);
	}
	int Seek(int Offset,int Base)const
	{
		return fseek(fp,Offset,Base);
	}
	int Position()const
	{
		return ftell(fp);
	}
	bool Eof()const
	{
		if (!IsOpen)return true;
		return feof(fp);
	}
	bool Available()const
	{
		return IsOpen;
	}
    void Rewind()const
    {
        rewind(fp);
    }
    int Flush()const
    {
        return fflush(fp);
    }

    bool ReadData(std::string& Str)const
    {
        const size_t MaxSize = 65536;
        static char Buf[MaxSize];
        int64_t Size;
        if (!Read(&Size, sizeof(Size), 1))return false;
        char* _Buf;
        if (Size > MaxSize - 1)_Buf = new char[(size_t)Size + 2];
        else _Buf = Buf;
        _Buf[Read(_Buf, 1, (size_t)Size)] = 0;
        Str = _Buf;
        if (Size > MaxSize - 1)delete[] _Buf;
        return true;
    }
    bool WriteData(const std::string& Str)const
    {
        int64_t Size = Str.size();
        if (!Write(&Size, sizeof(Size), 1))return false;
        Write(Str.c_str(), 1, Str.size());
        return true;
    }
    bool WriteLabel(const std::string& Str)const
    {
        if (!Write(Str.c_str(), 1, Str.size()))return false;
        WriteData('\0');
        return true;
    }
    bool ReadLabel(std::string& Str)const//�����ԣ���
    {
        const size_t MaxSize = 65536;
        static char Buf[MaxSize];
        size_t Cur = 0;
        while (ReadData(Buf[Cur]))if (Buf[Cur++] == '\0')break;
        Str = Buf;
        return true;
    }
    template<typename T>
    bool WriteData(const T& Data)const
    {
        return Write(&Data, sizeof(T), 1);
    }
    template<typename T>
    bool ReadData(T& Data)const
    {
        return Read(&Data, sizeof(T), 1);
    }
    template<typename T>
    bool WriteVector(const std::vector<T>& Data)const//�����ԣ���
    {
        if (!WriteData((int64_t)Data.size()))return false;
        return Write(Data.data(), sizeof(T), Data.size());
    }
    template<typename T>
    bool ReadVector(std::vector<T>& Data)const//�����ԣ���
    {
        int64_t Size;
        if (!ReadData(Size))return false;
        Data.resize((size_t)Size);
        return Read(Data.data(), sizeof(T), Size);
    }
    int WriteVector(const std::vector<std::string>& Data)const//�����ԣ���
    {
        if (!WriteData((int64_t)Data.size()))return 0;
        int Ret = 0;
        for (const auto& d : Data)
        {
            if (!WriteData(d)) break;
            else ++Ret;
        }
        return Ret;
    }
    int ReadVector(std::vector<std::string>& Data)const//�����ԣ���
    {
        int64_t Size;
        if (!ReadData(Size))return 0;
        Data.resize((size_t)Size);
        int Ret = 0;
        for (auto& d : Data)
        {
            if (!ReadData(d)) break;
            else ++Ret;
        }
        return Ret;
    }
    template<typename T>
    int WriteVector(const std::vector<T>& Data, const std::function<bool(const ExtFileClass&, const T&)>& Proc)const//�����ԣ���
    {
        if (!WriteData((int64_t)Data.size()))return 0;
        int Ret = 0;
        for (const auto& d : Data)
        {
            if (!Proc(*this, d)) break;
            else ++Ret;
        }
        return Ret;
    }
    template<typename T>
    int WriteVector(std::vector<T>& Data, const std::function<bool(const ExtFileClass&, T&)>& Proc)const//�����ԣ���
    {
        if (!WriteData((int64_t)Data.size()))return 0;
        int Ret = 0;
        for (auto& d : Data)
        {
            if (!Proc(*this, d)) break;
            else ++Ret;
        }
        return Ret;
    }
    template<typename T>
    int ReadVector(std::vector<T>& Data, const std::function<bool(const ExtFileClass&, T&)>& Proc)const//�����ԣ���
    {
        int64_t Size;
        if (!ReadData(Size))return 0;
        Data.resize((size_t)Size);
        int Ret = 0;
        for (auto& d : Data)
        {
            if (!Proc(*this, d)) break;
            else ++Ret;
        }
        return Ret;
    }

	~ExtFileClass()
	{
		if (IsOpen)Close();
	}
};

