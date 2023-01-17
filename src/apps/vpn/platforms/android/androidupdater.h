/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ANDROIDUPDATER_H
#define ANDROIDUPDATER_H

#include "update/updater.h"



/**
 * @brief This Class implements the Updater against the Google Play API.
 * 
 * 
 * on Start it will check with google play if there is an update available.
 * It will emit: 
 *  updateAvailable if there is an update 
 * 
 *  updateReqiured will be emitted if the Severty Level of the Update is Level 5!
 *  
 * calling update will prompt the google update screen. 
 * 
 * For details check the AndroidVersionAPI class. 
 */
class AndroidUpdater final : public Updater {
  Q_DISABLE_COPY_MOVE(AndroidUpdater)

 public:
  AndroidUpdater(QObject* parent);
  ~AndroidUpdater();

  void start(Task* task) override;


  void onUpdateData(const QString& data);
};

#endif  // ANDROIDUPDATER_H
