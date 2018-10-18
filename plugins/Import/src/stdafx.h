/*

Import plugin for Miranda NG

Copyright (c) 2012-18 Miranda NG team (https://miranda-ng.org)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#pragma once

#define _CRT_NONSTDC_NO_DEPRECATE

#include <windows.h>
#include <commctrl.h> // datetimepicker

#include <malloc.h>
#include <time.h>

#include <memory>

#include <win2k.h>
#include <newpluginapi.h>
#include <m_langpack.h>
#include <m_system.h>
#include <m_database.h>
#include <m_protosvc.h>
#include <m_icolib.h>
#include <m_clist.h>
#include <m_db_int.h>
#include <m_metacontacts.h>
#include <m_import.h>
#include <m_gui.h>

#include "version.h"
#include "resource.h"

// dbrw
#include "dbrw\dbrw.h"
#include "dbrw\dbintf.h"

// Global constants

#define IMPORT_MODULE  "MIMImport"

#define MS_IMPORT_SERVICE "MIMImport/Import"        // Service for main menu item
#define MS_IMPORT_CONTACT "MIMImport/ImportContact" // Service for contact menu item

struct CMPlugin : public PLUGIN<CMPlugin>
{
	CMPlugin();

	int Load() override;
};

// Keys
#define IMP_KEY_FR     "FirstRun"         // First run

#define WIZM_GOTOPAGE       (WM_USER+10)  // wParam=0, lParam=page class
#define WIZM_DISABLEBUTTON  (WM_USER+11)  // wParam=0:back, 1:next, 2:cancel
#define WIZM_SETCANCELTEXT  (WM_USER+12)  // lParam=(char*)newText
#define WIZM_ENABLEBUTTON   (WM_USER+13)  // wParam=0:back, 1:next, 2:cancel

class CWizardPageDlg : public CDlgBase
{
	CCtrlButton btnOk, btnCancel;

protected:
	virtual void OnNext() PURE;
	virtual void OnCancel();

public:
	CWizardPageDlg(int dlgId);

	void onClick_Ok(CCtrlButton*) { OnNext(); }
	void onClick_Cancel(CCtrlButton*) { OnCancel(); }
};

void    AddMessage(const wchar_t* fmt, ...);
LRESULT RunWizard(CWizardPageDlg*, bool bModal);
void    SetProgress(int);

class CIntroPageDlg : public CWizardPageDlg
{
public:
	CIntroPageDlg();

	bool OnInitDialog() override;
	void OnNext() override;
};

class CProgressPageDlg : public CWizardPageDlg
{
	CCtrlListBox m_list;
	CTimer m_timer;

public:
	CProgressPageDlg();

	bool OnInitDialog() override;
	void OnDestroy() override;
	void OnNext() override;
	
	void OnTimer(CTimer*);
	void OnContextMenu(CCtrlBase*);

	void AddMessage(const wchar_t *pMsg);
	void SetProgress(int);
};

class CMirandaPageDlg : public CWizardPageDlg
{
	void SearchForLists(const wchar_t *mirandaPath, const wchar_t *mirandaProf);

	CCtrlButton btnBack, btnOther;
	CCtrlListBox m_list;

public:
	CMirandaPageDlg();

	bool OnInitDialog() override;
	void OnDestroy() override;
	void OnNext() override;
	
	void onClick_Back(CCtrlButton*);
	void onClick_Other(CCtrlButton*);

	void onSelChanged_list(CCtrlListBox*);
};

class CMirandaOptionsPageDlg : public CWizardPageDlg
{
	CCtrlButton btnBack;

public:
	CMirandaOptionsPageDlg();

	bool OnInitDialog() override;
	void OnNext() override;

	void onClick_Back(CCtrlButton*);
};

class CMirandaAdvOptionsPageDlg : public CWizardPageDlg
{
	CCtrlButton btnBack;
	CCtrlCheck chkSince, chkAll, chkOutgoing, chkIncoming, chkMsg, chkUrl, chkFT, chkOther;

public:
	CMirandaAdvOptionsPageDlg();

	bool OnInitDialog() override;
	void OnNext() override;

	void onClick_Back(CCtrlButton*);

	void onChange_Since(CCtrlCheck*);
	void onChange_All(CCtrlCheck*);
	void onChange_Msg(CCtrlCheck*);
	void onChange_Url(CCtrlCheck*);
	void onChange_FT(CCtrlCheck*);
	void onChange_Other(CCtrlCheck*);
};

class CFinishedPageDlg : public CWizardPageDlg
{
public:
	CFinishedPageDlg();

	bool OnInitDialog() override;
	void OnNext() override;
	void OnCancel() override;
};

bool IsDuplicateEvent(MCONTACT hContact, DBEVENTINFO dbei);

int CreateGroup(const wchar_t *name, MCONTACT hContact);

extern HWND g_hwndWizard, g_hwndAccMerge;
extern wchar_t importFile[MAX_PATH];
extern time_t dwSinceDate;
extern bool g_bServiceMode, g_bSendQuit;
extern int g_iImportOptions;
extern MCONTACT g_hImportContact;

HANDLE GetIconHandle(int iIconId);
void   RegisterIcons(void);

void RegisterMContacts();
void RegisterJson();

/////////////////////////////////////////////////////////////////////////////////////////

class CContactImportDlg : public CDlgBase
{
	MCONTACT m_hContact;
	int m_flags = 0;

	CCtrlButton m_btnOpenFile, m_btnOk;
	CCtrlEdit edtFileName;

public:
	CContactImportDlg(MCONTACT hContact);

	int getFlags() const { return m_flags; }

	bool OnInitDialog() override;
	bool OnApply() override;

	void onClick_Ok(CCtrlButton*);
	void onClick_OpenFile(CCtrlButton*);
};
