/*

  bos_oracle


*/

#include <eosio/eosio.hpp>
// #include <eosio/fixedpoint.hpp>
// #include <eosio/chain.h>
#include "bos.arbitration.cpp"
#include "bos.consumer.cpp"
#include "bos.fee.cpp"
#include "bos.oracle/bos.oracle.hpp"
#include "bos.provider.cpp"
#include "bos.riskcontrol.cpp"
#include "bos.util.cpp"

using namespace eosio;

// EOSIO_DISPATCH(bos_oracle,
// (regservice)(unregservice)(execaction)
// )

struct transferx {
  name from;
  name to;
  asset quantity;
  string memo;

  EOSLIB_SERIALIZE(transferx, (from)(to)(quantity)(memo))
};

extern "C" void apply(uint64_t receiver, uint64_t code, uint64_t action) {
  if (action == "onerror"_n.value) {
    /* onerror is only valid if it is for the "eosio" code account and
     * authorized by "eosio"'s "active permission */
    check(code == "eosio"_n.value,
          "onerror action's are only valid from the \"eosio\" system account");
  }

  if (code == receiver || action == "onerror"_n.value) {
    switch (action) {
      // NB: Add custom method in bracets after () to use them as
      // endpoints
      EOSIO_DISPATCH_HELPER(
          bos_oracle,
         (regservice)(unregservice)(execaction)(unstakeasset)(pushdata)(oraclepush)(
              addfeetypes)(claim)(subscribe)(requestdata)(starttimer)(cleardata)
              (uploadeviden)(uploadresult)(acceptarbi)(unstakearbi)(claimarbi)(timertimeout)
              (deposit)(withdraw)
              (importwps)(setstatus)
              (setparameter)
              )
    }
  }
  if (code == "eosio.token"_n.value && action == "transfer"_n.value) {

    datastream<const char *> ds = datastream<const char *>(nullptr, 0);

    bos_oracle thiscontract(name(receiver), name(code), ds);

    const transferx &t = unpack_action_data<transferx>();
    thiscontract.on_transfer(t.from, t.to, t.quantity, t.memo);
  }
}