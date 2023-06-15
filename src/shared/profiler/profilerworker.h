/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef PROFILERWORKER_H
#define PROFILERWORKER_H

#include <QObject>
#include <QJSEngine>
#include <QJSONObject>
#include <QtQml/private/qqmlengine_p.h>

class ProfilerWorker final : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY_MOVE(ProfilerWorker)

  public:
    explicit ProfilerWorker(QObject* parent, int sampleRateInMS=1, QQmlEngine* target = nullptr):
      QObject(parent),
      m_sampleRateInMS{sampleRateInMS},
      m_target{target}{

    };
   
  public slots:
    void start();
    void stop();

  signals:
    void done(QJsonObject report);

  protected:
    void timerEvent(QTimerEvent *event) override;
  private:
    int m_timerID = 0; // 0 == No timer
    int m_sampleRateInMS = 1;
    QQmlEngine* m_target= nullptr;
    QV4::ExecutionEngine * m_js_engine = nullptr;
    

    struct Measurement{
      QVector<QV4::StackFrame> stackTrace;
      qint64 timestamp;
    };
    QList<Measurement> m_samples;

};

#endif  // PROFILERWORKER_H
