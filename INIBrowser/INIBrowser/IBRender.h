#pragma once
#include "IBFront.h"
#include "FromEngine/RFBump.h"
#include "IBSave.h"
#include <any>

class IBR_Setting
{
public:
    const wchar_t* SettingName;
    void SetSettingName(const wchar_t* Name);
    void CallReadSetting();
    bool IsReadSettingComplete();
    void CallSaveSetting();//call every 5 second with [setting] window
    bool IsSaveSettingComplete();

    void RenderUI();
};



dImVec2 operator+(const dImVec2 a, const dImVec2 b);
dImVec2 operator-(const dImVec2 a, const dImVec2 b);
ImVec4 operator+(const ImVec4 a, const ImVec4 b);
dImVec2 operator/(const dImVec2 a, const double b);
dImVec2 operator*(const dImVec2 a, const double b);

int IBG_ExitProcess();








struct IBR_Debug
{
    struct _UICond
    {
        bool LoopShow{ true }, OnceShow{ true };
    }UICond;
    struct _Data
    {
        bool Nope;
    }Data;

    std::vector<StdMessage>DebugVec, DebugVecOnce;
    void AddMsgCycle(const StdMessage& Msg);
    void ClearMsgCycle();
    void AddMsgOnce(const StdMessage& Msg);

    void DebugLoad();

    void RenderUI();
    void RenderUIOnce();
};

#define _PROJ_CMD_UPDATE
#define _PROJ_CMD_CAN_UNDO
#define _PROJ_CMD_WRITE
#define _PROJ_CMD_READ
#define _PROJ_CMD_NOINTERRUPT //此操作不挂起后端线程
#define _PROJ_CMD_BACK_CONST

#define _CALL_CMD

#define _RETURN_BACK_DATA //使用相应返回值时请先RInterruptF锁住后端线程再读写



struct IBR_Section;
struct IBR_Project;
struct IBB_Section_Desc;
extern const IBB_Section_Desc IBB_Section_DescNull;

struct IBR_SectionData
{
    IBB_Section_Desc Desc;
    ImVec2 EqPos, EqSize;
    bool Exists{ false };
    bool Hovered{ false };
    bool First{ true };
    bool IsOpen{ true };

    //int UpdateResult;


    IBR_SectionData();
    IBR_SectionData(const IBB_Section_Desc& D);

    void RenderUI();
};

struct IBR_Section
{
    IBR_Project* Root;
    uint64_t ID;
    //改动其中存储内容应修改IBR_Project::GetSection

    
    _RETURN_BACK_DATA IBB_Section* _PROJ_CMD_READ GetBack() ;
    _RETURN_BACK_DATA const IBB_Section* _PROJ_CMD_READ GetBack() _PROJ_CMD_BACK_CONST const;
    _RETURN_BACK_DATA const IBB_Section* _PROJ_CMD_NOINTERRUPT _PROJ_CMD_READ GetBack_Unsafe() const;

    //若Sec不存在给Function传入nullptr
    template<typename T>
    T _PROJ_CMD_READ _PROJ_CMD_WRITE OperateBackData(const std::function<T(IBB_Section*)>& Function);
    /*template<typename T>
    T _PROJ_CMD_READ _PROJ_CMD_WRITE OperateBackDataWithUndo(
        const std::function<T(IBB_Section*)>& Undo,
        const std::function<T(IBB_Section*)>& Redo);*/

    const IBB_Section_Desc& _PROJ_CMD_NOINTERRUPT GetSectionDesc() _PROJ_CMD_BACK_CONST const;

    //此Sec是否存在
    bool _PROJ_CMD_READ HasBack() const;

    _RETURN_BACK_DATA IBB_VariableList* _PROJ_CMD_READ GetVarList() _PROJ_CMD_BACK_CONST const;
    IBB_VariableList _PROJ_CMD_READ GetVarListCopy() _PROJ_CMD_BACK_CONST const;
    IBB_VariableList _PROJ_CMD_READ GetVarListFullCopy(bool PrintExtraData) _PROJ_CMD_BACK_CONST const;

    bool _PROJ_CMD_WRITE _PROJ_CMD_CAN_UNDO _PROJ_CMD_UPDATE SetVarList(const IBB_VariableList& NewList);

