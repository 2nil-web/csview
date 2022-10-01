
#define STRICT

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <tchar.h>

#include <cstdio>
#include <vector>
#include <string>
#include <tuple>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <fstream>

#include "resource.h"

#ifndef WIN32
#define GET_WM_COMMAND_ID(wp, lp)   (wp)
#define GET_WM_COMMAND_HWND(wp, lp) (HWND)(LOWORD(lp))
#define GET_WM_COMMAND_CMD(wp, lp)  HIWORD(lp)
#endif

#define ID_LISTVIEW  2000

// Local Function Prototypes
#define ErrorHandler() ErrorHandlerEx(__LINE__, __FILE__)
void ErrorHandlerEx(WORD, LPSTR);

// Global Variables
HINSTANCE g_hInst;
TCHAR g_szClassName[]=TEXT("wintailClass");
// Nom du fichier csv à visualiser
std::string g_filename;
// Séparateur du fichier csv (par défaut ;)
char g_separator=';';
// Nombre maximum de colonne dans le csv
size_t g_maxCol=0;
// Entête du csv
std::vector < std::string > g_header;
// Contenu du csv
std::vector < std::vector < std::string >> g_sheet;
// Pour calculer la plus large taille de chaque colonne : get<0>=row, get<1>=text.size, get<2>=pixel width
std::vector <std::tuple < size_t, size_t, int >> g_widestCol;

void ErrorHandlerEx(WORD wLine, LPSTR lpszFile) {
  LPVOID lpvMessage;
  DWORD dwError;
  TCHAR szBuffer[256];

  // Allow FormatMessage() to look up the error code returned by GetLastError
  dwError=FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM,
    NULL,
    GetLastError(),
    MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
    (LPTSTR) & lpvMessage,
    0,
    NULL);

  // Check to see if an error occurred calling FormatMessage()
  if (0 == dwError) {
    _sntprintf_s(szBuffer, _countof(szBuffer), _TRUNCATE,
      TEXT("An error occurred calling FormatMessage().") TEXT("Error Code %d"),
      GetLastError());
    MessageBox(NULL,
      szBuffer,
      TEXT("Generic"),
      MB_ICONSTOP | MB_ICONEXCLAMATION);
    return;
  }

  // Display the error information along with the place the error happened.
  _sntprintf_s(szBuffer, _countof(szBuffer), _TRUNCATE,
    TEXT("Generic, Line=%d, File=%s"), wLine, lpszFile);
  MessageBox(NULL, (char * ) lpvMessage, szBuffer, MB_ICONEXCLAMATION | MB_OK);
}

std::vector<std::wstring> cmdLineToWsVec() {
  LPWSTR *wav;
  int ac;
  wav=CommandLineToArgvW(GetCommandLineW(), &ac);
  return std::vector<std::wstring>(wav, wav+ac);
//  std::vector<std::string> args=std::vector<std::string>(wav, wav+ac);
}

std::vector<std::string> cmdLineToSVec() {
  std::vector<std::wstring> wsv=cmdLineToWsVec();
  std::vector<std::string> sv;
  for(auto ws:wsv) sv.push_back(std::string(ws.begin(), ws.end()));

  return sv;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
  HWND hWnd;
  TCHAR szTitle[MAX_PATH]=TEXT("");

  g_hInst=hInstance;

  LoadString(g_hInst, IDS_APPTITLE, szTitle, sizeof(szTitle) / sizeof(szTitle[0]));

  //  Create a main window for this application instance.  
  hWnd=CreateWindowEx(0,
    g_szClassName,
    szTitle,
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    NULL,
    NULL,
    hInstance,
    NULL);

  //  If window could not be created, return "failure" 

  if (!hWnd)
    return FALSE;

  //  Make the window visible; update its client area; and return "success" 

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);
  return TRUE;

}

INT_PTR CALLBACK AboutDlgProc(HWND hDlg,
  UINT uMessage,
  WPARAM wParam,
  LPARAM /*lParam*/ ) {
  switch (uMessage) {
  case WM_INITDIALOG:
    return TRUE;

  case WM_COMMAND:
    switch (wParam) {
    case IDOK:
      EndDialog(hDlg, IDOK);
      break;

    case IDCANCEL:
      EndDialog(hDlg, IDOK);
      break;
    }
    return TRUE;
  }

  return FALSE;
}

