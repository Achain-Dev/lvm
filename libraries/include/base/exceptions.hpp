#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

#include <fc/exception/exception.hpp>

namespace lvm {
    namespace global_exception {
        FC_DECLARE_EXCEPTION(blockchain_exception, 30000, "Blockchain Exception");
        FC_DECLARE_DERIVED_EXCEPTION(invalid_pts_address, lvm::global_exception::blockchain_exception, 30001, "invalid pts address");
        FC_DECLARE_DERIVED_EXCEPTION(addition_overflow, lvm::global_exception::blockchain_exception, 30002, "addition overflow");
        FC_DECLARE_DERIVED_EXCEPTION(subtraction_overflow, lvm::global_exception::blockchain_exception, 30003, "subtraction overflow");
        FC_DECLARE_DERIVED_EXCEPTION(asset_type_mismatch, lvm::global_exception::blockchain_exception, 30004, "asset/price mismatch");
        FC_DECLARE_DERIVED_EXCEPTION(unsupported_chain_operation, lvm::global_exception::blockchain_exception, 30005, "unsupported chain operation");
        FC_DECLARE_DERIVED_EXCEPTION(unknown_transaction, lvm::global_exception::blockchain_exception, 30006, "unknown transaction");
        FC_DECLARE_DERIVED_EXCEPTION(duplicate_transaction, lvm::global_exception::blockchain_exception, 30007, "duplicate transaction");
        FC_DECLARE_DERIVED_EXCEPTION(zero_amount, lvm::global_exception::blockchain_exception, 30008, "zero amount");
        FC_DECLARE_DERIVED_EXCEPTION(zero_price, lvm::global_exception::blockchain_exception, 30009, "zero price");
        FC_DECLARE_DERIVED_EXCEPTION(asset_divide_by_self, lvm::global_exception::blockchain_exception, 30010, "asset divide by self");
        FC_DECLARE_DERIVED_EXCEPTION(asset_divide_by_zero, lvm::global_exception::blockchain_exception, 30011, "asset divide by zero");
        FC_DECLARE_DERIVED_EXCEPTION(new_database_version, lvm::global_exception::blockchain_exception, 30012, "new database version");
        FC_DECLARE_DERIVED_EXCEPTION(unlinkable_block, lvm::global_exception::blockchain_exception, 30013, "unlinkable block");
        FC_DECLARE_DERIVED_EXCEPTION(price_out_of_range, lvm::global_exception::blockchain_exception, 30014, "price out of range");
        FC_DECLARE_DERIVED_EXCEPTION(illegal_transaction, lvm::global_exception::blockchain_exception, 30015, "illegal transaction");
        FC_DECLARE_DERIVED_EXCEPTION(block_numbers_not_sequential, lvm::global_exception::blockchain_exception, 30016, "block numbers not sequential");
        FC_DECLARE_DERIVED_EXCEPTION(invalid_previous_block_id, lvm::global_exception::blockchain_exception, 30017, "invalid previous block");
        FC_DECLARE_DERIVED_EXCEPTION(invalid_block_time, lvm::global_exception::blockchain_exception, 30018, "invalid block time");
        FC_DECLARE_DERIVED_EXCEPTION(time_in_past, lvm::global_exception::blockchain_exception, 30019, "time is in the past");
        FC_DECLARE_DERIVED_EXCEPTION(time_in_future, lvm::global_exception::blockchain_exception, 30020, "time is in the future");
        FC_DECLARE_DERIVED_EXCEPTION(invalid_block_digest, lvm::global_exception::blockchain_exception, 30021, "invalid block digest");
        FC_DECLARE_DERIVED_EXCEPTION(invalid_delegate_signee, lvm::global_exception::blockchain_exception, 30022, "invalid delegate signee");
        FC_DECLARE_DERIVED_EXCEPTION(failed_checkpoint_verification, lvm::global_exception::blockchain_exception, 30023, "failed checkpoint verification");
        FC_DECLARE_DERIVED_EXCEPTION(wrong_chain_id, lvm::global_exception::blockchain_exception, 30024, "wrong chain id");
        FC_DECLARE_DERIVED_EXCEPTION(unknown_block, lvm::global_exception::blockchain_exception, 30025, "unknown block");
        FC_DECLARE_DERIVED_EXCEPTION(block_older_than_undo_history, lvm::global_exception::blockchain_exception, 30026, "block is older than our undo history allows us to process");
        FC_DECLARE_DERIVED_EXCEPTION(version_error, lvm::global_exception::blockchain_exception, 30027, "Please delete data and use --resync-blockchain ");
        FC_DECLARE_DERIVED_EXCEPTION(invalid_address, lvm::global_exception::blockchain_exception, 30028, "invalid address");
        FC_DECLARE_DERIVED_EXCEPTION(out_of_max_block_size, lvm::global_exception::blockchain_exception, 30029, "out of max block size");
        FC_DECLARE_DERIVED_EXCEPTION(out_of_max_trx_size, lvm::global_exception::blockchain_exception, 30030, "out of max trx size");
        
