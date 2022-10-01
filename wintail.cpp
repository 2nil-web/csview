
#define STRICT

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <tchar.h>

#include <cstdio>
#include <string>
#include <tuple>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>

#include "wintail.h"

// Local Function Prototypes
#define ErrorHandler() ErrorHandlerEx(__LINE__, __FILE__)
void ErrorHandlerEx(WORD, LPSTR);

LRESULT ListViewNotify(HWND, LPARAM);
void SwitchView(HWND, DWORD);
BOOL DoContextMenu(HWND, WPARAM, LPARAM);
void UpdateMenu(HWND, HMENU);
BOOL InsertListViewItems(HWND);
void PositionHeader(HWND);

// Global Variables
HINSTANCE g_hInst;
TCHAR g_szClassName[]=TEXT("wintailClass");

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR /*lpCmdLine*/ , int nCmdShow) {
  MSG msg;
  g_hInst=hInstance;

  if (!hPrevInstance)
    if (!InitApplication(hInstance))
      return FALSE;

  //required to use the common controls
  InitCommonControls();

  //  Perform initializations that apply to a specific instance 
  if (!InitInstance(hInstance, nCmdShow))
    return FALSE;

  //  Acquire and dispatch messages until a WM_QUIT uMessage is received. 

  while (GetMessage( & msg, NULL, 0x00, 0x00)) {
    TranslateMessage( & msg);
    DispatchMessage( & msg);
  }

  return (int) msg.wParam;
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
  wcex.hbrBackground=(HBRUSH)(COLOR_WINDOW + 1);
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
    wc.hbrBackground=(HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName=MAKEINTRESOURCE(IDM_MAIN_MENU);
    wc.lpszClassName=g_szClassName;

    aReturn=RegisterClass( & wc);
  }

  return aReturn;
}

