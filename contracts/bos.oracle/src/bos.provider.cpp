#include "bos.oracle/bos.oracle.hpp"
#include <cmath>
#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/transaction.hpp>
using namespace eosio;
using std::string;
/**
 * @brief
 *
 * @param service_id
 * @param account
 * @param amount
 * @param service_price
 * @param fee_type
 * @param data_format
 * @param data_type
 * @param criteria
 * @param acceptance
 * @param declaration
 * @param injection_method
 * @param duration
 * @param provider_limit
 * @param update_cycle
 * @param update_start_time
 */
void bos_oracle::regservice(uint64_t service_id, name account,
                            asset amount, asset service_price,
                            uint64_t fee_type, std::string data_format,
                            uint64_t data_type, std::string criteria,
                            uint64_t acceptance, std::string declaration,
                            uint64_t injection_method, uint64_t duration,
                            uint64_t provider_limit, uint64_t update_cycle,
                            time_point_sec update_start_time) {
                              cout << 33333333;
                              print(1234567);
                              //print("=====1");
  require_auth(account);
  uint64_t new_service_id = service_id;
  data_services svctable(_self, _self.value);
 auto service_itr = svctable.find(service_id);
  if (service_itr == svctable.end()) { 
    //  if (0 == service_id) {
     //print("=====2");
    // add service
    svctable.emplace(_self, [&](auto &s) {
      s.service_id = svctable.available_primary_key()+(0==svctable.available_primary_key()?1:0);
      //print("$$$$$$$$$$$$$");
      //print(s.service_id);
      //print("**************");
      s.service_price = service_price;
      s.fee_type = fee_type;
      s.data_format = data_format;
      s.data_type = data_type;
      s.criteria = criteria;
      s.acceptance = acceptance;
      s.declaration = declaration;
      s.injection_method = injection_method;
      s.amount = asset(0,core_symbol());//amount;
      s.duration = duration;
      s.provider_limit = provider_limit;
      s.update_cycle = update_cycle;
      s.update_start_time = update_start_time;
      s.last_update_number = 0;
      s.appeal_freeze_period = 0;
      s.exceeded_risk_control_freeze_period = 0;
      s.guarantee_id = 0;
      s.risk_control_amount = asset(0, core_symbol());
      s.pause_service_stake_amount = asset(0, core_symbol());
      s.freeze_flag = false;
      s.emergency_flag = false;
      s.status = service_status::service_in;
      new_service_id = s.service_id;
      
    });
  }
    //print("=====1");
  // transfer(account, provider_account, amount, "");
    //print("=====1");
  // add provider
  data_providers providertable(_self, _self.value);
  auto provider_itr = providertable.find(account.value);
  if (provider_itr == providertable.end()) {
    providertable.emplace(_self, [&](auto &p) {
      p.account = account;
      p.total_stake_amount = asset(0,core_symbol());//amount;
      // p.pubkey = "";
      p.total_freeze_amount = asset(0, core_symbol());
      p.unconfirmed_amount = asset(0, core_symbol());
      p.claim_amount = asset(0, core_symbol());
      p.last_claim_time = time_point_sec();
    });
  } else {
    providertable.modify(provider_itr, same_payer, [&](auto &p) {
      p.total_stake_amount += asset(0,core_symbol());//amount;
    });
  }
    //print("===service_id==");
    //print(new_service_id);
  data_service_provisions provisionstable(_self, new_service_id);

  auto provision_itr = provisionstable.find(account.value);
  check(provision_itr == provisionstable.end(), "the account has subscribed service   ");

  provisionstable.emplace(_self, [&](auto &p) {
    p.service_id = new_service_id;
    p.account = account;
    p.amount = asset(0, core_symbol());
    p.freeze_amount = asset(0, core_symbol());
    p.service_income = asset(0, core_symbol());
    p.status = provision_status::provision_reg;
    p.public_information = "";
    p.stop_service =false;
  });
    //print("=====1");
  provider_services provservicestable(_self, account.value);
  uint64_t create_time_sec =
      static_cast<uint64_t>(update_start_time.sec_since_epoch());
  auto provservice_itr = provservicestable.find(create_time_sec);
  check(provservice_itr == provservicestable.end(),
        "same account register service interval too short ");
  provservicestable.emplace(_self, [&](auto &p) {
    p.service_id = new_service_id;
    p.create_time = update_start_time;
  });
    //print("=====1");
    data_service_stakes svcstaketable(_self, _self.value);
  auto svcstake_itr = svcstaketable.find(new_service_id);
  if(svcstake_itr == svcstaketable.end())
  {
 svcstaketable.emplace(_self, [&](auto &ss) {
      ss.service_id = new_service_id;
      ss.amount = asset(0,core_symbol());//amount;
      ss.freeze_amount = asset(0, core_symbol());
      ss.unconfirmed_amount = asset(0, core_symbol());
  });
  }
  else
  {
    svcstaketable.modify(svcstake_itr, same_payer,
                         [&](auto &ss) { 
                           ss.amount += asset(0,core_symbol());//amount;
                          });
  }


}