LRESULT ListViewNotify(HWND /*hWnd*/ , LPARAM lParam) {
  LPNMHDR lpnmh=(LPNMHDR) lParam;
  //HWND     hwndListView=GetDlgItem(hWnd, ID_LISTVIEW);

  switch (lpnmh -> code) {
  case LVN_GETDISPINFO: {
    LV_DISPINFO * lpdi=(LV_DISPINFO * ) lParam;

    if (lpdi -> item.iSubItem == 0) { // 1ére colonne
      if (lpdi -> item.mask & LVIF_TEXT) {
        if ((int) g_sheet.size() > lpdi -> item.iItem && g_sheet[lpdi -> item.iItem].size() > 0) {
          _tcsncpy_s(lpdi -> item.pszText, lpdi -> item.cchTextMax, std::to_string(lpdi -> item.iItem+1).c_str(), _TRUNCATE);
        } else _tcsncpy_s(lpdi -> item.pszText, lpdi -> item.cchTextMax, "", _TRUNCATE);
      }

      //if(lpdi->item.mask & LVIF_IMAGE) { lpdi->item.iImage=0; }
    } else { // Colonnes suivantes
      if (lpdi -> item.mask & LVIF_TEXT) {
        if ((int) g_sheet.size() > lpdi -> item.iItem && (int) g_sheet[lpdi -> item.iItem].size() >= lpdi -> item.iSubItem) {
          _tcsncpy_s(lpdi -> item.pszText, lpdi -> item.cchTextMax, g_sheet[lpdi -> item.iItem][lpdi -> item.iSubItem-1].c_str(), _TRUNCATE);
        } else _tcsncpy_s(lpdi -> item.pszText, lpdi -> item.cchTextMax, "", _TRUNCATE);
      }
    }
  }
  return 0;

  case LVN_ODCACHEHINT: {
    /*
    LPNMLVCACHEHINT   lpCacheHint=(LPNMLVCACHEHINT)lParam;
    This sample doesn't use this notification, but this is sent when the 
    ListView is about to ask for a range of items. On this notification, 
    you should load the specified items into your local cache. It is still 
    possible to get an LVN_GETDISPINFO for an item that has not been cached, 
    therefore, your application must take into account the chance of this 
    occurring.
    */
  }
  return 0;

  case LVN_ODFINDITEM: {
    /*
    LPNMLVFINDITEM lpFindItem=(LPNMLVFINDITEM)lParam;
    This sample doesn't use this notification, but this is sent when the 
    ListView needs a particular item. Return -1 if the item is not found.
    */
  }
  return 0;
  }

  return 0;
}

unsigned int readCsv(std::string fname, char sep, HWND hwnd) {
  std::ifstream fp(fname);
  std::string ln, cell;
  std::vector < std::string > row;
  unsigned int count=0;

  // Le 1er nuplet (tuple) c'est pour la largeur de la 1ére colonne qui indique le numéro de ligne ...
  g_widestCol.push_back(std::make_tuple(0, 0, 0));

  if (fp.is_open()) {
    while (std::getline(fp, ln)) {
      std::stringstream ss(ln);
      row.clear();
      size_t iPos=2;

      std::string countS=std::to_string(count+1);
      int lvgsw=ListView_GetStringWidth(hwnd, countS.c_str());

      if (lvgsw > std::get < 2 > (g_widestCol[0])) {
        g_widestCol[0]=std::make_tuple(count, countS.size(), lvgsw);
      }

      while (std::getline(ss, cell, sep)) {
        row.push_back(cell);

        if (count > 0) {
          if (g_widestCol.size() < iPos) g_widestCol.push_back(std::make_tuple(count, cell.size(), ListView_GetStringWidth(hwnd, cell.c_str())));
          else {
            lvgsw=ListView_GetStringWidth(hwnd, cell.c_str());
            if (lvgsw > std::get < 2 > (g_widestCol[iPos-1])) g_widestCol[iPos-1]=std::make_tuple(count, cell.size(), lvgsw);
          }
          iPos++;
        }
      }

      if (row.size() > g_maxCol) g_maxCol=row.size();
      if (count == 0) g_header=row;
      else g_sheet.push_back(row);
      count++;
    }

    fp.close();
  }

  return count;
}

