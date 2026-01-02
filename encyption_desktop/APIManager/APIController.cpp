/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include "APIController.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QDirIterator>
#include <QDateTime>
#include <QDebug>
#include "AES_Key_genration.h"
#include "AES_File_encryption.h"
#include "RSA_Key_Encryption.h"

APIController::APIController(DBManager *db, QObject *parent) 
    : QObject(parent), m_db(db) {}

QByteArray APIController::processEncryptionRequest(const QByteArray &jsonBody) {
    // 1. Parse JSON Input
    QJsonDocument doc = QJsonDocument::fromJson(jsonBody);
    if (doc.isNull() || !doc.isObject()) {
        return "{\"status\": \"error\", \"message\": \"Invalid JSON format\"}";
    }

    QJsonObject obj = doc.object();
    QString sourceDir = obj["source_dir"].toString();
    QString outputBaseDir = obj["output_dir"].toString();

    // 2. Validate Paths
    QDir src(sourceDir);
    if (!src.exists() || sourceDir.isEmpty()) {
        return "{\"status\": \"error\", \"message\": \"Source directory does not exist\"}";
    }

    // 3. Setup Encryption Environment
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString finalEncFolder = outputBaseDir + "/encryption_" + timestamp;
    QDir().mkpath(finalEncFolder);
    
    // Generate Key
    std::vector<unsigned char> key;
    try {
        key = AESKeyGeneration::generate256BitKey();
    } catch (...) {
        return "{\"status\": \"error\", \"message\": \"Libsodium initialization failed\"}";
    }

    // Save Key File (Metadata)
    QString keyPath = finalEncFolder + "/session_key.txt";
    QFile keyFile(keyPath);
    if (keyFile.open(QIODevice::WriteOnly)) {
        keyFile.write(QByteArray(reinterpret_cast<const char*>(key.data()), key.size()).toHex());
        keyFile.close();
    }

    // 4. Run Encryption
    int successCount = 0;
    QDirIterator it(sourceDir, QDir::Files, QDirIterator::Subdirectories);
    
    qDebug() << "Processing Request for:" << sourceDir;
    
    while (it.hasNext()) {
        QString filePath = it.next();
        // Prevent recursive loop if output is inside source
        if (filePath.startsWith(finalEncFolder)) continue; 
        if (filePath.contains("/db/")) continue;

        if (AESFileEncryption::encryptFile(filePath, sourceDir, finalEncFolder, key)) {
            successCount++;
        }
    }

    // 5. Log to DB
    bool dbLog = m_db->logEncryptedFolder(sourceDir, finalEncFolder, keyPath, timestamp);

    // 6. Return JSON Response
    QJsonObject response;
    response["status"] = "success";
    response["files_encrypted"] = successCount;
    response["output_path"] = finalEncFolder;
    response["db_logged"] = dbLog;
    response["timestamp"] = timestamp;

    return QJsonDocument(response).toJson(QJsonDocument::Compact);
}

// Endpoint: POST /register_machine
QByteArray APIController::registerMachine(const QByteArray &jsonBody) {
    QJsonDocument doc = QJsonDocument::fromJson(jsonBody);
    QJsonObject obj = doc.object();
    
    QString machineId = obj["machine_id"].toString();
    QString machineName = obj["machine_name"].toString(); // <--- Parse Name
    QString pubKey = obj["public_key"].toString();

    if (machineId.isEmpty() || pubKey.isEmpty()) {
        return "{\"status\":\"error\", \"message\":\"Missing machine_id or public_key\"}";
    }

    // Pass name to DBManager
    if (m_db->addMachine(machineId, machineName, pubKey)) {
        return "{\"status\":\"success\", \"message\":\"Machine registered\"}";
    }
    return "{\"status\":\"error\", \"message\":\"Database write failed\"}";
}

// Endpoint: GET /records
QByteArray APIController::getAuditRecords() {
    QJsonObject root;
    root["encrypted_folders"] = m_db->getAllEncryptedFolders();
    root["registered_machines"] = m_db->getAllMachines();
    return QJsonDocument(root).toJson();
}

// Endpoint: POST /rsa_encrypt_key
QByteArray APIController::encryptKeyForMachine(const QByteArray &jsonBody) {
    QJsonDocument doc = QJsonDocument::fromJson(jsonBody);
    QJsonObject obj = doc.object();

    QString keyPath = obj["key_path"].toString();
    
    // Check for either ID or Name
    QString identifier;
    if (obj.contains("machine_id")) identifier = obj["machine_id"].toString();
    else if (obj.contains("machine_name")) identifier = obj["machine_name"].toString();

    // 1. Validation
    if (identifier.isEmpty() || !QFile::exists(keyPath)) {
        return "{\"status\":\"error\", \"message\":\"Invalid Input or Key File missing\"}";
    }

    // 2. Get Machine Public Key
    QString pubKey = m_db->getMachinePublicKey(identifier);
    if (pubKey.isEmpty()) {
        return "{\"status\":\"error\", \"message\":\"Machine not found\"}";
    }

    // 3. RSA Wrap Logic (Existing) ...
    QString rsaKeyPath;
    if (!RSAKeyEncryption::wrapKey(keyPath, pubKey, rsaKeyPath)) {
        return "{\"status\":\"error\", \"message\":\"RSA Encryption failed\"}";
    }

    // 4. Create 'rsa_ready' Folder Logic (Existing) ...
    QFileInfo keyInfo(keyPath);
    QDir sourceDir = keyInfo.absoluteDir();
    QString parentPath = sourceDir.absolutePath(); // This matches 'enc_path' in DB
    
    QString cleanId = identifier;
    cleanId.replace(QRegExp("[^a-zA-Z0-9_-]"), "_");
    QString readyFolderPath = parentPath + "/rsa_ready_" + cleanId;
    
    QDir().mkpath(readyFolderPath);

    // ... (File Copying Loop) ...
    QDirIterator it(parentPath, QDir::Files | QDir::NoDotAndDotDot);
    while (it.hasNext()) {
        QString currentFilePath = it.next();
        QString fileName = QFileInfo(currentFilePath).fileName();
        
        // Skip session key and subfolders
        if (fileName == keyInfo.fileName() || currentFilePath == readyFolderPath) continue;

        QFile::copy(currentFilePath, readyFolderPath + "/" + fileName);
    }

    // --- 5. NEW: LOG TO DATABASE ---
    
    // A. Find the ID of the original encryption run
    int folderId = m_db->getFolderIdByEncPath(parentPath);
    
    // B. Log the record
    if (folderId != -1) {
        if (!m_db->logRSAEncryption(folderId, identifier, readyFolderPath)) {
             qWarning() << "Failed to write RSA log to DB";
        }
    } else {
        qWarning() << "Could not find parent folder ID for path:" << parentPath;
    }

    // 6. Return Response
    QJsonObject res;
    res["status"] = "success";
    res["locked_for"] = identifier;
    res["distribution_folder"] = readyFolderPath;
    res["db_record_created"] = (folderId != -1);
    
    return QJsonDocument(res).toJson();
}