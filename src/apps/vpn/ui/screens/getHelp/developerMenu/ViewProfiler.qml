/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import QtQuick 2.5
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

import Mozilla.Shared 1.0
import Mozilla.VPN 1.0
import components 0.1
import components.forms 0.1


Item {
    id: root
    MZMenu {
        id: menu
        // Do not translate this string!
        title: "Profiler"
        _menuOnBackClicked: () => getHelpStackView.pop()
    }

    ColumnLayout {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: menu.bottom
        anchors.topMargin: MZTheme.theme.listSpacing * 3
        spacing: MZTheme.theme.listSpacing * 2
        width: parent.width - MZTheme.theme.windowMargin * 2


        RowLayout {
            Layout.fillWidth: true

            MZButton {
                // Do not translate this string!
                text: "start"
                onClicked: () => {
                    Profiler.start()
                }

                Layout.fillWidth: true
            }

            MZButton {
                // Do not translate this string!
                text: "stop"
                onClicked: () => {
                    Profiler.stop();
                }
                Layout.fillWidth: true
            }
        }

    }

}
