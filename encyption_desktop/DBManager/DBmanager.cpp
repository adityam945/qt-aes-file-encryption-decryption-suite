/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include "DBmanager.h"
#include <QSqlError>
#include <QDebug>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

DBManager::DBManager() {
    // Automatically determine the path: <build_dir>/db/Encryption_Host.db shadow build path /db
    QString dbDir = qApp->applicationDirPath() + "/db";
    QDir().mkpath(dbDir); // Ensure folder exists

    QString m_dbPath = dbDir + "/Encryption_Host.db";

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(m_dbPath);

    if (!m_db.open()) {
        qCritical() << "Error: Connection with database failed at" << m_dbPath;
        qCritical() << m_db.lastError().text();
    } else {
        qDebug() << "Database connected at:" << m_dbPath;
        createTables();
    }
}

DBManager::~DBManager() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DBManager::isOpen() const {
    return m_db.isOpen();
}

bool DBManager::createTables() {
    QSqlQuery query;
    bool success = true;

    QString rsaTable = "CREATE TABLE IF NOT EXISTS RSA_Machines ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                       "machine_id TEXT UNIQUE, "
                       "machine_name TEXT UNIQUE, "
                       "public_key TEXT, " 
                       "created_at TEXT)";
    
    if (!query.exec(rsaTable)) {
        qCritical() << "Table Creation Error (RSA_Machines):" << query.lastError().text();
        success = false;
    }

    QString aesTable = "CREATE TABLE IF NOT EXISTS AES_encrypted_folders ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                       "source_path TEXT, "
                       "enc_path TEXT, "
                       "enc_key_path TEXT, "
                       "timestamp TEXT)";

    if (!query.exec(aesTable)) {
        qCritical() << "Table Creation Error (RSA_Machines):" << query.lastError().text();
        success = false;
    }

    QString rsaKeyTable = "CREATE TABLE IF NOT EXISTS RSA_Key_encryptions ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "folder_id INTEGER, "
                          "machine_id TEXT, "
                          "output_dir TEXT, "
                          "timestamp TEXT, "
                          "FOREIGN KEY(folder_id) REFERENCES AES_encrypted_folders(id))";

    if (!query.exec(rsaKeyTable)) {
        qCritical() << "Table Creation Error (RSA_Key_encryptions):" << query.lastError().text();
        success = false;
    }
    return success;
}


bool DBManager::logEncryptedFolder(const QString& src, const QString& enc, const QString& keyPath, const QString& ts) {
    qDebug() << "Logging: " << src << enc << keyPath << ts;
    QSqlQuery query;
    query.prepare("INSERT INTO AES_encrypted_folders (source_path, enc_path, enc_key_path, timestamp) "
                  "VALUES (:src, :enc, :key, :ts)");
    query.bindValue(":src", src);
    query.bindValue(":enc", enc);
    query.bindValue(":key", keyPath);
    query.bindValue(":ts", ts);

    if(!query.exec()) {
        qCritical() << "------------------------------------------------";
        qCritical() << "SQL INSERT FAILED!";
        qCritical() << "Error:" << query.lastError().text();
        qCritical() << "Query:" << query.lastQuery();
        qCritical() << "------------------------------------------------";
        return false;
    }

    qDebug() << "SQL Insert Success. Row ID:" << query.lastInsertId().toInt();
    return true;
}

bool DBManager::storeEncryptedAESKey(int folderId, const QByteArray& rsaEncryptedAesKey) {
    QSqlQuery query;
    query.prepare("INSERT INTO RSA_Key_encryptions (folder_id, encrypted_aes_key) "
                  "VALUES (:fid, :key)");
    query.bindValue(":fid", folderId);
    query.bindValue(":key", rsaEncryptedAesKey);
    return query.exec();
}

