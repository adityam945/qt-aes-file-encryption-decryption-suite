/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include "MainWindow.h"
// Imprts
#include <QDebug>
#include <QDateTime>
#include <QThread>
#include <QUrl>
#include <QProgressBar>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QDirIterator>
#include <QHeaderView>
#include <QDesktopServices>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <QSqlError>
#include <QSqlQuery>

// .h Includes
#include "EncryptionWorker.h"
#include "AES_File_encryption.h"
#include "AES_Key_genration.h"
#include "RSA_Key_Encryption.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    m_db = new DBManager();
    desktopUI();
    
    if (!m_db->isOpen()) {
        consoleLogUI("Error: Database failed to open.");
    } else {
        consoleLogUI("Database connected.");
        // qDebug() << __LINE__ << endl;
        refreshMachineList();
        refreshAuditTab();
    }
}

MainWindow::~MainWindow() {
    delete m_db;
}

void MainWindow::desktopUI() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QTabWidget *tabWidget = new QTabWidget;
    // qDebug() << __LINE__ << endl;

    QWidget *aesTab = new QWidget;
    QVBoxLayout *aesLayout = new QVBoxLayout(aesTab);
    QHBoxLayout *srcLayout = new QHBoxLayout;
    m_srcDirInput = new QLineEdit;
    m_srcDirInput->setPlaceholderText("Select Folder to Encrypt...");
    // qDebug() << __LINE__ << endl;
    QPushButton *btnBrowseSrc = new QPushButton("Browse");
    srcLayout->addWidget(new QLabel("Source Folder:"));
    srcLayout->addWidget(m_srcDirInput);
    srcLayout->addWidget(btnBrowseSrc);

    // qDebug() << __LINE__ << endl;
    QHBoxLayout *outLayout = new QHBoxLayout;
    m_outDirInput = new QLineEdit;
    m_outDirInput->setPlaceholderText("Select Output Destination...");
    // qDebug() << __LINE__ << endl;
    QPushButton *btnBrowseOut = new QPushButton("Browse");
    outLayout->addWidget(new QLabel("Output Folder:"));
    outLayout->addWidget(m_outDirInput);
    outLayout->addWidget(btnBrowseOut);


    m_progressBar = new QProgressBar();
    m_progressBar->setValue(0);
    // qDebug() << __LINE__ << endl;
    m_progressBar->setTextVisible(true);
    m_progressBar->setAlignment(Qt::AlignCenter);
    m_progressBar->setStyleSheet("QProgressBar { border: 1px solid grey; border-radius: 4px; text-align: center; } "
                                 "QProgressBar::chunk { background-color: #4CAF50; }");


                                 m_btnEncrypt = new QPushButton("Start AES Encryption"); 
    m_btnEncrypt->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 10px;");
    // qDebug() << __LINE__ << endl;

    aesLayout->addLayout(srcLayout);
    // qDebug() << __LINE__ << endl;
    aesLayout->addLayout(outLayout);
    aesLayout->addWidget(m_progressBar); // Add Progress Bar
    aesLayout->addWidget(m_btnEncrypt);
    // qDebug() << __LINE__ << endl;
    aesLayout->addStretch();
    
    tabWidget->addTab(aesTab, "1. AES Encryption");

    // TAB 2
    QWidget *rsaTab = new QWidget;
    // qDebug() << __LINE__ << endl;
    QVBoxLayout *rsaLayout = new QVBoxLayout(rsaTab);
    QHBoxLayout *keyLayout = new QHBoxLayout;
    // qDebug() << __LINE__ << endl;
    m_keyFileInput = new QLineEdit;
    QPushButton *btnBrowseKey = new QPushButton("Browse");
    keyLayout->addWidget(new QLabel("AES Key File:"));
    keyLayout->addWidget(m_keyFileInput);
    keyLayout->addWidget(btnBrowseKey);
    // qDebug() << __LINE__ << endl;
    QHBoxLayout *rsaDestLayout = new QHBoxLayout;
    m_rsaDestInput = new QLineEdit;
    // qDebug() << __LINE__ << endl;
    QPushButton *btnBrowseRsaDest = new QPushButton("Browse");
    rsaDestLayout->addWidget(new QLabel("Save To:     "));
    // qDebug() << __LINE__ << endl;
    rsaDestLayout->addWidget(m_rsaDestInput);
    rsaDestLayout->addWidget(btnBrowseRsaDest);

    QHBoxLayout *machineLayout = new QHBoxLayout;
    m_machineCombo = new QComboBox;
    // qDebug() << __LINE__ << endl;
    QPushButton *btnRefresh = new QPushButton("Refresh List");
    machineLayout->addWidget(new QLabel("Lock for:    "));
    // qDebug() << __LINE__ << endl;
    machineLayout->addWidget(m_machineCombo, 1);
    machineLayout->addWidget(btnRefresh);

    QPushButton *btnWrap = new QPushButton("RSA Lock Key");
    btnWrap->setStyleSheet("background-color: #2196F3; color: white; font-weight: bold; padding: 10px;");
    rsaLayout->addLayout(keyLayout);
    rsaLayout->addLayout(rsaDestLayout);
    // qDebug() << __LINE__ << endl;
    rsaLayout->addLayout(machineLayout);
    rsaLayout->addWidget(btnWrap);
    rsaLayout->addStretch();
    tabWidget->addTab(rsaTab, "2. RSA Key Wrapping");

    

    QWidget *auditTab = new QWidget;
    QVBoxLayout *auditLayout = new QVBoxLayout(auditTab);
    m_auditTable = new QTableWidget();
    m_auditTable->setColumnCount(4);
    QStringList headers; headers << "Source" << "Machine" << "Time" << "Output";
    m_auditTable->setHorizontalHeaderLabels(headers);
    // qDebug() << __LINE__ << endl;
    m_auditTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_auditTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_auditTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QHBoxLayout *auditBtnLayout = new QHBoxLayout;
    // qDebug() << __LINE__ << endl;
    QPushButton *btnRefreshAudit = new QPushButton("Refresh List");
    QPushButton *btnRemoveRecord = new QPushButton("Remove Selected");
    btnRemoveRecord->setStyleSheet("background-color: #f44336; color: white;");

    auditBtnLayout->addWidget(btnRefreshAudit);
    auditBtnLayout->addStretch();
    auditBtnLayout->addWidget(btnRemoveRecord);
    auditLayout->addWidget(m_auditTable);
    auditLayout->addLayout(auditBtnLayout);
    tabWidget->addTab(auditTab, "3. Audit Logs");

    // Main 
    m_logConsole = new QTextEdit;
    // qDebug() << __LINE__ << endl;
    m_logConsole->setReadOnly(true);
    m_logConsole->setStyleSheet("background-color: #222; color: #0f0; font-family: Monospace;");
    mainLayout->addWidget(tabWidget);
    // qDebug() << __LINE__ << endl;
    mainLayout->addWidget(new QLabel("Logs:"));
    mainLayout->addWidget(m_logConsole);
    
    // Connections
    connect(btnBrowseSrc, &QPushButton::clicked, this, &MainWindow::browseSourceDirAES);
    connect(btnBrowseOut, &QPushButton::clicked, this, &MainWindow::browseOutputDirAES);
    connect(m_btnEncrypt, &QPushButton::clicked, this, &MainWindow::startAesEncryption);

    connect(btnBrowseKey, &QPushButton::clicked, this, &MainWindow::browseAESKeyFile);
    connect(btnBrowseRsaDest, &QPushButton::clicked, this, &MainWindow::browseRsaDestDir);
    connect(btnRefresh, &QPushButton::clicked, this, &MainWindow::refreshMachineList);
    connect(btnWrap, &QPushButton::clicked, this, &MainWindow::startRsaWrapping);

    connect(btnRefreshAudit, &QPushButton::clicked, this, &MainWindow::refreshAuditTab);
    connect(btnRemoveRecord, &QPushButton::clicked, this, &MainWindow::removeAuditRecord);

    resize(800, 600);
    setWindowTitle("Encrpytion Destop Suite-");
}