void bos_oracle::unstakeasset(uint64_t service_id, name account,
                              asset amount, std::string memo) {
  require_auth(account);
  stake_asset(service_id, account, -amount);
}

void bos_oracle::stakeasset(uint64_t service_id, name account,
                            asset amount, std::string memo) {
  require_auth(account);
  check (amount.amount > 0,"stake amount could not equal zero") ;
  transfer(account, provider_account, amount, "");
  
  stake_asset(service_id, account, amount);
}

/**
 * @brief
 *
 * @param service_id
 * @param account
 * @param amount
 */
void bos_oracle::stake_asset(uint64_t service_id, name account,
                             asset amount) {
 
  // if (amount.amount > 0) {
  //    transfer(account, provider_account, amount, "");
  // }

  data_providers providertable(_self, _self.value);
  auto provider_itr = providertable.find(account.value);
  check(provider_itr != providertable.end(), "");

  data_service_provisions provisionstable(_self, service_id);

  auto provision_itr = provisionstable.find(account.value);
  check(provision_itr != provisionstable.end(),
        "account does not subscribe services");

  if (amount.amount < 0) {
    check(provider_itr->total_stake_amount >=
              asset(std::abs(amount.amount), core_symbol()),
          "");
    check(provision_itr->amount >=
              asset(std::abs(amount.amount), core_symbol()),
          "");
  }

  providertable.modify(provider_itr, same_payer,
                       [&](auto &p) { p.total_stake_amount += amount; });

  provisionstable.modify(provision_itr, same_payer,
                         [&](auto &p) { p.amount += amount; });

  if (amount.amount < 0) {
    transfer(provider_account, account,
             asset(std::abs(amount.amount), core_symbol()), "");
  }


   data_service_stakes svcstaketable(_self, _self.value);
  auto svcstake_itr = svcstaketable.find(service_id);
  if(svcstake_itr == svcstaketable.end())
  {
 svcstaketable.emplace(_self, [&](auto &ss) {
      ss.amount = amount;
      ss.freeze_amount = asset(0, core_symbol());
  });
  }
  else
  {
    svcstaketable.modify(svcstake_itr, same_payer,
                         [&](auto &ss) { ss.amount += amount; });
  }


}

/**
 * @brief
 *
 * @param service_id
 * @param fee_types
 * @param service_prices
 */
void bos_oracle::addfeetypes(uint64_t service_id,
                             std::vector<uint8_t> fee_types,
                             std::vector<asset> service_prices) {
  require_auth(_self);
  check(fee_types.size() > 0 && fee_types.size() == service_prices.size(),
        "fee_types size have to equal service prices size");
  data_service_fees feetable(_self, service_id);
  for (int i = 0; i < fee_types.size(); i++) {
    auto fee_type = fee_types[i];
    auto service_price = service_prices[i];
    addfeetype(service_id, fee_type, service_price);
  }
}

void bos_oracle::addfeetype(uint64_t service_id, uint8_t fee_type,
                            asset service_price) {
  require_auth(_self);
  data_service_fees feetable(_self, service_id);
  check(fee_type >= fee_type::fee_times && fee_type < fee_type::fee_type_count,
        "unknown fee type");

  auto fee_itr = feetable.find(static_cast<uint64_t>(fee_type));
  if (fee_itr == feetable.end()) {
    feetable.emplace(_self, [&](auto &f) {
      f.service_id = service_id;
      f.fee_type = fee_type;
      f.service_price = service_price;
    });
  } else {
    feetable.modify(fee_itr, same_payer, [&](auto &f) {
      f.service_id = service_id;
      f.fee_type = fee_type;
      f.service_price = service_price;
    });
  }
}

