#pragma once

#include "db/ProxyEntity.hpp"
#include "db/traffic/TrafficData.hpp"

namespace NekoRay::fmt {
    class BuildConfigResult {
    public:
        QString error;
        QJsonObject coreConfig;

        QList<QSharedPointer<traffic::TrafficData>> outboundStats; // all, but not including direct
        QSharedPointer<traffic::TrafficData> outboundStat; // main
    };

    class BuildConfigStatus {
    public:
        QSharedPointer<BuildConfigResult> result;

        QJsonArray domainListDNSRemote;
        QJsonArray domainListDNSDirect;
        QJsonArray domainListRemote;
        QJsonArray domainListDirect;
        QJsonArray ipListRemote;
        QJsonArray ipListDirect;

        QJsonArray routingRules;
        QJsonObject hosts;

        QJsonArray inbounds;
        QJsonArray outbounds;

        QList<int> globalProfiles;
    };

    QSharedPointer<BuildConfigResult> BuildConfig(const QSharedPointer<ProxyEntity> &ent, bool forTest);

    QString BuildChain(bool extra, const QList<QSharedPointer<ProxyEntity>> &ents,
                       const QSharedPointer<BuildConfigStatus> &status);
}
