
#include "profilerworker.h"

#include <QtQml/private/qqmlengine_p.h>
#include <mach/mach.h>

#include <QJsonObject>
#include <QObject>
#include <QQmlEngine>

#include "logger.h"
#include "qmlengineholder.h"

namespace {
Logger logger("Profile_Worker");
// Doubt we will have bigger then that.
constexpr auto MAX_STACK_DEPTH = 30;
}  // namespace

void ProfilerWorker::start(thread_t parentThread) {
  m_parentThread = parentThread;
  QThread* thread = QThread::currentThread();
  logger.debug() << "Profiler-Worker on thread" << thread->currentThreadId();
  if (m_target == nullptr) {
    m_target = QmlEngineHolder::instance()->engine();
  }
  auto privateEngine = QQmlEnginePrivate::get(m_target);
  m_js_engine = privateEngine->v4engine();
  m_samples.clear();
  m_timerID = startTimer(m_sampleRateInMS, Qt::PreciseTimer);
  logger.debug() << "ProfileWorker Started!";
}

void ProfilerWorker::timerEvent(QTimerEvent* event) {
  std::array<QV4::CppStackFrame, MAX_STACK_DEPTH> stack_buffer;
  const auto timestamp = QDateTime::currentSecsSinceEpoch();

  // -- ZA WARUDO --
  if (thread_suspend(m_parentThread) != KERN_SUCCESS) {
    // We failed to suspend the js thread, so it's too dangerous to continue
    return;
  };
  // We have now frozen time for the main thread.
  // However the main thread might be allocating memory right now,
  // and having a lock on that.
  // So we cannot allocate anything or we will deadlock!

  // Not JS running, all good continue and exit.
  if (m_js_engine->currentStackFrame == nullptr) {
    if (thread_resume(m_parentThread) != KERN_SUCCESS) {
        printf("OHNO WE BROKE THE WORLD");
    }
    return;
  }
  // We have something in the JS stack,
  // currentStackFrame is a struct pointing to a *v4Function and a
  // ParentStackframe So we just need to copy those, we can reconstruct &&
  // analyze later.
  auto* current_frame = m_js_engine->currentStackFrame;
  uint8_t count = 0;
  while (current_frame != nullptr && count < MAX_STACK_DEPTH) {
    memcpy(&stack_buffer.at(count), &current_frame, sizeof(QV4::CppStackFrame));
    count++;
    current_frame = current_frame->parentFrame();
  }
  if (thread_resume(m_parentThread) != KERN_SUCCESS) {
      printf("OHNO WE BROKE THE WORLD");
  }
  if (count == 0) {
    // Nothing captured.
    return;
  }
  QVector<QV4::CppStackFrame> out(count);
  for (int i = 0; i < count; i++) {
    out.append(stack_buffer.at(i));
  }
  m_samples.append(Measurement{out, timestamp});

  if (thread_resume(m_parentThread) != KERN_SUCCESS) {
    printf("OHNO WE BROKE THE WORLD");
  }
  logger.error() << "Current: " << m_samples.length();
}

void ProfilerWorker::stop() {
    killTimer(m_timerID);
    m_timerID = 0;
    logger.debug() << "Profiler Stopped!";
    logger.debug() << "Captured Frames:" << m_samples.length();
    
    // Now Let's build a profile out of that info!
    QJsonObject profile;
    
    
    QJsonObject meta;
    
}

QJsonObject ProfilerWorker::getProfileMeta(){
    QJsonObject meta;

    meta.insert("interval", "Milliseconds");
    meta.insert("startTime", "Milliseconds");
    meta.insert("endTime", QJsonValue::Undefined);
    meta.insert("profilingStartTime", QJsonValue::Undefined);
    meta.insert("profilingEndTime", QJsonValue::Undefined);
    meta.insert("processType", 0);
    meta.insert("extensions", QJsonObject());
    meta.insert("categories", QJsonObject());
    meta.insert("product", "Firefox");
    meta.insert("stackwalk", 0);
    meta.insert("debug", QJsonValue::Undefined);
    meta.insert("version", 0);
    meta.insert("preprocessedProfileVersion", 0);
    meta.insert("abi", QJsonValue::Undefined);
    meta.insert("misc", QJsonValue::Undefined);
    meta.insert("oscpu", QJsonValue::Undefined);
    meta.insert("mainMemory", "Bytes");
    meta.insert("platform", "string");
    meta.insert("toolkit", "string");
    meta.insert("appBuildID", QJsonValue::Undefined);
    meta.insert("arguments", QJsonValue::Undefined);
    meta.insert("sourceURL", QJsonValue::Undefined);
    meta.insert("physicalCPUs", QJsonValue::Undefined);
    meta.insert("logicalCPUs", QJsonValue::Undefined);
    meta.insert("CPUName", QJsonValue::Undefined);
    meta.insert("symbolicated", QJsonValue::Undefined);
    meta.insert("symbolicationNotSupported", QJsonValue::Undefined);
    meta.insert("updateChannel", "string");
    meta.insert("visualMetrics", QJsonObject());
    meta.insert("configuration", QJsonObject());
    meta.insert("markerSchema", QJsonArray());
    meta.insert("sampleUnits", QJsonValue::Undefined);
    meta.insert("device", QJsonValue::Undefined);
    meta.insert("importedFrom", QJsonValue::Undefined);
    meta.insert("usesOnlyOneStackType", QJsonValue::Undefined);
    meta.insert("doesNotUseFrameImplementation", QJsonValue::Undefined);
    meta.insert("sourceCodeIsNotOnSearchfox", QJsonValue::Undefined);
    meta.insert("extra", QJsonArray());
    meta.insert("initialVisibleThreads", QJsonArray());
    meta.insert("initialSelectedThreads", QJsonArray());
    meta.insert("keepProfileThreadOrder", QJsonValue::Undefined);

    return meta;
}
