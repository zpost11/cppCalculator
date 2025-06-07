#include <windows.h> // Core Windows API header
#include <string>    // For std::string and std::to_wstring, std::stod
#include <vector>    // For std::vector
#include <cmath>     // For fmod (if needed for floating point modulo)
#include <stdexcept> // For std::invalid_argument and std::out_of_range

// Global Variables (for simplicity in this example)
HWND g_hwndDisplay; // Handle to the display edit control
std::wstring g_currentInput = L"0"; // Current number being typed
double g_total = 0.0;            // Accumulated total
wchar_t g_operator = L'\0';      // Current operator (+, -, *, /)
bool g_newOperation = true;      // Flag to indicate if we're starting a new number

// --- Control IDs (Unique identifiers for our buttons and display) ---
#define IDC_DISPLAY 1000
#define IDC_BUTTON_0 1001
#define IDC_BUTTON_1 1002
#define IDC_BUTTON_2 1003
#define IDC_BUTTON_3 1004
#define IDC_BUTTON_4 1005
#define IDC_BUTTON_5 1006
#define IDC_BUTTON_6 1007
#define IDC_BUTTON_7 1008
#define IDC_BUTTON_8 1009
#define IDC_BUTTON_9 1010
#define IDC_BUTTON_DOT 1011
#define IDC_BUTTON_PLUS 1012
#define IDC_BUTTON_MINUS 1013
#define IDC_BUTTON_MULTIPLY 1014
#define IDC_BUTTON_DIVIDE 1015
#define IDC_BUTTON_EQUALS 1016
#define IDC_BUTTON_CLEAR 1017     // C (Clear Entry)
#define IDC_BUTTON_ALLCLEAR 1018  // AC (All Clear)

// --- Forward declaration of the Window Procedure ---
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// --- WinMain: Entry point for the Windows application ---
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"CalculatorWindowClass";

    // 1. Register the Window Class
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;       // Pointer to the window procedure
    wc.hInstance = hInstance;       // Handle to the instance of the application
    wc.lpszClassName = CLASS_NAME;  // Name of the window class
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1); // Background color (standard button face color)
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);      // Cursor for the window

    if (!RegisterClassW(&wc)) {
        MessageBoxW(NULL, L"Window Registration Failed!", L"Error", MB_ICONERROR | MB_OK);
        return 0;
    }

    // 2. Create the Main Window
    HWND hwnd = CreateWindowExW(
        0,                                   // Optional window styles
        CLASS_NAME,                          // Window class name
        L"Simple Calculator",                // Window title
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, // Window style (WS_CLIPCHILDREN is important for child controls)
        CW_USEDEFAULT, CW_USEDEFAULT,        // Initial x, y position
        400, 500,                            // Initial width, height
        NULL,                                // Parent window handle (NULL for desktop)
        NULL,                                // Menu handle
        hInstance,                           // Instance handle
        NULL                                 // Additional application data
    );

    if (hwnd == NULL) {
        MessageBoxW(NULL, L"Window Creation Failed!", L"Error", MB_ICONERROR | MB_OK);
        return 0;
    }

    // 3. Show and Update the Main Window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // 4. Message Loop (The heart of the GUI application)
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg); // Translates virtual-key messages into character messages
        DispatchMessage(&msg);  // Dispatches a message to a window procedure
    }

    return (int)msg.wParam;
}

// --- Helper function to update the display ---
void UpdateDisplay() {
    SetWindowTextW(g_hwndDisplay, g_currentInput.c_str());
}

