/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_RPC_CLIENT_H
#define CCRAFT_RPC_CLIENT_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <memory>
#include <condition_variable>
#include <set>

#include "../../common/timer.h"
#include "../../common/spin-lock.h"
#include "../../common/resource-pool.h"
#include "../../net/common-def.h"
#include "../../iservice.h"
#include "../imessage-handler.h"
#include "../../common/thread-pool.h"

namespace google {
namespace protobuf {
class Message;
}
}

namespace ccraft {
namespace common {
class MemPool;
}
namespace net {
class ISocketService;
class NotifyMessage;
}
namespace rpc {
/**
 * [Message format]
 *    client -> server :  |net common header(Message::HeaderSize() bytes)|msg type|[handler id(2bytes)|protobuf msg(n bytes)]|
 *    server -> client :  |net common header(Message::HeaderSize() bytes)|msg type|[rpc code(2bytes)|protobuf msg(n bytes or 0 bytes if no return value)]|
 */
/**
 * Rpc client base class.
 */
class ARpcClientSync : public IService, public IMessageHandler {
public:
    /**
     *
     * @param workThreadsCnt
     * @param timeout
     * @param memPool 如果为空，则内部自己构造。
     */
    ARpcClientSync(net::ISocketService *ss, common::cctime_t timeout, uint16_t workThreadsCnt, common::MemPool *memPool = nullptr);
    virtual ~ARpcClientSync();

    virtual bool Start();
    virtual bool Stop();

    void HandleMessage(std::shared_ptr<net::NotifyMessage> sspNM) override;

protected:
    class RpcCtx {
    public:
        enum class State {
            Ok          = 0,
            Timeout     = 1,
            BrokenPipe  = 2
        };
    public:
        RpcCtx() {
            cv = new std::condition_variable();
            mtx = new std::mutex();
        }

        ~RpcCtx() {
            DELETE_PTR(mtx);
            DELETE_PTR(cv);
        }

        void Release() {
            ssp_nm.reset();
        }

    public:
        std::condition_variable             *cv        = nullptr;
        std::mutex                          *mtx       = nullptr;
        net::net_peer_info_t                 peer;
        uint16_t                             handlerId = 0;
        uint64_t                             msgId     = 0;
        bool                                 complete  = false;
        State                                state     = State::Ok;
        common::Timer::EventId               timer_ev;
        std::shared_ptr<net::NotifyMessage>  ssp_nm;
    };

protected:
    bool registerRpc(std::string &&rpcName, uint16_t id);
    void finishRegisterRpc();
    /**
     *
     * @param rpcName
     * @param msg
     * @return nullptr为失败，否则为成功。作为recvMessage的入参。
     */
    RpcCtx* sendMessage(std::string &&rpcName, std::shared_ptr<google::protobuf::Message> msg, net::net_peer_info_t &&peer);
    std::shared_ptr<net::NotifyMessage> recvMessage(RpcCtx* rc);

private:
    void proc_recv_msg(std::shared_ptr<net::NotifyMessage> sspNM);

private:
    bool                                                           m_bStopped          = true;
    // 关联关系，无需本类释放。
    net::ISocketService                                           *m_pSocketService    = nullptr;
    bool                                                           m_bRegistered       = false;
    std::unordered_map<std::string, uint16_t>                      m_hmapRpcs;
    std::unordered_map<uint64_t, RpcCtx*>                          m_hmapRpcCtxs;
    std::unordered_map<net::net_peer_info_t, std::set<RpcCtx*>>    m_hmapPeerRpcs;
    common::spin_lock_t                                            m_slRpcCtxs = UNLOCKED;
    common::cctime_t                                               m_timeout;
    bool                                                           m_bOwnPool = false;
    common::MemPool                                               *m_pMemPool;
    common::ResourcePool<RpcCtx>                                   m_rpcCtxPool = common::ResourcePool<RpcCtx>(200);
    uint16_t                                                       m_iWorkThreadsCnt       = 0;
    common::ThreadPool<std::shared_ptr<net::NotifyMessage>>       *m_pWorkThreadPool = nullptr;
}; // class ARpcClientSync
} // namespace rpc
} // namespace ccraft

#endif //CCRAFT_RPC_CLIENT_H
