//===------------------- dppl_utils.h - dpctl-C_API ---*--- C++ -----*-----===//
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
/// This file defines common helper functions used in other places in DPPL.
//===----------------------------------------------------------------------===//

#pragma once

#include "dppl_data_types.h"
#include "Support/DllExport.h"
#include "Support/ExternC.h"
#include "Support/MemOwnershipAttrs.h"

DPPL_C_EXTERN_C_BEGIN

/*!
 * @brief Deletes the C String argument.
 *
 * @param    str            C string to be deleted
 */
DPPL_API
void DPPLCString_Delete (__dppl_take const char* str);

/*!
 * @brief Deletes an array of size_t elements.
 *
 * @param    arr            Array to be deleted.
 */
DPPL_API
void DPPLSize_t_Array_Delete (__dppl_take size_t* arr);

DPPL_C_EXTERN_C_END
