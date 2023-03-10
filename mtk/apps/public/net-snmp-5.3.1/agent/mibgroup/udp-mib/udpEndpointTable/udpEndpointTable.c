/*
 * Note: this file originally auto-generated by mib2c using
 *       version : 1.48 $ of : mfd-top.m2c,v $ 
 *
 * $Id: //BBN_Linux/Branch/Branch_for_Rel_EN7512_SDK_20150916/tclinux_phoenix/apps/public/net-snmp-5.3.1/agent/mibgroup/udp-mib/udpEndpointTable/udpEndpointTable.c#1 $
 */
/** \page MFD helper for udpEndpointTable
 *
 * \section intro Introduction
 * Introductory text.
 *
 */
/*
 * standard Net-SNMP includes 
 */
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

/*
 * include our parent header 
 */
#include "udpEndpointTable.h"

#include <net-snmp/agent/mib_modules.h>

#include "udpEndpointTable_interface.h"

oid             udpEndpointTable_oid[] = { UDPENDPOINTTABLE_OID };
int             udpEndpointTable_oid_size =
OID_LENGTH(udpEndpointTable_oid);

udpEndpointTable_registration udpEndpointTable_user_context;

void            initialize_table_udpEndpointTable(void);
void            shutdown_table_udpEndpointTable(void);


/**
 * Initializes the udpEndpointTable module
 */
void
init_udpEndpointTable(void)
{
    DEBUGMSGTL(("verbose:udpEndpointTable:init_udpEndpointTable",
                "called\n"));

    /*
     * TODO:300:o: Perform udpEndpointTable one-time module initialization.
     */

    /*
     * here we initialize all the tables we're planning on supporting
     */
    if (should_init("udpEndpointTable"))
        initialize_table_udpEndpointTable();

}                               /* init_udpEndpointTable */

/**
 * Shut-down the udpEndpointTable module (agent is exiting)
 */
void
shutdown_udpEndpointTable(void)
{
    if (should_init("udpEndpointTable"))
        shutdown_table_udpEndpointTable();

}

/**
 * Initialize the table udpEndpointTable 
 *    (Define its contents and how it's structured)
 */
void
initialize_table_udpEndpointTable(void)
{
    udpEndpointTable_registration *user_context;
    u_long          flags;

    DEBUGMSGTL(("verbose:udpEndpointTable:initialize_table_udpEndpointTable", "called\n"));

    /*
     * TODO:301:o: Perform udpEndpointTable one-time table initialization.
     */

    /*
     * TODO:302:o: |->Initialize udpEndpointTable user context
     * if you'd like to pass in a pointer to some data for this
     * table, allocate or set it up here.
     */
    /*
     * a netsnmp_data_list is a simple way to store void pointers. A simple
     * string token is used to add, find or remove pointers.
     */
    user_context =
        netsnmp_create_data_list("udpEndpointTable", NULL, NULL);

    /*
     * No support for any flags yet, but in the future you would
     * set any flags here.
     */
    flags = 0;

    /*
     * call interface initialization code
     */
    _udpEndpointTable_initialize_interface(user_context, flags);
}                               /* initialize_table_udpEndpointTable */

/**
 * Shutdown the table udpEndpointTable 
 */
void
shutdown_table_udpEndpointTable(void)
{
    /*
     * call interface shutdown code
     */
    _udpEndpointTable_shutdown_interface(&udpEndpointTable_user_context);
}

/**
 * pre-request callback
 *
 * @param user_context
 * @retval MFD_SUCCESS              : success.
 * @retval MFD_ERROR                : other error
 */
int
udpEndpointTable_pre_request(udpEndpointTable_registration * user_context)
{
    DEBUGMSGTL(("verbose:udpEndpointTable:udpEndpointTable_pre_request",
                "called\n"));

    /*
     * TODO:510:o: Perform udpEndpointTable pre-request actions.
     */

    return MFD_SUCCESS;
}                               /* udpEndpointTable_pre_request */

/**
 * post-request callback
 *
 * Note:
 *   New rows have been inserted into the container, and
 *   deleted rows have been removed from the container and
 *   released.
 * @param user_context
 * @param rc : MFD_SUCCESS if all requests succeeded
 *
 * @retval MFD_SUCCESS : success.
 * @retval MFD_ERROR   : other error (ignored)
 */
int
udpEndpointTable_post_request(udpEndpointTable_registration * user_context,
                              int rc)
{
    DEBUGMSGTL(("verbose:udpEndpointTable:udpEndpointTable_post_request",
                "called\n"));

    /*
     * TODO:511:o: Perform udpEndpointTable post-request actions.
     */

    return MFD_SUCCESS;
}                               /* udpEndpointTable_post_request */


/**********************************************************************
 **********************************************************************
 ***
 *** Table udpEndpointTable
 ***
 **********************************************************************
 **********************************************************************/
/*
 * UDP-MIB::udpEndpointTable is subid 7 of udp.
 * Its status is Current.
 * OID: .1.3.6.1.2.1.7.7, length: 8
 */

