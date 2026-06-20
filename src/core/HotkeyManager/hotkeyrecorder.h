#ifndef HOTKEYRECORDER_H
#define HOTKEYRECORDER_H

#include <QObject>
#include <QString>

class HotkeyRecorder : public QObject
{
    Q_OBJECT
public:
    static HotkeyRecorder* instance();

    void startRecording();
    void stopRecording();
    bool isRecording() const;

signals:
    // Emitted when a valid shortcut is pressed
    void sequenceRecorded(const QString &sequence);

private:
    explicit HotkeyRecorder(QObject *parent = nullptr);
    ~HotkeyRecorder();

    static HotkeyRecorder* s_instance;
    static void* m_hook; // Stores the HHOOK pointer safely
    static bool m_recording;
    
    // Modifier tracking
    static bool m_ctrl;
    static bool m_alt;
    static bool m_shift;
    static bool m_meta;

    // Windows API callback function (uses generic types in header to avoid windows.h pollution)
    static long long __stdcall LowLevelKeyboardProc(int nCode, unsigned long long wParam, long long lParam);
};

#endif // HOTKEYRECORDER_H
