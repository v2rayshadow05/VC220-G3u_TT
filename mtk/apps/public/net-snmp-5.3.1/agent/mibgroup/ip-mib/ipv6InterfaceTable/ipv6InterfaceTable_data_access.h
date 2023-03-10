/*
 * Note: this file originally auto-generated by mib2c using
 *       version : 1.17 $ of : mfd-data-access.m2c,v $
 *
 * $Id: //BBN_Linux/Branch/Branch_for_Rel_EN7512_SDK_20150916/tclinux_phoenix/apps/public/net-snmp-5.3.1/agent/mibgroup/ip-mib/ipv6InterfaceTable/ipv6InterfaceTable_data_access.h#1 $
 */
#ifndef IPV6INTERFACETABLE_DATA_ACCESS_H
#define IPV6INTERFACETABLE_DATA_ACCESS_H

#ifdef __cplusplus
extern          "C" {
#endif


    /*
     *********************************************************************
     * function declarations
     */

    /*
     *********************************************************************
     * Table declarations
     */
/**********************************************************************
 **********************************************************************
 ***
 *** Table ipv6InterfaceTable
 ***
 **********************************************************************
 **********************************************************************/
    /*
     * IP-MIB::ipv6InterfaceTable is subid 30 of ip.
     * Its status is Current.
     * OID: .1.3.6.1.2.1.4.30, length: 8
     */


    int
     
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        ipv6InterfaceTable_init_data(ipv6InterfaceTable_registration *
                                     ipv6InterfaceTable_reg);


    void            ipv6InterfaceTable_container_init(netsnmp_container
                                                      **container_ptr_ptr);
    void            ipv6InterfaceTable_container_shutdown(netsnmp_container
                                                          *container_ptr);

    int             ipv6InterfaceTable_container_load(netsnmp_container
                                                      *container);
    void            ipv6InterfaceTable_container_free(netsnmp_container
                                                      *container);

    int
     
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        ipv6InterfaceTable_row_prep(ipv6InterfaceTable_rowreq_ctx *
                                    rowreq_ctx);



#ifdef __cplusplus
}
#endif
#endif                          /* IPV6INTERFACETABLE_DATA_ACCESS_H */
