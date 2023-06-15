
#include <QObject> 

#include <QQmlEngine>
#include <QtQml/private/qqmlengine_p.h>
#include <QJsonObject>

#include "qmlengineholder.h"
#include "logger.h"


#include "profiler.h"

namespace{
    Logger logger("Profiler");
    Profiler* s_instance= nullptr;

}
Profiler* Profiler::instance(){
    if(s_instance == nullptr){
        s_instance = new Profiler(qApp);
    }
    return s_instance;
}


void Profiler::start(int sampleRateInMS, QQmlEngine* target){
    QThread *thread = QThread::currentThread();
    logger.debug() << "Profiler-Service on thread" << thread->currentThreadId();
    m_worker= new ProfilerWorker(nullptr,sampleRateInMS,target);
    m_worker->moveToThread(m_thread);
    connect(m_worker, &ProfilerWorker::done,this,&Profiler::onReport);
    connect(this, &Profiler::startWorker, m_worker, &ProfilerWorker::start);
    connect(this, &Profiler::stopWorker, m_worker, &ProfilerWorker::stop);
    
    m_thread->start();
    emit startWorker();
}

void Profiler::initialize(){
    qmlRegisterSingletonInstance("Mozilla.VPN", 1, 0, "Profiler", Profiler::instance());
}


void Profiler::stop(){
    emit stopWorker();
}

void Profiler::onReport(QJsonObject report){
    m_thread->quit();
    m_worker->deleteLater();
    logger.debug() << "Got a report?!";
}