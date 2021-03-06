//===------- test_sycl_queue_manager.cpp - dpctl-C_API ---*--- C++ ----*---===//
//
//               Data Parallel Control Library (dpCtl)
//
// Copyright 2020 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file has unit test cases for functions defined in
/// dppl_sycl_queue_interface.h and dppl_sycl_queue_manager.h.
///
//===----------------------------------------------------------------------===//
#include "dppl_sycl_context_interface.h"
#include "dppl_sycl_device_interface.h"
#include "dppl_sycl_queue_interface.h"
#include "dppl_sycl_queue_manager.h"
#include <gtest/gtest.h>
#include <thread>

#include <CL/sycl.hpp>

using namespace std;
using namespace cl::sycl;

namespace
{
void foo (size_t & num)
{
    auto q1 = DPPLQueueMgr_PushQueue(DPPL_OPENCL, DPPL_CPU, 0);
    auto q2 = DPPLQueueMgr_PushQueue(DPPL_OPENCL, DPPL_GPU, 0);
    // Capture the number of active queues in first
    num = DPPLQueueMgr_GetNumActivatedQueues();
    DPPLQueueMgr_PopQueue();
    DPPLQueueMgr_PopQueue();
    DPPLQueue_Delete(q1);
    DPPLQueue_Delete(q2);
}

void bar (size_t & num)
{
    auto q1 = DPPLQueueMgr_PushQueue(DPPL_OPENCL, DPPL_GPU, 0);
    // Capture the number of active queues in second
    num = DPPLQueueMgr_GetNumActivatedQueues();
    DPPLQueueMgr_PopQueue();
    DPPLQueue_Delete(q1);
}

bool has_devices ()
{
    bool ret = false;
    for (auto &p : platform::get_platforms()) {
        if (p.is_host())
            continue;
        if(!p.get_devices().empty()) {
            ret = true;
            break;
        }
    }
    return ret;
}

}

struct TestDPPLSyclQueueManager : public ::testing::Test
{ };


TEST_F (TestDPPLSyclQueueManager, CheckDPPLGetCurrentQueue)
{
    if(!has_devices())
        GTEST_SKIP_("Skipping: No Sycl devices.\n");

    DPPLSyclQueueRef q = nullptr;
    ASSERT_NO_THROW(q = DPPLQueueMgr_GetCurrentQueue());
    ASSERT_TRUE(q != nullptr);
}


TEST_F (TestDPPLSyclQueueManager, CheckDPPLGetOpenCLCpuQ)
{
    if(!has_devices())
        GTEST_SKIP_("Skipping: No Sycl devices.\n");

    auto nOpenCLCpuQ = DPPLQueueMgr_GetNumQueues(DPPL_OPENCL, DPPL_CPU);
    if(!nOpenCLCpuQ)
        GTEST_SKIP_("Skipping: No OpenCL CPU device found.");

    auto q = DPPLQueueMgr_GetQueue(DPPL_OPENCL, DPPL_CPU, 0);
    EXPECT_TRUE(q != nullptr);
    auto sycl_q = reinterpret_cast<queue*>(q);
    auto be = sycl_q->get_context().get_platform().get_backend();
    EXPECT_EQ(be, backend::opencl);
    auto devty = sycl_q->get_device().get_info<info::device::device_type>();
    EXPECT_EQ(devty, info::device_type::cpu);

    auto non_existent_device_num = nOpenCLCpuQ + 1;
    // Non-existent device number should return nullptr
    auto null_q = DPPLQueueMgr_GetQueue(DPPL_OPENCL, DPPL_CPU,
                                        non_existent_device_num);
    ASSERT_TRUE(null_q == nullptr);
}

TEST_F (TestDPPLSyclQueueManager, CheckDPPLGetOpenCLGpuQ)
{
    if(!has_devices())
        GTEST_SKIP_("Skipping: No Sycl devices.\n");

    auto nOpenCLGpuQ = DPPLQueueMgr_GetNumQueues(DPPL_OPENCL, DPPL_GPU);
    if(!nOpenCLGpuQ)
        GTEST_SKIP_("Skipping: No OpenCL GPU device found.\n");

    auto q = DPPLQueueMgr_GetQueue(DPPL_OPENCL, DPPL_GPU, 0);
    EXPECT_TRUE(q != nullptr);
    auto sycl_q = reinterpret_cast<queue*>(q);
    auto be = sycl_q->get_context().get_platform().get_backend();
    EXPECT_EQ(be, backend::opencl);
    auto devty = sycl_q->get_device().get_info<info::device::device_type>();
    EXPECT_EQ(devty, info::device_type::gpu);

    auto non_existent_device_num = nOpenCLGpuQ + 1;
    // Non-existent device number should return nullptr
    auto null_q = DPPLQueueMgr_GetQueue(DPPL_OPENCL, DPPL_GPU,
                                        non_existent_device_num);
    ASSERT_TRUE(null_q == nullptr);
}

TEST_F (TestDPPLSyclQueueManager, CheckDPPLGetLevel0GpuQ)
{
    if(!has_devices())
        GTEST_SKIP_("Skipping: No Sycl devices.\n");

    auto nL0GpuQ = DPPLQueueMgr_GetNumQueues(DPPL_LEVEL_ZERO, DPPL_GPU);
    if(!nL0GpuQ)
        GTEST_SKIP_("Skipping: No OpenCL GPU device found.\n");

    auto q = DPPLQueueMgr_GetQueue(DPPL_LEVEL_ZERO, DPPL_GPU, 0);
    EXPECT_TRUE(q != nullptr);
    auto sycl_q = reinterpret_cast<queue*>(q);
    auto be = sycl_q->get_context().get_platform().get_backend();
    EXPECT_EQ(be, backend::level_zero);
    auto devty = sycl_q->get_device().get_info<info::device::device_type>();
    EXPECT_EQ(devty, info::device_type::gpu);

    auto non_existent_device_num = nL0GpuQ + 1;
    // Non-existent device number should return nullptr
    auto null_q = DPPLQueueMgr_GetQueue(DPPL_LEVEL_ZERO, DPPL_GPU,
                                          non_existent_device_num);
    ASSERT_TRUE(null_q == nullptr);
}