/**
 * @brief
 *
 * @param service_id
 * @param provider
 * @param data_json
 * @param is_request
 */
void bos_oracle::multipush(uint64_t service_id, name provider,
                           string data_json, bool is_request) {
                             //print("==========multipush");
  require_auth(provider);
  check(service_status::service_in == get_service_status(service_id),
        "service and subscription must be available");

  auto push_data = [this](uint64_t service_id, name provider, name contract_account,
                      name action_name, uint64_t request_id,
                      string data_json) {
                        //print("====push_data========");
                        //print(contract_account);
                        //print("=======contract_account====");
                         //print(request_id);
                         //print("=======request_id=======");
    transaction t;
    t.actions.emplace_back(
        permission_level{_self, active_permission}, _self, "innerpush"_n,
        std::make_tuple(service_id, provider, contract_account, action_name,
                        request_id, data_json));
    t.delay_sec = 0;
    uint128_t deferred_id =
        (uint128_t(service_id) << 64) | contract_account.value;
    cancel_deferred(deferred_id);
    t.send(deferred_id, _self,true);
  };

     //print("=======is_request==true= before====");
  if (is_request) {
     //print("=======is_request==true=====");
    // request
    uint64_t request_id = get_request_by_last_push(service_id, provider);
    std::vector<std::tuple<name, name, uint64_t>> receive_contracts =
        get_request_list(service_id, request_id);
    //print("=======is_request==for=====");
    for (const auto &rc : receive_contracts) {
           //print(request_id);
                         //print("=======request_id==true=====");
      push_data(service_id, provider, std::get<0>(rc), std::get<1>(rc),
               std::get<2>(rc), data_json);
    }
  } else {

    // subscription
    std::vector<std::tuple<name, name>> subscription_receive_contracts =
        get_subscription_list(service_id);

    for (const auto &src : subscription_receive_contracts) {
      push_data(service_id, provider, std::get<0>(src), std::get<1>(src), 0,
               data_json);
    }
  }
}

void bos_oracle::pushdata(uint64_t service_id, name provider,
                          name contract_account, name action_name,
                          uint64_t request_id, string data_json) {
                            //print("=====pushdata====");
                        //print(contract_account);
                        //print("====222===contract_account");
  require_auth(provider);
  //  action(permission_level{_self, "active"_n},
  //          _self, "innerpush"_n,
  //          std::make_tuple(aservice_id,  provider,
  //                          contract_account,  action_name,
  //                          request_id, data_json))
  //       .send();

      // inline innerpush 
      // {
      //    innerpush_action innerpush_act{ _self, { _self, active_permission } };
      //    innerpush_act.send( service_id,  provider,
      //                      contract_account,  action_name,
      //                      request_id, data_json );
      // }

 transaction t;
    t.actions.emplace_back(
        permission_level{_self, active_permission}, _self, "innerpush"_n,
        std::make_tuple(service_id, provider, contract_account, action_name,
                        request_id, data_json));
    t.delay_sec = 0;
    uint128_t deferred_id =
        (uint128_t(service_id) << 64) | contract_account.value;
    cancel_deferred(deferred_id);
    t.send(deferred_id, _self,true);

                          }
/**
 * @brief
 *
 * @param service_id
 * @param provider
 * @param contract_account
 * @param action_name
 * @param data_json
 * @param request_id
 */
void bos_oracle::innerpush(uint64_t service_id, name provider,
                          name contract_account, name action_name,
                          uint64_t request_id, string data_json) {
  //print("======@@@@@@@@@@@@@@@@@@@@@@@@@@@@===innerpush====================");
  require_auth(_self);
  check(service_status::service_in == get_service_status(service_id) &&
            subscription_status::subscription_subscribe ==
                get_subscription_status(service_id, contract_account,
                                        action_name),
        "service and subscription must be available");

  if (0 == request_id) {
    time_point_sec pay_time =
        get_payment_time(service_id, contract_account, action_name);
    pay_time+=eosio::days(30);
    if (pay_time < time_point_sec(now())) {
      fee_service(service_id, contract_account, action_name,
                  fee_type::fee_month);
    }
  }

  data_service_provision_logs logtable(_self, _self.value);
  logtable.emplace(_self, [&](auto &l) {
    l.log_id = logtable.available_primary_key();
    l.service_id = service_id;
    l.account = provider;
    l.data_json = data_json;
    l.update_number = 0;
    l.status = 0;
    // l.provider_sig = provider_signature;
    l.contract_account = contract_account;
    l.action_name = action_name;
    l.request_id = request_id;
    l.update_time = time_point_sec(now());
  });

  add_times(service_id, provider, contract_account, action_name,
            0 != request_id);

  require_recipient(contract_account);
}

