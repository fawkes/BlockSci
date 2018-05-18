//
//  multisig_pubkey_script.hpp
//  blocksci
//
//  Created by Harry Kalodner on 4/10/18.
//

#ifndef multisig_pubkey_script_hpp
#define multisig_pubkey_script_hpp

#include "pubkey_base_script.hpp"

#include <blocksci/blocksci_export.h>

namespace blocksci {
    template <>
    class BLOCKSCI_EXPORT ScriptAddress<AddressType::MULTISIG_PUBKEY> : public PubkeyAddressBase {
    public:
        constexpr static AddressType::Enum addressType = AddressType::MULTISIG_PUBKEY;
        
        ScriptAddress(uint32_t addressNum_, const PubkeyData *data_, DataAccess &access_) : PubkeyAddressBase(addressNum_, addressType, data_, access_) {}
        ScriptAddress(uint32_t addressNum_, DataAccess &access_);
        
        std::string addressString() const;
        
        std::string toString() const;
        
        std::string toPrettyString() const {
            return addressString();
        }
    };
} // namespace blocksci

#endif /* multisig_pubkey_script_hpp */