TEST_F (TestDPPLSyclQueueManager, CheckGetNumActivatedQueues)
{
    if(!has_devices())
        GTEST_SKIP_("Skipping: No Sycl devices.\n");

    size_t num0, num1, num2, num4;

    auto nOpenCLCpuQ = DPPLQueueMgr_GetNumQueues(DPPL_OPENCL, DPPL_CPU);
    auto nOpenCLGpuQ = DPPLQueueMgr_GetNumQueues(DPPL_OPENCL, DPPL_GPU);

    // Add a queue to main thread
    if(!nOpenCLCpuQ || !nOpenCLGpuQ)
        GTEST_SKIP_("Skipping as no OpenCL GPU device found.\n");

    auto q = DPPLQueueMgr_PushQueue(DPPL_OPENCL, DPPL_CPU, 0);

    std::thread first (foo, std::ref(num1));
    std::thread second (bar, std::ref(num2));

    // synchronize threads:
    first.join();
    second.join();

    // Capture the number of active queues in first
    num0 = DPPLQueueMgr_GetNumActivatedQueues();
    DPPLQueueMgr_PopQueue();
    num4 = DPPLQueueMgr_GetNumActivatedQueues();

    // Verify what the expected number of activated queues each time a thread
    // called getNumActivatedQueues.
    EXPECT_EQ(num0, 1ul);
    EXPECT_EQ(num1, 2ul);
    EXPECT_EQ(num2, 1ul);
    EXPECT_EQ(num4, 0ul);

    DPPLQueue_Delete(q);
}

TEST_F (TestDPPLSyclQueueManager, CheckDPPLDumpDeviceInfo)
{
    if(!has_devices())
        GTEST_SKIP_("Skipping: No Sycl devices.\n");
    auto q = DPPLQueueMgr_GetCurrentQueue();
    EXPECT_NO_FATAL_FAILURE(DPPLDevice_DumpInfo(DPPLQueue_GetDevice(q)));
    EXPECT_NO_FATAL_FAILURE(DPPLQueue_Delete(q));
}

TEST_F (TestDPPLSyclQueueManager, CheckIsCurrentQueue)
{
    if(!has_devices())
        GTEST_SKIP_("Skipping: No Sycl devices.\n");
    if(!DPPLQueueMgr_GetNumQueues(DPPL_OPENCL, DPPL_GPU))
        GTEST_SKIP_("Skipping: No OpenCL GPU.\n");

    auto Q0 = DPPLQueueMgr_GetCurrentQueue();
    EXPECT_TRUE(DPPLQueueMgr_IsCurrentQueue(Q0));
    auto Q1 = DPPLQueueMgr_PushQueue(DPPL_OPENCL, DPPL_GPU, 0);
    EXPECT_TRUE(DPPLQueueMgr_IsCurrentQueue(Q1));
    DPPLQueue_Delete(Q1);
    DPPLQueueMgr_PopQueue();
    EXPECT_TRUE(DPPLQueueMgr_IsCurrentQueue(Q0));
    DPPLQueue_Delete(Q0);
}

TEST_F (TestDPPLSyclQueueManager, CheckIsCurrentQueue2)
{
    if(!DPPLQueueMgr_GetNumQueues(DPPL_OPENCL, DPPL_CPU) ||
       !DPPLQueueMgr_GetNumQueues(DPPL_OPENCL, DPPL_GPU))
        GTEST_SKIP_("Skipping: No OpenCL GPU and OpenCL CPU.\n");

    auto Q1 = DPPLQueueMgr_PushQueue(DPPL_OPENCL, DPPL_GPU, 0);
    EXPECT_TRUE(DPPLQueueMgr_IsCurrentQueue(Q1));
    auto Q2 = DPPLQueueMgr_PushQueue(DPPL_OPENCL, DPPL_CPU, 0);
    EXPECT_TRUE(DPPLQueueMgr_IsCurrentQueue(Q2));
    EXPECT_FALSE(DPPLQueueMgr_IsCurrentQueue(Q1));
    DPPLQueue_Delete(Q2);
    DPPLQueueMgr_PopQueue();
    EXPECT_TRUE(DPPLQueueMgr_IsCurrentQueue(Q1));
    DPPLQueue_Delete(Q1);
    DPPLQueueMgr_PopQueue();
}

TEST_F (TestDPPLSyclQueueManager, CreateQueueFromDeviceAndContext)
{
    auto Q = DPPLQueueMgr_GetCurrentQueue();
    auto D = DPPLQueue_GetDevice(Q);
    auto C = DPPLQueue_GetContext(Q);

    auto Q2 = DPPLQueueMgr_GetQueueFromContextAndDevice(C, D);
    auto D2 = DPPLQueue_GetDevice(Q2);
    auto C2 = DPPLQueue_GetContext(Q2);

    EXPECT_TRUE(DPPLDevice_AreEq(D, D2));
    EXPECT_TRUE(DPPLContext_AreEq(C, C2));

    DPPLQueue_Delete(Q2);
    DPPLQueue_Delete(Q);
}

