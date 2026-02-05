# TowerToys

A modular desktop productivity utility for Windows inspired by Microsoft PowerToys, built with **C++ and Qt**.

---

## Overview

TowerToys is a system-level productivity toolkit designed for power users.  
It runs in the background via the system tray and provides multiple independent utilities through a single unified interface.

Core principles:
- Modular design
- Background-first execution
- Local-first communication (no cloud dependency)

---

## Features (v1.0)

### 🧩 Modular Architecture
- PowerToys-style modular system
- Utilities can be enabled/disabled independently
- New modules can be added with minimal coupling

---

### ⚡ Macro Manager
- Create macros consisting of multiple actions:
  - Open Application
  - Open URL
  - Type Keystroke *(partial support)*
  - Open VS Code Folder
  - Open Folder
  - Run Command
- Assign global hotkeys (shortcuts) to trigger macros
- Edit existing macros and actions
- Macros persist across application restarts

---

### 🌐 Local File Share
- Start a local web server accessible over Wi-Fi
- Upload files from any device’s browser to the PC’s Downloads folder
- Download files from the PC to other devices
- Connect via URL or QR code

---

### 📋➡️📱 Clipboard Sync (PC → Device)
- Automatically pushes text copied on the PC to connected devices
- Works via:
  - Web clients
  - Android companion app
- Requires Local File Share server to be running

---

### ⚙️ System & Settings
- Runs discreetly via system tray
- Option to start automatically on Windows startup
- Selectable visual theme (Light / Dark)

---

## 📱 Companion App (Android)

For a better clipboard sync experience, especially reliable background syncing, an optional Android companion app is available.

- Built with **Kotlin**
- Provides enhanced clipboard sync support

👉 Companion App Repository & APK:  
https://github.com/DoonMad/TowerToys-Companion

---

## 🌐 Website

Product website and documentation:

👉 https://github.com/DoonMad/TowerToys-Website

---

## 🛠 Tech Stack

- **Desktop:** C++, Qt 6 (Widgets), CMake  
- **Mobile:** Kotlin, Android  
- **Website:** React (Vite), Tailwind CSS  

---

## Installation ⬇️

### 🖥 Desktop Application (Windows)
Download the latest Windows installer:

- **[Download TowerToys Desktop (v1.0.0)](https://github.com/DoonMad/TowerToys/releases/latest)**

---

### 📱 Companion App (Android)
For clipboard sync, install the Android companion app:

- **[TowerToys Companion – APK Download](https://github.com/DoonMad/towertoys-companion/releases/latest)**



---

## Building 🔨

Requirements:
- Qt 6 (tested with 6.9.3 MinGW)
- CMake
- `qrcodegen` and `QHotkey` libraries (included in `lib/`)

Steps:
1. Open `CMakeLists.txt` in Qt Creator
2. Build using a Desktop Kit

---

## How to Use 🤔

1. Install and run TowerToys (appears in system tray)
2. Click tray icon to open the main window
3. Navigate modules using the left panel

### File Share / Clipboard Sync
1. Go to **Local File Share & Sync Server**
2. Click **Start Server**
3. On another device (same Wi-Fi):
   - Open the displayed URL or scan QR code  
   **OR**
   - Use the Companion App for clipboard sync

---

## Known Issues (v1.0) ⚠️

- Phone → PC clipboard sync is not implemented (Android system app limitation)
- Type Keystroke action currently logs actions but does not simulate global keystrokes
- Console-based commands run in the launch console (e.g., Qt Creator output) instead of a new window

---