void bos_oracle::multipublish(uint64_t service_id, name provider,
                           string data_json, bool is_request) {
  require_auth(provider);
  check(service_status::service_in == get_service_status(service_id),
        "service and subscription must be available");

  auto publish_data = [this](uint64_t service_id, name provider, name contract_account,
                      name action_name, uint64_t request_id,
                      string data_json) {
    transaction t;
    t.actions.emplace_back(
        permission_level{_self, active_permission}, _self, "innerpublish"_n,
        std::make_tuple(service_id, provider, contract_account, action_name,
                        request_id, data_json));
    t.delay_sec = 0;
    uint128_t deferred_id =
        (uint128_t(service_id) << 64) | contract_account.value;
    cancel_deferred(deferred_id);
    t.send(deferred_id, _self,true);
  };

  if (is_request) {
    // request
    uint64_t request_id = get_request_by_last_push(service_id, provider);
    std::vector<std::tuple<name, name, uint64_t>> receive_contracts =
        get_request_list(service_id, request_id);
    for (const auto &rc : receive_contracts) {
      publish_data(service_id, provider, std::get<0>(rc), std::get<1>(rc),
               std::get<2>(rc), data_json);
    }
  } else {

    // subscription
    std::vector<std::tuple<name, name>> subscription_receive_contracts =
        get_subscription_list(service_id);

    for (const auto &src : subscription_receive_contracts) {
      publish_data(service_id, provider, std::get<0>(src), std::get<1>(src), 0,
               data_json);
    }
  }
}

void bos_oracle::autopublish(uint64_t service_id, name provider,
                             uint64_t request_id,
                             string data_json) {
   data_services svctable(_self, _self.value);
  auto service_itr = svctable.find(service_id);
  check(service_itr != svctable.end(), "service does not exist");
  
  ///begin|____________________update cycle____________________________|
  ///begin|____________________duration__|end
  //begin = nowsec/update_cycle*update_cycles
  //end = update_number * update_cycle +duration
  uint64_t  update_cycle = service_itr->update_cycle;
   check(update_cycle > 0, "update cycle could not greater than zero");
  uint64_t duration  = service_itr->duration;
   check(duration > 0 && duration <= update_cycle, "wrong duration value");
  uint64_t now_sec = time_point_sec(now()).sec_since_epoch();
  uint64_t  update_number = now_sec/update_cycle;
  uint64_t end_sec = update_number * update_cycle +duration;
  check( end_sec >= now_sec," push cycle is expired,wait until next ");
  // {
  //   print(" auto publishdata   if( end_sec < now_sec)");
  //   return;
  // }
 
  print(" auto publishdata  in");
  require_auth(provider);

  transaction t;
  t.actions.emplace_back(permission_level{_self, active_permission}, _self,
                         "innerpublish"_n,
                         std::make_tuple(service_id, provider, update_number, request_id, data_json));
  t.delay_sec = 0;
  uint128_t deferred_id =
      (uint128_t(service_id) << 64) | update_number;
  cancel_deferred(deferred_id);
  t.send(deferred_id, _self, true);
}

void bos_oracle::publishdata(uint64_t service_id, name provider,
                             uint64_t update_number, uint64_t request_id,
                             string data_json) {
  //print(" publishdata in");
  require_auth(provider);

  transaction t;
  t.actions.emplace_back(permission_level{_self, active_permission}, _self,
                         "innerpublish"_n,
                         std::make_tuple(service_id, provider, update_number, request_id, data_json));
  t.delay_sec = 0;
  uint128_t deferred_id =
      (uint128_t(service_id) << 64) | update_number;
  cancel_deferred(deferred_id);
  t.send(deferred_id, _self, true);
}