// --- Function to perform calculation ---
void Calculate() {
    double currentNum = 0.0;
    try {
        currentNum = std::stod(g_currentInput);
    }
    catch (const std::invalid_argument& e) { // Fix: Add '& e' to catch the exception object
        // Handle invalid input, though our buttons should prevent this
        MessageBoxW(NULL, (L"Invalid number format: " + std::wstring(e.what(), e.what() + strlen(e.what()))).c_str(), L"Error", MB_OK | MB_ICONERROR);
        g_currentInput = L"0";
        g_total = 0.0;
        g_operator = L'\0';
        g_newOperation = true;
        UpdateDisplay();
        return;
    }
    catch (const std::out_of_range& e) { // Fix: Add '& e' to catch the exception object
        MessageBoxW(NULL, (L"Number out of range: " + std::wstring(e.what(), e.what() + strlen(e.what()))).c_str(), L"Error", MB_OK | MB_ICONERROR);
        g_currentInput = L"0";
        g_total = 0.0;
        g_operator = L'\0';
        g_newOperation = true;
        UpdateDisplay();
        return;
    }

    if (g_operator == L'+') {
        g_total += currentNum;
    }
    else if (g_operator == L'-') {
        g_total -= currentNum;
    }
    else if (g_operator == L'*') {
        g_total *= currentNum;
    }
    else if (g_operator == L'/') {
        if (currentNum == 0.0) {
            MessageBoxW(NULL, L"Division by zero!", L"Error", MB_OK | MB_ICONERROR);
            // Reset calculator on division by zero
            g_currentInput = L"0";
            g_total = 0.0;
            g_operator = L'\0';
            g_newOperation = true;
            UpdateDisplay();
            return;
        }
        g_total /= currentNum;
    }
    else { // First number or after AC/equals
        g_total = currentNum;
    }

    // Convert result to string for display, using a precision to avoid scientific notation for small numbers
    // and to limit decimal places.
    g_currentInput = std::to_wstring(g_total);

    // Remove trailing zeros and decimal point if it's an integer
    size_t dot_pos = g_currentInput.find(L'.');
    if (dot_pos != std::wstring::npos) {
        while (g_currentInput.back() == L'0') {
            g_currentInput.pop_back();
        }
        if (g_currentInput.back() == L'.') {
            g_currentInput.pop_back();
        }
    }
    UpdateDisplay();
    g_newOperation = true; // Ready for a new number after calculation
}

