#pragma once
#include"FromEngine/Include.h"

typedef void* TE_FileID;
void    (*TE_GiveCurrentDir) (void*);
void    (*TE_Initialize)     (TE_FileID*);
void    (*TE_Destroy)        (TE_FileID);
bool    (*TE_OpenFile)       (TE_FileID, LPCWSTR, int64_t, void(CALLBACK*)(), void(CALLBACK*)());
int64_t (*TE_TellClose)      (const TE_FileID, bool);
bool    (*TE_IsFileOpen)     (const TE_FileID);
bool    (*TE_SetActive)      (const TE_FileID);
int64_t (*TE_GetWText)       (const TE_FileID, LPWSTR(CALLBACK*)(int64_t), int64_t);
LPCWSTR (*TE_GetModuleAbout) ();

