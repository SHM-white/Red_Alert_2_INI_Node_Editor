#pragma once

#include"Include.h"

struct IniSpecial:public Ini::IniFile
{
	std::vector<int> CutLineBegin;
	std::vector<int> CutLineEnd;
	bool Load(const char* fname);
	IniSpecial()=default;
	IniSpecial(const char* fname)
	{
		Load(fname);
	}
};

bool IniSpecial::Load(const char* fname)
{
	using namespace std;
	using namespace Ini;
	
	IniName.Set(fname);
	ExtFileClass sf;
	sf.Open(fname, "r");
	if ( !sf.Available() || sf.Eof()) return Loaded = false;
	BufString buf0 = { 0 };
	char* buf = buf0;
	FileSec[StrHash("")] = IniSection("");
	hash_t cur_sec = StrHash("");
	char* cmt_start_pos = NULL;
	int AtLine,SecN=0,LastKeyLine=0;
	
	for(AtLine=0;!sf.Eof();++AtLine)
	{
		bool has_cmt = true;
		memset(buf0, 0, sizeof(buf0));
		sf.GetStr(buf0, MAX_LINE_LENGTH);
		buf = buf0;
		buf = RemoveFrontSpace(buf0);//SECTION / KEY 均如此处理 
		RemoveBackSpace(buf);
		int buf_len = strlen(buf);
		if (buf_len < 2)continue;//空行优化 
		cmt_start_pos = strchr(buf, ';');
		if (cmt_start_pos == NULL)//没注释 
		{
			cmt_start_pos = buf + buf_len;
			has_cmt = false;
		}
		*cmt_start_pos = 0;//暂时截断 
		if (cmt_start_pos == buf)//;COMMENT
		{
			if (has_cmt)*cmt_start_pos = ';';
			continue;
		}
		if (*buf == '[')//[SECTION]
		{
			char* ttp = strchr(buf, ']');
			if (ttp != NULL)
			{
				*ttp = 0;
				StringData tmp_label;
				tmp_label.Set(buf + 1);
				FileSec[tmp_label.HashVal] = IniSection(tmp_label);
				cur_sec = tmp_label.HashVal;
				if (has_cmt)*cmt_start_pos = ';';
				SectionSequence.push_back(tmp_label.HashVal);
				*ttp = ']';
				
				if(SecN)CutLineBegin.push_back(LastKeyLine+1);
				else CutLineBegin.push_back(AtLine);
				if(SecN)CutLineEnd.push_back(AtLine-1);
				++SecN;
				
				continue;
			}
		}
		if (strchr(buf, '=') == NULL)//dhsfjkhdsj;COMMENT  or  sdhfjdhsdjf
		{
			if (has_cmt)*cmt_start_pos = ';';
			continue;
		}
		{//a=b;COMMENT
			IniLine tmp_l;
			char* a = new BufString, * b = new BufString;
			sscanf(buf, " %[^=]= %[^;]", a, b);
			RemoveBackSpace(a);
			RemoveBackSpace(b);
			tmp_l.Key.Set(a); tmp_l.Value.Set(b);
			FileSec[cur_sec].SecStr[tmp_l.GetHash()] = tmp_l;
			hash_t htp = tmp_l.GetHash();
			FileSec[cur_sec].KeySequence.push_back(htp);
			if (has_cmt)*cmt_start_pos = ';';
			delete[] a; delete[] b;
			
			LastKeyLine=AtLine;
			
			continue;
		}
	}
	
	if(SecN)
	{
		CutLineBegin[0]=0;//覆盖开头 
		CutLineEnd.push_back(AtLine-1);
	}
	
	sf.Close();
	return Loaded = true;
}
