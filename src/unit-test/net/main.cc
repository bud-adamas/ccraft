/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <gtest/gtest.h>

#include "../../common/common-def.h"
#include "./tcp/server/tcp-server-test-case.h"
#include "../../net/common-def.h"
#include "../../common/mem-pool.h"
#include "../../net/net-protocal-stacks/inet-stack-worker-manager.h"
#include "../../net/net-protocal-stacks/msg-worker-managers/unique-worker-manager.h"
#include "../../net/socket-service-factory.h"
#include "../../net/rcv-message.h"
#include "../../common/buffer.h"
#include "tcp/test-snd-message.h"

void recv_msg(std::shared_ptr<ccraft::net::NotifyMessage> sspNM);

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    ccraft::common::initialize();
    ccraft::test::TcpServerTest::Run();

    auto res = RUN_ALL_TESTS();

    getchar();
    return res;
}

TEST(NetTest, ServerTest) {
    ccraft::net::net_peer_info_t peerInfo = {
        {
            .addr = "localhost",
            .port = 2210
        },
        .sp = ccraft::net::SocketProtocal::Tcp
    };

    std::shared_ptr<ccraft::net::net_addr_t> ssp_npt(nullptr);
    std::shared_ptr<ccraft::net::INetStackWorkerManager> sspMgr1 =
        std::shared_ptr<ccraft::net::INetStackWorkerManager>(new ccraft::net::UniqueWorkerManager());
    auto netService1 = ccraft::net::SocketServiceFactory::CreateService(ccraft::net::SocketProtocal::Tcp, ssp_npt, 2210, ccraft::common::g_pMemPool,
                                                                       std::bind(recv_msg, std::placeholders::_1),
                                                                       sspMgr1);
    EXPECT_EQ(netService1->Start(2, ccraft::net::NonBlockingEventModel::Posix), true);
    auto *tsm11 = new ccraft::test::TestSndMessage(ccraft::common::g_pMemPool, ccraft::net::net_peer_info_t(peerInfo), "1-1 ---client request: hello server!");
    EXPECT_EQ(netService1->SendMessage(tsm11), true);

    std::shared_ptr<ccraft::net::INetStackWorkerManager> sspMgr2 =
        std::shared_ptr<ccraft::net::INetStackWorkerManager>(new ccraft::net::UniqueWorkerManager());
    auto netService2 = ccraft::net::SocketServiceFactory::CreateService(ccraft::net::SocketProtocal::Tcp, ssp_npt, 2210, ccraft::common::g_pMemPool,
                                                                        std::bind(recv_msg, std::placeholders::_1),
                                                                        sspMgr2);
    EXPECT_EQ(netService2->Start(2, ccraft::net::NonBlockingEventModel::Posix), true);
    auto *tsm2 = new ccraft::test::TestSndMessage(ccraft::common::g_pMemPool, ccraft::net::net_peer_info_t(peerInfo), "2 ---client request: hello server!");
    EXPECT_EQ(netService2->SendMessage(tsm2), false);
    DELETE_PTR(tsm2);
    auto *tsm12 = new ccraft::test::TestSndMessage(ccraft::common::g_pMemPool, ccraft::net::net_peer_info_t(peerInfo), "1-2 ---client request: hello server!");
    EXPECT_EQ(netService1->SendMessage(tsm12), true);

    std::shared_ptr<ccraft::net::INetStackWorkerManager> sspMgr3 =
        std::shared_ptr<ccraft::net::INetStackWorkerManager>(new ccraft::net::UniqueWorkerManager());
    auto netService3 = ccraft::net::SocketServiceFactory::CreateService(ccraft::net::SocketProtocal::Tcp, ssp_npt, 2211, ccraft::common::g_pMemPool,
                                                                        std::bind(recv_msg, std::placeholders::_1),
                                                                        sspMgr3);

    EXPECT_EQ(netService3->Start(2, ccraft::net::NonBlockingEventModel::Posix), true);

    auto *tsm3 = new ccraft::test::TestSndMessage(ccraft::common::g_pMemPool, ccraft::net::net_peer_info_t(peerInfo), "3 ---client request: hello server!");
    EXPECT_EQ(netService1->SendMessage(tsm3), true);

    for (int i = 0; i < 100; ++i) {
        std::stringstream ss;
        ss << "1-" << i + 3 << " ---client request: hello server!";
        auto *tsm13 = new ccraft::test::TestSndMessage(ccraft::common::g_pMemPool, ccraft::net::net_peer_info_t(peerInfo), ss.str());
        EXPECT_EQ(netService1->SendMessage(tsm13), true);
        usleep(1000 * 50);
    }
}

void recv_msg(std::shared_ptr<ccraft::net::NotifyMessage> sspNM) {
    switch (sspNM->GetType()) {
        case ccraft::net::NotifyMessageType::Message: {
            ccraft::net::MessageNotifyMessage *mnm = dynamic_cast<ccraft::net::MessageNotifyMessage*>(sspNM.get());
            auto rm = mnm->GetContent();
            if (rm) {
                auto respBuf = rm->GetDataBuffer();
#ifdef WITH_MSG_ID
                #ifdef BULK_MSG_ID
                        std::cout << "response = "  << respBuf->Pos << ", " << "message id is { ts = " << rm->GetId().ts
                                  << ", seq = " << rm->GetId().seq << "}" << std::endl;
#else
                        std::cout << "response = "  << respBuf->GetPos() << ", " << "message id is " << rm->GetId() << "." << std::endl;
#endif
#else
                std::cout << "response = "  << respBuf->Pos << "." << std::endl;
#endif
            }
            break;
        }
        case ccraft::net::NotifyMessageType::Worker: {
            ccraft::net::WorkerNotifyMessage *wnm = dynamic_cast<ccraft::net::WorkerNotifyMessage*>(sspNM.get());
            if (wnm) {
                std::cout << "worker notify message , rc = " << (int)wnm->GetCode() << ", message = " << wnm->What() << std::endl;
            }
            break;
        }
        case ccraft::net::NotifyMessageType::Server: {
            ccraft::net::ServerNotifyMessage *snm = dynamic_cast<ccraft::net::ServerNotifyMessage*>(sspNM.get());
            if (snm) {
                std::cout << "server notify message , rc = " << (int)snm->GetCode() << ", message = " << snm->What() << std::endl;
            }
            break;
        }
    }
}