void bos_oracle::innerpublish(uint64_t service_id, name provider,
                          uint64_t update_number,
                          uint64_t request_id, string data_json) {
  print(" innerpublish in");
 name contract_account = _self;  // placeholder
 name action_name = _self;// placeholder
  require_auth(_self);
  check(service_status::service_in == get_service_status(service_id),
        "service and subscription must be available");

  // if (0 == request_id) {
  //   time_point_sec pay_time =
  //       get_payment_time(service_id, contract_account, action_name);
  //   pay_time+=eosio::days(30);
  //   if (pay_time < time_point_sec(now())) {
  //     fee_service(service_id, contract_account, action_name,
  //                 fee_type::fee_month);
  //   }
  // }

  data_service_provision_logs logtable(_self, service_id);
  logtable.emplace(_self, [&](auto &l) {
    l.log_id = logtable.available_primary_key();
    l.service_id = service_id;
    l.account = provider;
    l.data_json = data_json;
    l.update_number = update_number;
    l.status = 0;
    // l.provider_sig = provider_signature;
    l.contract_account = contract_account;
    l.action_name = action_name;
    l.request_id = request_id;
    l.update_time = time_point_sec(now());
  });

  add_times(service_id, provider, contract_account, action_name,
            0 != request_id);

  print("check publish before=",provider,"s=",service_id,"u=",update_number);
  check_publish_service(service_id,update_number);
  // require_recipient(contract_account);
}

/**
 * @brief
 *
 * @param account
 * @param receive_account
 */
void bos_oracle::claim(name account, name receive_account) {
  require_auth(account);
  provider_services proviservicestable(_self, account.value);
  uint64_t consumption = 0;
  uint64_t month_consumption = 0;
  uint64_t service_times = 0;
  uint64_t provide_times = 0;
  uint64_t service_month_times = 0;
  uint64_t provide_month_times = 0;
  uint64_t claimreward = 0;
  uint64_t income = 0;
  uint64_t month_income = 0;

  asset stake_freeze_amount = asset(0, core_symbol());
  asset service_stake_freeze_amount = asset(0, core_symbol());
  asset total_stake_freeze_amount = asset(0, core_symbol());
  uint64_t stake_freeze_income = 0;

  data_providers providertable(_self, _self.value);
  auto provider_itr = providertable.find(account.value);
  check(provider_itr != providertable.end(), "");

  check(time_point_sec(now()) - provider_itr->last_claim_time >= eosio::days(1),
        "claim span must be greater than one day");

  auto calc_income = [](uint64_t service_times, uint64_t provide_times,
                        uint64_t consumption) -> uint64_t {
                          //print("==========provide_times=================");
                          //print(provide_times);
                           //print("==========service_times=================");
                            //print(service_times);

    uint64_t income = 0;
     if (provide_times > 0 && service_times >= provide_times) {

    income = consumption * provide_times / static_cast<double>(service_times);
     }

    return income;
  };

  for (const auto &p : proviservicestable) {
    std::tie(consumption, month_consumption) = get_consumption(p.service_id);
    std::tie(service_times, service_month_times, provide_times,
             provide_month_times) = get_times(p.service_id, account);

    std::tie(stake_freeze_amount, service_stake_freeze_amount) =
        get_freeze_stat(p.service_id, account);

    income += calc_income(service_times, provide_times, consumption * 0.8);

    month_income += calc_income(service_month_times, provide_month_times,
                                month_consumption * 0.8);

    uint64_t stake_income = (consumption + month_consumption) * 0.8;
    // check(stake_freeze_amount.amount > 0 &&
    //           service_stake_freeze_amount.amount > stake_freeze_amount.amount,
    //       "provider freeze_amount and service_times must greater than zero");
  stake_freeze_income = 0;
  if(stake_freeze_amount.amount > 0 &&
              service_stake_freeze_amount.amount >=stake_freeze_amount.amount)
              {
    stake_freeze_income =
        stake_income * stake_freeze_amount.amount /
        static_cast<double>(service_stake_freeze_amount.amount);
              }
  }
  asset new_income =
      asset(income + month_income + stake_freeze_income, core_symbol()) -
      provider_itr->claim_amount;

  check(new_income.amount > 0, "no income ");

  providertable.modify(provider_itr, same_payer,
                       [&](auto &p) { p.claim_amount += new_income; });

  transfer(consumer_account, receive_account, new_income, "claim");
  
}

/**
 * @brief
 *
 * @param service_id
 * @param action_type
 */
