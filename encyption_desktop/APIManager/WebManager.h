/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef WEBMANAGER_H
#define WEBMANAGER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "DBmanager.h"
#include "APIController.h"

class WebManager : public QObject
{
    Q_OBJECT
public:
    explicit WebManager(QObject *parent = nullptr);
    ~WebManager();
    void start(quint16 port);

signals:
    void requestShowUi();
    
private slots:
    void onNewConnection();
    void onClientData();
    void onClientDisconnected();

private:
    QTcpServer *m_server;
    DBManager *m_db;
    APIController *m_controller;
    
    void sendResponse(QTcpSocket *socket, int statusCode, const QByteArray &body);
};

#endif