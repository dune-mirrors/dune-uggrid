// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
// SPDX-FileCopyrightInfo: Copyright © DUNE Project contributors, see file LICENSE.md in module root
// SPDX-License-Identifier: LGPL-2.1-or-later

/** \file

   Defines macros to put symbols into namespaces if C++ compiler is
   used. Everything is put into the namespace UG, dimension-dependent
   functions into UG::D2 or UG::D3

   in a header-files use:

   #include "namespace.h"

    START_UG_NAMESPACE

    ...

    END_UG_NAMESPACE

   for stuff that is independent of the space dimension or

   #include "namespace.h"

    START_UGDIM_NAMESPACE

    ...

    END_UGDIM_NAMESPACE

   else.  In the implementation:

   #include "namespace.h"   // of course

    USING_UG_NAMESPACE       // for stuff from the namespace UG

   or
    USING_UGDIM_NAMESPACE    // for stuff from UG::D3 resp. UG::D2

   Write

    int NS_PREFIX function(...) { ... };

   if function is declared in UG and

    int NS_DIM_PREFIX function(...) { ... };

   if it is declared in a namespace with dimension.

 */

#ifndef UG_NAMESPACE_H
#define UG_NAMESPACE_H

#define START_UG_NAMESPACE namespace UG {
#define END_UG_NAMESPACE }
#define END_UGDIM_NAMESPACE }}
#define NS_PREFIX UG::
#define USING_UG_NAMESPACE using namespace UG;

#ifdef UG_DIM_3
#define START_UGDIM_NAMESPACE namespace UG { namespace D3 {
#define USING_UGDIM_NAMESPACE using namespace UG::D3;
#define USING_UG_NAMESPACES namespace UG {namespace D3 {} } using namespace UG; using namespace UG::D3;
#define NS_DIM_PREFIX UG::D3::
#else
#define START_UGDIM_NAMESPACE namespace UG { namespace D2 {
#define USING_UGDIM_NAMESPACE using namespace UG::D2;
#define USING_UG_NAMESPACES namespace UG {namespace D2 {} } using namespace UG; using namespace UG::D2;
#define NS_DIM_PREFIX UG::D2::
#endif

/* check if the required symbols exist */
#if !defined(NS_PREFIX) || !defined(START_UG_NAMESPACE) || !defined(END_UG_NAMESPACE)
# error missing symbol!
#endif

#endif
