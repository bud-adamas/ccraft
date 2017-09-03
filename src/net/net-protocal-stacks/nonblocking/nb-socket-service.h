/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_NET_CORE_NBSOCKETSERVICE_H
#define CCRAFT_NET_CORE_NBSOCKETSERVICE_H

#include <memory>
#include <cassert>

#include "../inet-stack-worker-manager.h"
#include "../../abstract-socket-service.h"
#include "socket/event-drivers/ievent-driver.h"
#include "../../message.h"
#include "../../notify-message.h"

// TODO(sunchao): 可配值
#define MAX_EVENTS  256

namespace ccraft {
namespace net {
class AEventManager;

/**
 * 支持Tcp/Udp(暂未实现)协议的收发server。
 * TODO(sunchao): 扩展为可listen多个端口？
 */
class NBSocketService : public ASocketService {
public:
    /**
     *
     * @param nlt 如果为空，则是为仅仅一个服务于client的服务，否则为server信息，会开启server的服务。
     * @param sspMgr  worker的管理策略。
     * @param memPool 内存池。
     * @param sspMgr 所有权归本类所有，用户不可以保留。
     */
    NBSocketService(SocketProtocal sp, std::shared_ptr<net_addr_t> sspNat, uint16_t logicPort,
                    std::shared_ptr<INetStackWorkerManager> sspMgr,
                    common::MemPool *memPool, NotifyMessageCallbackHandler msgCallbackHandler);

    ~NBSocketService() override;

    /**
     * 开启服务。
     * @param m 模式。
     * @return 成功true,失败false.
     */
    bool Start(uint16_t ioThreadsCnt, NonBlockingEventModel m) override;

    /**
     * 一旦stop，就不能再用了(不可以重新start再用)。
     * @return
     */
    bool Stop() override;

    /**
     * 一旦发送成功，则m的所有权便属于了框架，user无需也不可以再管理此SndMessage，m生命周期由框架控制。
     * 如果发送失败，则m的生命周期由调用者控制。
     * @param m
     * @return
     */
    bool SendMessage(SndMessage *m) override;

private:
    bool connect(net_peer_info_t &npt);
    void on_stack_connect(AFileEventHandler *handler);
    bool on_logic_connect(AFileEventHandler *handler);
    void on_finish(AFileEventHandler *handler);

private:
    uint16_t                                           m_iLogicPort = 0;
    std::shared_ptr<INetStackWorkerManager>            m_pNetStackWorkerManager = nullptr;
    // 关联关系，外部传入的，根据谁创建谁销毁原则，本类无需释放。
    common::MemPool                                   *m_pMemPool = nullptr;
    AEventManager                                     *m_pEventManager = nullptr;
    NotifyMessageCallbackHandler                       m_msgCallback;
    bool                                               m_bStopped = true;


}; // class NBSocketService
}  // namespace net
} // namespace ccraft

#endif //CCRAFT_NET_CORE_NBSOCKETSERVICE_H