        FC_DECLARE_EXCEPTION(evaluation_error, 31000, "Evaluation Error");
        FC_DECLARE_DERIVED_EXCEPTION(negative_deposit, lvm::global_exception::evaluation_error, 31001, "negative deposit");
        FC_DECLARE_DERIVED_EXCEPTION(not_a_delegate, lvm::global_exception::evaluation_error, 31002, "not a delegate");
        FC_DECLARE_DERIVED_EXCEPTION(unknown_balance_entry, lvm::global_exception::evaluation_error, 31003, "unknown balance entry");
        FC_DECLARE_DERIVED_EXCEPTION(insufficient_funds, lvm::global_exception::evaluation_error, 31004, "insufficient funds");
        FC_DECLARE_DERIVED_EXCEPTION(missing_signature, lvm::global_exception::evaluation_error, 31005, "missing signature");
        FC_DECLARE_DERIVED_EXCEPTION(invalid_claim_password, lvm::global_exception::evaluation_error, 31006, "invalid claim password");
        FC_DECLARE_DERIVED_EXCEPTION(invalid_withdraw_condition, lvm::global_exception::evaluation_error, 31007, "invalid withdraw condition");
        FC_DECLARE_DERIVED_EXCEPTION(negative_withdraw, lvm::global_exception::evaluation_error, 31008, "negative withdraw");
        FC_DECLARE_DERIVED_EXCEPTION(not_an_active_delegate, lvm::global_exception::evaluation_error, 31009, "not an active delegate");
        FC_DECLARE_DERIVED_EXCEPTION(expired_transaction, lvm::global_exception::evaluation_error, 31010, "expired transaction");
        FC_DECLARE_DERIVED_EXCEPTION(invalid_transaction_expiration, lvm::global_exception::evaluation_error, 31011, "invalid transaction expiration");
        FC_DECLARE_DERIVED_EXCEPTION(oversized_transaction, lvm::global_exception::evaluation_error, 31012, "transaction exceeded the maximum transaction size");
        FC_DECLARE_DERIVED_EXCEPTION(operator_and_owner_not_the_same, lvm::global_exception::evaluation_error, 31013, "operator and owner not the same");
        FC_DECLARE_DERIVED_EXCEPTION(too_much_signature, lvm::global_exception::evaluation_error, 31014, "too much signature");
        FC_DECLARE_DERIVED_EXCEPTION(too_much_balances_withdraw_not_used_up, lvm::global_exception::evaluation_error, 31015, "too much balances withdraw which balance not used up ");
        FC_DECLARE_DERIVED_EXCEPTION(deposit_to_one_address_twice, lvm::global_exception::evaluation_error, 31016, "deposit to one address twice");
        FC_DECLARE_DERIVED_EXCEPTION(too_much_deposit, lvm::global_exception::evaluation_error, 31017, "too much deposit");
        
