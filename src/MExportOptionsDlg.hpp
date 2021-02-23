// MExportOptionsDlg.hpp --- "Export Options" Dialog
//////////////////////////////////////////////////////////////////////////////
// RisohEditor --- Another free Win32 resource editor
// Copyright (C) 2017-2018 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "resource.h"
#include "MWindowBase.hpp"
#include "RisohSettings.hpp"

//////////////////////////////////////////////////////////////////////////////

class MExportOptionsDlg : public MDialogBase
{
public:
    MExportOptionsDlg() : MDialogBase(IDD_EXP_OPTIONS)
    {
    }

    void Reload(HWND hwnd)
    {
        CheckDlgButton(hwnd, chx1, g_settings.bSepFilesByLang ? BST_CHECKED : BST_UNCHECKED);
        CheckDlgButton(hwnd, chx2, g_settings.bUseBeginEnd ? BST_CHECKED : BST_UNCHECKED);
        CheckDlgButton(hwnd, chx3, g_settings.bSelectableByMacro ? BST_CHECKED : BST_UNCHECKED);
        CheckDlgButton(hwnd, chx4, g_settings.bBackup ? BST_CHECKED : BST_UNCHECKED);
        CheckDlgButton(hwnd, chx5, g_settings.bRedundantComments ? BST_CHECKED : BST_UNCHECKED);
        CheckDlgButton(hwnd, chx6, g_settings.bWrapManifest ? BST_CHECKED : BST_UNCHECKED);
        CheckDlgButton(hwnd, chx7, g_settings.bRCFileUTF16 ? BST_CHECKED : BST_UNCHECKED);

        SendDlgItemMessageW(hwnd, cmb1, CB_ADDSTRING, 0, (LPARAM)L"-old");
        SendDlgItemMessageW(hwnd, cmb1, CB_ADDSTRING, 0, (LPARAM)L"-bak");
        SendDlgItemMessageW(hwnd, cmb1, CB_ADDSTRING, 0, (LPARAM)L"~");
        SetDlgItemTextW(hwnd, cmb1, g_settings.strBackupSuffix.c_str());
    }

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
    {
        Reload(hwnd);

        CenterWindowDx();
        return TRUE;
    }

    void OnOK(HWND hwnd)
    {
        g_settings.bSepFilesByLang = (IsDlgButtonChecked(hwnd, chx1) == BST_CHECKED);
        g_settings.bUseBeginEnd = (IsDlgButtonChecked(hwnd, chx2) == BST_CHECKED);
        g_settings.bSelectableByMacro = (IsDlgButtonChecked(hwnd, chx3) == BST_CHECKED);
        g_settings.bBackup = (IsDlgButtonChecked(hwnd, chx4) == BST_CHECKED);
        g_settings.bRedundantComments = (IsDlgButtonChecked(hwnd, chx5) == BST_CHECKED);
        g_settings.bWrapManifest = (IsDlgButtonChecked(hwnd, chx6) == BST_CHECKED);
        g_settings.bRCFileUTF16 = (IsDlgButtonChecked(hwnd, chx7) == BST_CHECKED);

        WCHAR szText[32];
        GetDlgItemTextW(hwnd, cmb1, szText, _countof(szText));
        mstr_trim(szText);
        g_settings.strBackupSuffix = szText;

        if (szText[0] == 0)
            g_settings.bBackup = FALSE;

        EndDialog(IDOK);
    }

    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
    {
        switch (id)
        {
        case IDOK:
            OnOK(hwnd);
            break;
        case IDCANCEL:
            EndDialog(hwnd, IDCANCEL);
            break;
        }
    }

    virtual INT_PTR CALLBACK
    DialogProcDx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
            DO_MSG(WM_INITDIALOG, OnInitDialog);
            DO_MSG(WM_COMMAND, OnCommand);
        }
        return DefaultProcDx();
    }
};