bool DBManager::addMachine(const QString& machineId, const QString& machineName, const QString& publicKeyHex) {
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO RSA_Machines (machine_id, machine_name, public_key, created_at) "
                  "VALUES (:mid, :name, :pub, :date)");
    
    query.bindValue(":mid", machineId);
    query.bindValue(":name", machineName); 
    query.bindValue(":pub", publicKeyHex);
    query.bindValue(":date", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    
    if (!query.exec()) {
        qCritical() << "Add Machine SQL Error:" << query.lastError().text();
        return false;
    }
    return true;
}

QString DBManager::getMachinePublicKey(const QString& identifier) {
    QSqlQuery query;
    // Check if the identifier matches the ID OR the Name
    query.prepare("SELECT public_key FROM RSA_Machines WHERE machine_id = :id OR machine_name = :name");
    query.bindValue(":id", identifier);
    query.bindValue(":name", identifier);
    
    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }
    return QString(); // Return empty if not found
}
QJsonArray DBManager::getAllEncryptedFolders() {
    QJsonArray list;
    QSqlQuery query("SELECT * FROM AES_encrypted_folders");
    while (query.next()) {
        QJsonObject obj;
        obj["id"] = query.value("id").toInt();
        obj["source_path"] = query.value("source_path").toString();
        obj["enc_path"] = query.value("enc_path").toString();
        obj["key_path"] = query.value("enc_key_path").toString();
        obj["timestamp"] = query.value("timestamp").toString();
        list.append(obj);
    }
    return list;
}

QJsonArray DBManager::getAllMachines() {
    QJsonArray list;
    QSqlQuery query("SELECT machine_id, created_at FROM RSA_Machines");
    while (query.next()) {
        QJsonObject obj;
        obj["machine_id"] = query.value("machine_id").toString();
        obj["created_at"] = query.value("created_at").toString();
        list.append(obj);
    }
    return list;
}

int DBManager::getFolderIdByEncPath(const QString& encPath) {
    QSqlQuery query;
    query.prepare("SELECT id FROM AES_encrypted_folders WHERE enc_path = :path");
    query.bindValue(":path", encPath);
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return -1; // Not found
}

bool DBManager::logRSAEncryption(int folderId, const QString& machineId, const QString& outputDir) {
    QSqlQuery query;
    query.prepare("INSERT INTO RSA_Key_encryptions (folder_id, machine_id, output_dir, timestamp) "
                  "VALUES (:fid, :mid, :out, :ts)");
    
    query.bindValue(":fid", folderId);
    query.bindValue(":mid", machineId);
    query.bindValue(":out", outputDir);
    query.bindValue(":ts", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    if (!query.exec()) {
        qCritical() << "RSA Log Error:" << query.lastError().text();
        return false;
    }
    return true;
}

QJsonArray DBManager::getFullAuditLog() {
    QJsonArray list;
    
    // JOIN Query: connect the Encryption Event -> Folder Details -> Machine Name
    QString sql = "SELECT k.id, a.source_path, m.machine_name, k.output_dir, k.timestamp "
                  "FROM RSA_Key_encryptions k "
                  "LEFT JOIN AES_encrypted_folders a ON k.folder_id = a.id "
                  "LEFT JOIN RSA_Machines m ON k.machine_id = m.machine_id";

    QSqlQuery query(sql);
    while (query.next()) {
        QJsonObject obj;
        obj["id"] = query.value("id").toInt(); // RSA_Key_encryptions ID
        obj["source"] = query.value("source_path").toString();
        obj["machine"] = query.value("machine_name").toString();
        
        // Fallback if machine was deleted or ID missing
        if (obj["machine"].toString().isEmpty()) obj["machine"] = "Unknown ID";
        
        obj["output"] = query.value("output_dir").toString();
        obj["time"] = query.value("timestamp").toString();
        list.append(obj);
    }
    return list;
}

bool DBManager::deleteAuditRecord(int rsaKeyId) {
    QSqlQuery query;
    query.prepare("DELETE FROM RSA_Key_encryptions WHERE id = :id");
    query.bindValue(":id", rsaKeyId);
    return query.exec();
}