#include "hotkeyrecorder.h"
#include <QDebug>
#include <QStringList>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

HotkeyRecorder* HotkeyRecorder::s_instance = nullptr;
void* HotkeyRecorder::m_hook = nullptr;
bool HotkeyRecorder::m_recording = false;
bool HotkeyRecorder::m_ctrl = false;
bool HotkeyRecorder::m_alt = false;
bool HotkeyRecorder::m_shift = false;
bool HotkeyRecorder::m_meta = false;

HotkeyRecorder* HotkeyRecorder::instance()
{
    if (!s_instance) {
        s_instance = new HotkeyRecorder();
    }
    return s_instance;
}

HotkeyRecorder::HotkeyRecorder(QObject *parent) : QObject(parent)
{
}

HotkeyRecorder::~HotkeyRecorder()
{
    stopRecording();
}

void HotkeyRecorder::startRecording()
{
#ifdef Q_OS_WIN
    if (m_recording) return;
    
    // SetWindowsHookEx takes WH_KEYBOARD_LL (13)
    m_hook = SetWindowsHookEx(13, (HOOKPROC)LowLevelKeyboardProc, GetModuleHandle(nullptr), 0);
    
    if (m_hook) {
        m_recording = true;
        m_ctrl = m_alt = m_shift = m_meta = false; // Reset state
        qDebug() << "HotkeyRecorder: Hook installed.";
    } else {
        qWarning() << "HotkeyRecorder: Failed to install hook.";
    }
#endif
}

void HotkeyRecorder::stopRecording()
{
#ifdef Q_OS_WIN
    if (!m_recording) return;
    
    UnhookWindowsHookEx((HHOOK)m_hook);
    m_hook = nullptr;
    m_recording = false;
    qDebug() << "HotkeyRecorder: Hook uninstalled.";
#endif
}

bool HotkeyRecorder::isRecording() const
{
    return m_recording;
}

#ifdef Q_OS_WIN
long long __stdcall HotkeyRecorder::LowLevelKeyboardProc(int nCode, unsigned long long wParam, long long lParam)
{
    // HC_ACTION = 0
    if (nCode == 0 && m_recording) {
        KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *)lParam;
        
        // We only care about keydown to record the sequence. 
        // We block both KEYDOWN and KEYUP to prevent OS actions.
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            DWORD vkCode = p->vkCode;
            
            bool isModifier = false;
            if (vkCode == VK_LCONTROL || vkCode == VK_RCONTROL || vkCode == VK_CONTROL) { isModifier = true; }
            if (vkCode == VK_LMENU || vkCode == VK_RMENU || vkCode == VK_MENU) { isModifier = true; }
            if (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT || vkCode == VK_SHIFT) { isModifier = true; }
            if (vkCode == VK_LWIN || vkCode == VK_RWIN) { isModifier = true; }
                               
            if (!isModifier) {
                // A non-modifier key was pressed. Build the sequence!
                QStringList parts;
                
                // Use GetAsyncKeyState to robustly check the physical key state.
                // This fixes the bug where users hold modifiers BEFORE clicking the record button.
                if (GetAsyncKeyState(VK_CONTROL) & 0x8000) parts << "Ctrl";
                if (GetAsyncKeyState(VK_MENU) & 0x8000) parts << "Alt";
                if (GetAsyncKeyState(VK_SHIFT) & 0x8000) parts << "Shift";
                if ((GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000)) parts << "Meta";
                
                QString keyStr;
                switch (vkCode) {
                    case VK_SPACE: keyStr = "Space"; break;
                    case VK_RETURN: keyStr = "Return"; break;
                    case VK_TAB: keyStr = "Tab"; break;
                    case VK_ESCAPE: keyStr = "Esc"; break;
                    case VK_BACK: keyStr = "Backspace"; break;
                    case VK_UP: keyStr = "Up"; break;
                    case VK_DOWN: keyStr = "Down"; break;
                    case VK_LEFT: keyStr = "Left"; break;
                    case VK_RIGHT: keyStr = "Right"; break;
                    case VK_DELETE: keyStr = "Del"; break;
                    case VK_INSERT: keyStr = "Ins"; break;
                    case VK_HOME: keyStr = "Home"; break;
                    case VK_END: keyStr = "End"; break;
                    case VK_PRIOR: keyStr = "PgUp"; break;
                    case VK_NEXT: keyStr = "PgDown"; break;
                    default:
                        if (vkCode >= VK_F1 && vkCode <= VK_F24) {
                            keyStr = QString("F%1").arg(vkCode - VK_F1 + 1);
                        } else {
                            UINT mappedChar = MapVirtualKey(vkCode, MAPVK_VK_TO_CHAR);
                            if (mappedChar != 0) {
                                keyStr = QString((char)mappedChar).toUpper();
                            } else {
                                keyStr = QString("Key_%1").arg(vkCode, 0, 16);
                            }
                        }
                        break;
                }
                
                parts << keyStr;
                QString finalSequence = parts.join("+");
                
                if (s_instance) {
                    emit s_instance->sequenceRecorded(finalSequence);
                }
            }
        }
        
        // Return 1 to swallow the key press/release completely
        return 1;
    }
    
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}
#else
long long __stdcall HotkeyRecorder::LowLevelKeyboardProc(int, unsigned long long, long long) { return 0; }
#endif