/*
 * ---------------------------------------------------------------------
 * * TODO:200:r: Implement udpEndpointTable data context functions.
 */


/**
 * set mib index(es)
 *
 * @param tbl_idx mib index structure
 * @param udpEndpointLocalAddressType_val
 * @param udpEndpointLocalAddress_val_ptr
 * @param udpEndpointLocalAddress_val_ptr_len
 * @param udpEndpointLocalPort_val
 * @param udpEndpointRemoteAddressType_val
 * @param udpEndpointRemoteAddress_val_ptr
 * @param udpEndpointRemoteAddress_val_ptr_len
 * @param udpEndpointRemotePort_val
 * @param udpEndpointInstance_val
 *
 * @retval MFD_SUCCESS     : success.
 * @retval MFD_ERROR       : other error.
 *
 * @remark
 *  This convenience function is useful for setting all the MIB index
 *  components with a single function call. It is assume that the C values
 *  have already been mapped from their native/rawformat to the MIB format.
 */
int
udpEndpointTable_indexes_set_tbl_idx(udpEndpointTable_mib_index * tbl_idx,
                                     u_long
                                     udpEndpointLocalAddressType_val,
                                     char *udpEndpointLocalAddress_val_ptr,
                                     size_t
                                     udpEndpointLocalAddress_val_ptr_len,
                                     u_long udpEndpointLocalPort_val,
                                     u_long
                                     udpEndpointRemoteAddressType_val, char
                                     *udpEndpointRemoteAddress_val_ptr,
                                     size_t
                                     udpEndpointRemoteAddress_val_ptr_len,
                                     u_long udpEndpointRemotePort_val,
                                     u_long udpEndpointInstance_val)
{
    DEBUGMSGTL(("verbose:udpEndpointTable:udpEndpointTable_indexes_set_tbl_idx", "called\n"));

    /*
     * udpEndpointLocalAddressType(1)/InetAddressType/ASN_INTEGER/long(u_long)//l/a/w/E/r/d/h 
     */
    tbl_idx->udpEndpointLocalAddressType = udpEndpointLocalAddressType_val;

    /*
     * udpEndpointLocalAddress(2)/InetAddress/ASN_OCTET_STR/char(char)//L/a/w/e/R/d/h 
     */
    tbl_idx->udpEndpointLocalAddress_len = sizeof(tbl_idx->udpEndpointLocalAddress) / sizeof(tbl_idx->udpEndpointLocalAddress[0]);      /* max length */
    /*
     * make sure there is enough space for udpEndpointLocalAddress data
     */
    if ((NULL == tbl_idx->udpEndpointLocalAddress) ||
        (tbl_idx->udpEndpointLocalAddress_len <
         (udpEndpointLocalAddress_val_ptr_len))) {
        snmp_log(LOG_ERR, "not enough space for value\n");
        return MFD_ERROR;
    }
    tbl_idx->udpEndpointLocalAddress_len =
        udpEndpointLocalAddress_val_ptr_len;
    memcpy(tbl_idx->udpEndpointLocalAddress,
           udpEndpointLocalAddress_val_ptr,
           udpEndpointLocalAddress_val_ptr_len *
           sizeof(udpEndpointLocalAddress_val_ptr[0]));

    /*
     * udpEndpointLocalPort(3)/InetPortNumber/ASN_UNSIGNED/u_long(u_long)//l/a/w/e/R/d/H 
     */
    tbl_idx->udpEndpointLocalPort = udpEndpointLocalPort_val;

    /*
     * udpEndpointRemoteAddressType(4)/InetAddressType/ASN_INTEGER/long(u_long)//l/a/w/E/r/d/h 
     */
    tbl_idx->udpEndpointRemoteAddressType =
        udpEndpointRemoteAddressType_val;

    /*
     * udpEndpointRemoteAddress(5)/InetAddress/ASN_OCTET_STR/char(char)//L/a/w/e/R/d/h 
     */
    tbl_idx->udpEndpointRemoteAddress_len = sizeof(tbl_idx->udpEndpointRemoteAddress) / sizeof(tbl_idx->udpEndpointRemoteAddress[0]);   /* max length */
    /*
     * make sure there is enough space for udpEndpointRemoteAddress data
     */
    if ((NULL == tbl_idx->udpEndpointRemoteAddress) ||
        (tbl_idx->udpEndpointRemoteAddress_len <
         (udpEndpointRemoteAddress_val_ptr_len))) {
        snmp_log(LOG_ERR, "not enough space for value\n");
        return MFD_ERROR;
    }
    tbl_idx->udpEndpointRemoteAddress_len =
        udpEndpointRemoteAddress_val_ptr_len;
    memcpy(tbl_idx->udpEndpointRemoteAddress,
           udpEndpointRemoteAddress_val_ptr,
           udpEndpointRemoteAddress_val_ptr_len *
           sizeof(udpEndpointRemoteAddress_val_ptr[0]));

    /*
     * udpEndpointRemotePort(6)/InetPortNumber/ASN_UNSIGNED/u_long(u_long)//l/a/w/e/R/d/H 
     */
    tbl_idx->udpEndpointRemotePort = udpEndpointRemotePort_val;

    /*
     * udpEndpointInstance(7)/UNSIGNED32/ASN_UNSIGNED/u_long(u_long)//l/a/w/e/R/d/h 
     */
    tbl_idx->udpEndpointInstance = udpEndpointInstance_val;


    return MFD_SUCCESS;
}                               /* udpEndpointTable_indexes_set_tbl_idx */