        FC_DECLARE_DERIVED_EXCEPTION(invalid_account_name, lvm::global_exception::evaluation_error, 31101, "invalid account name");
        FC_DECLARE_DERIVED_EXCEPTION(unknown_account_id, lvm::global_exception::evaluation_error, 31102, "unknown account id");
        FC_DECLARE_DERIVED_EXCEPTION(unknown_account_name, lvm::global_exception::evaluation_error, 31103, "unknown account name");
        FC_DECLARE_DERIVED_EXCEPTION(account_expired, lvm::global_exception::evaluation_error, 31106, "account expired");
        FC_DECLARE_DERIVED_EXCEPTION(account_already_registered, lvm::global_exception::evaluation_error, 31107, "account already registered");
        FC_DECLARE_DERIVED_EXCEPTION(account_key_in_use, lvm::global_exception::evaluation_error, 31108, "account key already in use");
        FC_DECLARE_DERIVED_EXCEPTION(account_retracted, lvm::global_exception::evaluation_error, 31109, "account retracted");
        FC_DECLARE_DERIVED_EXCEPTION(unknown_delegate_slate, lvm::global_exception::evaluation_error, 31111, "unknown delegate slate");
        FC_DECLARE_DERIVED_EXCEPTION(too_may_delegates_in_slate, lvm::global_exception::evaluation_error, 31112, "too many delegates in slate");
        FC_DECLARE_DERIVED_EXCEPTION(pay_balance_remaining, lvm::global_exception::evaluation_error, 31113, "pay balance remaining");
        FC_DECLARE_DERIVED_EXCEPTION(message_too_long, lvm::global_exception::evaluation_error, 31114, "message is to long");
        
        FC_DECLARE_DERIVED_EXCEPTION(not_a_delegate_signature, lvm::global_exception::evaluation_error, 31202, "not delegates signature");
        
        FC_DECLARE_DERIVED_EXCEPTION(invalid_precision, lvm::global_exception::evaluation_error, 31301, "invalid precision");
        FC_DECLARE_DERIVED_EXCEPTION(invalid_asset_symbol, lvm::global_exception::evaluation_error, 31302, "invalid asset symbol");
        FC_DECLARE_DERIVED_EXCEPTION(unknown_asset_id, lvm::global_exception::evaluation_error, 31303, "unknown asset id");
        FC_DECLARE_DERIVED_EXCEPTION(asset_symbol_in_use, lvm::global_exception::evaluation_error, 31304, "asset symbol in use");
        FC_DECLARE_DERIVED_EXCEPTION(invalid_asset_amount, lvm::global_exception::evaluation_error, 31305, "invalid asset amount");
        FC_DECLARE_DERIVED_EXCEPTION(negative_issue, lvm::global_exception::evaluation_error, 31306, "negative issue");
        FC_DECLARE_DERIVED_EXCEPTION(over_issue, lvm::global_exception::evaluation_error, 31307, "over issue");
        FC_DECLARE_DERIVED_EXCEPTION(unknown_asset_symbol, lvm::global_exception::evaluation_error, 31308, "unknown asset symbol");
        FC_DECLARE_DERIVED_EXCEPTION(asset_id_in_use, lvm::global_exception::evaluation_error, 31309, "asset id in use");
        FC_DECLARE_DERIVED_EXCEPTION(not_user_issued, lvm::global_exception::evaluation_error, 31310, "not user issued");
        FC_DECLARE_DERIVED_EXCEPTION(invalid_asset_name, lvm::global_exception::evaluation_error, 31311, "invalid asset name");
        FC_DECLARE_DERIVED_EXCEPTION(balance_operation_overflow, lvm::global_exception::evaluation_error, 31312, "balance operation overflow");
        
