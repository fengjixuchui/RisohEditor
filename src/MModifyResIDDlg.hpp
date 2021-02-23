// MModifyResIDDlg.hpp --- "Modify Resource ID" Dialog
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
#include "ConstantsDB.hpp"
#include "Res.hpp"

void ReplaceFullWithHalf(MStringW& strText);

//////////////////////////////////////////////////////////////////////////////

class MModifyResIDDlg : public MDialogBase
{
public:
    MString m_str1;
    MString m_str2;

    MModifyResIDDlg(MString str1, MString str2)
        : MDialogBase(IDD_MODIFYRESID), m_str1(str1), m_str2(str2)
    {
    }

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
    {
        SetDlgItemText(hwnd, edt1, m_str1.c_str());

        int value = mstr_parse_int(m_str2.c_str(), true);
        SetDlgItemInt(hwnd, edt3, value, TRUE);

        SendDlgItemMessage(hwnd, scr1, UDM_SETRANGE, 0,
                           MAKELPARAM((WORD)SHRT_MAX, (WORD)SHRT_MIN));

        CenterWindowDx();
        return TRUE;
    }

    void OnOK(HWND hwnd)
    {
        MString str1 = GetDlgItemText(hwnd, edt1);
        ReplaceFullWithHalf(str1);
        mstr_trim(str1);
        if (str1.empty())
        {
            HWND hEdt1 = GetDlgItem(hwnd, edt1);
            Edit_SetSel(hEdt1, 0, -1);
            SetFocus(hEdt1);
            ErrorBoxDx(IDS_ENTERTEXT);
            return;
        }
        m_str1 = str1;

        MString str2 = GetDlgItemText(hwnd, edt1);
        mstr_trim(str2);
        if (str2.empty())
        {
            HWND hEdt1 = GetDlgItem(hwnd, edt1);
            Edit_SetSel(hEdt1, 0, -1);
            SetFocus(hEdt1);
            ErrorBoxDx(IDS_ENTERID);
            return;
        }

        MString str3 = GetDlgItemText(hwnd, edt3);
        ReplaceFullWithHalf(str3);
        mstr_trim(str3);
        if (str3.empty())
        {
            HWND hEdt3 = GetDlgItem(hwnd, edt3);
            Edit_SetSel(hEdt3, 0, -1);
            SetFocus(hEdt3);
            ErrorBoxDx(IDS_ENTERTEXT);
            return;
        }
        m_str2 = str3;

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
            EndDialog(IDCANCEL);
            break;
        case edt1:
            if (codeNotify == EN_CHANGE)
            {
                MString text = GetDlgItemText(hwnd, edt1);

                ConstantsDB::TableType table;
                table = g_db.GetTable(L"RESOURCE.ID.PREFIX");

                INT i = 0;
                for (auto& table_entry : table)
                {
                    if (text.find(table_entry.name) == 0)
                    {
                        text = g_db.GetName(L"RESOURCE.ID.TYPE", i);
                        SetDlgItemText(hwnd, edt2, text.c_str());
                        i = -1;
                        break;
                    }
                    ++i;
                }
                if (i != -1)
                {
                    SetDlgItemText(hwnd, edt2, NULL);
                }
            }
            break;
        }
    }

    virtual INT_PTR CALLBACK
    DialogProcDx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
            HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
            HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        }
        return DefaultProcDx();
    }
};
