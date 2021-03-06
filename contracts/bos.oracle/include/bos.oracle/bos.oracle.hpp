#pragma once
/*

  bos_oracle



*/

#include "bos.constants.hpp"
#include "bos.oracle/tables/arbitration.hpp"
#include "bos.oracle/tables/consumer.hpp"
#include "bos.oracle/tables/oracle_api.hpp"
#include "bos.oracle/tables/provider.hpp"
#include "bos.oracle/tables/riskcontrol.hpp"
#include <cmath>
#include <eosio/eosio.hpp>
using namespace eosio;
using namespace std;

class [[eosio::contract("bos.oracle")]] bos_oracle : public eosio::contract {
 private:
   oracle_meta_parameters_singleton _oracle_meta_parameters_singleton;
   oracle_meta_parameters _oracle_meta_parameters;

 public:
   static constexpr eosio::name token_account{"eosio.token"_n};
   static constexpr eosio::name active_permission{"active"_n};
   //  static constexpr symbol _core_symbol = symbol(symbol_code("BOS"), 4);

   static time_point_sec current_time_point_sec();
   void check_data(const std::string& data, const std::string& data_name) {
      check(_oracle_meta_parameters.version == current_oracle_version, "config parameters must first be initialized ");
      uint16_t data_size = unpack<oracle_parameters>(_oracle_meta_parameters.parameters_data).max_data_size;
      std::string checkmsg = data_name + " declaration could not be greater than  " + std::to_string(data_size);
      check(data.size() <= data_size, checkmsg.c_str());
   }

   void check_stake(const asset& amount, const std::string& data_name, uint64_t limit_value) {
      std::string checkmsg = data_name + " could not be less than " + std::to_string(limit_value);
      check(amount >= asset(limit_value, core_symbol()), checkmsg.c_str());
   }

   using contract::contract;
   bos_oracle(name receiver, name code, datastream<const char*> ds) : contract(receiver, code, ds), _oracle_meta_parameters_singleton(_self, _self.value) {
      _oracle_meta_parameters = _oracle_meta_parameters_singleton.exists() ? _oracle_meta_parameters_singleton.get() : oracle_meta_parameters{};
   }
   ~bos_oracle() { _oracle_meta_parameters_singleton.set(_oracle_meta_parameters, _self); }

   /// bos.provider begin
   ///
   ///

   /**
    * @brief  Registers oracle service information
    */
   [[eosio::action]] void regservice(name account, asset base_stake_amount, std::string data_format, uint8_t data_type, std::string criteria, uint8_t acceptance, uint8_t injection_method,
                                     uint32_t duration, uint8_t provider_limit, uint32_t update_cycle);

   /**
    * @brief  Unstakes asset by provider
    */
   [[eosio::action]] void unstakeasset(uint64_t service_id, name account, asset amount, std::string memo);

   /**
    * @brief  Adds fee types
    */
   [[eosio::action]] void addfeetypes(uint64_t service_id, std::vector<uint8_t> fee_types, std::vector<asset> service_prices);

   /**
    * @brief Pushs data by provider
    */
   [[eosio::action]] void pushdata(uint64_t service_id, name provider, uint64_t cycle_number, uint64_t request_id, string data);

   /**
    * @brief  Performs inner push data action
    */
   [[eosio::action]] void oraclepush(uint64_t service_id, uint64_t cycle_number, uint64_t request_id, string data, name contract_account);

   /**
    * @brief  claim  income for providing data
    */
   [[eosio::action]] void claim(name account, name receive_account);

   /**
    * @brief   Performs oracle service special action
    */
   [[eosio::action]] void execaction(uint64_t service_id, uint8_t action_type);

   /**
    * @brief  Unregister oracle service by provider
    */
   [[eosio::action]] void unregservice(uint64_t service_id, name account, uint8_t status);

   /**
    * @brief  Starts timer for handling timeout of updating data
    */
   [[eosio::action]] void starttimer(uint64_t service_id, uint64_t cycle_number, uint64_t request_id);

