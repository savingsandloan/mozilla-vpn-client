/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sentryadapter.h"

#ifndef UNIT_TEST
// This is not found when building tests ... weird.
#  include <QtQml/private/qqmlengine_p.h>
#endif
#include <sentry.h>

#include <QDir>
#include <QQuickItem>
#include <QScopeGuard>
#include <QStandardPaths>

#include "constants.h"
#include "feature.h"
#include "leakdetector.h"
#include "logger.h"
#include "loghandler.h"
#include "qmlengineholder.h"
#include "sentrywatchdog.h"
#include "settingsholder.h"
#include "tasks/tasksentry.h"
#include "tasks/tasksentryconfig.h"
#include "taskscheduler.h"

namespace {
SentryAdapter* s_instance = nullptr;
Logger logger("Sentry");

}  // namespace

SentryAdapter* SentryAdapter::instance() {
  if (s_instance == nullptr) {
    s_instance = new SentryAdapter();
  }
  return s_instance;
}
SentryAdapter::SentryAdapter() { MZ_COUNT_CTOR(SentryAdapter); }
SentryAdapter::~SentryAdapter() { MZ_COUNT_DTOR(SentryAdapter); }

void SentryAdapter::init() {
  if (!Feature::get(Feature::Feature_sentry)->isSupported()) {
    return;
  }
  if (m_initialized) {
    return;
  }
  auto settings = SettingsHolder::instance();
  if (!settings->hasSentryEndpoint() || !settings->hasSentryDSN()) {
    // If we have no info on where to put crash info, let's
    // query that and retry later.
    logger.info() << "No sentry config present, attempting to fetch";
    auto sentryConfig = new TaskSentryConfig();
    TaskScheduler::scheduleTask(sentryConfig);
    return;
  }
  auto log = LogHandler::instance();

  auto dsn = settings->sentryDSN();

  QDir dataDir(
      QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
  QString sentryFolder = dataDir.absoluteFilePath("sentry");

  connect(log, &LogHandler::logEntryAdded, this,
          &SentryAdapter::onLoglineAdded);

  sentry_options_t* options = sentry_options_new();
  sentry_options_set_max_breadcrumbs(options, 500);
  sentry_options_set_dsn(options, dsn.toLocal8Bit().constData());
  sentry_options_set_environment(
      options, Constants::inProduction() ? "production" : "stage");
  sentry_options_set_release(
      options, Constants::versionString().toLocal8Bit().constData());
  sentry_options_set_database_path(options,
                                   sentryFolder.toLocal8Bit().constData());
  sentry_options_set_on_crash(options, &SentryAdapter::onCrash, NULL);

#ifdef MZ_DEBUG
  // If this a debug build, changes are high sentry has no symbols for the stack
  // trace, but also changes are high they are in the current binary aswell. So
  // let's symbolize locally before sending to sentry.
  sentry_options_set_symbolize_stacktraces(options, 1);
#endif

#ifdef SENTRY_NONE_TRANSPORT
  sentry_transport_t* transport =
      sentry_transport_new(&SentryAdapter::transportEnvelope);
  sentry_transport_set_state(transport, nullptr);
  sentry_options_set_transport(options, transport);
#endif

  // Uncomment the following line for debugging purposes.
  // Be warned, it's spammy to stdout.
  // sentry_options_set_debug(options, 1);

  if (sentry_init(options) == -1) {
    logger.error() << "Sentry failed to init!";
    return;
  };
  // Sentry is ready now.
  initWatchdog();

  m_initialized = true;
  setPlatformTag();
  logger.info() << "Sentry initialized";
}

void SentryAdapter::report(const QString& errorType, const QString& message,
                           bool attachStackTrace, bool attachContext) {
  if (!m_initialized) {
    return;
  }
  auto context_guard = qScopeGuard([&] { removeContext("global"); });
  if (attachContext) {
    auto context = globalEventContext.value();
    if (!context.isEmpty()) {
      setContext("global", context);
    } else {
      logger.error()
          << "Sentry was tasked to attach Context, but none was given";
    }
  }
  sentry_value_t event = sentry_value_new_event();
  sentry_value_t exc = sentry_value_new_exception(errorType.toLocal8Bit(),
                                                  message.toLocal8Bit());

  if (attachStackTrace) {
    sentry_value_set_stacktrace(exc, NULL, 0);
  }

  sentry_event_add_exception(event, exc);
  sentry_capture_event(event);
}

void SentryAdapter::onBeforeShutdown() {
  sentry_end_session();
  sentry_close();
}

void SentryAdapter::onLoglineAdded(const QByteArray& line) {
  if (!m_initialized) {
    return;
  }
  // Todo: we could certainly catch this more early and format the data ?
  // (VPN-3276)
  sentry_value_t crumb =
      sentry_value_new_breadcrumb("Logger", line.constData());
  sentry_add_breadcrumb(crumb);
}

sentry_value_t SentryAdapter::onCrash(const sentry_ucontext_t* uctx,
                                      sentry_value_t event, void* closure) {
  logger.info() << "Sentry ON CRASH";
  captureQMLStacktrace("Client Crashed, Current QML Stack:");
  return event;
}

// static
void SentryAdapter::transportEnvelope(sentry_envelope_t* envelope,
                                      void* state) {
  Q_UNUSED(state);
  size_t sentry_buf_size = 0;
  char* sentry_buf = sentry_envelope_serialize(envelope, &sentry_buf_size);

  // Qt Will copy this.
  auto qt_owned_buffer = QByteArray(sentry_buf, sentry_buf_size);
  // We can now free the envelope.
  sentry_envelope_free(envelope);
  sentry_free(sentry_buf);

  auto t = new TaskSentry(qt_owned_buffer);
  TaskScheduler::scheduleTaskNow(t);
}

SentryAdapter::UserConsentResult SentryAdapter::hasCrashUploadConsent() const {
  // We have not yet asked the user - let's do that.
  if (m_userConsent == UserConsentResult::Pending) {
    emit needsCrashReportScreen();
  }
  return m_userConsent;
}
// Q_Invokeable
void SentryAdapter::allowCrashReporting() {
  m_userConsent = UserConsentResult::Allowed;
  emit userConsentChanged();
}
// Q_Invokeable
void SentryAdapter::declineCrashReporting() {
  m_userConsent = UserConsentResult::Forbidden;
  emit userConsentChanged();
}

void SentryAdapter::captureQMLStacktrace(const char* description) {
#ifndef UNIT_TEST
  auto engine = QmlEngineHolder::instance()->engine();
  auto privateEngine = QQmlEnginePrivate::get(engine);
  QV4::ExecutionEngine* qv4Engine = privateEngine->v4engine();
  QVector<QV4::StackFrame> stackTrace = qv4Engine->stackTrace(15);
  sentry_value_t crumb = sentry_value_new_breadcrumb("info", description);
  sentry_value_set_by_key(crumb, "category",
                          sentry_value_new_string("stacktrace"));
  sentry_value_set_by_key(crumb, "level",
                          sentry_value_new_string("stacktrace"));
  sentry_value_t data = sentry_value_new_object();
  for (int i = 0; i < stackTrace.count(); i++) {
    const QV4::StackFrame& frame = stackTrace.at(i);
    sentry_value_t sentry_frame = sentry_value_new_object();

    sentry_value_set_by_key(
        sentry_frame, "function",
        sentry_value_new_string(frame.function.toLocal8Bit().constData()));
    sentry_value_set_by_key(
        sentry_frame, "source",
        sentry_value_new_string(frame.source.toLocal8Bit().constData()));
    sentry_value_set_by_key(sentry_frame, "line",
                            sentry_value_new_int32(frame.line));
    sentry_value_set_by_key(sentry_frame, "column",
                            sentry_value_new_int32(frame.column));
    sentry_value_set_by_key(data, std::to_string(i).c_str(), sentry_frame);
  }
  sentry_value_set_by_key(crumb, "data", data);
  sentry_add_breadcrumb(crumb);
#else
  Q_UNUSED(description);
#endif
}

void SentryAdapter::setPlatformTag() const {
#if defined(MZ_LINUX)
  sentry_set_tag("platform", "Linux");
#elif defined(MZ_MACOS)
  sentry_set_tag("platform", "MacOS");
#elif defined(MZ_WINDOWS)
  sentry_set_tag("platform", "Windows");
#elif defined(MZ_IOS)
  sentry_set_tag("platform", "iOS");
#elif defined(MZ_ANDROID)
  sentry_set_tag("platform", "Android");
#else
  sentry_set_tag("platform", "Dummy");
#endif
}

void SentryAdapter::initWatchdog() {
  QPointer<SentryWatchdog> watchdog = SentryWatchdog::create(this);
  if (watchdog.isNull()) {
    logger.error() << "Nooo watchdooog";
    return;
  }
  connect(watchdog, &SentryWatchdog::timeout, this,
          [&, watchdog](qint64 timestamp) {
            logger.error() << "ANR ANR ANR";
            logger.error() << "Last Seen" << timestamp;
            report("ANR", "Timeout", false, true);
            // watchdog->start();
          });
  watchdog->start();
}

void SentryAdapter::setContext(QString name, QVariantMap values) {
  QMapIterator<QString, QVariant> iterator(values);
  sentry_value_t sentry_values = sentry_value_new_object();
  while (iterator.hasNext()) {
    iterator.next();
    QString key = iterator.key();
    QVariant value = iterator.value();
    QMetaType value_type = value.metaType();
    sentry_value_t sentry_value;
    switch (value_type.id()) {
      case QMetaType::Void:
        [[fallthrough]];
      case QMetaType::Nullptr:
        [[fallthrough]];
      case QMetaType::VoidStar:
        [[fallthrough]];
      case QMetaType::UnknownType:
        sentry_value = sentry_value_new_null();
        break;
      case QMetaType::Int:
        [[fallthrough]];
      case QMetaType::UInt:
        sentry_value = sentry_value_new_int32(value.toInt());
        break;
      case QMetaType::Double:
        sentry_value = sentry_value_new_double(value.toDouble());
        break;
      case QMetaType::Bool:
        sentry_value = sentry_value_new_bool(value.toBool());
        break;
      case QMetaType::QString:
        sentry_value =
            sentry_value_new_string(value.toString().toLocal8Bit().constData());
        break;
      default:
        sentry_value = sentry_value_new_null();
        logger.error() << "Sentry Context contains unknown type: "
                       << value_type.name();
        break;
    }
    sentry_value_set_by_key(sentry_values, key.toLocal8Bit().constData(),
                            sentry_value);
  }
  sentry_set_context(name.toLocal8Bit().constData(), sentry_values);
}

void SentryAdapter::removeContext(QString name) {
  sentry_remove_context(name.toLocal8Bit().constData());
}
