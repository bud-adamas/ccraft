/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <google/protobuf/message.h>

#include "response.h"
#include "protobuf-utils.h"

namespace ccraft {
namespace rpc {
uint32_t RpcResponse::getDerivePayloadLength() {
    if (m_pMsg.get()) {
        return sizeof(CodeType) + m_pMsg->ByteSize();
    } else {
        return sizeof(CodeType);
    }
}

void RpcResponse::encodeDerive(common::Buffer *b) {
    ByteOrderUtils::WriteUInt16(b->GetPos(), (CodeType)m_code);
    b->MoveHeadBack(sizeof(CodeType));
    if (m_pMsg.get()) {
        ProtoBufUtils::Serialize(m_pMsg.get(), b);
    }
}

uint32_t RpcErrorResponse::getDerivePayloadLength() {
    return sizeof(CodeType);
}

void RpcErrorResponse::encodeDerive(common::Buffer *b) {
    ByteOrderUtils::WriteUInt16(b->GetPos(), (CodeType)m_code);
}
} // namespace rpc
} // namespace ccraft