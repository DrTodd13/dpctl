//===-- dppy_oneapi_interface.hpp - DPPY-SYCL interface ---*- C++ -*-------===//
//
//                     Data Parallel Python (DPPY)
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
/// This file contains the declaration of a C++ API to expose a lightweight SYCL
/// interface for the Python dppy package.
///
//===----------------------------------------------------------------------===//

#pragma once

#include <cstdint>
#include <deque>

namespace dppy
{

/*!
 * Redefinition of Sycl's device_type so that we do not have to include
 * sycl.hpp here, and in the Python bindings.
 */
enum class sycl_device_type : unsigned int
{
    cpu,
    gpu,
    accelerator,
    custom,
    automatic,
    host,
    all
};


////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// DppyOneAPIRuntime /////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*! \class DppyOneAPIRuntime
 *  \brief A runtime and context factory class
 *
 */
class DppyOneAPIRuntime
{
public:
    int64_t getNumPlatforms (size_t *platforms) const;
    int64_t getCurrentQueue (void **Q) const;
    int64_t getQueue (void **Q,
                      dppy::sycl_device_type DeviceTy,
                      size_t DNum = 0) const;
    int64_t resetGlobalQueue (dppy::sycl_device_type DeviceTy,
                              size_t DNum = 0);
    /*!
     * Push a new sycl queue to the top of the activate_queues deque. The
     * newly activated queue is returned to caller inside the Q object.
     */
    int64_t activateQueue (void **Q,
                           dppy::sycl_device_type DeviceTy,
                           size_t DNum);
    int64_t deactivateCurrentQueue ();
    int64_t dump () const;
    int64_t dump_queue (const void *Q) const;
};


int64_t deleteQueue (void *Q);

} /* end of namespace dppy */
