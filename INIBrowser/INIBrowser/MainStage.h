// Copyright (C) 2019 Nomango

#pragma once


#include"FromEngine/Include.h"
#include"FromEngine/global_timer.h"
#include"FromEngine/Csf.h"
#include"Global.h"
#include"FromEngine/SpIni.h"
#include"IBFront.h"
#include"IBRender.h"
#include"Pack.h"
#include<ShlObj.h>

std::string FontPath = ".\\Resources\\";//全过程不变
std::wstring Defaultpath{ L"C:\\" };
std::string Pathbuf, Desktop, TextEditAboutU8;
std::wstring PathbufW;

int FPSLimit = -1;

wchar_t CurrentDirW[5000];//调用当前目录时必须调用这两个，不要临时求！
char CurrentDirA[5000];//因为OpenFileDialog会改变当前目录，所以必须提前加载

int RecentFileLimit = 10;
std::vector<std::string>RecentFilePath;//UTF-8


int IBG_ExitProcess();

int DynamicDataXDelta, DynamicDataYDelta;

bool Await{ true };
bool ProjectLoad_Final{ false };

void ControlPanel();
void ControlPanel_Debug();


std::vector<std::function<void(int)>>DelayWindow;

RECT FinalWP;

int CurrentINIPage = 0;

namespace ImGui
{
    ImVec2 GetLineEndPos()
    {
        return { ImGui::GetWindowPos().x + ImGui::GetWindowWidth(),ImGui::GetCursorScreenPos().y };
    }
    ImVec2 GetLineBeginPos()
    {
        return { ImGui::GetWindowPos().x ,ImGui::GetCursorScreenPos().y };
    }
    bool IsWindowClicked(ImGuiMouseButton Button)
    {
        return ImGui::IsWindowHovered() && ImGui::IsMouseClicked(Button);
    }
}




namespace _TempSelectLink
{
    const ImColor ForegroundCoverColor(0, 145, 255, 35);
    const ImColor ForegroundMarkColor(0, 100, 255, 255);
    const ImColor LinkingLineColor(255, 168, 21, 255);
    const ImColor LegalLineColor(255, 138, 5, 255);
    const ImColor IllegalLineColor(255, 45, 45, 255);
    bool InLinkProcess;
    int LinkFrom, LinkAt;
    ImVec2 MouseStartPos;
    
    void InLink(int From, int At)
    {
        if (!InLinkProcess)
        {
            InLinkProcess = true;
            LinkFrom = From;
            LinkAt = At;
            MouseStartPos = ImGui::GetLineEndPos();
        }
    }
    int OutLink()
    {
        if (InLinkProcess)
        {
            InLinkProcess = false;
            return LinkAt;
        }
        return -1;
    }
    bool IsInLink()
    {
        return InLinkProcess;
    }

    struct SectionRect
    {
        ImVec2 PosUL, PosDR;
    };
    std::vector<SectionRect>SecRect;
    void AddSecRect()
    {
        SecRect.emplace_back(SectionRect{});
        SecRect.back().PosUL = ImGui::GetWindowPos();
        SecRect.back().PosDR = {
            SecRect.back().PosUL.x + ImGui::GetWindowSize().x,
            SecRect.back().PosUL.y + ImGui::GetWindowSize().y };
    }
    void ClearLoop()
    {
        SecRect.clear();
    }

}

const int MaxSec = 2000;
int InitialNSec = 0, CurrentNSec = InitialNSec;
bool PressA[MaxSec]{}, PressB[MaxSec]{};
std::vector<int>Linkto[MaxSec];
std::vector<dImVec2>LinkStartPosL[MaxSec];
std::vector<dImVec2>LinkStartPosR[MaxSec];
dImVec2 WindowStartPosUL[MaxSec];
dImVec2 WindowStartPosUR[MaxSec];
dImVec2 WindowZoomDirection[MaxSec];
double RealHeight[MaxSec];
bool WindowFocus[MaxSec];

dImVec2 InitialWindowZoomDirection[MaxSec];
double InitialSTScale;
int PrevSet = 0;