void PositionHeader(HWND hwndListView) {
  HWND hwndHeader=GetWindow(hwndListView, GW_CHILD);
  DWORD dwStyle=GetWindowLong(hwndListView, GWL_STYLE);

  // To ensure that the first item will be visible, create the control without the LVS_NOSCROLL style and then add it here
  dwStyle |= LVS_NOSCROLL;
  SetWindowLong(hwndListView, GWL_STYLE, dwStyle);

  // Only do this if we are in report view and were able to get the g_header hWnd
  if (((dwStyle & LVS_TYPEMASK) == LVS_REPORT) && hwndHeader) {
    RECT rc;
    HD_LAYOUT hdLayout;
    WINDOWPOS wpos;

    GetClientRect(hwndListView, & rc);
    hdLayout.prc=& rc;
    hdLayout.pwpos=& wpos;

    Header_Layout(hwndHeader, & hdLayout);

    SetWindowPos(hwndHeader, wpos.hwndInsertAfter,
      wpos.x, wpos.y, wpos.cx, wpos.cy,
      wpos.flags | SWP_SHOWWINDOW);

    ListView_EnsureVisible(hwndListView, 0, FALSE);
  }
}

BOOL InsertListViewItems(HWND hwndListView) {
  //empty the list
  ListView_DeleteAllItems(hwndListView);
  //set the number of items in the list
  ListView_SetItemCount(hwndListView, g_sheet.size());
  return TRUE;
}

BOOL InitListView(HWND hwndListView) {
  LV_COLUMN lvColumn;
  size_t i;

  //empty the list
  ListView_DeleteAllItems(hwndListView);

  //initialize the columns
  lvColumn.mask=LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
  lvColumn.fmt=LVCFMT_LEFT;
  lvColumn.cx=120;

  lvColumn.pszText=(char * )
  "#";
  ListView_InsertColumn(hwndListView, 0, & lvColumn);

  for (i=0; i < g_maxCol; i++) {
    lvColumn.pszText=(char * ) g_header[i].c_str();
    ListView_InsertColumn(hwndListView, i+1, & lvColumn);
  }

  InsertListViewItems(hwndListView);
  return TRUE;
}

void ResizeListView(HWND hwndListView, HWND hwndParent) {
  RECT rc;
  GetClientRect(hwndParent, & rc);
  MoveWindow(hwndListView, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE);
  //only call this if we want the LVS_NOSCROLL style
  //PositionHeader(hwndListView);
}

HWND CreateListView(HINSTANCE /*hInstance*/ , HWND hwndParent) {
  DWORD dwStyle;
  HWND hwndListView;
  //HIMAGELIST  himlSmall;
  //HIMAGELIST  himlLarge;
  //BOOL        bSuccess=TRUE;

  dwStyle=WS_TABSTOP |
    WS_CHILD |
    WS_BORDER |
    WS_VISIBLE |
    LVS_AUTOARRANGE |
    LVS_REPORT |
    LVS_OWNERDATA;

  hwndListView=CreateWindowEx(WS_EX_CLIENTEDGE, // ex style
    WC_LISTVIEW, // class name-defined in commctrl.h
    TEXT(""), // dummy text
    dwStyle, // style
    0, 0, 0, 0, // x, y, width, height
    hwndParent, // parent
    (HMENU) ID_LISTVIEW, // ID
    g_hInst, // instance
    NULL); // no extra data

  if (!hwndListView) return NULL;

  ResizeListView(hwndListView, hwndParent);

  return hwndListView;
}

void UpdateMenu(HWND hwndListView, HMENU hMenu) {
  UINT uID=IDM_LIST;
  DWORD dwStyle;

  // Uncheck all
  CheckMenuItem(hMenu, IDM_LARGE_ICONS, MF_BYCOMMAND | MF_UNCHECKED);
  CheckMenuItem(hMenu, IDM_SMALL_ICONS, MF_BYCOMMAND | MF_UNCHECKED);
  CheckMenuItem(hMenu, IDM_LIST, MF_BYCOMMAND | MF_UNCHECKED);
  CheckMenuItem(hMenu, IDM_REPORT, MF_BYCOMMAND | MF_UNCHECKED);

  // Check the appropriate view menu item
  dwStyle=GetWindowLong(hwndListView, GWL_STYLE);
  switch (dwStyle & LVS_TYPEMASK) {
  case LVS_ICON:
    uID=IDM_LARGE_ICONS;
    break;

  case LVS_SMALLICON:
    uID=IDM_SMALL_ICONS;
    break;

  case LVS_LIST:
    uID=IDM_LIST;
    break;

  case LVS_REPORT:
    uID=IDM_REPORT;
    break;
  }

  CheckMenuRadioItem(hMenu, IDM_LARGE_ICONS, IDM_REPORT, uID, MF_BYCOMMAND | MF_CHECKED);
}

