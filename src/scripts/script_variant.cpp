
//
//  script_variant.cpp
//  blocksci
//
//  Created by Harry Kalodner on 4/11/18.
//

#include <blocksci/scripts/script_variant.hpp>
#include <blocksci/chain/transaction.hpp>
#include <blocksci/chain/input.hpp>
#include <blocksci/chain/output.hpp>
#include <blocksci/address/equiv_address.hpp>

#include <internal/address_info.hpp>
#include <internal/data_access.hpp>
#include <internal/script_access.hpp>

namespace {
    template<blocksci::AddressType::Enum type>
    struct ScriptCreateFunctor {
        static blocksci::ScriptVariant f(uint32_t scriptNum, blocksci::DataAccess &access) {
            return blocksci::ScriptAddress<type>(scriptNum, access.getScripts().getScriptData<dedupType(type)>(scriptNum), access);
        }
    };
    
    static constexpr auto scriptCreator = blocksci::make_dynamic_table<blocksci::AddressType, ScriptCreateFunctor>();
}

namespace blocksci {

    AnyScript::AnyScript(const Address &address) : wrapped(scriptCreator.at(static_cast<size_t>(address.type))(address.scriptNum, address.getAccess())) {}
    AnyScript::AnyScript(uint32_t addressNum, AddressType::Enum type, DataAccess &access) : wrapped(scriptCreator.at(static_cast<size_t>(type))(addressNum, access)) {}
    
	Transaction AnyScript::getFirstTransaction() const {
		return mpark::visit([&](auto &scriptAddress) { return scriptAddress.getFirstTransaction(); }, wrapped);
	}

    ranges::optional<Transaction> AnyScript::getTransactionRevealed() const {
    	return mpark::visit([&](auto &scriptAddress) { return scriptAddress.getTransactionRevealed(); }, wrapped);
    }

	EquivAddress AnyScript::getEquivAddresses(bool nestedEquivalent) const {
        return mpark::visit([&](auto &scriptAddress) { return scriptAddress.getEquivAddresses(nestedEquivalent); }, wrapped);
    }

	int64_t AnyScript::calculateBalance(BlockHeight height) {
		return mpark::visit([&](auto &scriptAddress) { return scriptAddress.calculateBalance(height); }, wrapped);
	}
    
    ranges::any_view<Output> AnyScript::getOutputs() {
    	return mpark::visit([&](auto &scriptAddress) { return scriptAddress.getOutputs(); }, wrapped);
    }

    std::vector<Input> AnyScript::getInputs() {
    	return mpark::visit([&](auto &scriptAddress) { return scriptAddress.getInputs(); }, wrapped);
    }

    std::vector<Transaction> AnyScript::getTransactions() {
    	return mpark::visit([&](auto &scriptAddress) { return scriptAddress.getTransactions(); }, wrapped);
    }

    std::vector<Transaction> AnyScript::getOutputTransactions() {
    	return mpark::visit([&](auto &scriptAddress) { return scriptAddress.getOutputTransactions(); }, wrapped);
    }

    std::vector<Transaction> AnyScript::getInputTransactions() {
    	return mpark::visit([&](auto &scriptAddress) { return scriptAddress.getInputTransactions(); }, wrapped);
    }
}
