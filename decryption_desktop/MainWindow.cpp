/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QDateTime>
#include <QDebug>
#include <QDirIterator>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <sodium.h>
#include <QThread>

// Backend Includes
#include "AES_File_Decryption.h"
#include "RSA_Key_Decryption.h"
#include "DecryptionWorker.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    m_netManager = new QNetworkAccessManager(this);
    if (sodium_init() < 0) {
        QMessageBox::critical(this, "Error", "Libsodium failed to initialize.");
    }
    setupUi();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    QHBoxLayout *topLayout = new QHBoxLayout;
    QLabel *title = new QLabel("Decryption Suite");
    title->setStyleSheet("font-size: 16px; font-weight: bold;");
    
    QPushButton *btnRegister = new QPushButton("Register This Machine");
    btnRegister->setStyleSheet("background-color: #673AB7; color: white; font-weight: bold; padding: 6px;");
    
    topLayout->addWidget(title);
    topLayout->addStretch();
    topLayout->addWidget(btnRegister);

    mainLayout->addLayout(topLayout);

    QTabWidget *tabWidget = new QTabWidget;

    QWidget *rsaTab = new QWidget;
    QVBoxLayout *rsaLayout = new QVBoxLayout(rsaTab);
    QHBoxLayout *h1 = new QHBoxLayout;
    m_lockedKeyInput = new QLineEdit;
    m_lockedKeyInput->setPlaceholderText("Select session_key.txt.rsa_locked...");
    QPushButton *btnBrowseLocked = new QPushButton("Browse");
    h1->addWidget(new QLabel("Locked Key File:"));
    h1->addWidget(m_lockedKeyInput);
    h1->addWidget(btnBrowseLocked);

    // qDebug() << __LINE__ << endl;

    QHBoxLayout *h2 = new QHBoxLayout;
    m_privKeyInput = new QLineEdit;
    m_privKeyInput->setPlaceholderText("Select decryption_machine.private_key...");
    QPushButton *btnBrowsePriv = new QPushButton("Browse");
    h2->addWidget(new QLabel("Private Key:    "));
    h2->addWidget(m_privKeyInput);
    h2->addWidget(btnBrowsePriv);

    // qDebug() << __LINE__ << endl;
    QPushButton *btnUnlock = new QPushButton("Unlock AES Key");
    btnUnlock->setStyleSheet("background-color: #2196F3; color: white; padding: 8px; font-weight: bold;");

    rsaLayout->addLayout(h1);
    rsaLayout->addLayout(h2);
    rsaLayout->addWidget(btnUnlock);
    rsaLayout->addStretch();
    tabWidget->addTab(rsaTab, "1. Unlock Key (RSA)");

    // TAB AES File Decryption
    QWidget *aesTab = new QWidget;
    QVBoxLayout *aesLayout = new QVBoxLayout(aesTab);
    QHBoxLayout *a1 = new QHBoxLayout;
    m_encFolderInput = new QLineEdit;
    m_encFolderInput->setPlaceholderText("Select the 'rsa_ready' folder...");
    // qDebug() << __LINE__ << endl;

    QPushButton *btnBrowseEnc = new QPushButton("Browse");
    a1->addWidget(new QLabel("Encrypted Folder:"));
    a1->addWidget(m_encFolderInput);
    a1->addWidget(btnBrowseEnc);

    QHBoxLayout *a2 = new QHBoxLayout;
    m_aesKeyInput = new QLineEdit;
    m_aesKeyInput->setPlaceholderText("Select the UNLOCKED session_key.txt...");
    QPushButton *btnBrowseAesKey = new QPushButton("Browse");
    a2->addWidget(new QLabel("Unlocked AES Key:"));
    a2->addWidget(m_aesKeyInput);
    a2->addWidget(btnBrowseAesKey);

    QHBoxLayout *a3 = new QHBoxLayout;
    m_decOutputInput = new QLineEdit;
    m_decOutputInput->setPlaceholderText("Select where to save decrypted files...");
    QPushButton *btnBrowseDecOut = new QPushButton("Browse");

    a3->addWidget(new QLabel("Output Folder:   "));
    a3->addWidget(m_decOutputInput);
    a3->addWidget(btnBrowseDecOut);

    m_progressBar = new QProgressBar();
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(true);
    m_progressBar->setStyleSheet("QProgressBar { border: 1px solid grey; border-radius: 4px; text-align: center; } "
                                 "QProgressBar::chunk { background-color: #2196F3; }"); // Blue for decrypt

    m_btnDecrypt = new QPushButton("Start Full Decryption");
    m_btnDecrypt->setStyleSheet("background-color: #4CAF50; color: white; padding: 10px; font-weight: bold;");

    aesLayout->addLayout(a1);
    aesLayout->addLayout(a2);
    aesLayout->addLayout(a3);
    aesLayout->addWidget(m_progressBar);
    aesLayout->addWidget(m_btnDecrypt);
    aesLayout->addStretch();
    tabWidget->addTab(aesTab, "2. Decrypt Files (AES)");

    mainLayout->addWidget(tabWidget);

    m_logConsole = new QTextEdit;
    m_logConsole->setReadOnly(true);
    m_logConsole->setStyleSheet("background-color: #222; color: #0f0;");
    mainLayout->addWidget(new QLabel("Logs:"));
    mainLayout->addWidget(m_logConsole);
    // --- CONNECTIONS ---
    connect(btnRegister, &QPushButton::clicked, this, &MainWindow::onRegisterClicked);
    connect(btnBrowseLocked, &QPushButton::clicked, this, &MainWindow::browseLockedFile);
    connect(btnBrowsePriv, &QPushButton::clicked, this, &MainWindow::browsePrivateKey);
    
    connect(btnUnlock, &QPushButton::clicked, this, &MainWindow::unlockSessionKey);
    
    connect(btnBrowseEnc, &QPushButton::clicked, this, &MainWindow::browseEncFolder);
    connect(btnBrowseAesKey, &QPushButton::clicked, this, &MainWindow::browseAesKey);
    connect(btnBrowseDecOut, &QPushButton::clicked, this, &MainWindow::browseDecOutput);
    
    connect(m_btnDecrypt, &QPushButton::clicked, this, &MainWindow::startDecryption);

    resize(700, 550);
    setWindowTitle("Decryption Client");
}