void bos_oracle::execaction(uint64_t service_id, uint64_t action_type) {
  require_auth(_self);
  check(service_status::service_freeze == action_type||service_status::service_emergency == action_type, "unknown action type,support action:freeze(3),emergency(4)");
  data_services svctable(_self, _self.value);
  auto service_itr = svctable.find(service_id);
  check(service_itr != svctable.end(), "service does not exist");
  svctable.modify(service_itr, _self, [&](auto &s) {
    if (service_status::service_freeze == action_type) {
      s.freeze_flag = true;
    } else {
      s.emergency_flag = true;
    }
  });
}

/**
 * @brief
 *
 * @param service_id
 * @param signature
 * @param account
 * @param status
 */
void bos_oracle::unregservice(uint64_t service_id, name account,
                              uint64_t status) {
  require_auth(account);
 

  data_service_provisions provisionstable(_self, service_id);
  auto provision_itr = provisionstable.find(account.value);
  check(provision_itr != provisionstable.end(),
        "provider does not subscribe service ");

  provisionstable.modify(provision_itr, same_payer,
                         [&](auto &p) { p.status = status; });

  if (static_cast<uint64_t>(service_status::service_cancel) == status) {
    svc_provision_cancel_applys applytable(_self, _self.value);
    applytable.emplace(_self, [&](auto &a) {
      a.apply_id = applytable.available_primary_key();
      a.service_id = service_id;
      a.provider = account;
      a.status = apply_status::apply_init;
      a.cancel_time = time_point_sec(now());
      a.finish_time = time_point_sec();
    });

    provider_services provservicestable(_self, account.value);

    auto provservice_itr = provservicestable.find(service_id);
    check(provservice_itr != provservicestable.end(),
          "provider does not subscribe service");
    provservicestable.erase(provservice_itr);

    asset freeze_amount = asset(0, core_symbol()); /// calculates  unfinish code
    asset amount = asset(0, core_symbol());
    check(asset(0, core_symbol()) == freeze_amount,
          "freeze amount must equal zero");

    transfer(provider_account, account, amount, "");
  }


   // data_services svctable(_self, _self.value);
  // auto service_itr = svctable.find(service_id);
  // check(service_itr != svctable.end(), "service does not exist");
  // svctable.modify(service_itr, _self, [&](auto &s) { s.status = status; });

}


/**
 * @brief 
 * 
 * @param service_id 
 * @param contract_account 
 * @param action_name 
 * @param amount 
 */
void bos_oracle::starttimer() {
  //print("%%%%%%%%%%%%%%%%%%%%%%%%starttimer^^^^^^^^^^^^^^^^^^^^^^^^^^^");
  require_auth(_self);
  // data_services svctable(_self, _self.value);
  // auto service_itr = svctable.find(service_id);
  // check(service_itr != svctable.end(), "service does not exist");
    check_publish_services();
   start_timer();
  
  
}

void bos_oracle::start_timer()
{

  transaction t;
  t.actions.emplace_back(permission_level{_self, active_permission}, _self,
                         "starttimer"_n, std::make_tuple(0));
  t.delay_sec = 10; // seconds
  uint128_t deferred_id =
      (uint128_t(12345) << 64) | 67890;
  cancel_deferred(deferred_id);
  t.send(deferred_id, _self);
  
}

void bos_oracle::check_publish_services()
{
  print("check_publish_service");
  data_services svctable(_self, _self.value);

  std::vector<std::tuple<uint64_t,uint64_t,uint64_t>> service_numbers = get_publish_services_update_number();

  for(auto it = service_numbers.begin();it != service_numbers.end();++it)
  {
    string data_json = get_publish_data(std::get<0>(*it),std::get<1>(*it),std::get<2>(*it));
   if(!data_json.empty())
   {
       print("if(!data_json.empty())");
     save_publish_data(std::get<0>(*it),std::get<1>(*it),data_json);
   }
  }
  
}

void bos_oracle::check_publish_service(uint64_t service_id,uint64_t update_number)
{
    print("check_publish_service");

  data_services svctable(_self, _self.value);
  auto service_itr = svctable.find(service_id);
  check(service_itr != svctable.end(), "service does not exist");

  uint64_t  pc = get_provider_count(service_id);
  uint64_t ppc = get_publish_provider_count(service_id, update_number);
  if(pc<service_itr->provider_limit || ppc < pc)
  {
     print("check_publish_service  if(pc<service_itr->provider_limit || ppc < pc)",service_itr->provider_limit,"ppc=",ppc,",pc=",pc);
    return;
  }
 
  string data_json = get_publish_data(service_id,update_number,service_itr->provider_limit);
   if(!data_json.empty())
   {
     print("check_publish_service  if(!data_json.empty())");
     save_publish_data(service_id,update_number,data_json);
   }
    
}