void MainWindow::consoleLogUI(const QString &msg) {
    // qDebug() << __LINE__ << endl;
    QString time = QDateTime::currentDateTime().toString("[HH:mm:ss] ");
    m_logConsole->append(time + msg);
}

void MainWindow::browseSourceDirAES() {
    // qDebug() << __LINE__ << endl;
    QString dir = QFileDialog::getExistingDirectory(this, "Select Source Folder");
    if (!dir.isEmpty()) m_srcDirInput->setText(dir);
}

void MainWindow::browseOutputDirAES() {
    // qDebug() << __LINE__ << endl;
    QString dir = QFileDialog::getExistingDirectory(this, "Select Output Folder");
    if (!dir.isEmpty()) m_outDirInput->setText(dir);
}

void MainWindow::browseAESKeyFile() {
    // qDebug() << __LINE__ << endl;
    QString file = QFileDialog::getOpenFileName(this, "Select AES Key", QString(), "Text Files (*.txt)");
    if (!file.isEmpty()) m_keyFileInput->setText(file);
}

void MainWindow::browseRsaDestDir() {
    // qDebug() << __LINE__ << endl;
    QString dir = QFileDialog::getExistingDirectory(this, "Select Destination for RSA Folder");
    if (!dir.isEmpty()) m_rsaDestInput->setText(dir);
}


