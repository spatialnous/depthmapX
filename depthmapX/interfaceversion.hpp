// SPDX-FileCopyrightText: 2017 Christian Sailer
// SPDX-FileCopyrightText: 2021 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "../version.hpp"

#define INTERFACE_MAJOR_VERSION DEPTHMAPX_MAJOR_VERSION
#define INTERFACE_MINOR_VERSION DEPTHMAPX_MINOR_VERSION
#define INTERFACE_REVISION_VERSION DEPTHMAPX_REVISION_VERSION

// leave these alone - C Preprocessor magic to get stuff to the right format
#define TITLE_BASE                                                                                 \
    TITLE_BASE_FORMAT("depthmapX", INTERFACE_MAJOR_VERSION, INTERFACE_MINOR_VERSION,               \
                      INTERFACE_REVISION_VERSION)
