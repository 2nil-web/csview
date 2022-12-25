
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
#include <chrono>
#include <filesystem>
#include <fstream>


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "resource.h"

#ifndef WIN32
#define GET_WM_COMMAND_ID(wp, lp)   (wp)
#define GET_WM_COMMAND_HWND(wp, lp) (HWND)(LOWORD(lp))
#define GET_WM_COMMAND_CMD(wp, lp)  HIWORD(lp)
#endif

#define ID_LISTVIEW  2000

#define trc std::cout << "LINE " << __LINE__ << std::endl;

// Local Function Prototypes
#define ErrorHandler() ErrorHandlerEx(__LINE__, __FILE__)
void ErrorHandlerEx(WORD, LPSTR);

// Global Variables
HINSTANCE g_hInst;
TCHAR g_szClassName[]=TEXT("wintailClass");
// Nom du fichier csv à visualiser
std::string g_filename;
bool g_autoref=true;
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

std::string InfMsg=R"(WinTail displays the content of a csv file.
At least one argument to provide the name of file to view.
A second argument (true, on, 1 or anything else for false) to indicate if constant file polling is required (this is the default).
A third argument to modify the polling interval in millisecond (default is 2000 ms). Value is overwritten by environment variable 'REFRESH_INTERVAL'.
A fourth argument to define the csv separator (default is ';').)";

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