BOOL InitInstance(HINSTANCE hInstance,
  int nCmdShow) {
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

size_t maxCol=0;
size_t widestRow=0;
std::vector < std::string > header;
std::vector < std::vector < std::string >> sheet;
struct tWidestCell {
  size_t row, size, pixel_width;
};
// get<0>=row, get<1>=text.size, get<2>=pixel width
std::vector < std::tuple < size_t, size_t, int >> widestCol;

unsigned int readCsv(std::string fname, char sep, HWND hwnd) {
  std::ifstream fp(fname);
  std::string ln, cell;
  std::vector < std::string > row;
  unsigned int count=0;
  size_t maxCharPerCol=0;

  // Le 1er tuple c'est pour la largeur de la 1ére colonne qui indique le numéro de ligne ...
  widestCol.push_back(std::make_tuple(0, 0, 0));

  if (fp.is_open()) {
    while (std::getline(fp, ln)) {
      if (ln.size() > maxCharPerCol) {
        maxCharPerCol=ln.size();
        widestRow=count;
      }

      std::stringstream ss(ln);
      row.clear();
      size_t iPos=2;

      std::string countS=std::to_string(count + 1);
      int lvgsw=ListView_GetStringWidth(hwnd, countS.c_str());

      if (lvgsw > std::get < 2 > (widestCol[0])) {
        widestCol[0]=std::make_tuple(count, countS.size(), lvgsw);
      }

      while (std::getline(ss, cell, sep)) {
        row.push_back(cell);

        if (count > 0) {
          if (widestCol.size() < iPos) widestCol.push_back(std::make_tuple(count, cell.size(), ListView_GetStringWidth(hwnd, cell.c_str())));
          else {
            lvgsw=ListView_GetStringWidth(hwnd, cell.c_str());
            if (lvgsw > std::get < 2 > (widestCol[iPos - 1])) widestCol[iPos - 1]=std::make_tuple(count, cell.size(), lvgsw);
          }
          iPos++;
        }
      }

      if (row.size() > maxCol) maxCol=row.size();
      if (count == 0) header=row;
      else sheet.push_back(row);
      count++;
    }

    fp.close();
  }
  //for(size_t i=0; i < widestCol.size(); i++) std::cout << "Widest cell for column " << i << " is at row " << get<0>(widestCol[i]) << " with character size of " << get<1>(widestCol[i]) << " and pixels size of " << get<2>(widestCol[i]) << std::endl;
  /*
    std::cout << "COUNT " << count << ", SHEET.SIZE " << sheet.size() << std::endl;
    for (size_t l=0; l < sheet.size(); l++) {
        std::cout << l << ':';
      for (size_t c=0; c < sheet[l].size(); c++) {
        std::cout << sheet[l][c] << "#";
      }
      std::cout << std::endl << std::flush;
    }
  */
  return count;
}

LRESULT CALLBACK MainWndProc(HWND hWnd,
  UINT uMessage,
  WPARAM wParam,
  LPARAM lParam) {
  static HWND hwndListView;

  switch (uMessage) {
  case WM_CREATE:
    // create the TreeView control
    hwndListView=CreateListView(g_hInst, hWnd);

    readCsv("websetup.log", ';', hwndListView);

    //initialize the TreeView control
    InitListView(hwndListView);

    // https://stackoverflow.com/questions/9255540/how-auto-size-the-columns-width-of-a-list-view-in-virtual-mode
    // get<0>=row, get<1>=text.size, get<2>=pixel width
    for (size_t i=0; i < widestCol.size(); i++) {
      std::cout << "Widest cell for column " << i << " is at row " << get < 0 > (widestCol[i]) << " with character size of " << get < 1 > (widestCol[i]) << " and pixels size of " << get < 2 > (widestCol[i]) << std::endl;
      ListView_SetColumnWidth(hwndListView, i, 20 + get < 2 > (widestCol[i]));
    }
    break;

  case WM_NOTIFY:
    return ListViewNotify(hWnd, lParam);

  case WM_SIZE:
    ResizeListView(hwndListView, hWnd);
    std::cout << "WM_SIZE" << std::endl;
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
    WC_LISTVIEW, // class name - defined in commctrl.h
    TEXT(""), // dummy text
    dwStyle, // style
    0, // x position
    0, // y position
    0, // width
    0, // height
    hwndParent, // parent
    (HMENU) ID_LISTVIEW, // ID
    g_hInst, // instance
    NULL); // no extra data

  if (!hwndListView)
    return NULL;

  ResizeListView(hwndListView, hwndParent);

  //set the image lists
  /*
  himlSmall=ImageList_Create(16, 16, ILC_COLORDDB | ILC_MASK, 1, 0);
  himlLarge=ImageList_Create(32, 32, ILC_COLORDDB | ILC_MASK, 1, 0);

  if (himlSmall && himlLarge) {
     HICON hIcon;

     //set up the small image list
     hIcon=(HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_DISK), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
     ImageList_AddIcon(himlSmall, hIcon);

     //set up the large image list
     hIcon=LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_DISK));
     ImageList_AddIcon(himlLarge, hIcon);

     ListView_SetImageList(hwndListView, himlSmall, LVSIL_SMALL);
     ListView_SetImageList(hwndListView, himlLarge, LVSIL_NORMAL);
     }
  */
  return hwndListView;
}

void ResizeListView(HWND hwndListView, HWND hwndParent) {
  RECT rc;

  GetClientRect(hwndParent, & rc);

  MoveWindow(hwndListView,
    rc.left,
    rc.top,
    rc.right - rc.left,
    rc.bottom - rc.top,
    TRUE);

  //only call this if we want the LVS_NOSCROLL style
  //PositionHeader(hwndListView);
}

void PositionHeader(HWND hwndListView) {
  HWND hwndHeader=GetWindow(hwndListView, GW_CHILD);
  DWORD dwStyle=GetWindowLong(hwndListView, GWL_STYLE);

  // To ensure that the first item will be visible, create the control without the LVS_NOSCROLL style and then add it here
  dwStyle |= LVS_NOSCROLL;
  SetWindowLong(hwndListView, GWL_STYLE, dwStyle);

  // Only do this if we are in report view and were able to get the header hWnd
  if (((dwStyle & LVS_TYPEMASK) == LVS_REPORT) && hwndHeader) {
    RECT rc;
    HD_LAYOUT hdLayout;
    WINDOWPOS wpos;

    GetClientRect(hwndListView, & rc);
    hdLayout.prc=& rc;
    hdLayout.pwpos=& wpos;

    Header_Layout(hwndHeader, & hdLayout);

    SetWindowPos(hwndHeader,
      wpos.hwndInsertAfter,
      wpos.x,
      wpos.y,
      wpos.cx,
      wpos.cy,
      wpos.flags | SWP_SHOWWINDOW);

    ListView_EnsureVisible(hwndListView, 0, FALSE);
  }
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

  for (i=0; i < maxCol; i++) {
    lvColumn.pszText=(char * ) header[i].c_str();
    ListView_InsertColumn(hwndListView, i + 1, & lvColumn);
  }

  InsertListViewItems(hwndListView);
  return TRUE;
}

BOOL InsertListViewItems(HWND hwndListView) {
  //empty the list
  ListView_DeleteAllItems(hwndListView);

  //set the number of items in the list
  ListView_SetItemCount(hwndListView, sheet.size());

  return TRUE;
}

LRESULT ListViewNotify(HWND /*hWnd*/ , LPARAM lParam) {
  LPNMHDR lpnmh=(LPNMHDR) lParam;
  //HWND     hwndListView=GetDlgItem(hWnd, ID_LISTVIEW);

  switch (lpnmh -> code) {
  case LVN_GETDISPINFO: {
    LV_DISPINFO * lpdi=(LV_DISPINFO * ) lParam;

    if (lpdi -> item.iSubItem == 0) { // 1ére colonne
      if (lpdi -> item.mask & LVIF_TEXT) {
        if ((int) sheet.size() > lpdi -> item.iItem && sheet[lpdi -> item.iItem].size() > 0) {
          _tcsncpy_s(lpdi -> item.pszText, lpdi -> item.cchTextMax, std::to_string(lpdi -> item.iItem + 1).c_str(), _TRUNCATE);
        } else _tcsncpy_s(lpdi -> item.pszText, lpdi -> item.cchTextMax, "", _TRUNCATE);
      }

      //if(lpdi->item.mask & LVIF_IMAGE) { lpdi->item.iImage=0; }
    } else { // Colonnes suivantes
      if (lpdi -> item.mask & LVIF_TEXT) {
        if ((int) sheet.size() > lpdi -> item.iItem && (int) sheet[lpdi -> item.iItem].size() >= lpdi -> item.iSubItem) {
          _tcsncpy_s(lpdi -> item.pszText, lpdi -> item.cchTextMax, sheet[lpdi -> item.iItem][lpdi -> item.iSubItem - 1].c_str(), _TRUNCATE);
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

void SwitchView(HWND hwndListView, DWORD dwView) {
  DWORD dwStyle=GetWindowLong(hwndListView, GWL_STYLE);

  SetWindowLong(hwndListView, GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | dwView);
  ResizeListView(hwndListView, GetParent(hwndListView));
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
    LOWORD(lParam),
    HIWORD(lParam),
    0,
    hWnd,
    NULL);

  DestroyMenu(hMenuLoad);

  return TRUE;
}

void UpdateMenu(HWND hwndListView, HMENU hMenu) {
  UINT uID=IDM_LIST;
  DWORD dwStyle;

  //uncheck all of these guys
  CheckMenuItem(hMenu, IDM_LARGE_ICONS, MF_BYCOMMAND | MF_UNCHECKED);
  CheckMenuItem(hMenu, IDM_SMALL_ICONS, MF_BYCOMMAND | MF_UNCHECKED);
  CheckMenuItem(hMenu, IDM_LIST, MF_BYCOMMAND | MF_UNCHECKED);
  CheckMenuItem(hMenu, IDM_REPORT, MF_BYCOMMAND | MF_UNCHECKED);

  //check the appropriate view menu item
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

