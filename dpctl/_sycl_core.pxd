##===------------- sycl_core.pxd - dpctl module --------*- Cython -*-------===##
##
##                      Data Parallel Control (dpCtl)
##
## Copyright 2020 Intel Corporation
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##    http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##
##===----------------------------------------------------------------------===##
##
## \file
## This file declares the extension types and functions for the Cython API
## implemented in sycl_core.pyx.
##
##===----------------------------------------------------------------------===##

# distutils: language = c++
# cython: language_level=3

from ._backend cimport *
from libc.stdint cimport uint32_t


cdef class SyclContext:
    ''' Wrapper class for a Sycl Context
    '''
    cdef DPPLSyclContextRef _ctxt_ref

    @staticmethod
    cdef SyclContext _create (DPPLSyclContextRef ctxt)
    cpdef bool equals (self, SyclContext ctxt)
    cdef DPPLSyclContextRef get_context_ref (self)


cdef class SyclDevice:
    ''' Wrapper class for a Sycl Device
    '''
    cdef DPPLSyclDeviceRef _device_ref
    cdef const char *_vendor_name
    cdef const char *_device_name
    cdef const char *_driver_version
    cdef uint32_t _max_compute_units
    cdef uint32_t _max_work_item_dims
    cdef size_t *_max_work_item_sizes
    cdef size_t _max_work_group_size
    cdef uint32_t _max_num_sub_groups
    cdef bool _int64_base_atomics
    cdef bool _int64_extended_atomics

    @staticmethod
    cdef SyclDevice _create (DPPLSyclDeviceRef dref)
    cdef DPPLSyclDeviceRef get_device_ref (self)
    cpdef get_device_name (self)
    cpdef get_device_type (self)
    cpdef get_vendor_name (self)
    cpdef get_driver_version (self)
    cpdef get_max_compute_units (self)
    cpdef get_max_work_item_dims (self)
    cpdef get_max_work_item_sizes (self)
    cpdef get_max_work_group_size (self)
    cpdef get_max_num_sub_groups (self)
    cpdef has_int64_base_atomics (self)
    cpdef has_int64_extended_atomics (self)


cdef class SyclEvent:
    ''' Wrapper class for a Sycl Event
    '''
    cdef  DPPLSyclEventRef _event_ref
    cdef list _args

    @staticmethod
    cdef  SyclEvent _create (DPPLSyclEventRef e, list args)
    cdef  DPPLSyclEventRef get_event_ref (self)
    cpdef void wait (self)


cdef class SyclKernel:
    ''' Wraps a sycl::kernel object created from an OpenCL interoperability
        kernel.
    '''
    cdef DPPLSyclKernelRef _kernel_ref
    cdef const char *_function_name
    cdef DPPLSyclKernelRef get_kernel_ref (self)

    @staticmethod
    cdef SyclKernel _create (DPPLSyclKernelRef kref)


cdef class SyclProgram:
    ''' Wraps a sycl::program object created from an OpenCL interoperability
        program.

        SyclProgram exposes the C API from dppl_sycl_program_interface.h. A
        SyclProgram can be created from either a source string or a SPIR-V
        binary file.
    '''
    cdef DPPLSyclProgramRef _program_ref

    @staticmethod
    cdef  SyclProgram _create (DPPLSyclProgramRef pref)
    cdef  DPPLSyclProgramRef get_program_ref (self)
    cpdef SyclKernel get_sycl_kernel(self, str kernel_name)


cdef class SyclQueue:
    ''' Wrapper class for a Sycl queue.
    '''
    cdef DPPLSyclQueueRef _queue_ref
    cdef SyclContext _context
    cdef SyclDevice _device

    cdef _raise_queue_submit_error (self, fname, errcode)
    cdef _raise_invalid_range_error (self, fname, ndims, errcode)
    cdef int _populate_args (self, list args, void **kargs,
                             DPPLKernelArgType *kargty)
    cdef int _populate_range (self, size_t Range[3], list gS, size_t nGS)

    @staticmethod
    cdef  SyclQueue _create (DPPLSyclQueueRef qref)
    @staticmethod
    cdef  SyclQueue _create_from_context_and_device (SyclContext ctx, SyclDevice dev)
    cpdef bool equals (self, SyclQueue q)
    cpdef SyclContext get_sycl_context (self)
    cpdef SyclDevice get_sycl_device (self)
    cdef  DPPLSyclQueueRef get_queue_ref (self)
    cpdef SyclEvent submit (self, SyclKernel kernel, list args, list gS,
                            list lS=*, list dEvents=*)
    cpdef void wait (self)
    cdef DPPLSyclQueueRef get_queue_ref (self)
    cpdef memcpy (self, dest, src, size_t count)
    cpdef prefetch (self, ptr, size_t count=*)
    cpdef mem_advise (self, ptr, size_t count, int mem)


cpdef SyclQueue get_current_queue()
cpdef get_current_device_type ()
cpdef get_current_backend()