HWND hwndListView=NULL;
LRESULT ListViewNotify(HWND, WPARAM, LPARAM lParam) {
  LPNMHDR lpnmh=(LPNMHDR)lParam;

  if (lpnmh->hwndFrom != hwndListView) return 0;

  switch (lpnmh->code) {
    case LVN_GETDISPINFO: {
      //std::cout << "VK_ESCAPE " << VK_ESCAPE << ", wParam " << wParam << ", lParam " << lParam << ", Notif code " << lpnmh->code << std::endl;
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
    return 1;

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
    return 1;

    case LVN_ODFINDITEM: {
      /*
      LPNMLVFINDITEM lpFindItem=(LPNMLVFINDITEM)lParam;
      This sample doesn't use this notification, but this is sent when the 
      ListView needs a particular item. Return -1 if the item is not found.
      */
    }
    return 1;
  }

  return 0;
}

time_t curr_mtime=0;
// Convert win32api FILETIME to unix time_t
time_t FT2t(FILETIME ft) {
  return ((LONGLONG)ft.dwLowDateTime + ((LONGLONG)(ft.dwHighDateTime) << 32LL)-116444736000000000LL)/10000000;
}

// Get last mtime of a file using stat and GetFileTime, return the highest value (i.e. the most recent time)
time_t get_mtime(std::string fname) {
  struct stat st;
  time_t last_mtime;
  stat(fname.c_str(), &st);

  HANDLE hFile=CreateFile(g_filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  if (hFile != INVALID_HANDLE_VALUE) {
    FILETIME LastWriteTime = { 0, 0 };
    GetFileTime(hFile, NULL, NULL, &LastWriteTime);
    CloseHandle(hFile);
    last_mtime=FT2t(LastWriteTime);
  }

  if (st.st_mtime > last_mtime) return st.st_mtime;

  return last_mtime;
}

Pas plus de 30 colonnes
size_t maxcell=29;
unsigned int readCsv(std::string fname, HWND hwnd, char sep=0) {
  g_sheet.clear();
  g_header.clear();
  g_widestCol.clear();
  //std::cout << "Starting to load in memory of the file " << fname << std::endl;
  char *smr=getenv("MAXCOL");
  if (smr) maxcell=std::stoi(smr);
  // nuplet pour évaluer la largeur de chaque colonne
  // Le 1er nuplet (tuple) c'est pour la largeur de la 1ére colonne qui indique le numéro de ligne ...
  g_widestCol.push_back(std::make_tuple(0, 0, 0));
  g_maxCol=0;

  // Get last modification time of the file, in case of needed refresh
  curr_mtime=get_mtime(fname);

  unsigned int count=0;
  std::ifstream fp(fname);

  if (fp.is_open()) {
    std::string ln, cell;
    std::vector < std::string > row;
    RECT rc;

    while (std::getline(fp, ln)) {
      std::stringstream ss(ln);
      //std::cout << ln << std::endl;
      row.clear();
      size_t iPos=2;
      if (sep == 0) iPos=1;

      std::string countS=std::to_string(count+1);
      int lvgsw=ListView_GetStringWidth(hwnd, countS.c_str());

      if (lvgsw > std::get < 2 > (g_widestCol[0])) {
        g_widestCol[0]=std::make_tuple(count, countS.size(), lvgsw);
      }

      for(;;) {
        if (sep == 0) {
          if (!std::getline(ss, cell)) break;
        } else {
          if (!std::getline(ss, cell, sep)) break;
        }
        row.push_back(cell);
        //std::cout << "Count " << count << ", cell " << cell << std::endl;

        if (count > 0) {
          ListView_GetItemRect(hwnd, count, &rc, LVIR_BOUNDS);
          //std::cout << "Pos " << count << "=(" << rc.left << ',' << rc.top << ',' << rc.right << ',' << rc.bottom << ')' << std::endl;

          if (g_widestCol.size() < iPos) g_widestCol.push_back(std::make_tuple(count, cell.size(), ListView_GetStringWidth(hwnd, cell.c_str())));
          else {
            lvgsw=ListView_GetStringWidth(hwnd, cell.c_str());
            if (lvgsw > std::get < 2 > (g_widestCol[iPos-1])) g_widestCol[iPos-1]=std::make_tuple(count, cell.size(), lvgsw);
          }
          iPos++;
        }

        //std::cout << "maxcell:" << maxcell << ", row.size():" << row.size() << ", g_maxCol:" << g_maxCol << std::endl;
        if (row.size() > maxcell) break;
      }

      if (row.size() > g_maxCol) g_maxCol=row.size();
      if (count == 0) g_header=row;
      else g_sheet.push_back(row);
      count++;
    }

    fp.close();
  }

  //std::cout << "End of the loading in memory of the file " << fname << std::endl;
  return count;
}

BOOL InsertListViewItems(HWND hwndListView) {
  //empty the list
  ListView_DeleteAllItems(hwndListView);
  //set the number of items in the list
  ListView_SetItemCount(hwndListView, g_sheet.size());
  return TRUE;
}

BOOL InitListView(HWND hwndListView) {
  size_t i;

  // Empty the list
  for (i=0; i <= 100; i++) ListView_DeleteColumn(hwndListView, 0);
  ListView_DeleteAllItems(hwndListView);

  //initialize the columns
  LV_COLUMN lvColumn;
  lvColumn.mask=LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
  lvColumn.fmt=LVCFMT_LEFT;
  lvColumn.cx=120;

  //std::cout << __LINE__ << std::endl;
  lvColumn.pszText=(char * )"#";
  ListView_InsertColumn(hwndListView, 0, &lvColumn);
  //std::cout << "g_maxCol " << g_maxCol << std::endl;

  //std::cout << __LINE__ << std::endl;
  for (i=0; i < g_maxCol; i++) {
    //std::cout << "Adding header " << i+1 << ", value " << g_header[i] << ", size " << g_header[i].size() << std::endl;
    lvColumn.pszText=(char * ) g_header[i].c_str();
    ListView_InsertColumn(hwndListView, i+1, &lvColumn);
  }

  //std::cout << __LINE__ << std::endl;
  InsertListViewItems(hwndListView);
  //std::cout << __LINE__ << std::endl;
  ListView_SetExtendedListViewStyle(hwndListView, LVS_EX_FULLROWSELECT); // Set style
  //std::cout << __LINE__ << std::endl;
  return TRUE;
}

void ResizeListView(HWND hwndListView, HWND hwndParent) {
  RECT rc;
  GetClientRect(hwndParent, &rc);
  MoveWindow(hwndListView, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE);
}

HWND CreateListView(HINSTANCE /*hInstance*/ , HWND hwndParent) {
  DWORD dwStyle;
  HWND hwndListView;

  dwStyle=WS_TABSTOP | WS_CHILD | WS_BORDER | WS_VISIBLE | LVS_AUTOARRANGE | LVS_REPORT | LVS_OWNERDATA;

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

BOOL DoContextMenu(HWND hWnd, WPARAM wParam, LPARAM lParam) {
  HWND hwndListView=(HWND) wParam;
  HMENU hMenuLoad, hMenu;

  if (hwndListView != GetDlgItem(hWnd, ID_LISTVIEW)) return FALSE;
  hMenuLoad=LoadMenu(g_hInst, MAKEINTRESOURCE(IDM_CONTEXT_MENU));
  hMenu=GetSubMenu(hMenuLoad, 0);

  if (GetMenuState(GetMenu(hWnd), IDM_AREF, MF_BYCOMMAND) & MF_CHECKED) {
    CheckMenuItem(hMenu, IDM_AREF, MF_BYCOMMAND | MF_CHECKED);
    EnableMenuItem(hMenu, IDM_REFR, MF_BYCOMMAND | MF_DISABLED);
  } else {
    CheckMenuItem(hMenu, IDM_AREF, MF_BYCOMMAND | MF_UNCHECKED);
    EnableMenuItem(hMenu, IDM_REFR, MF_BYCOMMAND | MF_ENABLED);
  }
  
  TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, LOWORD(lParam), HIWORD(lParam), 0, hWnd, NULL);
  DestroyMenu(hMenuLoad);
  return TRUE;
}

void ListViewToBottom( HWND hwnd) {
  int yMin, yMax;
  GetScrollRange(hwnd, SB_VERT, &yMin, &yMax);
  //std::cout << "yMin:" << yMin << ", yMax:" << yMax << std::endl;
  RECT rc;
  ListView_GetItemRect(hwnd, yMax, &rc, LVIR_SELECTBOUNDS);
  //std::cout << "Go to bottom=" << yMax << ", rect (" << rc.left << ',' << rc.top << ',' << rc.right << ',' << rc.bottom << ')' << std::endl;
  ListView_Scroll(hwnd, 0, rc.top);
  ListView_SetItemState(hwnd, yMax, LVIS_SELECTED, LVIS_SELECTED);
  ListView_SetItemState(hwnd, yMax, LVIS_FOCUSED, LVIS_FOCUSED);
  SetFocus(hwnd);
}

DWORD WINAPI RefreshFile(LPVOID data) {
  HWND hwnd=(HWND)data;
  HMENU hMenu=GetMenu(hwnd);
  //bool aref=GetMenuState(hMenu, IDM_AREF, MF_BYCOMMAND) & MF_CHECKED;
  EnableMenuItem(hMenu, IDM_AREF, MF_BYCOMMAND | MF_DISABLED);
  EnableMenuItem(hMenu, IDM_REFR, MF_BYCOMMAND | MF_DISABLED);
  return 0;
}

bool is_not_csv(std::string fn) {
  //store the position of last '.' in the file name
  size_t pos=fn.find_last_of(".");
  //store the characters after the '.' from the file_name string
  if (pos == std::string::npos) return true;

  std::string ext=fn.substr(pos+1);
  if (ext == "csv") return false;
  return true;
}

void mkListView(HWND hWnd) {
  // create the TreeView control
  if (hwndListView == NULL) hwndListView=CreateListView(g_hInst, hWnd);
  else ShowWindow(hwndListView, FALSE);

  char sep=g_separator;
  if (is_not_csv(g_filename)) sep=0;

  readCsv(g_filename, hwndListView, sep);
  InitListView(hwndListView);
  // https://stackoverflow.com/questions/9255540/how-auto-size-the-columns-width-of-a-list-view-in-virtual-mode
  // get<0>=row, get<1>=text.size, get<2>=pixel width
  //std::cout << "Widest cells for:";
  //size_t rw;
  size_t sumch=0, sumpx=0, ch, px;
  for (size_t i=0; i < g_widestCol.size(); i++) {
    //rw=get < 0 > (g_widestCol[i]);
    ch=get < 1 > (g_widestCol[i]);
    px=get < 2 > (g_widestCol[i]);
    //std::cout << '(' << i << ", " << rw << ")=" << ch << " ch, " << px << "px, ";
    sumch += ch;
    sumpx += px;
    ListView_SetColumnWidth(hwndListView, i, 20+px);
  }
  //std::cout << ", SUMCH " << sumch << ", SUMPX " << sumpx << std::endl;


  //std::cout << std::endl;
  ListViewToBottom(hwndListView);
  ShowWindow(hwndListView, TRUE);
}

void AutoRefresh(HWND hWnd, UINT , UINT_PTR , DWORD ) {
  time_t last_mtime=get_mtime(g_filename);

  if (curr_mtime != last_mtime) {
    curr_mtime=last_mtime;
    mkListView(hWnd);
  }
}


UINT g_refitv=2000;
void do_autoref(HWND hWnd, bool do_it) {
  if (do_it) {
    CheckMenuItem(GetMenu(hWnd), IDM_AREF, MF_BYCOMMAND | MF_CHECKED);
    EnableMenuItem(GetMenu(hWnd), IDM_REFR, MF_BYCOMMAND | MF_DISABLED);
    char *sri=getenv("REFRESH_INTERVAL");
    UINT ri;
    if (sri) ri=std::stoi(sri);
    else ri=g_refitv;
    
    SetTimer(hWnd, 1, ri, AutoRefresh);
  }
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
  switch (uMessage) {
  case WM_CREATE:
    mkListView(hWnd);
    do_autoref(hWnd, g_autoref);
    break;

  case WM_KEYDOWN:
    if (wParam == VK_ESCAPE) DestroyWindow(hWnd);
    break;

  case WM_NOTIFY:
    if (ListViewNotify(hWnd, wParam, lParam)) return 0;
    else {
      //Sleep(400);
      if (HIBYTE(GetKeyState(VK_ESCAPE)) != 0 && GetFocus() != hWnd) DestroyWindow(hWnd);
    }
    break;

  case WM_SIZE:
    ResizeListView(hwndListView, hWnd);
    break;

  case WM_CONTEXTMENU:
    if (DoContextMenu(hWnd, wParam, lParam)) return FALSE;
    break;

  case WM_COMMAND:
    switch (GET_WM_COMMAND_ID(wParam, lParam)) {
    case IDM_OPEN: {
      OPENFILENAME ofn;
      char fn[MAX_PATH]="";

      ZeroMemory(&ofn, sizeof(OPENFILENAME));
      ofn.lStructSize=sizeof(OPENFILENAME);
      ofn.hwndOwner=hWnd;
      ofn.lpstrFilter="Fichier csv\0*.csv\0Tout fichier\0*.*\0\0";
      ofn.nFilterIndex=0;

      //GetDlgItemText(hDlg, IDC_SUBFILE, fn, MAX_PATH);
      ofn.lpstrFile=fn;
      ofn.nMaxFile=MAX_PATH;

      ofn.lpstrFileTitle=NULL;
      ofn.lpstrInitialDir=NULL;
      ofn.lpstrTitle="Fichier texte ou csv";
      ofn.Flags=OFN_FILEMUSTEXIST|OFN_SHOWHELP;

      if (GetOpenFileName(&ofn)) {
        g_filename=fn;
        mkListView(hWnd);
      }
      } break;

    case IDM_REFR: {
      mkListView(hWnd);
      break;
    }

    case IDM_AREF:
      if (GetMenuState(GetMenu(hWnd), IDM_AREF, MF_BYCOMMAND) & MF_CHECKED) {
        CheckMenuItem(GetMenu(hWnd), IDM_AREF, MF_BYCOMMAND | MF_UNCHECKED);
        EnableMenuItem(GetMenu(hWnd), IDM_REFR, MF_BYCOMMAND | MF_ENABLED);
        KillTimer(hWnd, 1);
      } else do_autoref(hWnd, true);
      break;
    case IDM_EXIT:
      DestroyWindow(hWnd);
      break;

    case IDM_HELP:
      MessageBox(NULL, InfMsg.c_str(), "WinTail", MB_OK);
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
  wcex.lpfnWndProc=(WNDPROC)MainWndProc;
  wcex.cbClsExtra=0;
  wcex.cbWndExtra=0;
  wcex.hInstance=hInstance;
  wcex.hCursor=LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszMenuName=MAKEINTRESOURCE(IDM_MAIN_MENU);
  wcex.lpszClassName=g_szClassName;
  wcex.hIcon=LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_MAINICON));
  wcex.hIconSm=(HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_MAINICON), IMAGE_ICON, 16, 16, 0);

  aReturn=RegisterClassEx( & wcex);

  if (aReturn == 0) {
    //std::cout << "aRet = 0 !!" << std::endl;
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
  g_hInst=hInstance;
  if (!hPrevInstance && !InitApplication(hInstance)) return FALSE;

  MSG msg;
  std::vector<std::string> args=cmdLineToSVec();

  if (args.size() == 2 && (args[1] == "-h" || args[1] == "-help" || args[1] == "--help")) {
    std::cout << InfMsg << std::endl;
  }

  if (args.size() > 1) g_filename=args[1];
  if (args.size() > 2) g_autoref=args[2] == "1" || args[2] == "on" || args[2] == "true"?true:false;
  if (args.size() > 3) g_refitv=std::stoi(args[3]);
  if (args.size() > 4) {
    if (args[4] == "nosep") g_separator='\0';
    else g_separator=args[4][0];
  }

  // Required to use the common controls (not so sure as of 2022)
  InitCommonControls();
  if (!InitInstance(hInstance, nCmdShow)) return FALSE;

  // Acquire and dispatch messages
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return (int) msg.wParam;
}