   /**
    * @brief  Clears data when oracle data's accessable time is timeout
    */
   [[eosio::action]] void cleardata(uint64_t service_id, uint32_t time_length);

   /**
    * @brief  Sets config parameters
    */
   [[eosio::action]] void setparameter(ignore<uint8_t> version, ignore<oracle_parameters> parameters);

   using regservice_action = eosio::action_wrapper<"regservice"_n, &bos_oracle::regservice>;
   using unstakeasset_action = eosio::action_wrapper<"unstakeasset"_n, &bos_oracle::unstakeasset>;
   using addfeetypes_action = eosio::action_wrapper<"addfeetypes"_n, &bos_oracle::addfeetypes>;
   using pushdata_action = eosio::action_wrapper<"pushdata"_n, &bos_oracle::pushdata>;
   using oraclepush_action = eosio::action_wrapper<"oraclepush"_n, &bos_oracle::oraclepush>;
   using claim_action = eosio::action_wrapper<"claim"_n, &bos_oracle::claim>;
   using execaction_action = eosio::action_wrapper<"execaction"_n, &bos_oracle::execaction>;
   using unregister_action = eosio::action_wrapper<"unregservice"_n, &bos_oracle::unregservice>;
   using starttimer_action = eosio::action_wrapper<"starttimer"_n, &bos_oracle::starttimer>;
   using cleardata_action = eosio::action_wrapper<"cleardata"_n, &bos_oracle::cleardata>;
   using setparameter_action = eosio::action_wrapper<"setparameter"_n, &bos_oracle::setparameter>;

   ///
   ///
   /// bos.provider end
   /// bos.consumer begin
   ///
   ///

   /**
    * @brief   Subscribes oracle service by consumer
    */
   [[eosio::action]] void subscribe(uint64_t service_id, name contract_account, name account, std::string memo);

   /**
    * @brief   Requests data  by a consumer
    */
   [[eosio::action]] void requestdata(uint64_t service_id, name contract_account, name requester, std::string request_content);
   using subscribe_action = eosio::action_wrapper<"subscribe"_n, &bos_oracle::subscribe>;
   using requestdata_action = eosio::action_wrapper<"requestdata"_n, &bos_oracle::requestdata>;

   ///
   ///
   /// bos.consumer end
   /// bos.riskctrl begin
   ///
   ///

   /**
    * @brief  Deposits core tokens to oracle risk control fund
    */
   [[eosio::action]] void deposit(name from, name to, asset quantity, string memo, bool is_notify);

   /**
    * @brief  Withdraws core tokens from oracle risk control fund
    */
   [[eosio::action]] void withdraw(uint64_t service_id, name from, name to, asset quantity, string memo);

   using deposit_action = eosio::action_wrapper<"deposit"_n, &bos_oracle::deposit>;
   using withdraw_action = eosio::action_wrapper<"withdraw"_n, &bos_oracle::withdraw>;

   ///
   ///
   /// bos.riskctrl end

   // [[eosio::on_notify("eosio.token::transfer")]]
   void on_transfer(name from, name to, asset quantity, std::string memo);
   /// bos.arbitration begin
   ///
   ///

   /**
    * @brief Accepts invitation  for arbitrating case by a arbitrator
    */
   [[eosio::action]] void acceptarbi(name arbitrator, uint64_t arbitration_id);

   /**
    * @brief   Uploads evidence by both parties from arbitration case
    */
   [[eosio::action]] void uploadeviden(name account, uint64_t arbitration_id, std::string evidence);

   /**
    * @brief Uploads result by arbitrator
    */
   [[eosio::action]] void uploadresult(name arbitrator, uint64_t arbitration_id, uint8_t result, std::string comment);

   /**
    * @brief Unstake core tokens from arbitration stake fund
    */
   [[eosio::action]] void unstakearbi(uint64_t arbitration_id, name account, asset amount, std::string memo);

   /**
    * @brief  Claims income from arbitration stake fund
    */
   [[eosio::action]] void claimarbi(name account, name receive_account);