void MainWindow::startAesEncryption() {
    QString src = m_srcDirInput->text();
    QString out = m_outDirInput->text();

    if (src.isEmpty() || out.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please select both Source and Output folders.");
        return;
    }
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString finalEncFolder = out + "/encryption_" + timestamp;
    QDir().mkpath(finalEncFolder);

    // qDebug() << __LINE__ << endl;

    consoleLogUI("------------------------------------------------");
    consoleLogUI("Generating 256-bit AES Key...");
    
    std::vector<unsigned char> key;
    try {
        key = AESKeyGeneration::generate256BitKey();
    } catch (...) {
        consoleLogUI("Error: Libsodium failed to init.");
        return;
    }

    // qDebug() << __LINE__ << endl;

    QString keyPath = finalEncFolder + "/session_key.txt";
    QFile keyFile(keyPath);
    if (keyFile.open(QIODevice::WriteOnly)) {
        keyFile.write(QByteArray(reinterpret_cast<const char*>(key.data()), key.size()).toHex());
        keyFile.close();
    }

    m_btnEncrypt->setEnabled(false);
    m_progressBar->setValue(0);
    consoleLogUI("Starting encryption thread for: " + src);
    // qDebug() << __LINE__ << endl;

    QThread *thread = new QThread;
    EncryptionWorker *worker = new EncryptionWorker(src, finalEncFolder, key);
    

    worker->moveToThread(thread);

    // --- Connections ---
    
    // Start processing when thread starts
    connect(thread, &QThread::started, worker, &EncryptionWorker::process);
    
    // Update Progress Bar
    connect(worker, &EncryptionWorker::progressChanged, m_progressBar, &QProgressBar::setValue);
    
    // Log individual file completion (Optional - can be noisy for many files)
    // connect(worker, &EncryptionWorker::fileFinished, this, [](QString f){ qDebug() << "Encrypted:" << f; });

    // Handle Errors
    connect(worker, &EncryptionWorker::errorOccurred, this, [this](QString msg){
        consoleLogUI("Worker Error: " + msg);
    });

    // CLEANUP & COMPLETION Logic
    connect(worker, &EncryptionWorker::finished, thread, &QThread::quit);
    connect(worker, &EncryptionWorker::finished, worker, &EncryptionWorker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    // What happens when encryption is totally done:
    connect(worker, &EncryptionWorker::finished, this, [this, src, finalEncFolder, keyPath, timestamp](){
        
        // A. Log to Database
        if (m_db->logEncryptedFolder(src, finalEncFolder, keyPath, timestamp)) {
            consoleLogUI("Audit record saved to DB.");
        } else {
            consoleLogUI("Warning: Failed to save to DB.");
        }

        // B. Update UI
        m_progressBar->setValue(100);
        m_btnEncrypt->setEnabled(true);
        m_keyFileInput->setText(keyPath); // Auto-fill Tab 2

        // C. User Feedback
        consoleLogUI("Encryption Complete!");
        consoleLogUI("Output: " + finalEncFolder);
        QMessageBox::information(this, "Done", "Encryption Completed Successfully!\n\nFolder: " + finalEncFolder);
    });

    thread->start();
}



void MainWindow::refreshMachineList() {
    m_machineCombo->clear();
    m_machineCombo->addItem("<Select a machine from below>", "");
    if (!QSqlDatabase::database().isOpen()) {
        m_machineCombo->addItem("Error: DB Not Connected");
        return;
    }
    // qDebug() << __LINE__ << endl;
    QSqlQuery query;
    if (!query.exec("SELECT machine_id, machine_name, created_at FROM RSA_Machines")) {
        qCritical() << "SQL ERROR:" << query.lastError().text();
        return;
    }

    int count = 0;
    while (query.next()) {
        QString fullId = query.value("machine_id").toString();
        QString name = query.value("machine_name").toString();
        QString date = query.value("created_at").toString();

        if (name.isEmpty()) name = "Unknown Host";
        QString shortId = (fullId.length() > 5) ? fullId.right(5) : fullId;

        // qDebug() << __LINE__ << endl;


        QString displayText = QString("%1 (%2) [ID:...%3]")
                                .arg(name)
                                .arg(date)
                                .arg(shortId);

        // qDebug() << __LINE__ << endl;
        m_machineCombo->addItem(displayText, fullId);
        count++;
    }

    // qDebug() << __LINE__ << endl;

    if (count > 0) {
        m_machineCombo->setCurrentIndex(0);
        consoleLogUI(QString("Machine list refreshed. Loaded %1 machines.").arg(count));
    } else {
        m_machineCombo->clear();
        m_machineCombo->addItem("No machines registered yet", ""); // Empty data
        consoleLogUI("Machine list refreshed. Database is empty.");
    }
}

void MainWindow::startRsaWrapping() {
    QString keyPath = m_keyFileInput->text();
    QString destBaseDir = m_rsaDestInput->text();
    QString machineId = m_machineCombo->currentData().toString();
    QString machineLabel = m_machineCombo->currentText();

    // qDebug() << __LINE__ << endl;

    if (keyPath.isEmpty() || destBaseDir.isEmpty() || 
        machineId.isEmpty() || machineLabel == "<Select a machine from below>") {
        QMessageBox::warning(this, "Validation Error", 
                             "Please ensure you have selected:\n"
                             "- AES Key File\n"
                             "- Destination Folder\n"
                             "- Target Machine");
        return;
    }

    if (!QFile::exists(keyPath)) {
        QMessageBox::critical(this, "File Error", "The specified AES Key file does not exist:\n" + keyPath);
        return;
    }
    // qDebug() << __LINE__ << endl;
    QDir destDirCheck(destBaseDir);
    if (!destDirCheck.exists()) {
        QMessageBox::critical(this, "Folder Error", "The destination folder does not exist:\n" + destBaseDir);
        return;
    }

    consoleLogUI("------------------------------------------------");
    consoleLogUI("Starting RSA Lock for: " + machineLabel);

    QString pubKey = m_db->getMachinePublicKey(machineId);
    if (pubKey.isEmpty()) {
        QMessageBox::critical(this, "DB Error", "Public Key not found for this machine ID.");
        consoleLogUI("Error: No Public Key found in DB for ID: " + machineId);
        return;
    }


    QString outputRsaPath;

    if (!RSAKeyEncryption::wrapKey(keyPath, pubKey, outputRsaPath)) {
        consoleLogUI("Error: RSA Encryption process failed.");
        QMessageBox::critical(this, "Crypto Error", "Failed to encrypt the AES key.");
        return;
    }
    consoleLogUI("Key locked successfully (Temp): " + QFileInfo(outputRsaPath).fileName());

    // /
    
    QFileInfo keyInfo(keyPath);
    QDir sourceDir = keyInfo.absoluteDir();
    QString sourcePath = sourceDir.absolutePath();

    QString cleanId = machineId;
    cleanId.replace(QRegExp("[^a-zA-Z0-9_-]"), "_");
    
    QString readyFolderName = "rsa_ready_" + cleanId;
    QString readyFolderPath = destBaseDir + "/" + readyFolderName;
    
    // Create folder
    if (!QDir().mkpath(readyFolderPath)) {
        consoleLogUI("Error: Could not create distribution folder at: " + readyFolderPath);
        QMessageBox::critical(this, "FS Error", "Could not create folder:\n" + readyFolderPath);
        return;
    }
    consoleLogUI("Created distribution folder: " + readyFolderName);

    QDirIterator it(sourcePath, QDir::Files | QDir::NoDotAndDotDot);
    int filesCopied = 0;
    while (it.hasNext()) {
        QString currentFilePath = it.next();
        QString fileName = QFileInfo(currentFilePath).fileName();


        if (fileName == keyInfo.fileName()) continue;
        if (currentFilePath.startsWith(readyFolderPath)) continue;
        if (fileName.endsWith(".db")) continue;
        QString destPath = readyFolderPath + "/" + fileName;

        if (QFile::exists(destPath)) QFile::remove(destPath);

        if (QFile::copy(currentFilePath, destPath)) {
            filesCopied++;
        } else {
            consoleLogUI("Warning: Failed to copy: " + fileName);
        }
    }
    
   

    QString rsaKeyName = QFileInfo(outputRsaPath).fileName();
    QString rsaKeyDest = readyFolderPath + "/" + rsaKeyName;
    if (QFile::exists(rsaKeyDest)) QFile::remove(rsaKeyDest);
    
   
    
    if (!QFile::exists(rsaKeyDest)) {
       // qDebug() << __LINE__ << outputRsaPath << rsaKeyDest;
        QFile::copy(outputRsaPath, rsaKeyDest);
    }
    
    consoleLogUI(QString("Copied %1 files to distribution folder.").arg(filesCopied));

    int folderId = m_db->getFolderIdByEncPath(sourcePath);
    
    if (folderId != -1) {
        if (m_db->logRSAEncryption(folderId, machineId, readyFolderPath)) {
            consoleLogUI("Audit record saved to Database.");
        } else {
            consoleLogUI("Warning: Failed to write Audit Log to DB.");
        }
    } else {
        consoleLogUI("Warning: Parent folder not found in DB. Audit log skipped (Orphaned encryption).");
    }

    QMessageBox::information(this, "Success", 
                             "RSA Locking Complete!\n\n"
                             "Safe Distribution Folder:\n" + readyFolderPath + "\n\n"
                             "Contains: Encrypted Files + RSA Locked Key.\n"
                             "(Raw AES key was excluded)");
    
    refreshAuditTab(); 
    consoleLogUI("------------------------------------------------");
}

void MainWindow::refreshAuditTab() {
    m_auditTable->setRowCount(0);
    QJsonArray logs = m_db->getFullAuditLog();

    for (const auto &val : logs) {
        QJsonObject obj = val.toObject();
        int row = m_auditTable->rowCount();
        m_auditTable->insertRow(row);

        // Cols 0-2 (Standard Text)
        m_auditTable->setItem(row, 0, new QTableWidgetItem(obj["source"].toString()));
        m_auditTable->setItem(row, 1, new QTableWidgetItem(obj["machine"].toString()));
        m_auditTable->setItem(row, 2, new QTableWidgetItem(obj["time"].toString()));

        // --- NEW: Column 3 (Button) ---
        QWidget *btnWidget = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout(btnWidget);
        layout->setContentsMargins(2, 2, 2, 2); // Tight margins
        layout->setAlignment(Qt::AlignCenter);

        QPushButton *btnOpen = new QPushButton("Open Folder");
        btnOpen->setCursor(Qt::PointingHandCursor);
        
        QString path = obj["output"].toString();

        connect(btnOpen, &QPushButton::clicked, this, [this, path]() {
            openAuditFolder(path);
        });

        layout->addWidget(btnOpen);
        m_auditTable->setCellWidget(row, 3, btnWidget);
        m_auditTable->item(row, 0)->setData(Qt::UserRole, obj["id"].toInt());
    }
    consoleLogUI("Audit list refreshed.");
}



void MainWindow::removeAuditRecord() {
    int currentRow = m_auditTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Selection Error", "Please select a record to remove.");
        return;
    }

    int dbId = m_auditTable->item(currentRow, 0)->data(Qt::UserRole).toInt();
    QString machineName = m_auditTable->item(currentRow, 1)->text();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Deletion", 
                                  "Are you sure you want to delete the audit record for:\n" + machineName + 
                                  "\n\n(This only deletes the DB record, not the actual files on disk)",
                                  QMessageBox::Yes|QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        if (m_db->deleteAuditRecord(dbId)) {
            consoleLogUI("Record deleted successfully.");
            refreshAuditTab(); // Reload table
        } else {
            QMessageBox::critical(this, "Error", "Failed to delete record from database.");
        }
    }
}

// Helper function to open the folder
void MainWindow::openAuditFolder(const QString &path) {
    if (path.isEmpty()) return;

    QUrl url = QUrl::fromLocalFile(path);
    
    if (QDir(path).exists()) {
        QDesktopServices::openUrl(url);
        consoleLogUI("Opening folder: " + path);
    } else {
        QMessageBox::warning(this, "Error", "Folder does not exist:\n" + path);
        consoleLogUI("Error: Folder missing -> " + path);
    }
}