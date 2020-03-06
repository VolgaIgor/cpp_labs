#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <vector>

std::vector< int > coord;

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
void DrawLineBresenham( HDC hdc, int x1, int y1, int x2, int y2 );

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = CreateSolidBrush( 0x00FFFFFF );

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
               0,                   /* Extended possibilites for variation */
               szClassName,         /* Classname */
               _T("Code::Blocks Template Windows App"),       /* Title Text */
               WS_OVERLAPPEDWINDOW, /* default window */
               CW_USEDEFAULT,       /* Windows decides the position */
               CW_USEDEFAULT,       /* where the window ends up on the screen */
               544,                 /* The programs width */
               375,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* No menu */
               hThisInstance,       /* Program Instance handler */
               NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message) {
        case WM_DESTROY:
            PostQuitMessage (0);
            break;
        case WM_LBUTTONDOWN:
            InvalidateRect( hwnd, NULL, TRUE );

            WORD x, y;
            x = LOWORD(lParam);
            y = HIWORD(lParam);

            coord.push_back(x);
            coord.push_back(y);

            UpdateWindow( hwnd );

            break;
        case WM_RBUTTONDOWN:
            InvalidateRect( hwnd, NULL, TRUE );
            coord.clear();

            break;
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);

            if ( coord.size() >= 4 ) {
                for ( int i = 0; i < coord.size() - 2; i += 2 ) {
                    DrawLineBresenham( hdc, coord[i], coord[i + 1], coord[i + 2], coord[i + 3] );
                }
                if ( coord.size() >= 6 ) {
                    DrawLineBresenham( hdc, coord[ coord.size() - 2], coord[ coord.size() - 1 ], coord[0], coord[1] );
                }
            }

            EndPaint(hwnd, &ps);

            break;
        default:
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

void DrawLineBresenham( HDC hdc, int x1, int y1, int x2, int y2 ) {
    int dx = (x2 - x1 >= 0 ? 1 : -1);
    int dy = (y2 - y1 >= 0 ? 1 : -1);

    int lengthX = abs(x2 - x1);
    int lengthY = abs(y2 - y1);

    int x = x1;
    int y = y1;

    int length = ( lengthX > lengthY ) ? lengthX : lengthY;

    SetPixel(hdc, x, y, 0x00000000);

    if (lengthY <= lengthX) {
        int d = -lengthX;

        while( length-- ) {
            x += dx;
            d += lengthY << 1;
            if (d > 0) {
                d -= lengthX << 1;
                y += dy;
            }
            SetPixel(hdc, x, y, 0x00000000);
        }
    } else {
        int d = -lengthY;

        while( length-- ) {
            y += dy;
            d += lengthX << 1;
            if (d > 0) {
                d -= lengthY << 1;
                x += dx;
            }
            SetPixel(hdc, x, y, 0x00000000);
        }
    }
}
