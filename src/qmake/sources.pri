# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

SOURCES += \
        apppermission.cpp \
        authenticationlistener.cpp \
        authenticationinapp/authenticationinapp.cpp \
        authenticationinapp/authenticationinapplistener.cpp \
        authenticationinapp/authenticationinappsession.cpp \
        authenticationinapp/incrementaldecoder.cpp \
        captiveportal/captiveportal.cpp \
        captiveportal/captiveportaldetection.cpp \
        captiveportal/captiveportaldetectionimpl.cpp \
        captiveportal/captiveportalmonitor.cpp \
        captiveportal/captiveportalnotifier.cpp \
        captiveportal/captiveportalrequest.cpp \
        captiveportal/captiveportalrequesttask.cpp \
        closeeventhandler.cpp \
        collator.cpp \
        command.cpp \
        commandlineparser.cpp \
        commands/commandactivate.cpp \
        commands/commanddeactivate.cpp \
        commands/commanddevice.cpp \
        commands/commandlogin.cpp \
        commands/commandlogout.cpp \
        commands/commandselect.cpp \
        commands/commandservers.cpp \
        commands/commandstatus.cpp \
        commands/commandui.cpp \
        connectionbenchmark/benchmarktask.cpp \
        connectionbenchmark/benchmarktaskdownload.cpp \
        connectionbenchmark/benchmarktaskping.cpp \
        connectionbenchmark/connectionbenchmark.cpp \
        connectionhealth.cpp \
        constants.cpp \
        controller.cpp \
        cryptosettings.cpp \
        curve25519.cpp \
        dnshelper.cpp \
        dnspingsender.cpp \
        errorhandler.cpp \
        filterproxymodel.cpp \
        fontloader.cpp \
        hacl-star/Hacl_Chacha20.c \
        hacl-star/Hacl_Chacha20Poly1305_32.c \
        hacl-star/Hacl_Curve25519_51.c \
        hacl-star/Hacl_Poly1305_32.c \
        hawkauth.cpp \
        hkdf.cpp \
        iaphandler.cpp \
        imageproviderfactory.cpp \
        inspector/inspectorhandler.cpp \
        inspector/inspectoritempicker.cpp \
        inspector/inspectorutils.cpp \
        inspector/inspectorwebsocketconnection.cpp \
        inspector/inspectorwebsocketserver.cpp \
        ipaddress.cpp \
        ipaddresslookup.cpp \
        itempicker.cpp \
        leakdetector.cpp \
        localizer.cpp \
        logger.cpp \
        loghandler.cpp \
        logoutobserver.cpp \
        main.cpp \
        models/device.cpp \
        models/devicemodel.cpp \
        models/feature.cpp \
        models/featuremodel.cpp \
        models/feedbackcategorymodel.cpp \
        models/guide.cpp \
        models/guideblock.cpp \
        models/guidemodel.cpp \
        models/helpmodel.cpp \
        models/keys.cpp \
        models/licensemodel.cpp \
        models/server.cpp \
        models/servercity.cpp \
        models/servercountry.cpp \
        models/servercountrymodel.cpp \
        models/serverdata.cpp \
        models/supportcategorymodel.cpp \
        models/survey.cpp \
        models/surveymodel.cpp \
        models/tutorial.cpp \
        models/tutorialmodel.cpp \
        models/tutorialstep.cpp \
        models/tutorialstepbefore.cpp \
        models/tutorialstepnext.cpp \
        models/user.cpp \
        models/whatsnewmodel.cpp \
        mozillavpn.cpp \
        networkmanager.cpp \
        networkrequest.cpp \
        networkwatcher.cpp \
        notificationhandler.cpp \
        pinghelper.cpp \
        pingsender.cpp \
        pingsenderfactory.cpp \
        platforms/dummy/dummyapplistprovider.cpp \
        platforms/dummy/dummyiaphandler.cpp \
        platforms/dummy/dummynetworkwatcher.cpp \
        platforms/dummy/dummypingsender.cpp \
        qmlengineholder.cpp \
        releasemonitor.cpp \
        rfc/rfc1112.cpp \
        rfc/rfc1918.cpp \
        rfc/rfc4193.cpp \
        rfc/rfc4291.cpp \
        rfc/rfc5735.cpp \
        serveri18n.cpp \
        settingsholder.cpp \
        simplenetworkmanager.cpp \
        statusicon.cpp \
        tasks/account/taskaccount.cpp \
        tasks/adddevice/taskadddevice.cpp \
        tasks/authenticate/taskauthenticate.cpp \
        tasks/captiveportallookup/taskcaptiveportallookup.cpp \
        tasks/deleteaccount/taskdeleteaccount.cpp \
        tasks/getfeaturelist/taskgetfeaturelist.cpp \
        tasks/controlleraction/taskcontrolleraction.cpp \
        tasks/createsupportticket/taskcreatesupportticket.cpp \
        tasks/function/taskfunction.cpp \
        tasks/group/taskgroup.cpp \
        tasks/heartbeat/taskheartbeat.cpp \
        tasks/ipfinder/taskipfinder.cpp \
        tasks/products/taskproducts.cpp \
        tasks/release/taskrelease.cpp \
        tasks/removedevice/taskremovedevice.cpp \
        tasks/sendfeedback/tasksendfeedback.cpp \
        tasks/servers/taskservers.cpp \
        tasks/surveydata/tasksurveydata.cpp \
        taskscheduler.cpp \
        telemetry.cpp \
        theme.cpp \
        timersingleshot.cpp \
        update/updater.cpp \
        update/versionapi.cpp \
        urlopener.cpp \
        websockethandler.cpp

