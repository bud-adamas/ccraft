/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_INODE_INTERNAL_COMMUNICATOR_H
#define CCRAFT_INODE_INTERNAL_COMMUNICATOR_H

#include "../../rpc/common-def.h"

namespace ccraft {
namespace server {
class INodeInternalRpcHandler {
public:
    virtual ~INodeInternalRpcHandler() = default;
    virtual rpc::SP_PB_MSG OnAppendRfLog(rpc::SP_PB_MSG sspMsg) = 0;
}; // class IMessageHandler
} // namespace rpc
} // namespace ccraft

#endif //CCRAFT_INODE_INTERNAL_COMMUNICATOR_H
