#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <vector>
#include <algorithm>
#include <iostream>
/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
void DrawLine( HDC hdc, int x1, int y1, int x2, int y2, COLORREF color );

std::vector< POINT > points;
int tY = -1, bY = -1;

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


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc = GetDC(hwnd);;
    COLORREF black = RGB(0, 0, 0);
    COLORREF blue = RGB(0, 0, 255);

    switch (message) {
        case WM_DESTROY:
            PostQuitMessage (0);
            break;
        case WM_LBUTTONDOWN:

            POINT p;
            p.x = LOWORD(lParam);
            p.y = HIWORD(lParam);

            if ( points.size() > 0 ) {
                DrawLine( hdc, (*points.rbegin()).x, (*points.rbegin()).y, p.x, p.y, black );
            }

            if ( p.y < bY || bY == -1 ) bY = p.y;
            if ( p.y > tY || tY == -1 ) tY = p.y;

            points.push_back( p );

            break;
        case WM_RBUTTONDOWN:
            {
            DrawLine( hdc, (*points.begin()).x, (*points.begin()).y, (*points.rbegin()).x, (*points.rbegin()).y , black );
            points.push_back( points[0] );

            std::vector< int > adds;
            int i;

            for ( i = 0; i < points.size() - 1; i++ ) {
                DrawLine( hdc, points[i].x, points[i].y, points[i + 1].x, points[i + 1].y, blue );
            }

            for ( int y = bY; y < tY; y++ ) {
                adds.clear();
                for ( i = 0; i < points.size() - 1; i++ ) {
                    int minY, maxY;

                    int dy = points[i + 1].y - points[i].y;

                    if ( dy == 0 ) continue;

                    if ( points[i].y > points[i + 1].y ) {
                        maxY = points[i].y;
                        minY = points[i + 1].y;
                    } else {
                        minY = points[i].y;
                        maxY = points[i + 1].y;
                    }

                    if ( y > maxY || y < minY ) continue;

                    int x = ( (float)y - (float)points[i].y ) / ( (float)points[i + 1].y - (float)points[i].y ) * ( (float)points[i + 1].x - (float)points[i].x ) + (float)points[i].x;

                    if (
                        ( ( dy < 0 ) && ( points[i].x != x || points[i].y != y ) ) ||
                        ( ( dy > 0 ) && ( points[i + 1].x != x || points[i + 1].y != y ) )
                    ) {
                        adds.push_back( x );
                    }
                }

                if ( adds.size() > 1 ) {
                    std::sort( adds.begin(), adds.end() );
                    for ( i = 0; i < adds.size(); i+=2 ) {
                        DrawLine( hdc, adds[i], y, adds[i + 1], y, blue );
                    }
                }

            }
            }
            break;
        default:
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

void DrawLine( HDC hdc, int x1, int y1, int x2, int y2, COLORREF color ) {
    int dx = (x2 - x1 >= 0 ? 1 : -1);
    int dy = (y2 - y1 >= 0 ? 1 : -1);

    int lengthX = abs(x2 - x1);
    int lengthY = abs(y2 - y1);

    int x = x1;
    int y = y1;

    int length = ( lengthX > lengthY ) ? lengthX : lengthY;

    SetPixel(hdc, x, y, color);

    if (lengthY <= lengthX) {
        int d = -lengthX;

        while( length-- ) {
            x += dx;
            d += lengthY << 1;
            if (d > 0) {
                d -= lengthX << 1;
                y += dy;
            }
            SetPixel(hdc, x, y, color);
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
            SetPixel(hdc, x, y, color);
        }
    }
}