HEADERS += \
        appimageprovider.h \
        apppermission.h \
        applistprovider.h \
        authenticationlistener.h \
        authenticationinapp/authenticationinapp.h \
        authenticationinapp/authenticationinapplistener.h \
        authenticationinapp/authenticationinappsession.h \
        authenticationinapp/incrementaldecoder.h \
        captiveportal/captiveportal.h \
        captiveportal/captiveportaldetection.h \
        captiveportal/captiveportaldetectionimpl.h \
        captiveportal/captiveportalmonitor.h \
        captiveportal/captiveportalnotifier.h \
        captiveportal/captiveportalrequest.h \
        captiveportal/captiveportalrequesttask.h \
        closeeventhandler.h \
        collator.h \
        command.h \
        commandlineparser.h \
        commands/commandactivate.h \
        commands/commanddeactivate.h \
        commands/commanddevice.h \
        commands/commandlogin.h \
        commands/commandlogout.h \
        commands/commandselect.h \
        commands/commandservers.h \
        commands/commandstatus.h \
        commands/commandui.h \
        connectionbenchmark/benchmarktask.h \
        connectionbenchmark/benchmarktaskdownload.h \
        connectionbenchmark/benchmarktaskping.h \
        connectionbenchmark/benchmarktasksentinel.h \
        connectionbenchmark/connectionbenchmark.h \
        connectionhealth.h \
        constants.h \
        controller.h \
        controllerimpl.h \
        cryptosettings.h \
        curve25519.h \
        dnshelper.h \
        dnspingsender.h \
        errorhandler.h \
        filterproxymodel.h \
        fontloader.h \
        hawkauth.h \
        hkdf.h \
        iaphandler.h \
        imageproviderfactory.h \
        inspector/inspectorhandler.h \
        inspector/inspectoritempicker.h \
        inspector/inspectorutils.h \
        inspector/inspectorwebsocketconnection.h \
        inspector/inspectorwebsocketserver.h \
        ipaddress.h \
        ipaddresslookup.h \
        itempicker.h \
        leakdetector.h \
        localizer.h \
        logger.h \
        loghandler.h \
        logoutobserver.h \
        models/device.h \
        models/devicemodel.h \
        models/feature.h \
        models/featuremodel.h \
        models/feedbackcategorymodel.h \
        models/guide.h \
        models/guideblock.h \
        models/guidemodel.h \
        models/helpmodel.h \
        models/keys.h \
        models/licensemodel.h \
        models/server.h \
        models/servercity.h \
        models/servercountry.h \
        models/servercountrymodel.h \
        models/serverdata.h \
        models/supportcategorymodel.h \
        models/survey.h \
        models/surveymodel.h \
        models/tutorial.h \
        models/tutorialmodel.h \
        models/tutorialstep.h \
        models/tutorialstepbefore.h \
        models/tutorialstepnext.h \
        models/user.h \
        models/whatsnewmodel.h \
        mozillavpn.h \
        networkmanager.h \
        networkrequest.h \
        networkwatcher.h \
        networkwatcherimpl.h \
        notificationhandler.h \
        pinghelper.h \
        pingsender.h \
        pingsenderfactory.h \
        platforms/dummy/dummyapplistprovider.h \
        platforms/dummy/dummyiaphandler.h \
        platforms/dummy/dummynetworkwatcher.h \
        platforms/dummy/dummypingsender.h \
        qmlengineholder.h \
        releasemonitor.h \
        rfc/rfc1112.h \
        rfc/rfc1918.h \
        rfc/rfc4193.h \
        rfc/rfc4291.h \
        rfc/rfc5735.h \
        serveri18n.h \
        settingsholder.h \
        simplenetworkmanager.h \
        statusicon.h \
        task.h \
        tasks/account/taskaccount.h \
        tasks/adddevice/taskadddevice.h \
        tasks/authenticate/taskauthenticate.h \
        tasks/captiveportallookup/taskcaptiveportallookup.h \
        tasks/deleteaccount/taskdeleteaccount.h \
        tasks/getfeaturelist/taskgetfeaturelist.h \
        tasks/controlleraction/taskcontrolleraction.h \
        tasks/createsupportticket/taskcreatesupportticket.h \
        tasks/function/taskfunction.h \
        tasks/group/taskgroup.h \
        tasks/heartbeat/taskheartbeat.h \
        tasks/ipfinder/taskipfinder.h \
        tasks/products/taskproducts.h \
        tasks/release/taskrelease.h \
        tasks/removedevice/taskremovedevice.h \
        tasks/sendfeedback/tasksendfeedback.h \
        tasks/servers/taskservers.h \
        tasks/surveydata/tasksurveydata.h \
        taskscheduler.h \
        telemetry.h \
        theme.h \
        timersingleshot.h \
        update/updater.h \
        update/versionapi.h \
        urlopener.h \
        websockethandler.h

# Signal handling for unix platforms
unix {
    SOURCES += signalhandler.cpp
    HEADERS += signalhandler.h
}

RESOURCES += ui/resources.qrc
RESOURCES += ui/license.qrc
RESOURCES += ui/ui.qrc
RESOURCES += resources/certs/certs.qrc
RESOURCES += ui/guides.qrc
RESOURCES += ui/tutorials.qrc