        FC_DECLARE_DERIVED_EXCEPTION(delegate_vote_limit, lvm::global_exception::evaluation_error, 31401, "delegate_vote_limit");
        FC_DECLARE_DERIVED_EXCEPTION(insufficient_fee, lvm::global_exception::evaluation_error, 31402, "insufficient fee");
        FC_DECLARE_DERIVED_EXCEPTION(negative_fee, lvm::global_exception::evaluation_error, 31403, "negative fee");
        FC_DECLARE_DERIVED_EXCEPTION(missing_deposit, lvm::global_exception::evaluation_error, 31404, "missing deposit");
        FC_DECLARE_DERIVED_EXCEPTION(insufficient_relay_fee, lvm::global_exception::evaluation_error, 31405, "insufficient relay fee");
        
        FC_DECLARE_DERIVED_EXCEPTION(invalid_market, lvm::global_exception::evaluation_error, 31501, "invalid market");
        FC_DECLARE_DERIVED_EXCEPTION(unknown_market_order, lvm::global_exception::evaluation_error, 31502, "unknown market order");
        FC_DECLARE_DERIVED_EXCEPTION(shorting_base_shares, lvm::global_exception::evaluation_error, 31503, "shorting base shares");
        FC_DECLARE_DERIVED_EXCEPTION(insufficient_collateral, lvm::global_exception::evaluation_error, 31504, "insufficient collateral");
        FC_DECLARE_DERIVED_EXCEPTION(insufficient_depth, lvm::global_exception::evaluation_error, 31505, "insufficient depth");
        FC_DECLARE_DERIVED_EXCEPTION(insufficient_feeds, lvm::global_exception::evaluation_error, 31506, "insufficient feeds");
        FC_DECLARE_DERIVED_EXCEPTION(invalid_feed_price, lvm::global_exception::evaluation_error, 31507, "invalid feed price");
        
        FC_DECLARE_DERIVED_EXCEPTION(price_multiplication_overflow, lvm::global_exception::evaluation_error, 31601, "price multiplication overflow");
        FC_DECLARE_DERIVED_EXCEPTION(price_multiplication_underflow, lvm::global_exception::evaluation_error, 31602, "price multiplication underflow");
        FC_DECLARE_DERIVED_EXCEPTION(price_multiplication_undefined, lvm::global_exception::evaluation_error, 31603, "price multiplication undefined product 0*inf");
        