    //不建议跨INI类型复制，除非你确定你在做什么，并且为字段设置正确的变量表，以符合模板的整体约定
    bool _PROJ_CMD_WRITE _PROJ_CMD_CAN_UNDO _PROJ_CMD_UPDATE DuplicateSection(const IBB_Section_Desc& NewDesc) _PROJ_CMD_BACK_CONST const;
    IBR_Section  _PROJ_CMD_READ _PROJ_CMD_WRITE _PROJ_CMD_CAN_UNDO _PROJ_CMD_UPDATE DuplicateSectionAndBack(const IBB_Section_Desc& NewDesc) _PROJ_CMD_BACK_CONST const;

    //如果名字冲突则啥也不干并返回false
    bool _PROJ_CMD_WRITE _PROJ_CMD_CAN_UNDO _PROJ_CMD_UPDATE Rename(const std::string& NewName);

    bool _PROJ_CMD_WRITE _PROJ_CMD_CAN_UNDO Register(const std::string& Name, const std::string& IniName) _PROJ_CMD_BACK_CONST const;

    IBR_SectionData* _PROJ_CMD_READ _PROJ_CMD_NOINTERRUPT GetSectionData() const;//可能为空

private:
    IBB_Section_Desc* _PROJ_CMD_NOINTERRUPT GetDesc() const;
    IBB_Section* GetBack_Inl() const;
};

struct IBR_Project
{
    typedef uint64_t id_t;
    id_t MaxID{ 0 };//TODO:你还能使用超过ULL_MAX个ID？要是真的如此那就修一修
    std::map<id_t, IBR_SectionData> IBR_SectionMap;
    std::map<IBB_Section_Desc, id_t> IBR_Rev_SectionMap;

    struct _Plink
    {
        ImVec2 BeginL,BeginR;
        IBB_Section_Desc Dest;
    };
    std::vector<_Plink> LinkList;

    bool _PROJ_CMD_WRITE _PROJ_CMD_CAN_UNDO _PROJ_CMD_UPDATE AddModule(
        const std::string& ModuleName,
        const std::vector<std::string>& ParamList,
        const IBB_Section_Desc& MergeDesc);
    bool _PROJ_CMD_WRITE _PROJ_CMD_CAN_UNDO _PROJ_CMD_UPDATE AddModule(
        int ModuleID,
        const std::vector<std::string>& ParamList,
        const IBB_Section_Desc& MergeDesc);

    bool _PROJ_CMD_WRITE _PROJ_CMD_UPDATE UpdateAll();

    bool _PROJ_CMD_WRITE _PROJ_CMD_UPDATE ForceUpdate();

    template<typename T>
    T _PROJ_CMD_READ _PROJ_CMD_WRITE OperateBackData(const std::function<T(void)>& Function);
    /*template<typename T>
    T _PROJ_CMD_READ _PROJ_CMD_WRITE _PROJ_CMD_CAN_UNDO OperateBackDataWithUndo(
        const std::function<T(void)>& Undo,
        const std::function<T(void)>& Redo);*/

    //bool _PROJ_CMD_READ WriteTextToFolder();

    //它是UTF8纯粹是因为方便统一INI存储格式
    _TEXT_UTF8 std::string _PROJ_CMD_READ GetText(bool PrintExtraData) _PROJ_CMD_BACK_CONST;

    //这个东西没啥开销，除了复制一份Desc ； 不保证是否存在
    IBR_Section _PROJ_CMD_NOINTERRUPT _PROJ_CMD_READ GetSection(const IBB_Section_Desc& Desc) _PROJ_CMD_BACK_CONST;

    //不建议，会缺少模板里面约定的一部分Variable（包括类型标记）
    bool _PROJ_CMD_WRITE _PROJ_CMD_CAN_UNDO _PROJ_CMD_UPDATE CreateSection(const IBB_Section_Desc& Desc);
    IBR_Section _PROJ_CMD_READ _PROJ_CMD_WRITE _PROJ_CMD_CAN_UNDO _PROJ_CMD_UPDATE CreateSectionAndBack(const IBB_Section_Desc& Desc);