// --- Window Procedure: Handles messages sent to the window ---
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        // Create the display (read-only Edit Control)
        g_hwndDisplay = CreateWindowExW(
            WS_EX_CLIENTEDGE, // Extended window style (sunken border)
            L"EDIT",          // Predefined Edit control class
            L"0",             // Initial text
            WS_CHILD | WS_VISIBLE | ES_RIGHT | ES_READONLY, // Child, visible, right-aligned, read-only
            10, 10,           // x, y position
            360, 40,          // width, height
            hwnd,             // Parent window
            (HMENU)IDC_DISPLAY, // Control ID
            GetModuleHandle(NULL), // Instance handle
            NULL              // No creation parameters
        );

        // Set font for the display
        HFONT hFont = CreateFontW(
            30, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            VARIABLE_PITCH, L"Arial"
        );
        SendMessage(g_hwndDisplay, WM_SETFONT, (WPARAM)hFont, TRUE);

        // --- Create Buttons ---
        // Button positions and labels
        struct ButtonInfo {
            const wchar_t* text;
            int id;
            int row; // 0-indexed for conceptual layout
            int col; // 0-indexed for conceptual layout
        };

        std::vector<ButtonInfo> buttons = {
            {L"7", IDC_BUTTON_7, 0, 0}, {L"8", IDC_BUTTON_8, 0, 1}, {L"9", IDC_BUTTON_9, 0, 2}, {L"/", IDC_BUTTON_DIVIDE, 0, 3},
            {L"4", IDC_BUTTON_4, 1, 0}, {L"5", IDC_BUTTON_5, 1, 1}, {L"6", IDC_BUTTON_6, 1, 2}, {L"*", IDC_BUTTON_MULTIPLY, 1, 3},
            {L"1", IDC_BUTTON_1, 2, 0}, {L"2", IDC_BUTTON_2, 2, 1}, {L"3", IDC_BUTTON_3, 2, 2}, {L"-", IDC_BUTTON_MINUS, 2, 3},
            {L"0", IDC_BUTTON_0, 3, 0}, {L".", IDC_BUTTON_DOT, 3, 1}, {L"=", IDC_BUTTON_EQUALS, 3, 2}, {L"+", IDC_BUTTON_PLUS, 3, 3},
            {L"C", IDC_BUTTON_CLEAR, 4, 0}, {L"AC", IDC_BUTTON_ALLCLEAR, 4, 1}
        };

        int startY = 60; // Starting Y position for buttons below the display
        int buttonWidth = 80;
        int buttonHeight = 70;
        int padding = 10;

        HFONT hButtonFont = CreateFontW(
            24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            VARIABLE_PITCH, L"Arial"
        );

        for (const auto& btn : buttons) {
            HWND hButton = CreateWindowW(
                L"BUTTON",
                btn.text,
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                10 + (btn.col * (buttonWidth + padding)), // X position
                startY + (btn.row * (buttonHeight + padding)), // Y position
                buttonWidth, // Width
                buttonHeight, // Height
                hwnd,
                (HMENU)(INT_PTR)btn.id, // Fix: Cast to INT_PTR first, then to HMENU
                GetModuleHandle(NULL),
                NULL
            );
            SendMessage(hButton, WM_SETFONT, (WPARAM)hButtonFont, TRUE);
        }
        break;
    }

    case WM_COMMAND: {
        // Messages from child controls (buttons)
        int wmId = LOWORD(wParam); // Control ID
        // int wmEvent = HIWORD(wParam); // Notification code (e.g., BN_CLICKED for buttons)

        // Digit and decimal point buttons
        if (wmId >= IDC_BUTTON_0 && wmId <= IDC_BUTTON_DOT) {
            if (g_newOperation) {
                g_currentInput = L""; // Clear for new number
                g_newOperation = false;
            }
            wchar_t digit = L' ';
            switch (wmId) {
            case IDC_BUTTON_0: digit = L'0'; break;
            case IDC_BUTTON_1: digit = L'1'; break;
            case IDC_BUTTON_2: digit = L'2'; break;
            case IDC_BUTTON_3: digit = L'3'; break;
            case IDC_BUTTON_4: digit = L'4'; break;
            case IDC_BUTTON_5: digit = L'5'; break;
            case IDC_BUTTON_6: digit = L'6'; break;
            case IDC_BUTTON_7: digit = L'7'; break;
            case IDC_BUTTON_8: digit = L'8'; break;
            case IDC_BUTTON_9: digit = L'9'; break;
            case IDC_BUTTON_DOT:
                if (g_currentInput.find(L'.') == std::wstring::npos) { // Only one decimal point
                    digit = L'.';
                }
                else {
                    digit = L'\0'; // Do not append if already exists
                }
                break;
            }
            if (digit != L'\0') {
                if (g_currentInput == L"0" && digit != L'.') { // Replace initial 0
                    g_currentInput = std::wstring(1, digit); // Create a wstring from single wchar_t
                }
                else {
                    g_currentInput += digit;
                }
                UpdateDisplay();
            }
        }
        // Operator buttons
        else if (wmId >= IDC_BUTTON_PLUS && wmId <= IDC_BUTTON_DIVIDE) {
            // If there was a previous operation, calculate it first
            if (!g_newOperation) { // If a number was entered
                Calculate();
            }
            // Set the new operator
            switch (wmId) {
            case IDC_BUTTON_PLUS:    g_operator = L'+'; break;
            case IDC_BUTTON_MINUS:   g_operator = L'-'; break;
            case IDC_BUTTON_MULTIPLY: g_operator = L'*'; break;
            case IDC_BUTTON_DIVIDE:  g_operator = L'/'; break;
            }
            g_newOperation = true; // Next digit will start a new number
            g_currentInput = L"0"; // Clear display (conceptually for next input)
            UpdateDisplay();
        }
        // Equals button
        else if (wmId == IDC_BUTTON_EQUALS) {
            Calculate();
            g_operator = L'\0'; // Clear operator after calculation
        }
        // Clear Entry (C)
        else if (wmId == IDC_BUTTON_CLEAR) {
            g_currentInput = L"0";
            g_newOperation = true;
            UpdateDisplay();
        }
        // All Clear (AC)
        else if (wmId == IDC_BUTTON_ALLCLEAR) {
            g_currentInput = L"0";
            g_total = 0.0;
            g_operator = L'\0';
            g_newOperation = true;
            UpdateDisplay();
        }
        break;
    }

    case WM_CLOSE: // User clicked the close button (X)
        DestroyWindow(hwnd); // Destroy the window
        break;
    case WM_DESTROY: // Window is being destroyed
        PostQuitMessage(0); // Post a WM_QUIT message to terminate the message loop
        break;
    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam); // Default message processing
    }
    return 0; // Message processed
}