        FC_DECLARE_EXCEPTION(contract_error, 32000, "Contract Error");
        FC_DECLARE_DERIVED_EXCEPTION(contract_address_in_use, lvm::global_exception::contract_error, 32001, "Contract Address is in use");
        FC_DECLARE_DERIVED_EXCEPTION(contract_not_exist, lvm::global_exception::contract_error, 32002, "Contract with specified address is not existed!");
        FC_DECLARE_DERIVED_EXCEPTION(contract_upgraded, lvm::global_exception::contract_error, 32003, "Contract has been upgraded!");
        FC_DECLARE_DERIVED_EXCEPTION(contract_destroyed, lvm::global_exception::contract_error, 32004, "Contract has been destroyed!");
        FC_DECLARE_DERIVED_EXCEPTION(contract_name_illegal, lvm::global_exception::contract_error, 32005, "Contract name is illegal");
        FC_DECLARE_DERIVED_EXCEPTION(contract_desc_illegal, lvm::global_exception::contract_error, 32006, "Contract desc is illegal");
        FC_DECLARE_DERIVED_EXCEPTION(contract_no_permission, lvm::global_exception::contract_error, 32007, "Contract can only be upgraded/destroyed by it's owner");
        FC_DECLARE_DERIVED_EXCEPTION(contract_name_in_use, lvm::global_exception::contract_error, 32008, "Contract name is in use");
        FC_DECLARE_DERIVED_EXCEPTION(contract_code_invalid, lvm::global_exception::contract_error, 32009, "Contract code is invalid");
        FC_DECLARE_DERIVED_EXCEPTION(permanent_contract, lvm::global_exception::contract_error, 32010, "Permanent contract can not be destroyed");
        FC_DECLARE_DERIVED_EXCEPTION(contract_run_out_of_money, lvm::global_exception::contract_error, 32011, "Money is run out");
        FC_DECLARE_DERIVED_EXCEPTION(contract_bytescode_file_format_error, lvm::global_exception::contract_error, 32012, "Contract bytescode file format is error!");
        FC_DECLARE_DERIVED_EXCEPTION(method_not_exist, lvm::global_exception::contract_error, 32013, "Method not exsited!");
        FC_DECLARE_DERIVED_EXCEPTION(invalid_contract_filename, lvm::global_exception::contract_error, 32014, "Invalid contract filename!");
        FC_DECLARE_DERIVED_EXCEPTION(method_can_not_be_called_explicitly, lvm::global_exception::contract_error, 32015, "Method can't be called explicitly !");
        FC_DECLARE_DERIVED_EXCEPTION(not_be_result_of_execute, lvm::global_exception::contract_error, 32016, "This type of Operation can only exsited in execute result ");
        FC_DECLARE_DERIVED_EXCEPTION(contract_parameter_length_over_limit, lvm::global_exception::contract_error, 32017, "the parameter length of contract function is over limit");
        FC_DECLARE_DERIVED_EXCEPTION(invalid_margin_amount, lvm::global_exception::contract_error, 32018, "margin amount is invalid");
        FC_DECLARE_DERIVED_EXCEPTION(not_allow_withdraw_from_contract, lvm::global_exception::contract_error, 32019, "This type of Operation can only exsited in execute result or contract destroy trx");
        FC_DECLARE_DERIVED_EXCEPTION(deposit_contract_type_error, lvm::global_exception::contract_error, 32020, "DepositContract type error");
        FC_DECLARE_DERIVED_EXCEPTION(in_result_of_execute, lvm::global_exception::contract_error, 32021, "This type of Operation can only exsited in origin transaction ");
        FC_DECLARE_DERIVED_EXCEPTION(transfer_amount_not_bigger_than_0, lvm::global_exception::contract_error, 32022, "This type of Operation can only exsited in origin transaction ");
        FC_DECLARE_DERIVED_EXCEPTION(compile_contract_fail, lvm::global_exception::contract_error, 32023, "Compile contract to stream fail")
        FC_DECLARE_DERIVED_EXCEPTION(save_bytecode_to_gpcfile_fail, lvm::global_exception::contract_error, 32024, "Save byte code to gpc file fail")
        FC_DECLARE_DERIVED_EXCEPTION(code_hash_error, lvm::global_exception::contract_error, 32025, "Save byte code to gpc file fail")
        FC_DECLARE_DERIVED_EXCEPTION(contract_run_out_of_testing_money, lvm::global_exception::contract_error, 32026, "Testing money is run out");
        FC_DECLARE_DERIVED_EXCEPTION(contract_execute_error_in_testing, lvm::global_exception::contract_error, 32027, "Execute error in testing");
        FC_DECLARE_DERIVED_EXCEPTION(contract_execute_error, lvm::global_exception::contract_error, 32028, "Execute error");
        FC_DECLARE_DERIVED_EXCEPTION(transaction_contract_registered_in_not_found, lvm::global_exception::contract_error, 32029, "transaction contract registered in not found");
        FC_DECLARE_DERIVED_EXCEPTION(no_contract_registered_in_this_transaction, lvm::global_exception::contract_error, 32030, "no contract registered in this transaction");
        