    //同GetSection的HasBack
    bool _PROJ_CMD_READ HasSection(const IBB_Section_Desc& Desc) _PROJ_CMD_BACK_CONST;

    bool _PROJ_CMD_WRITE _PROJ_CMD_CAN_UNDO _PROJ_CMD_UPDATE DeleteSection(const IBB_Section_Desc& Desc);

    //不保证ID有效
    inline IBR_Section _PROJ_CMD_NOINTERRUPT _PROJ_CMD_READ GetSectionFromID(id_t id) _PROJ_CMD_BACK_CONST { return { this,id }; }

    std::optional<id_t> _PROJ_CMD_NOINTERRUPT _PROJ_CMD_READ GetSectionID(const IBB_Section_Desc& Desc) _PROJ_CMD_BACK_CONST;
    void _PROJ_CMD_NOINTERRUPT _PROJ_CMD_READ EnsureSection(const IBB_Section_Desc& Desc) _PROJ_CMD_BACK_CONST;

    bool _PROJ_CMD_UPDATE DataCheck();

    void Load(const IBS_Project&);
    void Save(IBS_Project&);
};


namespace IBR_RealCenter
{
    extern ImVec2 Center;
    extern dImVec2 WorkSpaceUL, WorkSpaceDR;
    bool Update();
}

namespace IBR_UICondition
{
    extern int CurrentScreenWidth, CurrentScreenHeight;
    extern bool MenuChangeShow;
    extern std::wstring WindowTitle;
    bool UpdateWindowTitle();
}

//按钮有特别的样式
struct IBR_MainMenu
{
    struct _Item
    {
        std::function<bool()> Button;
        std::function<void()> Menu;
    };
    std::vector<_Item> ItemList;
    IBR_MainMenu() {}
    IBR_MainMenu(const std::vector<_Item>& v) :ItemList(v) {}
    void RenderUIBar();
    void RenderUIMenu();
private:
    size_t Choice;
};
extern IBR_MainMenu IBR_Inst_Menu;

struct IBG_UndoStack
{
    struct _Item
    {
        _TEXT_ANSI std::string Id;
        std::function<void()> UndoAction, RedoAction;
        std::function<std::any()> Extra;
    };
    std::vector<_Item> Stack;
    //直接赋值得同时改Cursor
    int Cursor{ -1 };
    bool Undo();
    bool Redo();
    bool CanUndo() const;
    bool CanRedo() const;
    void Release();
    void Push(const _Item& a);
    void RenderUI();
    void Clear();
    _Item* Top();
};
extern IBG_UndoStack IBG_Undo;

namespace IBR_ProjectManager
{
    bool IsOpen();
    void OpenAction();
    void OpenRecentAction(const std::wstring& Path);
    void CreateAction();
    void CloseAction();
    void SaveAction();
    void SaveAsAction();
    void SaveOptAction();

};

namespace IBR_FullView
{
    extern ImVec2 EqCenter;
    extern float Ratio;
    extern ImVec2 ViewSize;
    void RenderUI();
}

namespace IBR_WorkSpace
{
    extern ImVec2 EqCenterPrev, ReCenterPrev;
    extern float RatioPrev;
    extern int UpdatePrevResult;
    void UpdatePrev();
    void  _PROJ_CMD_CAN_UNDO _PROJ_CMD_UPDATE UpdatePrevII();
    void  _PROJ_CMD_CAN_UNDO _PROJ_CMD_UPDATE RenderUI();
    enum _UpdatePrev
    {
        _UpdatePrev_None = 0,
        _UpdatePrev_Ratio = 1,
        _UpdatePrev_EqCenter = 2
    };
    inline ImVec2 EqPosToRePos(ImVec2 EqPos) { return (EqPos - IBR_FullView::EqCenter) * IBR_FullView::Ratio + IBR_RealCenter::Center; }
    inline ImVec2 RePosToEqPos(ImVec2 RePos) { return (RePos - IBR_RealCenter::Center) / IBR_FullView::Ratio + IBR_FullView::EqCenter; }
    bool InWorkSpace(ImVec2 RePos);
}
namespace IBR_SelectMode
{
    struct Mode
    {
        bool RestrictedToSections;
        std::function<void(IBR_Section, ImVec2)>Exit;
        StdMessage Cancel;
    };
    void EnterSelectMode(const Mode& Mode);
    void ExitSelectMode(IBR_Section Section, ImVec2 ClickRePos);
    void CancelSelectMode();
    void RenderUI();
    bool InSelectMode();
    const Mode& CurrentSelectMode();
}

