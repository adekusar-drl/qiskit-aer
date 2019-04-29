# This code is part of Qiskit.
#
# (C) Copyright IBM Corp. 2017 and later.
#
# This code is licensed under the Apache License, Version 2.0. You may
# obtain a copy of this license in the LICENSE.txt file in the root directory
# of this source tree or at http://www.apache.org/licenses/LICENSE-2.0.
#
# Any modifications or derivative works of this code must retain this
# copyright notice, and modified files need to carry a notice indicating
# that they have been altered from the originals.

"""
Cython wrapper for Aer QasmController.
"""

from libcpp.string cimport string

cdef extern from "simulators/qasm/qasm_controller.hpp" namespace "AER::Simulator":
    cdef cppclass QasmController:
        QasmController() except +

cdef extern from "base/controller.hpp" namespace "AER":
    cdef string controller_execute[QasmController](string &qobj) except +


def qasm_controller_execute(qobj):
    """Execute qobj on Aer C++ QasmController"""
    return controller_execute[QasmController](qobj)