BOOL DoContextMenu(HWND hWnd,
  WPARAM wParam,
  LPARAM lParam) {
  HWND hwndListView=(HWND) wParam;
  HMENU hMenuLoad,
  hMenu;

  if (hwndListView != GetDlgItem(hWnd, ID_LISTVIEW))
    return FALSE;

  hMenuLoad=LoadMenu(g_hInst, MAKEINTRESOURCE(IDM_CONTEXT_MENU));
  hMenu=GetSubMenu(hMenuLoad, 0);
  UpdateMenu(hwndListView, hMenu);

  TrackPopupMenu(hMenu,
    TPM_LEFTALIGN | TPM_RIGHTBUTTON,
    LOWORD(lParam), HIWORD(lParam),
    0, hWnd, NULL);

  DestroyMenu(hMenuLoad);

  return TRUE;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
  static HWND hwndListView;

  switch (uMessage) {
  case WM_CREATE:
    // create the TreeView control
    hwndListView=CreateListView(g_hInst, hWnd);

    readCsv(g_filename, g_separator, hwndListView);

    //initialize the TreeView control
    InitListView(hwndListView);

    // https://stackoverflow.com/questions/9255540/how-auto-size-the-columns-width-of-a-list-view-in-virtual-mode
    // get<0>=row, get<1>=text.size, get<2>=pixel width
    std::cout << "Widest cells for:";
    for (size_t i=0; i < g_widestCol.size(); i++) {
      std::cout << '(' << get < 0 > (g_widestCol[i]) << ',' << i << ")=" << get < 1 > (g_widestCol[i]) << "ch, " << get < 2 > (g_widestCol[i]) << "px, ";
      ListView_SetColumnWidth(hwndListView, i, 20+get < 2 > (g_widestCol[i]));
    }
    std::cout << std::endl;
    break;

  case WM_NOTIFY:
    return ListViewNotify(hWnd, lParam);

  case WM_SIZE:
    ResizeListView(hwndListView, hWnd);
    break;

  case WM_INITMENUPOPUP:
    UpdateMenu(hwndListView, GetMenu(hWnd));
    break;

  case WM_CONTEXTMENU:
    if (DoContextMenu(hWnd, wParam, lParam))
      return FALSE;
    break;

  case WM_COMMAND:
    switch (GET_WM_COMMAND_ID(wParam, lParam)) {
    case IDM_EXIT:
      DestroyWindow(hWnd);
      break;

    case IDM_ABOUT:
      DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUT), hWnd, AboutDlgProc);
      break;

    }
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  default:
    break;
  }
  return DefWindowProc(hWnd, uMessage, wParam, lParam);
}

BOOL InitApplication(HINSTANCE hInstance) {
  WNDCLASSEX wcex;
  ATOM aReturn;

  wcex.cbSize=sizeof(WNDCLASSEX);
  wcex.style=0;
  wcex.lpfnWndProc=(WNDPROC) MainWndProc;
  wcex.cbClsExtra=0;
  wcex.cbWndExtra=0;
  wcex.hInstance=hInstance;
  wcex.hCursor=LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszMenuName=MAKEINTRESOURCE(IDM_MAIN_MENU);
  wcex.lpszClassName=g_szClassName;
  wcex.hIcon=LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_MAINICON));
  wcex.hIconSm=(HICON) LoadImage(g_hInst, MAKEINTRESOURCE(IDI_MAINICON), IMAGE_ICON, 16, 16, 0);

  aReturn=RegisterClassEx( & wcex);

  if (0 == aReturn) {
    WNDCLASS wc;

    wc.style=0;
    wc.lpfnWndProc=(WNDPROC) MainWndProc;
    wc.cbClsExtra=0;
    wc.cbWndExtra=0;
    wc.hInstance=hInstance;
    wc.hIcon=LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_MAINICON));
    wc.hCursor=LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName=MAKEINTRESOURCE(IDM_MAIN_MENU);
    wc.lpszClassName=g_szClassName;

    aReturn=RegisterClass( & wc);
  }

  return aReturn;
}

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR /*lpCmdLine*/ , int nCmdShow) {
  if (!hPrevInstance && !InitApplication(hInstance)) return FALSE;

  MSG msg;
  g_hInst=hInstance;
  std::vector<std::string> args=cmdLineToSVec();

  if (args.size() > 1) g_filename=args[1];
  if (args.size() > 2) g_separator=args[2][0];

  // Required to use the common controls (not so sure as of 2022)
  //InitCommonControls();
  if (!InitInstance(hInstance, nCmdShow)) return FALSE;

  // Acquire and dispatch messages
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return (int) msg.wParam;
}