        FC_DECLARE_EXCEPTION(bytecode_error, 33000, "Contract Bytecode Error");
        FC_DECLARE_DERIVED_EXCEPTION(read_verify_code_fail, lvm::global_exception::bytecode_error, 33001, "Read verify code fail!");
        FC_DECLARE_DERIVED_EXCEPTION(read_bytescode_len_fail, lvm::global_exception::bytecode_error, 33002, "Read bytescode len fail!");
        FC_DECLARE_DERIVED_EXCEPTION(read_bytescode_fail, lvm::global_exception::bytecode_error, 33003, "Read bytescode fail!");
        FC_DECLARE_DERIVED_EXCEPTION(verify_bytescode_sha1_fail, lvm::global_exception::bytecode_error, 33003, "Verify bytescode SHA1 fail!");
        FC_DECLARE_DERIVED_EXCEPTION(read_api_count_fail, lvm::global_exception::bytecode_error, 33004, "Read api count fail!");
        FC_DECLARE_DERIVED_EXCEPTION(read_api_len_fail, lvm::global_exception::bytecode_error, 33005, "Read api len fail!");
        FC_DECLARE_DERIVED_EXCEPTION(read_api_fail, lvm::global_exception::bytecode_error, 33006, "Read api fail!");
        FC_DECLARE_DERIVED_EXCEPTION(read_offline_api_count_fail, lvm::global_exception::bytecode_error, 33007, "Read offline api count fail!");
        FC_DECLARE_DERIVED_EXCEPTION(read_offline_api_len_fail, lvm::global_exception::bytecode_error, 33008, "Read offline api len fail!");
        FC_DECLARE_DERIVED_EXCEPTION(read_offline_api_fail, lvm::global_exception::bytecode_error, 33009, "Read offline api fail!");
        FC_DECLARE_DERIVED_EXCEPTION(read_events_count_fail, lvm::global_exception::bytecode_error, 33010, "Read events count fail!");
        FC_DECLARE_DERIVED_EXCEPTION(read_events_len_fail, lvm::global_exception::bytecode_error, 33011, "Read events len fail!");
        FC_DECLARE_DERIVED_EXCEPTION(read_events_fail, lvm::global_exception::bytecode_error, 33012, "Read events fail!");
        FC_DECLARE_DERIVED_EXCEPTION(read_storage_count_fail, lvm::global_exception::bytecode_error, 33013, "Read storage count fail!");
        FC_DECLARE_DERIVED_EXCEPTION(read_storage_name_len_fail, lvm::global_exception::bytecode_error, 33014, "Read storage name len fail!");
        FC_DECLARE_DERIVED_EXCEPTION(read_storage_name_fail, lvm::global_exception::bytecode_error, 33015, "Read storage name fail!");
        FC_DECLARE_DERIVED_EXCEPTION(read_storage_type_fail, lvm::global_exception::bytecode_error, 33016, "Read storage type fail!");
        
        FC_DECLARE_EXCEPTION(lua_error, 34000, "Lua Error");
        FC_DECLARE_DERIVED_EXCEPTION(lua_simple_error, lvm::global_exception::lua_error, 34001, "lua simple error");
        FC_DECLARE_DERIVED_EXCEPTION(lua_memory_error, lvm::global_exception::lua_error, 34002, "lua memory error");
        FC_DECLARE_DERIVED_EXCEPTION(lua_lvm_error, lvm::global_exception::lua_error, 34003, "lua lvm error");
        FC_DECLARE_DERIVED_EXCEPTION(lua_parser_error, lvm::global_exception::lua_error, 34004, "lua parser error");
        FC_DECLARE_DERIVED_EXCEPTION(lua_compile_error, lvm::global_exception::lua_error, 34005, "lua compile error");
        FC_DECLARE_DERIVED_EXCEPTION(lua_executor_internal_error, lvm::global_exception::lua_error, 34005, "lua lvm internal error");
        