/**
 * @internal
 * set row context indexes
 *
 * @param reqreq_ctx the row context that needs updated indexes
 *
 * @retval MFD_SUCCESS     : success.
 * @retval MFD_ERROR       : other error.
 *
 * @remark
 *  This function sets the mib indexs, then updates the oid indexs
 *  from the mib index.
 */
int
udpEndpointTable_indexes_set(udpEndpointTable_rowreq_ctx * rowreq_ctx,
                             u_long udpEndpointLocalAddressType_val,
                             char *udpEndpointLocalAddress_val_ptr,
                             size_t udpEndpointLocalAddress_val_ptr_len,
                             u_long udpEndpointLocalPort_val,
                             u_long udpEndpointRemoteAddressType_val,
                             char *udpEndpointRemoteAddress_val_ptr,
                             size_t udpEndpointRemoteAddress_val_ptr_len,
                             u_long udpEndpointRemotePort_val,
                             u_long udpEndpointInstance_val)
{
    DEBUGMSGTL(("verbose:udpEndpointTable:udpEndpointTable_indexes_set",
                "called\n"));

    if (MFD_SUCCESS !=
        udpEndpointTable_indexes_set_tbl_idx(&rowreq_ctx->tbl_idx,
                                             udpEndpointLocalAddressType_val,
                                             udpEndpointLocalAddress_val_ptr,
                                             udpEndpointLocalAddress_val_ptr_len,
                                             udpEndpointLocalPort_val,
                                             udpEndpointRemoteAddressType_val,
                                             udpEndpointRemoteAddress_val_ptr,
                                             udpEndpointRemoteAddress_val_ptr_len,
                                             udpEndpointRemotePort_val,
                                             udpEndpointInstance_val))
        return MFD_ERROR;

    /*
     * convert mib index to oid index
     */
    rowreq_ctx->oid_idx.len = sizeof(rowreq_ctx->oid_tmp) / sizeof(oid);
    if (0 != udpEndpointTable_index_to_oid(&rowreq_ctx->oid_idx,
                                           &rowreq_ctx->tbl_idx)) {
        return MFD_ERROR;
    }

    return MFD_SUCCESS;
}                               /* udpEndpointTable_indexes_set */


/*---------------------------------------------------------------------
 * UDP-MIB::udpEndpointEntry.udpEndpointProcess
 * udpEndpointProcess is subid 8 of udpEndpointEntry.
 * Its status is Current, and its access level is ReadOnly.
 * OID: .1.3.6.1.2.1.7.7.1.8
 * Description:
The system's process ID for the process associated with
            this endpoint, or zero if there is no such process.
            This value is expected to be the same as
            HOST-RESOURCES-MIB::hrSWRunIndex or SYSAPPL-MIB::
            sysApplElmtRunIndex for some row in the appropriate
            tables.
 *
 * Attributes:
 *   accessible 1     isscalar 0     enums  0      hasdefval 0
 *   readable   1     iscolumn 1     ranges 0      hashint   0
 *   settable   0
 *
 *
 * Its syntax is UNSIGNED32 (based on perltype UNSIGNED32)
 * The net-snmp type is ASN_UNSIGNED. The C type decl is u_long (u_long)
 */
/**
 * Extract the current value of the udpEndpointProcess data.
 *
 * Set a value using the data context for the row.
 *
 * @param rowreq_ctx
 *        Pointer to the row request context.
 * @param udpEndpointProcess_val_ptr
 *        Pointer to storage for a u_long variable
 *
 * @retval MFD_SUCCESS         : success
 * @retval MFD_SKIP            : skip this node (no value for now)
 * @retval MFD_ERROR           : Any other error
 */
int
udpEndpointProcess_get(udpEndpointTable_rowreq_ctx * rowreq_ctx,
                       u_long * udpEndpointProcess_val_ptr)
{
   /** we should have a non-NULL pointer */
    netsnmp_assert(NULL != udpEndpointProcess_val_ptr);


    DEBUGMSGTL(("verbose:udpEndpointTable:udpEndpointProcess_get",
                "called\n"));

    netsnmp_assert(NULL != rowreq_ctx);

    /*
     * TODO:231:o: |-> Extract the current value of the udpEndpointProcess data.
     * copy (* udpEndpointProcess_val_ptr ) from rowreq_ctx->data
     */
    (*udpEndpointProcess_val_ptr) = rowreq_ctx->data.udpEndpointProcess;

    return MFD_SUCCESS;
}                               /* udpEndpointProcess_get */



/** @} */
/** @} */
/** @{ */
