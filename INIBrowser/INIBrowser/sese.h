#pragma once

#include"FromEngine/Include.h"

#define SESE 

int __StupidClick = 0, __StupidTime = 0, __Multi = 1;
auto __Site = L"https://api.nmb.show/1985acg.php";//L"";

void SeseCheck()
{
	__StupidClick += __Multi;
	if (__StupidClick >= 30 - 4 * __StupidTime)
	{
		if (!__StupidTime)
		{
			MessageBoxW(nullptr, L"std::iron_hammer : \n你也真够闲的慌的", L"灵魂拷问", MB_ICONQUESTION);
			MessageBoxW(nullptr, L"std::iron_hammer : \n鉴于你锲而不舍的精神", L"灵魂拷问", MB_ICONQUESTION);
			MessageBoxW(nullptr, L"std::iron_hammer : \n我这里有点好康的", L"灵魂拷问", MB_ICONQUESTION);
			MessageBoxW(nullptr, L"std::iron_hammer : \n来啦！来看就知道啦！", L"灵魂拷问", MB_ICONQUESTION);
		}
		else if (__StupidClick > 15)
		{
			MessageBoxW(nullptr, L"std::iron_hammer : \n你居然还想着涩涩", L"灵魂拷问", MB_ICONQUESTION);
			MessageBoxW(nullptr, L"std::iron_hammer : \n你不看INI老想着涩涩干什么", L"灵魂拷问", MB_ICONQUESTION);
			MessageBoxW(nullptr, L"std::iron_hammer : \n但是也没关系", L"灵魂拷问", MB_ICONQUESTION);
			MessageBoxW(nullptr, L"std::iron_hammer : \n她会继续涩涩的", L"灵魂拷问", MB_ICONQUESTION);
		}
		else if (__StupidClick > 4)
		{
			MessageBoxW(nullptr, L"std::iron_hammer : \n你居然还想着涩涩", L"作者的吐槽", MB_ICONQUESTION);
			MessageBoxW(nullptr, L"std::iron_hammer : \n但好像她不反感涩涩", L"作者的吐槽", MB_ICONQUESTION);
			MessageBoxW(nullptr, L"std::iron_hammer : \n好像还挺喜欢", L"作者的吐槽", MB_ICONQUESTION);
			MessageBoxW(nullptr, L"std::iron_hammer : \n她似乎越来越频繁的想涩涩了", L"作者的提示", MB_ICONQUESTION);
		}
		else
		{
			MessageBoxW(nullptr, L"std::iron_hammer : \n由于你一直涩涩", L"很抱歉", MB_ICONINFORMATION);
			MessageBoxW(nullptr, L"std::iron_hammer : \n她已经被你调教成了RBQ", L"很抱歉", MB_ICONINFORMATION);
			MessageBoxW(nullptr, L"std::iron_hammer : \n所以她只会涩涩了", L"很抱歉", MB_ICONINFORMATION);
			MessageBoxW(nullptr, L"std::iron_hammer : \n没救了，重启软件吧", L"很抱歉", MB_ICONINFORMATION);
			MessageBoxW(nullptr, L"std::iron_hammer : \n那我先走了", L"警告：作者溜之大吉", MB_ICONWARNING);
            glfwHideWindow(PreLink::window);
			WCHAR yell[2000] = L"RBQ : \n";
			for (int life = 18; life > 0; life--)
			{
				MessageBoxW(nullptr, L"RBQ : \n要涩涩！要贴贴！", L"INI浏览器", MB_HELP);
				::ShellExecute(nullptr, L"open", __Site, NULL, NULL, SW_SHOWNORMAL);
				wcscat(yell, L"啊~");
				MessageBoxW(nullptr, yell, L"INI浏览器", MB_HELP);
			}
			wcscat(yell, L"\n");
			for (int i = 0; i < 15; i++)
			{
				wcscat(yell, L"啊❤");
			}
			wcscat(yell, L"GC了呢❤呃~");
			MessageBoxW(nullptr, yell, L"INI浏览器", MB_HELP);
			MessageBoxW(nullptr, L"兴奋过度，卒", L"INI浏览器", MB_HELP);
            PreLink::CleanUp();
            exit(0);
		}

		__StupidClick = 0;
		__StupidTime++;
		::ShellExecute(nullptr, L"open", __Site, NULL, NULL, SW_SHOWNORMAL);
	}
}
