/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include "WebManager.h"
#include <QDebug>
#include <QCoreApplication>

WebManager::WebManager(QObject *parent) : QObject(parent)
{
    m_server = new QTcpServer(this);
    
    // Initialize DB and Controller
    m_db = new DBManager();
    m_controller = new APIController(m_db, this);

    connect(m_server, &QTcpServer::newConnection, this, &WebManager::onNewConnection);
}

WebManager::~WebManager() {
    delete m_db;
}

void WebManager::start(quint16 port) {
    if (m_server->listen(QHostAddress::Any, port)) {
        qDebug() << "==========================================";
        qDebug() << "Server Started. Listening on Port:" << port;
        qDebug() << "DB Status:" << (m_db->isOpen() ? "Connected" : "Failed");
        qDebug() << "Send POST requests to: http://localhost:" + QString::number(port) + "/encrypt";
        qDebug() << "==========================================";
    } else {
        qCritical() << "Server failed to start:" << m_server->errorString();
    }
}

void WebManager::onNewConnection() {
    QTcpSocket *socket = m_server->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &WebManager::onClientData);
    connect(socket, &QTcpSocket::disconnected, this, &WebManager::onClientDisconnected);
}

void WebManager::onClientData() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    QByteArray requestData = socket->readAll();
    QString requestStr(requestData);

    // --- Helper to extract JSON Body ---
    auto getBody = [&](const QString& req) -> QByteArray {
        int idx = req.indexOf("\r\n\r\n");
        return (idx != -1) ? requestData.mid(idx + 4) : QByteArray();
    };

    QByteArray response;
    int code = 200;
    if (requestStr.startsWith("POST /show_ui") || requestStr.startsWith("GET /show_ui")) {
        
        emit requestShowUi(); 
        
        response = "{\"status\": \"success\", \"message\": \"UI Launched on Server Host\"}";
    
    }
    // --- ROUTING ---
    else if (requestStr.startsWith("POST /encrypt ")) {
        // 1. AES File Encryption
        response = m_controller->processEncryptionRequest(getBody(requestStr));
    
    } else if (requestStr.startsWith("POST /register_machine ")) {
        // 2. Register Machine ID + PubKey
        response = m_controller->registerMachine(getBody(requestStr));

    } else if (requestStr.startsWith("GET /records ")) {
        // 3. View DB Data
        response = m_controller->getAuditRecords();

    } else if (requestStr.startsWith("POST /rsa_wrap_key ")) {
        // 4. RSA Key Wrapping
        response = m_controller->encryptKeyForMachine(getBody(requestStr));

    } else {
        code = 404;
        response = "{\"error\": \"Unknown Endpoint\"}";
    }

    sendResponse(socket, code, response);
}

void WebManager::sendResponse(QTcpSocket *socket, int statusCode, const QByteArray &body) {
    QString header = "HTTP/1.1 " + QString::number(statusCode) + " OK\r\n";
    header += "Content-Type: application/json\r\n";
    header += "Content-Length: " + QString::number(body.size()) + "\r\n";
    header += "Connection: close\r\n\r\n";

    socket->write(header.toUtf8());
    socket->write(body);
    socket->flush();
    socket->disconnectFromHost();
}

void WebManager::onClientDisconnected() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) socket->deleteLater();
}