void MainWindow::logOutput(const QString &msg) {
    QString time = QDateTime::currentDateTime().toString("[HH:mm:ss] ");
    m_logConsole->append(time + msg);
}

// REGISTRATION LOGIC
void MainWindow::onRegisterClicked() {
    bool ok;
    QString text = QInputDialog::getText(this, "Register Machine",
                                         "Enter Encryption Server URL:", QLineEdit::Normal,
                                         "http://localhost:8080", &ok);
    if (ok && !text.isEmpty()) {
        registerMachineWithServer(text);
    }
}

void MainWindow::registerMachineWithServer(const QString &url) {
    // 1. Generate Keys (Curve25519)
    unsigned char pk[crypto_box_PUBLICKEYBYTES];
    unsigned char sk[crypto_box_SECRETKEYBYTES];
    crypto_box_keypair(pk, sk);

    // 2. Save Private Key Locally
    QString privPath = "decryption_machine.private_key";
    QFile privFile(privPath);
    if (privFile.open(QIODevice::WriteOnly)) {
        privFile.write(reinterpret_cast<const char*>(sk), sizeof(sk));
        privFile.close();
        logOutput("New Private Key saved to: " + QFileInfo(privPath).absoluteFilePath());
        m_privKeyInput->setText(QFileInfo(privPath).absoluteFilePath());
    }

    // 3. Prepare JSON for Server
    QJsonObject json;
    
    // Generate simple ID
    QString machineId = QString(QCryptographicHash::hash(QByteArray((char*)pk, sizeof(pk)), QCryptographicHash::Sha256).toHex());
    QString machineName = "Decryption-Client-" + machineId.left(6);
    QString pubKeyHex = QByteArray(reinterpret_cast<const char*>(pk), sizeof(pk)).toHex();

    json["machine_id"] = machineId;
    json["machine_name"] = machineName;
    json["public_key"] = pubKeyHex;

    // 4. Send POST
    QNetworkRequest request(QUrl(url + "/register_machine"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_netManager->post(request, QJsonDocument(json).toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply, machineName](){
        if (reply->error() == QNetworkReply::NoError) {
            logOutput("Registration SUCCESS!");
            logOutput("Registered as: " + machineName);
            QMessageBox::information(this, "Success", "Machine Registered Successfully!\nName: " + machineName);
        } else {
            logOutput("Registration FAILED: " + reply->errorString());
            QMessageBox::critical(this, "Error", "Could not connect to server.\n" + reply->errorString());
        }
        reply->deleteLater();
    });
}

// TAB 1: RSA LOGIC
void MainWindow::browseLockedFile() {
    QString f = QFileDialog::getOpenFileName(this, "Select Locked Key", "", "Locked Files (*.rsa_locked)");
    if (!f.isEmpty()) m_lockedKeyInput->setText(f);
}

void MainWindow::browsePrivateKey() {
    QString f = QFileDialog::getOpenFileName(this, "Select Private Key", "", "Key Files (*.private_key *.key)");
    if (!f.isEmpty()) m_privKeyInput->setText(f);
}

void MainWindow::unlockSessionKey() {
    QString lockedPath = m_lockedKeyInput->text();
    QString privPath = m_privKeyInput->text();

    if (lockedPath.isEmpty() || privPath.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select both Locked Key and Private Key.");
        return;
    }

    logOutput("Attempting to unlock AES key...");
    std::vector<unsigned char> rawKey = RSAKeyDecryption::unwrapKey(lockedPath, privPath);

    if (rawKey.empty()) {
        logOutput("Error: Decryption failed. Wrong Private Key?");
        QMessageBox::critical(this, "Error", "Failed to unlock key.");
        return;
    }

    // Save the UNLOCKED key to a file so it can be used in Tab 2
    QString outPath = lockedPath; 
    outPath.replace(".rsa_locked", ".txt"); // session_key.txt
    
    QFile outFile(outPath);
    if (outFile.open(QIODevice::WriteOnly)) {
        // Save as Hex for visibility/consistency
        outFile.write(QByteArray(reinterpret_cast<const char*>(rawKey.data()), rawKey.size()).toHex());
        outFile.close();
        
        logOutput("Success! Key unlocked.");
        logOutput("Saved to: " + outPath);
        
        // Auto-fill Tab 2
        m_aesKeyInput->setText(outPath);
        m_encFolderInput->setText(QFileInfo(lockedPath).absolutePath());
    }
}

// TAB 2: AES LOGIC
void MainWindow::browseEncFolder() {
    QString d = QFileDialog::getExistingDirectory(this, "Select Encrypted Folder");
    if (!d.isEmpty()) m_encFolderInput->setText(d);
}

void MainWindow::browseAesKey() {
    QString f = QFileDialog::getOpenFileName(this, "Select Unlocked AES Key", "", "Text Files (*.txt)");
    if (!f.isEmpty()) m_aesKeyInput->setText(f);
}

void MainWindow::browseDecOutput() {
    QString d = QFileDialog::getExistingDirectory(this, "Select Output Folder");
    if (!d.isEmpty()) m_decOutputInput->setText(d);
}


void MainWindow::startDecryption() {
    QString encFolder = m_encFolderInput->text();
    QString keyPath = m_aesKeyInput->text();
    QString outFolder = m_decOutputInput->text();

    if (encFolder.isEmpty() || keyPath.isEmpty() || outFolder.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields.");
        return;
    }

    // 1. Load AES Key (Hex -> Binary)
    QFile keyFile(keyPath);
    if (!keyFile.open(QIODevice::ReadOnly)) {
        logOutput("Error: Cannot open key file.");
        return;
    }
    QByteArray hexKey = keyFile.readAll().trimmed();
    keyFile.close();
    
    QByteArray binKey = QByteArray::fromHex(hexKey);
    std::vector<unsigned char> rawKey(binKey.begin(), binKey.end());

    if (rawKey.size() != 32) {
        QMessageBox::warning(this, "Error", "Invalid Key Size. Must be 32 bytes.");
        return;
    }

    // 2. BLOCK UI (Disable Input)
    m_btnDecrypt->setEnabled(false);
    m_btnDecrypt->setText("Decrypting... Please Wait");
    m_progressBar->setValue(0);

    // 3. Setup Thread
    QThread *thread = new QThread;
    DecryptionWorker *worker = new DecryptionWorker(encFolder, outFolder, rawKey);
    worker->moveToThread(thread);

    // 4. Connect Signals
    connect(thread, &QThread::started, worker, &DecryptionWorker::process);
    
    // Update Progress
    connect(worker, &DecryptionWorker::progressChanged, m_progressBar, &QProgressBar::setValue);
    
    // Log Errors
    connect(worker, &DecryptionWorker::errorOccurred, this, [this](QString msg){
        logOutput("Worker Error: " + msg);
    });

    // Cleanup & Completion
    connect(worker, &DecryptionWorker::finished, thread, &QThread::quit);
    connect(worker, &DecryptionWorker::finished, worker, &DecryptionWorker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    // UI Unblock on Finish
    connect(worker, &DecryptionWorker::finished, this, [this, outFolder](){
        m_progressBar->setValue(100);
        m_btnDecrypt->setEnabled(true);
        m_btnDecrypt->setText("Start Full Decryption");
        
        logOutput("Decryption Process Finished.");
        QMessageBox::information(this, "Success", "Decryption Complete!\nFolder: " + outFolder);
    });

    // 5. Start
    logOutput("Starting decryption thread...");
    thread->start();
}
