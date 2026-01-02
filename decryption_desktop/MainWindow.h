/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QTextEdit>
#include <QNetworkAccessManager>
#include <QProgressBar>
#include <QPushButton>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onRegisterClicked();

    void browseLockedFile();
    void browsePrivateKey();
    void unlockSessionKey();
    void browseEncFolder();
    void browseAesKey();
    void browseDecOutput();
    
    void startDecryption();

private:
    void setupUi();
    void logOutput(const QString &msg);
    void registerMachineWithServer(const QString &url);

    QNetworkAccessManager *m_netManager;
    QTextEdit *m_logConsole;

    QLineEdit *m_lockedKeyInput;
    QLineEdit *m_privKeyInput;
    
    QLineEdit *m_encFolderInput;
    QLineEdit *m_aesKeyInput;
    QLineEdit *m_decOutputInput;
    QPushButton *m_btnDecrypt;

    QProgressBar *m_progressBar;

};  

#endif
