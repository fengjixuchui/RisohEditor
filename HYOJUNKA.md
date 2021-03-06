# Win32リソースの標準化

(UTF-8, Japanese)

新しいリソーエディタのリソース情報は、以下のガイドラインに従うべきです。
古いリソーエディタのデータを使っているなら、以下のガイドラインを適用する必要があります。

## 定義

- 「リソースID」とは、Win32リソースデータに使われるIDです。
- 「IDプレフィックス」とは、リソースIDのプレフィックス (prefix) です。
- 「エンティティ リソース」とは、RT_ICON、RT_CURSOR、RT_STRING、RT_MANIFEST、RT_VERSION、そしてRT_MESSAGETABLE以外のリソースデータです。
- エンティティ リソースの「リソース名」とは、16ビット整数値か、ワイド文字列か、16ビット整数値のマクロです。
- RT_ICONタイプのリソースデータは、RT_GROUP_ICON タイプのリソースデータによって参照されます。
- RT_CURSORタイプのリソースデータは、RT_GROUP_CURSOR タイプのリソースデータによって参照されます。
- RT_STRINGまたはRT_MESSAGETABLEリソースは、複数のリソースIDを含むことができます。
- RT_MANIFESTまたはRT_VERSIONのリソースIDは特別な意味を持ちます。
- 「ユーザーリソース」とは、ユーザーのプロジェクトが直接提供し、かつ、他のプロジェクトやフレームワークによって提供されない、Win32リソースデータです。
- 「ユーザーID」とは、ユーザーのプロジェクトが直接提供し、かつ、他のプロジェクトやフレームワークによって提供されないリソースIDです。
- 「アイコンID」とは、RT_GROUP_ICON タイプのリソースIDです。
- 「カーソルID」とは、RT_GROUP_CURSOR タイプのリソースIDです。
- 「文字列ID」とは、文字列テーブルの中の1個の文字列データのIDです。
- 「メッセージID」とは、メッセージテーブルの中の1個のメッセージデータのIDです。

## ガイドライン

Visual C++互換性のために、ファイル「resource.h」の中身は、次のようなC++コメントで始まるべきです。

```c
//{{NO_DEPENDENCIES}}
// Microsoft Visual C++ Compatible
// TheProject.rc
```

ここで、「TheProject.rc」は、この「resource.h」に対する実際のリソースファイル名で置き換えなければなりません。

ファイル「resource.h」は、インクルードガードを使うべきではありません。

ファイル「resource.h」の中で、文字列マクロを使わないで下さい。文字列値のリソースIDを使わないで下さい。

もし、rcファイルがシステムヘッダーの#includeを有するなら、そのインクルードは、次のように包まれるべきです：

```c
#define APSTUDIO_HIDDEN_SYMBOLS
#include <windows.h>
#include <commctrl.h>
#undef APSTUDIO_HIDDEN_SYMBOLS
```

ファイル「resource.h」の中身は、次のようなテキストで終わるべきです：

```c
#ifdef APSTUDIO_INVOKED
    #ifndef APSTUDIO_READONLY_SYMBOLS
        #define _APS_NO_MFC                 1
        #define _APS_NEXT_RESOURCE_VALUE    1000
        #define _APS_NEXT_COMMAND_VALUE     220
        #define _APS_NEXT_CONTROL_VALUE     1002
        #define _APS_NEXT_SYMED_VALUE       300
    #endif
#endif
```

ここに、_APS_NO_MFC マクロの値は1であるべきです。
ここに、_APS_NEXT_RESOURCE_VALUE マクロの値は、すべてのユーザー エンティティ リソースの最大整数値 + 1でなければなりません。
ここに、_APS_NEXT_COMMAND_VALUE マクロの値は、すべてのユーザー コマンド IDの最大整数値 + 1でなければなりません。
ここに、_APS_NEXT_CONTROL_VALUE マクロの値は、すべてのユーザー コントロールIDの最大整数値 + 1でなければなりません。
ここに、_APS_NEXT_SYMED_VALUE マクロの値は無視するべきです。

## IDプレフィックス

「resource.h」で定義されるユーザーIDプレフィックスは次の表に従うべきです：

| IDタイプ               | IDプレフィックス |
|------------------------|------------------|
| String ID              | IDS_             |
| Message ID             | MSGID_           |
| Command ID             | ID_              |
| Command ID (Old Type)  | IDM_             |
| Control ID             | IDC_             |
| Cursor ID              | IDC_             |
| Icon ID                | IDI_             |
| Dialog ID              | IDD_             |
| Bitmap ID              | IDB_             |
| その他のエンティティID | IDR_             |
| Window ID              | IDW_             |
| Help ID                | HID_             |

「IDP_」プレフィックスは使わないで下さい。

## 値と範囲

「resource.h」で定義されるユーザーIDは、次の表の範囲であるべきです。

| IDタイプ               | 束縛範囲      | 好ましい範囲  |
|------------------------|---------------|---------------|
| String ID              | 1～0x7FFF     | 100～0x7FFF   |
| Message ID             | 0～0xFFFFFFFF | 1～0x7FFFFFFF |
| Command ID             | 1～0x7FFF     | 100～0x7FFF   |
| Command ID (Old Type)  | 1～0x7FFF     | 100～0x7FFF   |
| Control ID             | 8～0xDFFF     | 1000～0x7FFF  |
| Cursor ID              | 1～0x7FFF     | 100～999      |
| Icon ID                | 1～0x7FFF     | 100～999      |
| Dialog ID              | 1～0x7FFF     | 100～0x7FFF   |
| Bitmap ID              | 1～0x7FFF     | 100～0x7FFF   |
| その他のエンティティID | 1～0x7FFF     | 100～0x7FFF   |
| Window ID              | 1～0x7FFF     | 1～0x7FFF     |
| Help ID                | 0～0xFFFFFFFF | 1～0x7FFFFFFF |

同じIDプレフィックスの２つの異なるリソースIDは、互いに違う値を持つべきです。
「IDC_」のリソースIDは、コントロールIDか、カーソルIDです。
もし、我々がIDの好ましい範囲に従えば、コントロールIDとカーソルIDは衝突しません。

## TEXTINCLUDE

Visual C++ 互換性のために、リソースファイルは、以下のような3個のTEXTINCLUDEデータを有しなければなりません：

```rc
#ifdef APSTUDIO_INVOKED

1 TEXTINCLUDE
BEGIN
    "resource.h\0"
END

2 TEXTINCLUDE 
BEGIN
    "#define APSTUDIO_HIDDEN_SYMBOLS\r\n"
    "#include <windows.h>\r\n"
    "#include <commctrl.h>\r\n"
    "#undef APSTUDIO_HIDDEN_SYMBOLS\r\n"
    "\0"
END

3 TEXTINCLUDE 
BEGIN
    "\r\n"
    "\0"
END

#endif    // APSTUDIO_INVOKED
```

## コメント

「resource.h」ファイルのリソースファイルは、CコメントよりもC++コメントを使うべきです。

## 参照

[https://msdn.microsoft.com/en-us/library/t2zechd4.aspx](https://msdn.microsoft.com/en-us/library/t2zechd4.aspx)