void ControlPanelTemp(int iButton)
{
    ImGui::Text(u8"UI测试中");

    _TempSelectLink::AddSecRect();

    std::vector<int>ReFill{};
    std::vector<dImVec2>ReFillPosL, ReFillPosR;
    int iLink = 0;
    for (int n : Linkto[iButton])
    {
        if (n == -1)
        {
            ImGui::PushID(n + 0x114513);
            if (!ImGui::SmallButton(u8"删除"))
            {
                ReFill.push_back(n);
                ReFillPosR.push_back(ImGui::GetLineEndPos());
                ReFillPosL.push_back(ImGui::GetLineBeginPos());
            }
            ImGui::SameLine();
            ImGui::Text(u8"未链接");
            ImGui::SameLine();
            ImGui::PushID(n + 0x114512);
            if (ImGui::SmallButton("+"))
            {
                _TempSelectLink::InLink(iButton, iLink);
            }
        }
        else
        {
            ImGui::PushID(n + 0x114513);
            if (!ImGui::SmallButton(u8"删除"))
            {
                ReFill.push_back(n);
                ReFillPosR.push_back(ImGui::GetLineEndPos());
                ReFillPosL.push_back(ImGui::GetLineBeginPos());
            }
            ImGui::SameLine();
            ImGui::Text(u8"链接到 %d ", n);
            ImGui::SameLine();
            ImGui::PushID(n + 0x114514);
            if (ImGui::ArrowButton("loopex", ImGuiDir_Right))
            {
                IBR_Inst_Debug.AddMsgOnce([=]() {ImGui::Text(u8"Window %d : Focus %d ", iButton, n); });
                DelayWindow.push_back([=](int u) {if (u == n) {
                    ImGui::SetNextWindowFocus(); ImGui::SetNextWindowCollapsed(false);
                }});
            }
        }
        iLink++;
    }

    
    WindowFocus[iButton] = ImGui::IsWindowFocused();
    Linkto[iButton] = ReFill;
    LinkStartPosL[iButton] = ReFillPosL;
    LinkStartPosR[iButton] = ReFillPosR;
    if (ImGui::Button(u8"链接+"))
    {
        Linkto[iButton].push_back(-1);
        LinkStartPosL[iButton].push_back({ 0.0f,0.0f });
        LinkStartPosR[iButton].push_back({ 0.0f,0.0f });
    }
    if (_TempSelectLink::IsInLink())
    {
        if (ImGui::IsWindowClicked(ImGuiMouseButton_Left))
        {
            Linkto[_TempSelectLink::LinkFrom][_TempSelectLink::OutLink()] = iButton;
        }
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            Linkto[_TempSelectLink::LinkFrom][_TempSelectLink::OutLink()] = -1;
        }
    }



}

bool WaitingInsertLoad{ false };
int A_INI = -2, A_Sec = -2;

BufString TempTargetPath;

bool ChangeWithFontSize;//EVENT : FONT_SCALE_CHANGE

bool PanelOnHold = false;


