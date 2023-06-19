/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef PROFILERWORKER_H
#define PROFILERWORKER_H

#include <QObject>
#include <QJSEngine>
#include <QJSONObject>
#include <QJSONArray>
#include <QtQml/private/qqmlengine_p.h>

#include <mach/mach.h>

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
    void start(thread_t parentThread);
    void stop();

  signals:
    void done(QJsonObject report);

  protected:
    void timerEvent(QTimerEvent *event) override;
  private:
    qint64 m_start_timestamp =0;
    qint64 m_end_timestamp =0;
    
    int m_timerID = 0; // 0 == No timer
    int m_sampleRateInMS = 1;
    QQmlEngine* m_target= nullptr;
    QV4::ExecutionEngine * m_js_engine = nullptr;
    thread_t m_parentThread; 
    

    struct Measurement{
      QVector<QV4::CppStackFrame> stackTrace;
      qint64 timestamp;
    };
    QList<Measurement> m_samples;
    
    
    /**
     * Meta information associated for the entire profile.
     * Returns JSON of : https://github.com/firefox-devtools/profiler/blob/main/src/types/profile.js#L750
     */
    QJsonObject getProfileMeta();
    QJsonObject getSamples();

    /**
     * Markers represent arbitrary events that happen within the browser. They have a
     * name, time, and potentially a JSON data payload. These can come from all over the
     * system. For instance Paint markers instrument the rendering and layout process.
     * Engineers can easily add arbitrary markers to their code without coordinating with
     * profiler.firefox.com to instrument their code.
     *
     * In the profile, these markers are raw and unprocessed. In the marker selectors, we
     * can run them through a processing pipeline to match up start and end markers to
     * create markers with durations, or even take a string-only marker and parse
     * it into a structured marker.
     */
    QJsonObject getMarkerTable();

    QJsonArray getTreadReport();

};

#endif  // PROFILERWORKER_H
