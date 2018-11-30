/** @file
 * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#include "val/common/val_client_defs.h"
#include "val/spe/val_partition_common.h"

int32_t server_test_psa_skip_with_invalid_handle(void);

server_test_t test_i037_server_tests_list[] = {
    NULL,
    server_test_psa_skip_with_invalid_handle,
    NULL,
};

int32_t server_test_psa_skip_with_invalid_handle(void)
{
    int32_t         status = VAL_STATUS_SUCCESS;
    psa_msg_t       msg = {0};

    status = val_process_connect_request(SERVER_UNSPECIFED_MINOR_V_SIG, &msg);
    if (val_err_check_set(TEST_CHECKPOINT_NUM(201), status))
    {
        psa_reply(msg.handle, PSA_CONNECTION_REFUSED);
        return status;
    }

    psa_reply(msg.handle, PSA_SUCCESS);

    status = val_process_call_request(SERVER_UNSPECIFED_MINOR_V_SIG, &msg);
    if (val_err_check_set(TEST_CHECKPOINT_NUM(202), status))
    {
        psa_reply(msg.handle, -2);
    }
    else
    {
        /* Setting boot.state before test check */
        status = val_set_boot_flag(BOOT_EXPECTED_NS);
        if (val_err_check_set(TEST_CHECKPOINT_NUM(203), status))
        {
            val_print(PRINT_ERROR, "\tFailed to set boot flag before check\n", 0);
            psa_reply(msg.handle, -3);
        }
        else
        {
            /* Test check- psa_skip with INVALID_HANDLE */
            psa_skip(INVALID_HANDLE, 0, 0);

            /* shouldn't have reached here */
            val_print(PRINT_ERROR,
                "\tpsa_skip with invalid handle should failed but successed\n", 0);

            /* Resetting boot.state to catch unwanted reboot */
            if (val_set_boot_flag(BOOT_EXPECTED_BUT_FAILED))
            {
                val_print(PRINT_ERROR, "\tFailed to set boot flag after check\n", 0);
            }

            status = VAL_STATUS_SPM_FAILED;
            psa_reply(msg.handle, -4);
        }
    }

    val_err_check_set(TEST_CHECKPOINT_NUM(204), status);
    status = ((val_process_disconnect_request(SERVER_UNSPECIFED_MINOR_V_SIG, &msg))
               ? VAL_STATUS_ERROR : status);
    psa_reply(msg.handle, PSA_SUCCESS);
    return status;
}