
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef LOOPHOLEADAPTER_H
#  define LOOPHOLEADAPTER_H

#  include <QCoreApplication>
#  include <QObject>
#include <QJsonObject>
#include <QProcess>
#include <QPointer>
#include <QProperty>
#include <QUrl>
#include <QByteArray>


class LoopholeAdapter : public QObject {
 public:
  explicit LoopholeAdapter(QObject* parent) : QObject(parent){
    active.setValue(false);
    currentURL.setValue(QUrl());
  };
  virtual ~LoopholeAdapter() = default;

  /**
  * The URL to connect to the Process
  */
  QProperty<QUrl> currentURL; 
  Q_PROPERTY(QUrl currentURL bindable currentURL)
  /**
  * Is the Proxy active?
  */
  QProperty<bool> active;
  Q_PROPERTY(bool active bindable active)

  /**
  * Start the Proxy
  */
  void start();
  /**
  * Stop the Proxy
  */
  void stop();

  /**
  * Returns true if it's supported and 
  * the system is in the right state
  */
  bool canActivate();

  private:

	  void readData();

	  const QString& binaryPath();

	  const QStringList getArguments();

	  void parseCommand(QByteArray command); 

	QJsonObject mCurrentConfig; 
	QPointer<QProcess> mCurrentProcess;
    QByteArray m_buffer;
};

#endif  // LOOPHOLEADAPTER_H