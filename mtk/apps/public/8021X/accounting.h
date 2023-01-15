/*
 * hostapd / RADIUS Accounting
 * Copyright (c) 2002-2005, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */
#ifdef ACCOUNTING_SUPPORT
#ifndef ACCOUNTING_H
#define ACCOUNTING_H
#include "rtdot1x.h"
#include <sys/time.h>

/*
struct os_time {
	long sec;
	long usec;
};
*/

#ifdef CONFIG_NO_ACCOUNTING
static inline void accounting_sta_get_id(struct apd_data *rtapd,
					 struct sta_info *sta)
{
}

static inline void accounting_sta_start(struct apd_data *rtapd,
					struct sta_info *sta)
{
}

static inline void accounting_sta_stop(struct apd_data *rtapd,
				       struct sta_info *sta)
{
}

static inline int accounting_init(struct apd_data *rtapd)
{
	return 0;
}

static inline void accounting_deinit(struct apd_data *rtapd)
{
}
#else /* CONFIG_NO_ACCOUNTING */
void accounting_sta_get_id(struct apd_data *rtapd, struct sta_info *sta);
void accounting_sta_start(struct apd_data *rtapd, struct sta_info *sta);
void accounting_sta_stop(struct apd_data *rtapd, struct sta_info *sta);
int accounting_init(struct apd_data *rtapd);
void accounting_deinit(struct apd_data *rtapd);
int os_get_time(struct os_time *t);
struct apd_radius_attr * apd_config_get_radius_attr(struct apd_radius_attr *attr, u8 type);
void accounting_sta_interim(struct apd_data *hapd,struct sta_info *sta);

#endif /* CONFIG_NO_ACCOUNTING */

#endif /* ACCOUNTING_H */
#endif /*ACCOUNTING_SUPPORT*/
