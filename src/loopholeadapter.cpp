
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "loopholeadapter.h"

#include <QFileInfo>
#include<QDir>
#include <QJsonParseError>
#include <QJsonDocument>

#include "logger.h"
#include "constants.h"

#ifdef MZ_WINDOWS
#include "platforms/windows/windowscommons.h"
constexpr auto const LOOPHOLE_BIN = "loophole.exe";
#endif


namespace {
Logger logger("LoopholeAdapter");
}


const QString& LoopholeAdapter::binaryPath(){
#ifdef MZ_WINDOWS
  auto binaryPath = WindowsCommons::getCurrentPath();
#else
#  error "Unsupported OS for LoopholeAdapter::binaryPath() !"
#endif
  auto info = QFileInfo(binaryPath);
  if (!info.exists()) {
    Q_ASSERT(false);
    return QString();
  }
  QDir dir = info.absoluteDir();
  return dir.filePath(LOOPHOLE_BIN);
}

void LoopholeAdapter::start(){
  auto loopholeFile = QFileInfo(binaryPath());
  if (!loopholeFile.exists()){
    Q_ASSERT(false);
    return;
  }

  if (mCurrentProcess){
    mCurrentProcess->kill();
    mCurrentProcess->deleteLater();
  }
  mCurrentProcess = new QProcess(this);
  mCurrentProcess->setProgram(loopholeFile.absoluteFilePath());
  mCurrentProcess->setArguments(getArguments());

  connect(mCurrentProcess, &QProcess::readyRead, this,
          &LoopholeAdapter::readData);
  mCurrentProcess->start(QIODeviceBase::ReadOnly);
}

void LoopholeAdapter::stop() {
  if (!mCurrentProcess) {
    active.setValue(false);
    return;
  }
  mCurrentProcess->kill();
  mCurrentProcess->deleteLater();
  active.setValue(false);
}


void LoopholeAdapter::readData() {
  logger.debug() << "Reading";

  Q_ASSERT(mCurrentProcess);
  Q_ASSERT(mCurrentProcess->isOpen());
  QByteArray input = mCurrentProcess->readAll();
  m_buffer.append(input);
  while (true) {
    int pos = m_buffer.indexOf("\n");
    if (pos == -1) {
      break;
    }
    QByteArray line = m_buffer.left(pos);
    m_buffer.remove(0, pos + 1);

    QByteArray command(line);
    command = command.trimmed();
    if (command.isEmpty()) {
      continue;
    }
    parseCommand(command);
  }
}

void LoopholeAdapter::parseCommand(QByteArray command) {
  if (command.isEmpty()) {
    return;
  }
  QJsonParseError parse_error;
  auto doc = QJsonDocument::fromJson(command, &parse_error);
  if (parse_error.error == QJsonParseError::NoError) {
    logger.error() << "Failed to parse: " << command;
    logger.error() << parse_error.errorString();
    return;
  }
  if (!doc.isObject()) {
    return;
  }
  auto obj = doc.object();

  auto type = obj["Type"].toString();
  if (!type.isNull()){
    logger.error() << "Malformed Payload: " << command;
  }
  if (type == "error") {
    logger.error() << "Loophole Error: " << obj["Message"].toString();
  }
  Q_ASSERT(type == "ready");
    
  auto username = obj["Username"].toString();
  auto password = obj["Password"].toString();
    
  if (!obj.contains("Port")){
    logger.error() << "Loophole did not send a port: " << command;
    return;
  }
  auto port = obj["Port"].toInt();

  QUrl proxy;
  proxy.setHost("localhost");
  proxy.setScheme("socks5");
  if (!username.isEmpty()){
    proxy.setUserName(username);
  }
  if (!password.isEmpty()){
    proxy.setPassword(password);
  }
  proxy.setPort(port);
}

bool LoopholeAdapter::canActivate(){
  return QFileInfo(binaryPath()).exists();
}

const QStringList LoopholeAdapter::getArguments(){
    // In Production, require auth, 
    // use a random port. 
    if (Constants::inProduction()){
        return QStringList({"-auth"});
    }
    // On Debug Mode, open a non authed Proxy 
    // with port fixed to 8040, to make debugging easy
    return QStringList({"-p 8040"});
}