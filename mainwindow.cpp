#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QMessageBox"
#include "QPushButton"
#include "AppController/appcontroller.h"
#include "modulemanager.h"
#include "welcomewidget.h"
#include "MacroManager/macromanagerwidget.h"
#include "FileShare/filesharewidget.h"
#include "ClipboardSync/clipboardsyncwidget.h"
#include "settingswidget.h"

#include <QSharedPointer>
#include <QVBoxLayout>
#include <QSplitter>
#include <QStackedWidget>
#include <QLabel>
#include <QHash>
#include <QString>
#include <QFrame>
#include <QIcon>
#include <QSize>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , controller(nullptr)
    , macroManager(nullptr)
    , shareServer(nullptr)
    , fileShareManager(nullptr)
    , clipboardSyncManager(nullptr)
    , moduleManager(nullptr)
    , stackWidget(nullptr)
{
    ui->setupUi(this);
    setWindowTitle("TowerToys");

    // Create and initialize core components
    controller = new AppController(this, this);
    controller->start();

    // Get manager pointers from controller
    macroManager = controller->getMacroManager();
    shareServer = controller->getShareServer();
    fileShareManager = controller->getFileShareManager();
    clipboardSyncManager = controller->getClipboardSyncManager();

    // Initialize ModuleManager and load modules
    moduleManager = new ModuleManager(this);
    if (moduleManager) {
        moduleInfoMap = moduleManager->loadModules();
    } else {
        qWarning() << "ModuleManager failed to initialize!";
    }

    setupDynamicUI();
}

// Destructor
MainWindow::~MainWindow()
{
    delete ui;
}

// Helper to update button selection state
void MainWindow::updateButtonSelection(QPushButton* selectedButton)
{
    // Iterate through the values (QPushButton*) in the map
    for (QPushButton* btn : moduleButtons) {
        if (btn) {
            btn->setChecked(btn == selectedButton);
        }
    }
}

// Function to set up the dynamic UI layout
void MainWindow::setupDynamicUI() {
    QWidget* centralWidgetContainer = new QWidget(this);
    QVBoxLayout* wrapperLayout = new QVBoxLayout(centralWidgetContainer);
    wrapperLayout->setContentsMargins(0, 0, 0, 0);

    QSplitter* mainSplitter = new QSplitter(Qt::Horizontal, centralWidgetContainer);
    wrapperLayout->addWidget(mainSplitter);

    // 2. Setup Left Sidebar Panel
    QFrame* sidePanel = new QFrame(mainSplitter);
    sidePanel->setObjectName("sidePanel");
    sidePanel->setMinimumWidth(200);
    QVBoxLayout *sideLayout = new QVBoxLayout(sidePanel); // Layout for sidePanel
    sideLayout->setContentsMargins(5,5,5,5);
    sideLayout->setSpacing(5);

    // Stacked Widget Setup
    stackWidget = new QStackedWidget(centralWidgetContainer); // Parent it and assign to member

    // Add Welcome Page
    QWidget *welcomePage = new WelcomeWidget(stackWidget);
    stackWidget->addWidget(welcomePage);

    // Add Settings Widget to Stack
    SettingsWidget *settings = new SettingsWidget(stackWidget);
    stackWidget->addWidget(settings);

    // Add Module Buttons
    QWidget* buttonContainer = new QWidget(sidePanel); // Parent container for buttons
    QVBoxLayout* buttonLayout = new QVBoxLayout(buttonContainer); // Layout for container
    buttonLayout->setContentsMargins(0,0,0,0);
    buttonLayout->setSpacing(5);

    moduleButtons.clear(); // Clearing map before filling

    // Loop through modules using const iterators
    for (auto it = moduleInfoMap.constBegin(); it != moduleInfoMap.constEnd(); ++it)
    {
        const QString& moduleKeyName = it.key(); // Use a different name to avoid confusion
        const ModuleInfo& moduleInfo = it.value();

        QPushButton* button = new QPushButton(moduleInfo.name, buttonContainer); // Parent button
        button->setCheckable(true); // Allow selection state

        // Set Icon
        if (!moduleInfo.iconPath.isEmpty()) {
            button->setIcon(QIcon(moduleInfo.iconPath));
            button->setIconSize(QSize(24, 24));
        }

        // Set Style and Size
        button->setFixedHeight(40);
        // setToolButtonStyle might depend on the specific style applied or require <QToolButton>
        button->setProperty("class", "sidebarButton");

        // Connect Button Click
        connect(button, &QPushButton::clicked, this, [this, moduleKeyName, button]() {
            onModuleButtonClicked(moduleKeyName); // Pass module name (key)
            updateButtonSelection(button);      // Update visual selection
        });

        // Store button pointer using module name (key)
        moduleButtons[moduleKeyName] = button;

        buttonLayout->addWidget(button); // Add button to the button layout
    }

    buttonLayout->addStretch(1); // Push buttons up within their container
    
    // Add Settings Button at the bottom
    QPushButton* settingsBtn = new QPushButton("Settings", buttonContainer);
    settingsBtn->setIcon(QIcon(":/resources/icons/settings.svg"));
    settingsBtn->setIconSize(QSize(24, 24));
    settingsBtn->setCheckable(true);
    settingsBtn->setFixedHeight(40);
    settingsBtn->setProperty("class", "sidebarButton");
    connect(settingsBtn, &QPushButton::clicked, this, [this, settings, settingsBtn]() {
        stackWidget->setCurrentWidget(settings);
        updateButtonSelection(settingsBtn);
    });
    // Add to map so updateButtonSelection handles it
    moduleButtons["Settings"] = settingsBtn;
    buttonLayout->addWidget(settingsBtn);

    sideLayout->addWidget(buttonContainer); // Add button container to main side layout
    sideLayout->setStretchFactor(buttonContainer, 1); // Allow button container to stretch

    // 7. Final Layout Assembly
    mainSplitter->addWidget(sidePanel);
    mainSplitter->addWidget(stackWidget);
    mainSplitter->setStretchFactor(1, 1); // Give stack widget horizontal stretch factor
    mainSplitter->setCollapsible(0, false); // Don't allow sidebar to fully collapse
    mainSplitter->setCollapsible(1, false);

    // 8. Set Central Widget
    setCentralWidget(centralWidgetContainer); // Apply the whole layout

    // Set initial state
    stackWidget->setCurrentWidget(welcomePage);
    updateButtonSelection(nullptr); // Deselect all buttons
}