namespace IBR_DynamicData
{
    void Read(int DefaultResX, int DefaultResY);
    void SetRandom();
    void Open();
    void Save();
    void Close();
}

namespace IBR_RecentManager
{
    void RenderUI();
    void Load();
    void Push(const std::wstring& Path);
    void Save();
}

namespace IBR_EditFrame
{
    extern IBR_Section CurSection;
    extern IBR_Project::id_t PrevId;
    extern bool Empty;
    void SetActive(IBR_Project::id_t id);
    void UpdateSection();
    void RenderUI();
    void Clear();
};

namespace IBR_Color
{
    static const ImColor BackgroundColor(253, 241, 157, 255);//背景色
    static const ImColor FocusWindowColor(170, 204, 244, 255);
    static const ImColor ClipFrameLineColor(108, 255, 45, 255);
    static const ImColor CenterCrossColor(255, 5, 5, 255);
    static const ImColor ForegroundCoverColor(0, 145, 255, 35);
    static const ImColor ForegroundMarkColor(0, 100, 255, 255);
    static const ImColor LegalLineColor(255, 138, 5, 255);
    static const ImColor LinkingLineColor(255, 168, 21, 255);
    static const ImColor IllegalLineColor(255, 45, 45, 255);
    static const ImColor ErrorTextColor(255, 45, 45, 255);
}


namespace IBR_PopupManager
{
    struct Popup
    {
        bool CanClose;//Only when Modal==true
        bool Modal;
        _TEXT_UTF8 std::string Title;
        ImGuiWindowFlags Flag{ ImGuiWindowFlags_None };
        StdMessage Show;
        StdMessage Close;//run when it can be closed and it is closed

        ImVec2 Size{ 0,0 };

        Popup& Create(const _TEXT_UTF8 std::string& title);
        Popup& CreateModal(const _TEXT_UTF8 std::string& title, bool canclose, StdMessage close = []() {});
        Popup& SetTitle(const _TEXT_UTF8 std::string& title);
        Popup& SetFlag(ImGuiWindowFlags flag);
        Popup& UnsetFlag(ImGuiWindowFlags flag);
        Popup& ClearFlag();
        Popup& SetSize(ImVec2 NewSize = { 0,0 });//{0,0}=default/auto
        Popup& PushTextPrev(const _TEXT_UTF8 std::string& Text);
        Popup& PushTextBack(const _TEXT_UTF8 std::string& Text);
        Popup& PushMsgPrev(StdMessage Msg);
        Popup& PushMsgBack(StdMessage Msg);
    };
    extern Popup CurrentPopup;
    extern bool HasPopup;
    inline void SetCurrentPopup(const Popup& sc) { HasPopup = true; CurrentPopup = sc; }
    inline void ClearCurrentPopup() { HasPopup = false; }
    Popup SingleText(const _TEXT_UTF8 std::string& StrId, const _TEXT_UTF8 std::string& Text, bool Modal);
    void RenderUI();
}


template<typename Cont>
class IBR_ListMenu
{
    std::vector<Cont>& List;
    int Page;
public:
    typedef Cont Type;
    typedef std::function<void(Cont&, int, int)> ActionType;
    std::string Tag;
    ActionType Action;

    IBR_ListMenu() = delete;
    IBR_ListMenu(std::vector<Cont>& L, const std::string& t, const ActionType& a) :
        List(L), Page(0), Tag(t), Action(a) {}

    void RenderUI();
};

namespace IBR_HintManager
{
    void Clear();
    void RenderUI();
    void SetHint(const _TEXT_UTF8 std::string& Str, int TimeLimitMillis = -1);
    void SetHintCustom(const std::function<bool(_TEXT_UTF8 std::string&)>& Fn);//返回true继续，false停止并Clear
    const std::string& GetHint();
    void Load();
}
