
#include <QObject> 

#include <QQmlEngine>
#include <QtQml/private/qqmlengine_p.h>
#include <QJsonObject>

#include "qmlengineholder.h"
#include "logger.h"


#include "profilerworker.h"

namespace{
    Logger logger("Profile_Worker");
    // Doubt we will have bigger then that. 
    constexpr auto MAX_STACK_DEPTH = 100;
}

void ProfilerWorker::start(){
    QThread *thread = QThread::currentThread();
    logger.debug() << "Profiler-Worker on thread" << thread->currentThreadId();
    if(m_target == nullptr){
        m_target = QmlEngineHolder::instance()->engine();
    }
    auto privateEngine = QQmlEnginePrivate::get(m_target);
    m_js_engine = privateEngine->v4engine();
    m_samples.clear();
    m_timerID = startTimer(m_sampleRateInMS, Qt::PreciseTimer);
    logger.debug() << "ProfileWorker Started!";
}

void ProfilerWorker::timerEvent(QTimerEvent *event){
   const auto timestamp = QDateTime::currentSecsSinceEpoch();

   const QVector<QV4::StackFrame> stackTrace = m_js_engine->stackTrace(MAX_STACK_DEPTH);
   if(stackTrace.length() == 0){
        // We Simply Might have nothing to report \_(+_*)_/
        return;
   }
   m_samples.append(Measurement{
        stackTrace,
        timestamp
   });
}
  
void ProfilerWorker::stop(){
    killTimer(m_timerID);
    m_timerID=0;
    logger.debug() << "Profiler Stopped!";
    logger.debug() << "Captured Frames:" << m_samples.length();
    m_samples.clear();
    
    QJsonObject report; 
}