// Slot called when a module button is clicked
void MainWindow::onModuleButtonClicked(const QString& moduleName)
{
    QWidget* widgetToShow = nullptr;

    if (loadedWidgets.contains(moduleName)){ // Use your loadedWidgets map name
        widgetToShow = loadedWidgets.value(moduleName);
    } else {
        widgetToShow = createWidgetForModule(moduleName);
        if (widgetToShow && stackWidget){ // Check stackWidget exists
            stackWidget->addWidget(widgetToShow);
            loadedWidgets.insert(moduleName, widgetToShow); // Use your loadedWidgets map name
        } else if (!widgetToShow) {
            qWarning() << "Failed to create widget for module:" << moduleName;
            return; // Don't try to switch if creation failed
        }
    }

    if (widgetToShow && stackWidget){
        stackWidget->setCurrentWidget(widgetToShow);
    } else {
        qWarning() << "Could not show widget for module (or stackWidget is null):" << moduleName;
    }
}

// Factory function to create module widgets
QWidget* MainWindow::createWidgetForModule(const QString& moduleName)
{
    // Ensure map contains the key before accessing
    if (!moduleInfoMap.contains(moduleName)) {
        qWarning() << "Module info not found for key:" << moduleName;
        return nullptr;
    }
    const ModuleInfo& info = moduleInfoMap.value(moduleName);

    // Parent new widgets to stackWidget for proper memory management and display
    if (info.widgetClass == "MacroManagerWidget") {
        return new MacroManagerWidget(macroManager, info, stackWidget); // Pass stackWidget as parent
    }
    if (info.widgetClass == "FileShareWidget") {
        return new FileShareWidget(shareServer, fileShareManager, info, stackWidget); // Pass stackWidget as parent
    }
    if (info.widgetClass == "ClipboardSyncWidget") {
        return new ClipboardSyncWidget(controller, clipboardSyncManager, info, stackWidget); // Pass stackWidget as parent
    }
    if (info.widgetClass == "SettingsWidget") {
        // This shouldn't normally be created here as it's static in the sidebar,
        // but handle it just in case it's listed in modules.json
        qWarning() << "SettingsWidget should not be created dynamically via modules.json";
        return new SettingsWidget(stackWidget); // Creating a placeholder instance if needed
    }
    if (info.widgetClass == "WelcomeWidget") {
        qWarning() << "WelcomeWidget should not be created dynamically via modules.json";
        return new WelcomeWidget(stackWidget); // Creating a placeholder instance if needed
    }


    qWarning() << "Unknown widget class specified in modules.json:" << info.widgetClass;
    // Creating a default placeholder if no match
    QWidget* placeholder = new QWidget(stackWidget);
    QVBoxLayout* layout = new QVBoxLayout(placeholder);
    QLabel* label = new QLabel(QString("Placeholder for %1").arg(info.name), placeholder);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
    return placeholder;
}