   /**
    * @brief   Starts timer for handling  timeout wher arbitration operation's waiting time is expired
    */
   [[eosio::action]] void timertimeout(uint64_t arbitration_id, uint8_t round, uint8_t timer_type);

   /**
    * @brief  Sets status for arbitration case  process
    */
   [[eosio::action]] void setstatus(uint64_t arbitration_id, uint8_t status);

   /**
    * @brief Imports wps auditors as fulltime arbitrators
    */
   [[eosio::action]] void importwps(vector<name> auditors);

   ///
   ///
   /// bos.arbitration end

 private:
   void save_id(uint8_t id_type, uint64_t id);
   uint128_t make_update_id(uint64_t cycle_number, uint64_t request_id);
   // provider
   void check_service_current_cycle_number(uint64_t service_id, uint64_t cycle_number);
   void update_service_current_log_status(uint64_t service_id, uint64_t cycle_number, uint64_t request_id, uint8_t data_type = data_deterministic, uint8_t status = log_status::log_sent);
   void addfeetype(uint64_t service_id, uint8_t fee_type, asset service_price);
   void innerpush(uint64_t service_id, name provider, uint64_t cycle_number, uint64_t request_id, string data);
   void innerpublish(uint64_t service_id, name provider, uint64_t cycle_number, uint64_t request_id, string data);
   void reg_service_provider(uint64_t service_id, name account);
   void check_service_status(uint64_t service_id);
   void update_service_status(uint64_t service_id);
   void check_service_provider_status(uint64_t service_id, name account);
   void update_service_provider_status(uint64_t service_id, name account);
   void stake_asset(uint64_t service_id, name account, asset amount, std::string memo);
   void update_stake_asset(uint64_t service_id, name account, asset amount);
   void add_times(uint64_t service_id, name account, name contract_account, bool is_request);
   std::tuple<uint64_t, uint64_t, uint64_t, uint64_t> get_times(uint64_t service_id, name account);
   time_point_sec get_payment_time(uint64_t service_id, name contract_account);
   uint8_t get_subscription_status(uint64_t service_id, name contract_account);
   uint8_t get_service_status(uint64_t service_id);
   void fee_service(uint64_t service_id, name contract_account, uint8_t fee_type);
   asset get_price_by_fee_type(uint64_t service_id, uint8_t fee_type);

   std::vector<std::tuple<name, asset>> get_provider_list(uint64_t service_id);

   void freeze_asset(uint64_t service_id, name account, asset amount, uint64_t arbitration_id = 0);
   uint64_t freeze_providers_amount(uint64_t service_id, const std::set<name>& available_providers, asset freeze_amount, uint64_t arbitration_id = 0);
   void clear_data(uint64_t service_id, uint32_t time_length);
   void start_timer(uint64_t service_id, uint64_t cycle_number, uint64_t request_id);
   void check_publish_service(uint64_t service_id, uint64_t cycle_number, uint64_t request_id, bool is_expired = false);
   void save_publish_data(uint64_t service_id, uint64_t cycle_number, uint64_t request_id, string data, uint8_t data_type = data_deterministic);
   uint64_t get_provider_count(uint64_t service_id);

   uint64_t get_publish_provider_count(uint64_t service_id, uint64_t cycle_number, uint64_t request_id);

   string get_publish_data(uint64_t service_id, uint64_t cycle_number, uint8_t provider_limit, uint64_t request_id);
   bool check_provider_no_push_data(uint64_t service_id, name provider, uint64_t cycle_number, uint64_t request_id, uint8_t data_type);

   /// consumer
   void pay_service(uint64_t service_id, name contract_account, asset amount);
   std::vector<name> get_subscription_list(uint64_t service_id);
   std::tuple<uint64_t, uint64_t> get_consumption(uint64_t service_id);