        FC_DECLARE_EXCEPTION(event_error, 35000, "Event Error");
        FC_DECLARE_DERIVED_EXCEPTION(event_parameter_length_over_limit, lvm::global_exception::event_error, 35001, "the parameter length of event function is over limit");
        FC_DECLARE_DERIVED_EXCEPTION(script_not_found_in_db, lvm::global_exception::event_error, 35002, "Script not found");
        FC_DECLARE_DERIVED_EXCEPTION(invalid_script_source_filename, lvm::global_exception::event_error, 35003, "Script source file name must end with .lua or .glua");
        FC_DECLARE_DERIVED_EXCEPTION(script_already_existed, lvm::global_exception::event_error, 35004, "Script already existed");
        FC_DECLARE_DERIVED_EXCEPTION(event_handler_existed, lvm::global_exception::event_error, 35005, "Event handler already existed");
        FC_DECLARE_DERIVED_EXCEPTION(script_has_been_disabled, lvm::global_exception::event_error, 35006, "Script has been disabled");
        FC_DECLARE_DERIVED_EXCEPTION(script_file_name_invalid, lvm::global_exception::event_error, 35007, "Script file name must end with .script");
        FC_DECLARE_DERIVED_EXCEPTION(EventHandler_not_found, lvm::global_exception::event_error, 35008, "EventHandler not found");
        FC_DECLARE_DERIVED_EXCEPTION(EventType_not_found, lvm::global_exception::event_error, 35008, "EventType not found");
        FC_DECLARE_DERIVED_EXCEPTION(compile_script_fail, lvm::global_exception::event_error, 35009, "Compile script to stream fail");
        FC_DECLARE_DERIVED_EXCEPTION(save_bytecode_to_scriptfile_fail, lvm::global_exception::event_error, 35010, "Save byte code to script file fail");
        FC_DECLARE_DERIVED_EXCEPTION(script_id_not_valid, lvm::global_exception::event_error, 35011, "not invalid script id");
        FC_DECLARE_DERIVED_EXCEPTION(Result_trx_id_not_found, lvm::global_exception::event_error, 35012, "result trx id not found");
        FC_DECLARE_DERIVED_EXCEPTION(Request_trx_id_not_found, lvm::global_exception::event_error, 35013, "request trx id not found");
        
        FC_DECLARE_EXCEPTION(sandbox_error, 36000, "Sandbox Error");
        FC_DECLARE_DERIVED_EXCEPTION(sandbox_reopen, lvm::global_exception::sandbox_error, 36001, "you have been in sandbox, if you mean to reopen sandbox, close it first!");
        FC_DECLARE_DERIVED_EXCEPTION(sandbox_command_forbidden, lvm::global_exception::sandbox_error, 36002, "in sandbox, you can not use this command!");
        FC_DECLARE_DERIVED_EXCEPTION(sandbox_reclose, lvm::global_exception::sandbox_error, 36003, "you have closed the sandbox already!");
        
        /*
        非异常定义，一些利用异常方式的状态处理
        */
        
        FC_DECLARE_EXCEPTION(evaluation_state, 41000, "Evaluation State");
        FC_DECLARE_DERIVED_EXCEPTION(ignore_check_required_fee_state, lvm::global_exception::evaluation_state, 41001, "No check required fee");
        
        FC_DECLARE_EXCEPTION(chaindatabasee_error, 42000, "chaindatabasee error");
        FC_DECLARE_DERIVED_EXCEPTION(store_and_index_a_seen_block, lvm::global_exception::chaindatabasee_error, 42001, "store_and_index_a_seen_block");
        
        FC_DECLARE_EXCEPTION(contract_exception, 43000, "contract Error");
        FC_DECLARE_EXCEPTION(rpc_exception, 44000, "RPC Error");
        FC_DECLARE_EXCEPTION(missing_parameter, 45000, "Missing Parameter");
        FC_DECLARE_EXCEPTION(unknown_method, 46000, "Unknown Method");
        FC_DECLARE_EXCEPTION(login_required, 47000, "Login Required");
        FC_DECLARE_EXCEPTION(rpc_msg_error, 48000, "Rpc TaskType Error");
        FC_DECLARE_EXCEPTION(task_type_error, 49000, "TaskType Error");
        FC_DECLARE_EXCEPTION(rpc_pointrt_null, 50000, "Rpc Pointer Null");
        FC_DECLARE_EXCEPTION(socket_send_error, 51000, "Socket Send Error");
        FC_DECLARE_EXCEPTION(socket_read_error, 52000, "Socket Read Error");
        FC_DECLARE_EXCEPTION(sync_socket_error, 53000, "Sync Socket Error");
        FC_DECLARE_EXCEPTION(async_socket_error, 54000, "Async Socket Error");
    }
} // lvm::global_exception
#endif
