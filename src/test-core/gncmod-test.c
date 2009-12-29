/*********************************************************************
 * gncmod-test.c
 * module definition/initialization for the test infrastructure
 *
 * Copyright (c) 2001 Linux Developers Group, Inc.
 *********************************************************************/

#include <stdio.h>
#include <gmodule.h>

#include "gnc-module.h"
#include "gnc-module-api.h"

GNC_MODULE_API_DECL(libgncmod_test)

/* version of the gnc module system interface we require */
int libgncmod_test_gnc_module_system_interface = 0;

/* module versioning uses libtool semantics. */
int libgncmod_test_gnc_module_current  = 0;
int libgncmod_test_gnc_module_revision = 0;
int libgncmod_test_gnc_module_age      = 0;


char *
libgncmod_test_gnc_module_path(void)
{
    return g_strdup("gnucash/test");
}

char *
libgncmod_test_gnc_module_description(void)
{
    return g_strdup("Basic GnuCash test infrastructure.");
}

int
libgncmod_test_gnc_module_init(int refcount)
{
    return TRUE;
}

int
libgncmod_test_gnc_module_end(int refcount)
{
    return TRUE;
}
