/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/serialization/json/tests/shared.h"

void test_serialize_check_consistency(void) {
  serializer_t serializer;
  const char* json_text =
      "{\"command\":\"checkConsistency\",\"tails\":"
      "[\"" TEST_81_TRYTES_1
      "\","
      "\"" TEST_81_TRYTES_2 "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  check_consistency_req_t* req = check_consistency_req_new();
  flex_trit_t trits_243[FLEX_TRIT_SIZE_243];

  TEST_ASSERT(flex_trits_from_trytes(trits_243, NUM_TRITS_HASH,
                                     (const tryte_t*)TEST_81_TRYTES_1,
                                     NUM_TRYTES_HASH, NUM_TRYTES_HASH));
  TEST_ASSERT(hash243_queue_push(&req->tails, trits_243) == RC_OK);

  TEST_ASSERT(flex_trits_from_trytes(trits_243, NUM_TRITS_HASH,
                                     (const tryte_t*)TEST_81_TRYTES_2,
                                     NUM_TRYTES_HASH, NUM_TRYTES_HASH));
  TEST_ASSERT(hash243_queue_push(&req->tails, trits_243) == RC_OK);

  serializer.vtable.check_consistency_serialize_request(&serializer, req,
                                                        serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  check_consistency_req_free(&req);
}

void test_deserialize_check_consistency(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* res_true = "{\"state\":true,\"info\":\" \",\"duration\":0}";
  const char* res_false =
      "{\"state\":false,\"info\":\"" CONSISTENCY_INFO "\",\"duration\":0}";

  check_consistency_res_t* consistent_res = check_consistency_res_new();
  serializer.vtable.check_consistency_deserialize_response(
      &serializer, res_true, consistent_res);
  TEST_ASSERT_TRUE(consistent_res->state == true);
  TEST_ASSERT_EQUAL_STRING(" ", consistent_res->info->data);
  check_consistency_res_free(consistent_res);

  consistent_res = check_consistency_res_new();
  serializer.vtable.check_consistency_deserialize_response(
      &serializer, res_false, consistent_res);
  TEST_ASSERT_TRUE(consistent_res->state == false);
  TEST_ASSERT_EQUAL_STRING(CONSISTENCY_INFO, consistent_res->info->data);
  check_consistency_res_free(consistent_res);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_serialize_check_consistency);
  RUN_TEST(test_deserialize_check_consistency);
  return UNITY_END();
}