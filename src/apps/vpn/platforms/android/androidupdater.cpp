/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "androidupdater.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>


#include "update/versionapi.h"
#include "androidversionapi.h"
#include "leakdetector.h"
#include "logger.h"
#include "task.h"


namespace {
Logger logger("AndroidUpdater");

constexpr auto SEVERE_UPDATE_LEVEL = 5;
}  // namespace

AndroidUpdater::AndroidUpdater(QObject* parent) : Updater(parent) {
  MZ_COUNT_CTOR(AndroidUpdater);
  logger.debug() << "AndroidUpdater created";
  connect(AndroidVersionAPI::Instance(), &AndroidVersionAPI::onUpdateResult,
          this, &AndroidUpdater::onUpdateData);  
}

AndroidUpdater::~AndroidUpdater() {
  MZ_COUNT_DTOR(AndroidUpdater);
  logger.debug() << "AndroidUpdater released";
}

void AndroidUpdater::start(Task*) {
  logger.info() << "Requested fresh update info";
  // Create a "legacy" VersionAPI check
  // we forward "updateRequired" so that we still can force 
  // update old versions. 
  auto fallBack = new VersionApi(this);
  connect(fallBack, &VersionApi::updateRequired, this, &AndroidUpdater::updateRequired);
  AndroidVersionAPI::Instance()->requestUpdateInfo();
}
/**
 * @brief Consumes the
 *
 * @param data - JSON data of VPNUpdater.UpdateResult
 */
void AndroidUpdater::onUpdateData(const QString& data) {
  if (data.isEmpty()) {
    logger.error() << "Failed to fetch android update data";
  }
  QJsonDocument json = QJsonDocument::fromJson(data.toLocal8Bit());
  if (!json.isObject()) {
    logger.error() << "Invalid JSON - object expected";
    return;
  }
  auto result = json.object();
  if (!result.contains("isUpdateAvailable")) {
    logger.info() << "No update available.";
    return;
  }

  bool isUpdateAvailable = result["isUpdateAvailable"].toBool();
  if (!isUpdateAvailable) {
    return;
  }
  int updateLevel = result["updateLevel"].toInt(0);
  int versionCode = result["versionCode"].toInt(0);

  logger.info() << "Found Update to APK Version" << versionCode;

  if (updateLevel >= SEVERE_UPDATE_LEVEL) {
    logger.info() << "Update is severe";
    emit updateRequired();
    return;
  }
  logger.info() << "Update is optional";
  emit updateRecommended();
}