void bos_oracle::save_publish_data(uint64_t service_id,uint64_t update_number,string  value)
{
    oracle_data oracledatatable(_self,service_id);
      auto itr = oracledatatable.find(update_number);
      check (itr== oracledatatable.end(),"repeat update number");

    oracledatatable.emplace(_self, [&](auto &d) {
      d.update_number =  update_number;
      d.value = value;
      d.timestamp = time_point_sec(now()).sec_since_epoch();
    });
}


uint64_t bos_oracle::get_provider_count(uint64_t service_id) {

  data_service_provisions provisionstable(_self, service_id);

  uint64_t  providers_count = 0;
  
  for (const auto &p : provisionstable) {
     if (p.status == provision_status::provision_reg && p.amount.amount-p.freeze_amount.amount > 0) {
      providers_count++;
    }
  }

  return providers_count;
}


uint64_t bos_oracle::get_publish_provider_count(uint64_t service_id,uint64_t update_number) {

  data_service_provision_logs logtable(_self, service_id);
  auto update_number_idx = logtable.get_index<"bynumber"_n>();
  auto update_number_itr_lower = update_number_idx.lower_bound(update_number);
  auto update_number_itr_upper= update_number_idx.upper_bound(update_number);
  uint64_t provider_count = 0;

  for (auto itr =   update_number_itr_lower;itr != update_number_idx.end() && itr != update_number_itr_upper;++itr) {
              provider_count++;
  }

  return provider_count;
}


string bos_oracle::get_publish_data(uint64_t service_id,uint64_t update_number,uint64_t  provider_limit) {

  if(provider_limit<3)
  {
    //print("provider limit equal to zero in get_publish_data");
    return "";
  }

  data_service_provision_logs logtable(_self, service_id);
  auto update_number_idx = logtable.get_index<"bynumber"_n>();
  auto update_number_itr_lower = update_number_idx.lower_bound(update_number);
  auto update_number_itr_upper= update_number_idx.upper_bound(update_number);
  std::map<string,int64_t> data_count;
  const int64_t  one_time = 1;
  uint64_t provider_count = 0;

  for (auto itr =   update_number_itr_lower;itr != update_number_idx.end() && itr != update_number_itr_upper;++itr) {
       auto it =  data_count.find(itr->data_json);
       if(it != data_count.end())
       {
         it->second++;
       }
       else
       {
         data_count[itr->data_json] = one_time;
       }

       provider_count++;
  }

  std::string result = "";
  if(provider_count>=provider_limit && data_count.size()==one_time)
  {
    result = data_count.begin()->first;
  }
  else if(provider_count>=provider_limit)
  {
    for(auto& d : data_count)
    {
          if(d.second >  provider_count/2+1)
          {
              result = data_count.begin()->first;
              //print("get data that is greater than one half of providers  ");
              break;
          }
    }

     //print("provider's data is not the same");
  }
  else
  {
    //print("provider's count is less than provider's limit");
  }
  
  

  return result;
}



std::vector<std::tuple<uint64_t,uint64_t,uint64_t>> bos_oracle::get_publish_services_update_number()
{
  std::vector<std::tuple<uint64_t,uint64_t,uint64_t>> service_numbers;

 data_services svctable(_self, _self.value);
  for (auto &s : svctable) {
    if (s.status != service_status::service_in || 0 == s.update_cycle) {
      continue;
    }

    uint32_t update_number = s.update_start_time.sec_since_epoch() / s.update_cycle;
    uint32_t now_sec = time_point_sec(now()).sec_since_epoch();
    if (s.update_cycle * update_number + s.duration <now_sec) {
      service_numbers.push_back(std::make_tuple(s.service_id, update_number,s.provider_limit));
      svctable.modify(s, _self, [&](auto &ss) { 
        ss.last_update_number = time_point_sec(now()).sec_since_epoch()/s.update_cycle;
        ss.update_start_time = time_point_sec(ss.last_update_number*s.update_cycle); 
        });
    }
  }


  return service_numbers;
}




// } // namespace bosoracle