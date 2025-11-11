# TowerToys

A modular desktop productivity utility for Windows inspired by PowerToys, built with C++/Qt.

## Description

TowerToys provides a collection of tools to enhance productivity on Windows. It runs conveniently from the system tray and offers modules like a Macro Manager and local network File/Clipboard Share.

## Features (v1.0)

* **Modular Architecture:** Easily enable/disable or add new utilities (PowerToys style).
* **Macro Manager:** ⚡
    * Create macros consisting of multiple actions (Open App, Open URL, Type Keystroke, Open VS Code Folder, **Open Folder**, **Run Command**).
    * Assign global hotkeys (shortcuts) to trigger macros.
    * Edit existing actions within macros.
    * Macros persist between application restarts.
* **Local File Share:** 🌐
    * Start a local web server accessible on your Wi-Fi network.
    * Upload files from any device's web browser directly to your PC's Downloads folder.
    * Download files from your PC's Downloads folder to any device via the web interface.
    * Connect easily via URL or QR code.
* **Clipboard Sync (PC-to-Device):** 📋➡️📱
    * Automatically pushes text copied on the PC to connected web clients or the companion app.
    * (Requires Local File Share server to be running).
* **System Tray Integration:** Runs discreetly in the background via the system tray icon.
* **Settings:** ⚙️
    * Option to start automatically on Windows startup.
    * Selectable application visual style (Theme).

## Installation ⬇️

* **[Download Installer (v1.0.0)](https://github.com/DoonMad/TowerToys/releases/latest)**

## Companion App 📱

For the best clipboard sync experience (especially reliable background sync), install the optional Android companion app.

* **[TowerToys Companion Repository & APK Download](https://github.com/DoonMad/TowerToys-Companion/releases/tag/v1.0.0)**

## Building 🔨

* Requires Qt 6 (tested with 6.9.3 MinGW) and CMake.
* Requires the `qrcodegen` and `QHotkey` libraries (included in the `lib/` directory).
* Open `CMakeLists.txt` in Qt Creator and build using a Desktop Kit.

## How to Use 🤔

1.  Install and run the application. It will appear in the system tray.
2.  Click the tray icon to open the main window.
3.  Explore modules using the left-side navigation.
4.  **For File Share / Clipboard Sync:** 📁↔️📋
    * Go to 'Local File Share & Sync Server'.
    * Click "Start Server".
    * On another device (phone, tablet, laptop) on the same Wi-Fi:
        * Open a web browser and go to the displayed URL or scan the QR code for basic file transfer and clipboard viewing.
        * OR: Install and open the Companion App and scan the QR code for better background clipboard sync.

## Known Issues (v1.0) ⚠️

* Phone-to-PC clipboard sync requires the companion app to be a system app (which is not possible) and is not yet implemented.
* `Type Keystroke` action currently only logs, doesn't simulate keys globally.
* **Console-based commands in `Run Command` (e.g., `ipconfig`) will currently output to the console window where TowerToys was launched (e.g., Qt Creator's Application Output) instead of a new window.**