   /// risk control
   void transfer(name from, name to, asset quantity, string memo);
   void oracle_transfer(name from, name to, asset quantity, string memo, bool is_deferred);
   void call_deposit(name from, name to, asset quantity, bool is_notify);
   void add_freeze_delay(uint64_t service_id, name account, time_point_sec start_time, uint32_t duration, asset amount, uint64_t type);
   void add_freeze(uint64_t service_id, name account, time_point_sec start_time, uint32_t duration, asset amount, uint64_t arbitration_id = 0);
   void add_delay(uint64_t service_id, name account, time_point_sec start_time, uint32_t duration, asset amount);

   uint64_t add_guarantee(uint64_t service_id, name account, asset amount, uint32_t duration);
   void sub_balance(name owner, asset value);
   void add_balance(name owner, asset value, name ram_payer);
   void add_freeze_stat(uint64_t service_id, name account, asset amount, uint64_t arbitration_id = 0);
   std::tuple<asset, asset> get_freeze_stat(uint64_t service_id, name account);
   void unfreeze_asset(uint64_t service_id, uint64_t arbitration_id);

   /// arbitration
   void _regarbitrat(name account, uint8_t type, asset amount, std::string public_info);
   void _appeal(name appellant, uint64_t service_id, asset amount, std::string reason, std::string evidence, uint8_t role_type);
   void _respcase(name respondent, uint64_t arbitration_id, asset amount, std::string evidence);

   void handle_arbitration(uint64_t arbitration_id);
   void handle_arbitration_result(uint64_t arbitration_id);
   void start_arbitration(arbitrator_type arbitype, uint64_t arbitration_id, uint64_t service_id);
   vector<name> random_arbitrator(uint64_t arbitration_id, uint8_t round, uint64_t required_arbitrator_count);
   void random_chose_arbitrator(uint64_t arbitration_id, uint8_t round, uint64_t service_id, uint64_t required_arbitrator_count);
   void add_arbitration_result(name arbitrator, uint64_t arbitration_id, uint8_t result, uint8_t round, std::string comment);
   void update_arbitration_correction(uint64_t arbitration_id);
   uint128_t make_deferred_id(uint64_t arbitration_id, uint8_t timer_type);
   void timeout_deferred(uint64_t arbitration_id, uint8_t round, uint8_t timer_type, uint32_t time_length);
   void handle_upload_result(uint64_t arbitration_id, uint8_t round);
   std::tuple<std::vector<name>, asset, std::vector<name>> get_stake_accounts_and_asset(uint64_t arbitration_id, uint8_t role_type);
   std::tuple<std::vector<name>, asset> get_provider_service_stakes(uint64_t service_id);
   void slash_service_stake(uint64_t service_id, const std::vector<name>& slash_accounts, const asset& amount);
   void slash_arbitration_stake(uint64_t arbitration_id, uint8_t role_type);
   void sub_balance(name owner, asset value, uint64_t arbitration_id);
   void add_balance(name owner, asset value, uint64_t arbitration_id, uint8_t role_type);
   void stake_arbitration(uint64_t id, name account, asset amount, uint8_t round, uint8_t role_type, string memo);
   void check_stake_arbitration(uint64_t id, name account, uint8_t round);
   void add_income(name account, asset quantity);
   std::vector<name> get_arbitrators_of_uploading_arbitration_result(uint64_t arbitration_id);
   uint64_t make_scope_value(uint64_t id, uint8_t round, bool is_check = true);
   std::vector<name> get_arbitrators(uint8_t arbitrator_type = arbitrator_type::fulltime);
   int get_arbitrators_count();
   void send_notify(const vector<name>& accounts, const std::string& memo, std::string checkmsg = "");
   void send_notify(const vector<name>& accounts, const vector<name>& resp_accounts, const std::string& memo);

   /// common
   symbol core_symbol() const {
      check(_oracle_meta_parameters.version == current_oracle_version, "config parameters must first be initialized ");
      auto paras = unpack<oracle_parameters>(_oracle_meta_parameters.parameters_data);
      symbol _core_symbol = symbol(symbol_code(paras.core_symbol), paras.precision);
      return _core_symbol;
   };
};
