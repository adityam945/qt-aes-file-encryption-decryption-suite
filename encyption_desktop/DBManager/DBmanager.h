/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QJsonArray>

class DBManager {
public:
    // UPDATE: Default constructor (no arguments)
    DBManager();
    ~DBManager();

    bool isOpen() const;
    bool createTables();

    // Updated signature from previous steps
    bool logEncryptedFolder(const QString& src, const QString& enc, const QString& keyPath, const QString& ts);
    bool storeEncryptedAESKey(int folderId, const QByteArray& rsaEncryptedAesKey);

    // Machine ID Management
    // Update the function signature
    bool addMachine(const QString& machineId, const QString& machineName, const QString& publicKeyHex);
    QString getMachinePublicKey(const QString& identifier);

    // Fetch Audit Logs (For API display)
    QJsonArray getAllEncryptedFolders();
    QJsonArray getAllMachines();

    // Add these to your class definition
    int getFolderIdByEncPath(const QString& encPath);
    bool logRSAEncryption(int folderId, const QString& machineId, const QString& outputDir);

    QJsonArray getFullAuditLog();
    bool deleteAuditRecord(int rsaKeyId);
private:
    QSqlDatabase m_db;
};

#endif
