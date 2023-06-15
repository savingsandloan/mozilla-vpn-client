/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef PROFILER_H
#define PROFILER_H

#include <QObject>
#include <QJSEngine>
#include <QThread>
#include <QtQml/private/qqmlengine_p.h>

#include "profilerworker.h"

class Profiler final : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY_MOVE(Profiler)

 private:
  explicit Profiler(QObject* parent):QObject(parent){
    m_thread = new QThread(this);
  };
 
 public:
  /**
   * @brief Returns a the Profile Handler 
   */
  static Profiler* instance(); 


  /**
   * @brief Registers itself into the Global QML Namespace
   */
  static void initialize();

  public:
  /**
   * @brief Start's to record a profile 
   * that the telemetry uploadEnabled state has changed.
   *
   * @param destination path where the final file should be placed
   * @param QJSEngine to target, will get the default if nullptr
   */
  Q_INVOKABLE void start(int sampleRateInMS=1, QQmlEngine* target = nullptr);
  
  /**
   * @brief Stops the recording 
   * that the telemetry uploadEnabled state has changed.
   */
  
  Q_INVOKABLE void stop();

  /**
   * @brief Returns true if a recording is currently happening 
  */
  inline bool isRunning(){ return m_worker!=nullptr; }

  public slots: 
    void onReport(QJsonObject report);
  signals:
    void startWorker();
    void stopWorker();

  private:
    QThread *m_thread;
    ProfilerWorker *m_worker;


};

#endif  // PROFILER_H