void ControlPanel()
{
    static bool First = true;

    _TempSelectLink::ClearLoop();
    GetWindowRect(MainWindowHandle, &FinalWP);
    IBR_UICondition::UpdateWindowTitle();
    IBR_WorkSpace::UpdatePrev();
    if (First)
    {
        IBR_DynamicData::Open();
        DynamicDataXDelta = ScrX - (FinalWP.right - FinalWP.left);
        DynamicDataYDelta = ScrY - (FinalWP.bottom - FinalWP.top);
        if (EnableLog)
        {
            GlobalLog.AddLog_CurTime(false);
            GlobalLog.AddLog("主循环开始运转。");
        }
        First = false;
    }
    IBR_UICondition::CurrentScreenWidth = FinalWP.right - FinalWP.left + DynamicDataXDelta;
    IBR_UICondition::CurrentScreenHeight = FinalWP.bottom - FinalWP.top + DynamicDataYDelta;
    IBR_DynamicData::Save();
    IBRF_CoreBump.IBR_AutoProc();


    ImGui::Begin("INIBrowserMainMenu", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoCollapse |
        //ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove );
    ImGui::SetWindowPos({ 0,-FontHeight * 0.0f });
    ImGui::SetWindowSize({ (float)(FinalWP.right - FinalWP.left - WindowSizeAdjustX),FontHeight * 2.0f });

    IBG_Undo.RenderUI();
    ImGui::SameLine();
    IBR_Inst_Menu.RenderUIBar();

    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 14.0f * FontHeight);
    ImGui::Text(u8"INI浏览器 V%s 平均FPS %.1f", Version.c_str(), ImGui::GetIO().Framerate);

    ImGui::End();

    if (IBR_UICondition::MenuChangeShow)
    {
        ImGui::SetNextWindowCollapsed(false);
        IBR_UICondition::MenuChangeShow = false;
    }
    ImGui::Begin(u8"主菜单", nullptr,
        //ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoNav |
        //ImGuiWindowFlags_NoCollapse |
        //ImGuiWindowFlags_NoBackground |
        //ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove );
    ImGui::SetWindowPos({ 0,FontHeight * 2.0f - WindowSizeAdjustY });
    ImGui::SetWindowSize({ std::max(ImGui::GetWindowWidth(),
        std::min(((float)(FinalWP.right - FinalWP.left - WindowSizeAdjustX)) / 4 ,400.0f)),
       (float)(FinalWP.bottom - FinalWP.top) - FontHeight * 3.5f + WindowSizeAdjustY});

    IBR_RealCenter::Update();
    IBR_Inst_Menu.RenderUIMenu();

    ImGui::NewLine(); ImGui::NewLine();
	ImGui::End();

    ChangeWithFontSize = (fabs(IBR_WorkSpace::RatioPrev - IBR_FullView::Ratio) > 0.001f);
    IBR_Inst_Debug.ClearMsgCycle();
    IBR_WorkSpace::UpdatePrevII();
    
    auto DelayWindowB = DelayWindow;
    DelayWindow.clear();
    auto RFontHeight = FontHeight;
    FontHeight = (int)(FontHeight * IBR_FullView::Ratio);
    dImVec2 PrevClipZoneCenter = IBR_RealCenter::Center;
    IBR_RealCenter::Center = (IBR_RealCenter::WorkSpaceUL + IBR_RealCenter::WorkSpaceDR) / 2.0;
    bool ChangeWithClipZone = (fabs(PrevClipZoneCenter.x - IBR_RealCenter::Center.x) > 0.001//EVENT : CLIP_ZONE_CHANGE
        || fabs(PrevClipZoneCenter.y - IBR_RealCenter::Center.y) > 0.001);
    double WindowHeightMin = FontHeight * 12.0, PrevWindowHeightMin = (int)(RFontHeight * IBR_WorkSpace::RatioPrev) * 12.0;
    ImVec2 OffsetDelta = IBR_WorkSpace::EqCenterPrev - IBR_FullView::EqCenter;
    bool ChangeWithOffset = fabs(OffsetDelta.x) > 0.001 || fabs(OffsetDelta.y) > 0.001;

    IBR_Inst_Debug.AddMsgCycle([=]() {ImGui::Text("PrevSet = %s", (PrevSet?"true":"false")); });
    IBR_Inst_Project.DataCheck();

    IBR_WorkSpace::RenderUI();

    for (int i = 0; i < CurrentNSec; i++)
    {
        for (auto f : DelayWindowB)f(i);

        ImGui::Begin((u8"子菜单 - " + std::to_string(i)).c_str(), nullptr, ImGuiWindowFlags_NoClamping);
        ImGui::SetWindowFontScale(IBR_FullView::Ratio);
        ImGui::PushClipRect(IBR_RealCenter::WorkSpaceUL, IBR_RealCenter::WorkSpaceDR, true);

        IBR_Inst_Debug.AddMsgCycle([=]() {ImGui::Text("Window %d Pos = ( %.2f, %.2f )", i, WindowStartPosUL[i].x, WindowStartPosUL[i].y); });
        //IBR_Inst_Debug.AddMsgCycle([=]() {ImGui::Text("Window %d Height = %.2f", i, RealHeight[i]); });

        ImVec2 SWSize{ FontHeight * 12.0f, 0.0f };
        if (fabs(WindowStartPosUL[i].x) < 0.01 || fabs(WindowStartPosUL[i].y) < 0.01)
            { WindowStartPosUL[i] = ImGui::GetWindowPos(); }
        if (fabs(RealHeight[i]) < 0.001) { RealHeight[i] = std::max(ImGui::GetWindowHeight(), FontHeight * 12.0f); }
        dImVec2 SWCenter = WindowStartPosUL[i] + (SWSize / 2.0);
        if (fabs(WindowZoomDirection[i].x) < 0.01 || fabs(WindowZoomDirection[i].y) < 0.01)
            { WindowZoomDirection[i] = SWCenter - IBR_RealCenter::Center; }


        if (ChangeWithFontSize)
        {

            if (PrevSet <= i)
            {
                ++PrevSet;
                InitialWindowZoomDirection[i] = WindowZoomDirection[i];
                InitialSTScale = IBR_WorkSpace::RatioPrev;
            }

            WindowZoomDirection[i] = InitialWindowZoomDirection[i] * IBR_FullView::Ratio / InitialSTScale;
            SWCenter = WindowZoomDirection[i] + IBR_RealCenter::Center;
            WindowStartPosUL[i] = SWCenter - (SWSize / 2.0);
            WindowStartPosUR[i] = WindowStartPosUL[i];
            WindowStartPosUR[i].x += ImGui::GetWindowWidth();
            SWSize.y = (float)RealHeight[i];
            ImGui::SetWindowPos(WindowStartPosUL[i]);

            if (fabs(RealHeight[i] - PrevWindowHeightMin) < FontHeight + 2.0)
            {
                SWSize.y = (float)WindowHeightMin;
                RealHeight[i] = WindowHeightMin;
                ImGui::SetWindowSize(SWSize);
            }
            else
            {
                RealHeight[i] = RealHeight[i] * IBR_FullView::Ratio / IBR_WorkSpace::RatioPrev;
                SWSize.y = (float)RealHeight[i];
                ImGui::SetWindowSize(SWSize);
            }

        }
        else
        {
            if (ChangeWithClipZone)
            {
                PrevSet = 0;
                //DebugVecOnce.push_back([=]() {ImGui::Text("Window %d Change Clip", i); });

                SWCenter = WindowZoomDirection[i] + IBR_RealCenter::Center;
                WindowStartPosUL[i] = SWCenter - (SWSize / 2.0);
                WindowStartPosUR[i] = WindowStartPosUL[i];
                WindowStartPosUR[i].x += ImGui::GetWindowWidth();
                SWSize.y = (float)RealHeight[i];
                ImGui::SetWindowPos(WindowStartPosUL[i]);
            }
            else if (ChangeWithOffset)
            {
                WindowZoomDirection[i] = WindowZoomDirection[i] + OffsetDelta;
                SWCenter = WindowZoomDirection[i] + IBR_RealCenter::Center;
                WindowStartPosUL[i] = SWCenter - (SWSize / 2.0);
                WindowStartPosUR[i] = WindowStartPosUL[i];
                WindowStartPosUR[i].x += ImGui::GetWindowWidth();
                SWSize.y = (float)RealHeight[i];
                ImGui::SetWindowPos(WindowStartPosUL[i]);
            }
            else
            {
                if (fabs((WindowStartPosUL[i] - ImGui::GetWindowPos()).max()) > 1.0 )
                {
                    PrevSet = 0;
                    //DebugVecOnce.push_back([=]() {ImGui::Text("Window %d Change Pos", i); });

                    WindowStartPosUL[i] = ImGui::GetWindowPos();
                    WindowStartPosUR[i] = ImGui::GetWindowPos();
                    WindowStartPosUR[i].x += ImGui::GetWindowWidth();
                    SWCenter = WindowStartPosUL[i] + (SWSize / 2.0);
                    WindowZoomDirection[i] = SWCenter - IBR_RealCenter::Center;
                }

                if (fabs((float)RealHeight[i] - ImGui::GetWindowHeight()) > 2.0f)
                {
                    PrevSet = 0;
                    //DebugVecOnce.push_back([=]() {ImGui::Text("Window %d Change", i); });
                }
                SWSize.y = std::max(ImGui::GetWindowHeight(), (float)WindowHeightMin);
                RealHeight[i] = SWSize.y;
                ImGui::SetWindowSize(SWSize);
                
            }
        }

        /* {
            ImDrawList* DList = ImGui::GetForegroundDrawList();
            DList->AddCircle(WindowStartPosUL[i], 5.0, _TempSelectLink::IllegalLineColor, 0, 2.0f);
            DList->AddCircle(WindowStartPosUL[i] + (SWSize / 2.0), 5.0, _TempSelectLink::IllegalLineColor, 0, 2.0f);
            DList->AddCircle(WindowStartPosUL[i] + SWSize, 5.0, _TempSelectLink::IllegalLineColor, 0, 2.0f);
            DList->AddRect(WindowStartPosUL[i], WindowStartPosUL[i] + SWSize, _TempSelectLink::LinkingLineColor, 0.0f, 0, 2.0f);
            DList->AddLine(WindowStartPosUL[i] + (SWSize / 2.0), IBR_RealCenter::Center, _TempSelectLink::ForegroundMarkColor, 8.0f);
        }*/

        ControlPanelTemp(i);
        ImGui::End();
    }


    ImDrawList* DList = ImGui::GetForegroundDrawList();
    ImDrawList* BList = ImGui::GetBackgroundDrawList();
    if (_TempSelectLink::IsInLink())
    {
        for (auto sq : _TempSelectLink::SecRect)
        {
            DList->AddRectFilled(sq.PosUL, sq.PosDR, _TempSelectLink::ForegroundCoverColor);
        }
        DList->AddLine(_TempSelectLink::MouseStartPos, ImGui::GetMousePos(), _TempSelectLink::LinkingLineColor, FontHeight / 5.0f);
    }
    for (int i = 0; i < CurrentNSec; i++)
    {
        for (int j = 0; j < (int)Linkto[i].size(); j++)
        {
            if (Linkto[i][j] == -1)continue;
            else if (Linkto[i][j] >= CurrentNSec)
            {
                auto par = LinkStartPosR[i][j];
                par.x -= FontHeight * 0.5f;
                par.y -= FontHeight * 0.5f;
                BList->AddLine(par, dImVec2{ par.x + FontHeight * 3.0, par.y},
                    _TempSelectLink::IllegalLineColor, FontHeight / 4.0f);
            }
            else
            {
                auto pal = LinkStartPosL[i][j];
                auto par = LinkStartPosR[i][j];
                auto pbl = WindowStartPosUL[Linkto[i][j]];
                auto pbr = WindowStartPosUR[Linkto[i][j]];
                pal.x += FontHeight * 0.5f;
                pal.y -= FontHeight * 0.5f;
                par.x -= FontHeight * 0.5f;
                par.y -= FontHeight * 0.5f;
                pbl.x += FontHeight * 0.5f;
                pbl.y += FontHeight * 0.5f;
                pbr.x -= FontHeight * 0.5f;
                pbr.y += FontHeight * 0.5f;
                ImVec2 pa, pb;
                pa = pal.x >= pbr.x ? pal : par;
                pb = pbl.x >= par.x ? pbl : pbr;

                pa.y += FontHeight / 6.0f;
                BList->AddBezierCurve(pa, { (pa.x + 4 * pb.x) / 5,pa.y }, { (4 * pa.x + pb.x) / 5,pb.y }, pb,
                    _TempSelectLink::LegalLineColor, FontHeight / 6.0f);
                pa.y -= FontHeight / 6.0f;
                BList->AddBezierCurve(pa, { (pa.x + 4 * pb.x) / 5,pa.y }, { (4 * pa.x + pb.x) / 5,pb.y }, pb,
                    _TempSelectLink::LegalLineColor, FontHeight / 6.0f);
                pa.y -= FontHeight / 6.0f;
                BList->AddBezierCurve(pa, { (pa.x + 4 * pb.x) / 5,pa.y }, { (4 * pa.x + pb.x) / 5,pb.y }, pb,
                    _TempSelectLink::LegalLineColor, FontHeight / 6.0f);
            }
        }
    }
    
    //IBR_WorkSpace::EqCenterPrev = IBR_FullView::EqCenter;
    FontHeight = RFontHeight;

    IBR_SelectMode::RenderUI();
    IBR_HintManager::RenderUI();
    IBR_PopupManager::RenderUI();
}



