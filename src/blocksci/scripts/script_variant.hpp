//
//  script_variant.hpp
//  blocksci
//
//  Created by Harry Kalodner on 10/14/17.
//

#ifndef script_variant_hpp
#define script_variant_hpp

#include <blocksci/blocksci_export.h>
#include "multisig_script.hpp"
#include "nonstandard_script.hpp"
#include "nulldata_script.hpp"
#include "pubkey_script.hpp"
#include "multisig_pubkey_script.hpp"
#include "scripthash_script.hpp"

#include <blocksci/util/util.hpp>

namespace blocksci {
    namespace internal {
        template<AddressType::Enum type>
        struct ScriptCreateFunctor {
            static ScriptVariant f(uint32_t scriptNum, DataAccess &access);
        };
        
        template<AddressType::Enum type>
        struct ScriptDataCreateFunctor {
            static ScriptDataVariant f(uint32_t scriptNum, const ScriptAccess &access);
        };
        
        static constexpr auto scriptCreator = make_dynamic_table<AddressType, ScriptCreateFunctor>();
        static constexpr auto scriptDataCreator = make_dynamic_table<AddressType, ScriptDataCreateFunctor>();
    }
    
    class BLOCKSCI_EXPORT AnyScript {
    public:
        AnyScript() = default;
        AnyScript(const Address &address) : wrapped(internal::scriptCreator.at(static_cast<size_t>(address.type))(address.scriptNum, address.getAccess())) {}
        AnyScript(uint32_t addressNum, AddressType::Enum type, DataAccess &access) : wrapped(internal::scriptCreator.at(static_cast<size_t>(type))(addressNum, access)) {}
        AnyScript(const ScriptVariant &var) : wrapped(var) {}

        uint32_t getScriptNum() const {
            return mpark::visit([&](auto &scriptAddress) { return scriptAddress.getScriptNum(); }, wrapped);
        }

        AddressType::Enum getType() const {
            return mpark::visit([&](auto &scriptAddress) { return scriptAddress.getType(); }, wrapped);
        }
        
        std::string toString() const {
            return mpark::visit([&](auto &scriptAddress) { return scriptAddress.toString(); }, wrapped);
        }
        
        std::string toPrettyString() const {
            return mpark::visit([&](auto &scriptAddress) { return scriptAddress.toPrettyString(); }, wrapped);
        }
        
        void visitPointers(const std::function<void(const Address &)> &func) {
            mpark::visit([&](auto &scriptAddress) { scriptAddress.visitPointers(func); }, wrapped);
        }
        
        uint32_t firstTxIndex() {
            return mpark::visit([&](auto &scriptAddress) { return scriptAddress.getFirstTxIndex(); }, wrapped);
        }
        
        ranges::optional<uint32_t> txRevealedIndex() {
            return mpark::visit([&](auto &scriptAddress) { return scriptAddress.getTxRevealedIndex(); }, wrapped);
        }

        Transaction getFirstTransaction() const;
        ranges::optional<Transaction> getTransactionRevealed() const;

        bool hasBeenSpent() const {
            return mpark::visit([&](auto &scriptAddress) { return scriptAddress.hasBeenSpent(); }, wrapped);
        }
    
        EquivAddress getEquivAddresses(bool nestedEquivalent) const;
        
        auto getOutputPointers() {
            return mpark::visit([&](auto &scriptAddress) { return scriptAddress.getOutputPointers(); }, wrapped);
        }
        
        int64_t calculateBalance(BlockHeight height);
        std::vector<Output> getOutputs();
        std::vector<Input> getInputs();
        std::vector<Transaction> getTransactions();
        std::vector<Transaction> getOutputTransactions();
        std::vector<Transaction> getInputTransactions();
        
        ScriptVariant wrapped;
    };
    
    class BLOCKSCI_EXPORT AnyScriptData {
    public:
        AnyScriptData(const RawAddress &address, const ScriptAccess &access) : wrapped(internal::scriptDataCreator.at(static_cast<size_t>(address.type))(address.scriptNum, access)) {}
        
        void visitPointers(const std::function<void(const RawAddress &)> &func) {
            mpark::visit([&](auto &scriptAddress) { scriptAddress.data->visitPointers(func); }, wrapped);
        }

        ScriptDataVariant wrapped;
    };
    
    namespace internal {
        template<AddressType::Enum type>
        ScriptVariant ScriptCreateFunctor<type>::f(uint32_t scriptNum, DataAccess &access) {
            return ScriptAddress<type>(scriptNum, access);
        }
        
        template<AddressType::Enum type>
        ScriptDataVariant ScriptDataCreateFunctor<type>::f(uint32_t scriptNum, const ScriptAccess &access) {
            auto &file = access.getFile<dedupType(type)>();
            return ScriptWrapper<type>{file.getDataAtIndex(scriptNum - 1)};
        }
    }
} // namespace blocksci

#endif /* script_variant_hpp */
