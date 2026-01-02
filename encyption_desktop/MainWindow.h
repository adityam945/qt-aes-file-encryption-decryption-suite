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
    // qDebug() << __LINE__ << endl;
#include <QComboBox>
#include <QTextEdit>
#include <QTableWidget>
#include <QProgressBar>
#include<QPushButton>

#include "DBmanager.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void browseSourceDirAES();
    void browseOutputDirAES();

    void browseAESKeyFile();
    void browseRsaDestDir();

    void startAesEncryption();

    void refreshMachineList();
    void startRsaWrapping();

    void refreshAuditTab();
    void removeAuditRecord();

    void openAuditFolder(const QString &path);

private:
    DBManager *m_db;

    void desktopUI();
    void consoleLogUI(const QString &msg);

    QTextEdit *m_logConsole;
    QLineEdit *m_srcDirInput;
    QLineEdit *m_outDirInput;


    QLineEdit *m_keyFileInput;
    QLineEdit *m_rsaDestInput;
    QComboBox *m_machineCombo;
    QProgressBar *m_progressBar;
    QPushButton *m_btnEncrypt;

    QTableWidget *m_auditTable;

};

#endif
