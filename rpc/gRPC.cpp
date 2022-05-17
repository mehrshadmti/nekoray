#include "gRPC.h"

#include <utility>

#ifndef NKR_NO_GRPC

namespace NekoRay::rpc {
    Client::Client(std::function<void(const QString &)> onError, const QString &target, const QString &token) {
        auto channel = grpc::CreateChannel(target.toStdString(), grpc::InsecureChannelCredentials());
        this->_stub = libcore::LibcoreService::NewStub(channel);
        this->onError = std::move(onError);
        this->token = token;
    }

#define MAKE_CONTEXT auto context = grpc::ClientContext(); context.AddMetadata("nekoray_auth", token.toStdString());
#define NOT_OK *rpcOK = false; \
    onError( \
            QString("error code: %1, error message: %2\n").arg(status.error_code()).arg( \
            status.error_message().c_str()) \
    );

    void Client::Exit() {
        MAKE_CONTEXT
        libcore::EmptyReq request;
        libcore::EmptyResp reply;
        grpc::Status status = _stub->Exit(&context, request, &reply);
    }

    QString Client::Start(bool *rpcOK, const QString &coreConfig) {
        {
            MAKE_CONTEXT
            libcore::LoadConfigReq request;
            request.set_coreconfig(coreConfig.toStdString());

            libcore::ErrorResp reply;
            grpc::Status status = _stub->Start(&context, request, &reply);

            if (status.ok()) {
                *rpcOK = true;
                return {reply.error().c_str()};
            } else {
                NOT_OK
                return "";
            }
        }
    }

    QString Client::Stop(bool *rpcOK) {
        {
            MAKE_CONTEXT
            libcore::EmptyReq request;

            libcore::ErrorResp reply;
            grpc::Status status = _stub->Stop(&context, request, &reply);

            if (status.ok()) {
                *rpcOK = true;
                return {reply.error().c_str()};
            } else {
                NOT_OK
                return "";
            }
        }
    }

    bool Client::keepAlive() {
        MAKE_CONTEXT
        libcore::EmptyReq request;

        libcore::EmptyResp reply;
        grpc::Status status = _stub->KeepAlive(&context, request, &reply);

        if (status.ok()) {
            return true;
        }
        return false;
    }

    libcore::TestResp Client::Test(bool *rpcOK, const libcore::TestReq &request) {
        MAKE_CONTEXT
        libcore::TestResp reply;
        grpc::Status status = _stub->Test(&context, request, &reply);

        if (!status.ok()) {
            NOT_OK
            return reply;
        }

        *rpcOK = true;
        return reply;
    }

    long long Client::QueryStats(bool *rpcOK, const std::string &tag, const std::string &direct) {
        MAKE_CONTEXT
        libcore::QueryStatsReq request;
        request.set_tag(tag);
        request.set_direct(direct);

        libcore::QueryStatsResp reply;
        grpc::Status status = _stub->QueryStats(&context, request, &reply);

        if (!status.ok()) {
            NOT_OK
            return 0;
        }

        *rpcOK = true;
        return reply.traffic();
    }
}

#endif