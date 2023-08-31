#include "test_watchdog.moc"

#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QThread>
#include <QTimer>

#include "../../sentrywatchdog.h"

class SentryWatchDogTest : public ::testing::Test {
 protected:
  // Create a Wrapper QCoreApplication
  // to let us use the QEventloop.
  int argc = 1;
  char* argv = "test";
  QCoreApplication* app;
  void SetUp() override { app = new QCoreApplication(argc, &argv); }
  void TearDown() override { delete app; }

  /**
   * @brief Wraps a Watchdog, the implementation of `runOnUIThread`
   * can be provided via arguments.
   */
  class FunctionWatchDog final : public SentryWatchdog {
   public:
    explicit FunctionWatchDog(
        QObject* parent, std::function<void(std::function<void()>)> executor)
        : SentryWatchdog(parent) {
      m_executor = executor;
    };
    ~FunctionWatchDog(){};
    std::function<void(std::function<void()> runnable)> m_executor;
    void runOnUIThread(std::function<void()> runnable) {
      m_executor(runnable);
    };
  };

  class WorkerThread : public QThread {
   public:
    WorkerThread(std::function<void()> r) { m_runnable = r; }
    std::function<void()> m_runnable;
    void run() override {
      m_runnable();
      exec();
    }
  };

  void runAfter(int msec_timeout, std::function<void()> runnable) {
    QPointer<QTimer> t = new QTimer();
    t->setSingleShot(true);
    QObject::connect(t, &QTimer::timeout, [&, t]() {
      if (!t.isNull()) {
        t->deleteLater();
      }
      runnable();
    });
    t->start(msec_timeout);
  }
  void setAppEventLoopTimeout(int msec_timeout) {
    runAfter(msec_timeout, [&]() { app->exit(); });
  }
  void runEventLoopForMs(int msec_timeout = 1000) {
    setAppEventLoopTimeout(msec_timeout);
    app->exec();
  }
};

TEST_F(SentryWatchDogTest, TimesOut) {
  /**
   * @brief If the UI runnable never kick's the watchdog
   * it should timeout.
   *
   */
  auto watchdog = new FunctionWatchDog(
      app, [](std::function<void()> runnable) { Q_UNUSED(runnable); });
  testing::AssertionResult result = testing::AssertionFailure()
                                    << " did not timeout ";
  QObject::connect(watchdog, &FunctionWatchDog::timeout, [&]() {
    result = testing::AssertionSuccess();
    app->exit();
  });
  watchdog->start(100);
  runEventLoopForMs(200);
  EXPECT_TRUE(result);
}

TEST_F(SentryWatchDogTest, ResetsOnKickSameThread) {
  /**
   * @brief In this Test Case we will "runOnUIThread" on the main thread
   * Therefore the Watchdog should never hit timeout during the
   * execution of the event loop
   */
  auto watchdog = new FunctionWatchDog(app, [](std::function<void()> runnable) {
    // This time Run the Setup.
    runnable();
  });
  testing::AssertionResult result = testing::AssertionSuccess();

  QObject::connect(watchdog, &FunctionWatchDog::timeout, [&]() {
    result = testing::AssertionFailure()
             << "The Watchdog timed out, even though the kick Timer was "
                "supposed to be on";
    app->exit();
  });
  watchdog->start(10);
  runEventLoopForMs(100);
  EXPECT_TRUE(result);
}

TEST_F(SentryWatchDogTest, ResetsOnKickCrossThread) {
  /**
   * @brief In this Test Case we will "runOnUIThread" on a new thread.
   * Therefore the Watchdog should never hit timeout during the
   * execution of the event loop
   */
  testing::AssertionResult result = testing::AssertionSuccess();

  QPointer<QThread> uiThread = nullptr;
  auto watchdog =
      new FunctionWatchDog(app, [&](std::function<void()> runnable) {
        uiThread = new WorkerThread(runnable);
        uiThread->start();
      });

  QObject::connect(watchdog, &FunctionWatchDog::timeout, [&]() {
    result = testing::AssertionFailure()
             << "The Watchdog timed out, even though the kick Timer was "
                "supposed to be on";
    app->exit();
  });
  watchdog->start(10);
  runEventLoopForMs(100);
  uiThread->terminate();
  EXPECT_TRUE(result);
}

TEST_F(SentryWatchDogTest, StopShouldStop) {
  testing::AssertionResult result = testing::AssertionSuccess();

  QPointer<QThread> uiThread = nullptr;
  auto watchdog =
      new FunctionWatchDog(app, [&](std::function<void()> runnable) {
        uiThread = new WorkerThread(runnable);
        uiThread->start();
      });

  QObject::connect(watchdog, &FunctionWatchDog::timeout, [&]() {
    result = testing::AssertionFailure()
             << "The Watchdog timed out, even though the kick Timer was "
                "supposed to be on";
    app->exit();
  });
  // Start the Watchdog with max 10ms delay
  watchdog->start(10);
  // After 20ms, stop it keep the thread.
  runAfter(20, [&]() { watchdog->stop(); });
  // Run the event loop,
  // we should not see a timeout.
  runEventLoopForMs(100);
  EXPECT_TRUE(result);
}

TEST_F(SentryWatchDogTest, TimesOutCrossThread) {
  /**
   * @brief In this Test Case we will "runOnUIThread" on a new thread.
   * Therefore the Watchdog should never hit timeout during the
   * execution of the event loop
   */
  testing::AssertionResult result = testing::AssertionFailure();

  QPointer<QThread> uiThread = nullptr;
  auto watchdog =
      new FunctionWatchDog(app, [&](std::function<void()> runnable) {
        uiThread = new WorkerThread(runnable);
        uiThread->start();
      });

  QObject::connect(watchdog, &FunctionWatchDog::timeout, [&]() {
    result = testing::AssertionSuccess();
    app->exit();
  });
  // Start the Watchdog with max 10ms delay
  watchdog->start(10);
  // After 20ms, stop the thread
  runAfter(20, [&]() { uiThread->terminate(); });
  // Run the event loop,
  // we should see a timeout.
  runEventLoopForMs(100);
  EXPECT_TRUE(result);
}
