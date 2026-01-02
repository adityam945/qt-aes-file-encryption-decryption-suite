/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include <QApplication>
#include <QMessageBox>
#include "MainWindow.h"
#include "WebManager.h"

int main(int argc, char *argv[])
{
    // 1. Initialize as a GUI app (Required for UI to work later)
    QApplication a(argc, argv);

    // 2. Create the Window (But DO NOT show it yet)
    // Pass 'false' or a flag if you want to initialize it differently
    MainWindow w; 
    
    // Optional: Set a property to keep the app running even if the window closes
    // so the server doesn't die when you close the UI.
    a.setQuitOnLastWindowClosed(false); 

    // 3. Start the Server
    WebManager server;
    server.start(8080);

    // 4. Connect Server Signal -> Window Slot
    // When server says "requestShowUi", we call w.show()
    QObject::connect(&server, &WebManager::requestShowUi, [&w](){
        // Ensure UI updates happen on the main thread (Safety)
        w.show();
        w.raise(); // Bring to front
        w.activateWindow();
    });

    // 5. Run Event Loop (Server runs in background, UI is hidden)
    return a.exec();
}