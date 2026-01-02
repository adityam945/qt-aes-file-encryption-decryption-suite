/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef APICONTROLLER_H
#define APICONTROLLER_H

#include <QObject>
#include <QJsonObject>
#include "DBmanager.h"

class APIController : public QObject
{
    Q_OBJECT
public:
    explicit APIController(DBManager *db, QObject *parent = nullptr);
    QByteArray processEncryptionRequest(const QByteArray &jsonBody);
    QByteArray registerMachine(const QByteArray &jsonBody);
    QByteArray getAuditRecords();
    QByteArray encryptKeyForMachine(const QByteArray &jsonBody);

private:
    DBManager *m_db;
};

#endif