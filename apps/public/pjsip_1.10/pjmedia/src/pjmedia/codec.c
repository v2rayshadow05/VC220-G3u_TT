/* $Id: codec.c 3553 2011-05-05 06:14:19Z nanang $ */
/* 
 * Copyright (C) 2008-2011 Teluu Inc. (http://www.teluu.com)
 * Copyright (C) 2003-2008 Benny Prijono <benny@prijono.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */
#include <pjmedia/codec.h>
#include <pjmedia/errno.h>
#include <pj/array.h>
#include <pj/assert.h>
#include <pj/log.h>
#include <pj/string.h>
#include <pj/os.h>
#include "cmsip_assert.h"

#define THIS_FILE   "codec.c"



/* Definition of default codecs parameters */
struct pjmedia_codec_default_param
{
    pj_pool_t		*pool;
    pjmedia_codec_param	*param;
};


/* Sort codecs in codec manager based on priorities */
static void sort_codecs(pjmedia_codec_mgr *mgr);


/*
 * Initialize codec manager.
 */
 /*ycw-pjsip. we have tow codec manager*/
#if 0
pj_status_t pjmedia_codec_mgr_init (pjmedia_codec_mgr *mgr,
					    pj_pool_factory *pf)
#else
pj_status_t pjmedia_codec_mgr_init (pjmedia_codec_mgr *mgr,
					    pj_pool_factory *pf, char* objName)
#endif
{
    pj_status_t status;

    PJ_ASSERT_RETURN(mgr && pf, PJ_EINVAL);

    /* Init codec manager */
    pj_bzero(mgr, sizeof(pjmedia_codec_mgr));
    mgr->pf = pf;
    pj_list_init (&mgr->factory_list);
    mgr->codec_cnt = 0;

	 #if 0
    /* Create pool */
    mgr->pool = pj_pool_create(mgr->pf, "codec-mgr", 256, 256, NULL);

    /* Create mutex */
    status = pj_mutex_create_recursive(mgr->pool, "codec-mgr", &mgr->mutex);
    if (status != PJ_SUCCESS)
	return status;
	 #else

	 if (NULL == objName) objName = "codec-mgr";

	 /* Create pool */
    mgr->pool = pj_pool_create(mgr->pf, objName, 256, 256, NULL);

    /* Create mutex */
    status = pj_mutex_create_recursive(mgr->pool, objName, &mgr->mutex);
	 
    if (status != PJ_SUCCESS) 	return status;
	 #endif

    return PJ_SUCCESS;
}

/*
 * Initialize codec manager.
 */
pj_status_t pjmedia_codec_mgr_destroy (pjmedia_codec_mgr *mgr)
{
    unsigned i;

    PJ_ASSERT_RETURN(mgr, PJ_EINVAL);

    /* Cleanup all pools of all codec default params */
    for (i=0; i<mgr->codec_cnt; ++i) {
	if (mgr->codec_desc[i].param) {
	    pj_assert(mgr->codec_desc[i].param->pool);
	    pj_pool_release(mgr->codec_desc[i].param->pool);
	}
    }

    /* Destroy mutex */
    if (mgr->mutex)
	pj_mutex_destroy(mgr->mutex);

    /* Release pool */
    if (mgr->pool)
	pj_pool_release(mgr->pool);

    /* Just for safety, set codec manager states to zero */
    pj_bzero(mgr, sizeof(pjmedia_codec_mgr));

    return PJ_SUCCESS;
}


/*
 * Register a codec factory.
 */
pj_status_t pjmedia_codec_mgr_register_factory( pjmedia_codec_mgr *mgr,
				    pjmedia_codec_factory *factory)
{
    pjmedia_codec_info info[PJMEDIA_CODEC_MGR_MAX_CODECS];
    unsigned i, count;
    pj_status_t status;

    PJ_ASSERT_RETURN(mgr && factory, PJ_EINVAL);

    /* Enum codecs */
    count = PJ_ARRAY_SIZE(info);
    status = factory->op->enum_info(factory, &count, info);
    if (status != PJ_SUCCESS)
	return status;

    pj_mutex_lock(mgr->mutex);

    /* Check codec count */
    if (count + mgr->codec_cnt > PJ_ARRAY_SIZE(mgr->codec_desc)) {
	pj_mutex_unlock(mgr->mutex);
	return PJ_ETOOMANY;
    }


    /* Save the codecs */
    for (i=0; i<count; ++i) {
	pj_memcpy( &mgr->codec_desc[mgr->codec_cnt+i],
		   &info[i], sizeof(pjmedia_codec_info));
	mgr->codec_desc[mgr->codec_cnt+i].prio = PJMEDIA_CODEC_PRIO_DISABLED;
	mgr->codec_desc[mgr->codec_cnt+i].factory = factory;
	pjmedia_codec_info_to_id( &info[i],
				  mgr->codec_desc[mgr->codec_cnt+i].id,
				  sizeof(pjmedia_codec_id));
    }

    /* Update count */
    mgr->codec_cnt += count;

    /* Re-sort codec based on priorities */
    sort_codecs(mgr);

    /* Add factory to the list */
    pj_list_push_back(&mgr->factory_list, factory);

    pj_mutex_unlock(mgr->mutex);

    return PJ_SUCCESS;
}


/*
 * Unregister a codec factory.
 */
pj_status_t pjmedia_codec_mgr_unregister_factory(
				pjmedia_codec_mgr *mgr, 
				pjmedia_codec_factory *factory)
{
    unsigned i;
    PJ_ASSERT_RETURN(mgr && factory, PJ_EINVAL);

    pj_mutex_lock(mgr->mutex);

    /* Factory must be registered. */
    if (pj_list_find_node(&mgr->factory_list, factory) != factory) {
	pj_mutex_unlock(mgr->mutex);
	return PJ_ENOTFOUND;
    }

    /* Erase factory from the factory list */
    pj_list_erase(factory);


    /* Remove all supported codecs from the codec manager that were created 
     * by the specified factory.
     */
    for (i=0; i<mgr->codec_cnt; ) {

	if (mgr->codec_desc[i].factory == factory) {
	    /* Release pool of codec default param */
	    if (mgr->codec_desc[i].param) {
		pj_assert(mgr->codec_desc[i].param->pool);
		pj_pool_release(mgr->codec_desc[i].param->pool);
	    }

	    /* Remove the codec from array of codec descriptions */
	    pj_array_erase(mgr->codec_desc, sizeof(mgr->codec_desc[0]), 
			   mgr->codec_cnt, i);
	    --mgr->codec_cnt;

	} else {
	    ++i;
	}
    }

    pj_mutex_unlock(mgr->mutex);

    return PJ_SUCCESS;
}


/*
 * Enum all codecs.
 */
pj_status_t pjmedia_codec_mgr_enum_codecs(pjmedia_codec_mgr *mgr, 
			      unsigned *count, 
			      pjmedia_codec_info codecs[],
			      unsigned *prio)
{
    unsigned i;

    PJ_ASSERT_RETURN(mgr && count && codecs, PJ_EINVAL);

    pj_mutex_lock(mgr->mutex);

    if (*count > mgr->codec_cnt)
	*count = mgr->codec_cnt;

    
    for (i=0; i<*count; ++i) {
	pj_memcpy(&codecs[i], 
		  &mgr->codec_desc[i].info, 
		  sizeof(pjmedia_codec_info));
    }

    if (prio) {
	for (i=0; i < *count; ++i)
	    prio[i] = mgr->codec_desc[i].prio;
    }

    pj_mutex_unlock(mgr->mutex);

    return PJ_SUCCESS;
}


/*
 * Get codec info for static payload type.
 */
pj_status_t pjmedia_codec_mgr_get_codec_info( pjmedia_codec_mgr *mgr,
				  unsigned pt,
				  const pjmedia_codec_info **p_info)
{
    unsigned i;

    PJ_ASSERT_RETURN(mgr && p_info && pt>=0 && pt < 96, PJ_EINVAL);

    pj_mutex_lock(mgr->mutex);

    for (i=0; i<mgr->codec_cnt; ++i) {
	if (mgr->codec_desc[i].info.pt == pt) {
	    *p_info = &mgr->codec_desc[i].info;

	    pj_mutex_unlock(mgr->mutex);
	    return PJ_SUCCESS;
	}
    }

    pj_mutex_unlock(mgr->mutex);

    return PJMEDIA_CODEC_EUNSUP;
}


/*
 * Convert codec info struct into a unique codec identifier.
 * A codec identifier looks something like "L16/44100/2".
 */
char* pjmedia_codec_info_to_id( const pjmedia_codec_info *info,
				        char *id, unsigned max_len )
{
    int len;

    PJ_ASSERT_RETURN(info && id && max_len, NULL);

    len = pj_ansi_snprintf(id, max_len, "%.*s/%u/%u", 
			   (int)info->encoding_name.slen,
			   pj_strnull(info->encoding_name.ptr),
			   info->clock_rate,
			   info->channel_cnt);

    if (len < 1 || len >= (int)max_len) {
	id[0] = '\0';
	return NULL;
    }

    return id;
}


/*
 * Find codecs by the unique codec identifier. This function will find
 * all codecs that match the codec identifier prefix. For example, if
 * "L16" is specified, then it will find "L16/8000/1", "L16/16000/1",
 * and so on, up to the maximum count specified in the argument.
 */
pj_status_t pjmedia_codec_mgr_find_codecs_by_id( pjmedia_codec_mgr *mgr,
				     const pj_str_t *codec_id,
				     unsigned *count,
				     const pjmedia_codec_info *p_info[],
				     unsigned prio[])
{
    unsigned i, found = 0;

    PJ_ASSERT_RETURN(mgr && codec_id && count && *count, PJ_EINVAL);

    pj_mutex_lock(mgr->mutex);

    for (i=0; i<mgr->codec_cnt; ++i) {

	if (codec_id->slen == 0 ||
	    pj_strnicmp2(codec_id, mgr->codec_desc[i].id, 
			 codec_id->slen) == 0) 
	{

	    if (p_info)
		p_info[found] = &mgr->codec_desc[i].info;
	    if (prio)
		prio[found] = mgr->codec_desc[i].prio;

	    ++found;

	    if (found >= *count)
		break;
	}

    }

    pj_mutex_unlock(mgr->mutex);

    *count = found;

    return found ? PJ_SUCCESS : PJ_ENOTFOUND;
}


/* Swap two codecs positions in codec manager */
static void swap_codec(pjmedia_codec_mgr *mgr, unsigned i, unsigned j)
{
    struct pjmedia_codec_desc tmp;

    pj_memcpy(&tmp, &mgr->codec_desc[i], sizeof(struct pjmedia_codec_desc));

    pj_memcpy(&mgr->codec_desc[i], &mgr->codec_desc[j], 
	       sizeof(struct pjmedia_codec_desc));

    pj_memcpy(&mgr->codec_desc[j], &tmp, sizeof(struct pjmedia_codec_desc));
}


/* Sort codecs in codec manager based on priorities */
static void sort_codecs(pjmedia_codec_mgr *mgr)
{
    unsigned i;

   /* Re-sort */
    for (i=0; i<mgr->codec_cnt; ++i)
	 {
	unsigned j, max;

		for (max=i, j=i+1; j<mgr->codec_cnt; ++j)
		{
	    if (mgr->codec_desc[j].prio > mgr->codec_desc[max].prio)
		max = j;
	}

	if (max != i)
	    swap_codec(mgr, i, max);
    }

    /* Change PJMEDIA_CODEC_PRIO_HIGHEST codecs to NEXT_HIGHER */
    for (i=0; i<mgr->codec_cnt; ++i)
	 {
	if (mgr->codec_desc[i].prio == PJMEDIA_CODEC_PRIO_HIGHEST)
	    mgr->codec_desc[i].prio = PJMEDIA_CODEC_PRIO_NEXT_HIGHER;
	else
	    break;
    }

	 /*ycw-pjsip. For the codec which has static payload type and dynamic payload type*/
	 for (i = 0; i < mgr->codec_cnt; ++i)
	 {
	 	unsigned j, max;
		for (max=i, j=i+1; j<mgr->codec_cnt; ++j)
		{
			/*Now, the codecs which has the same id is one by one.*/
			if (!strcmp(mgr->codec_desc[max].id, mgr->codec_desc[j].id))
			{
				if (mgr->codec_desc[j].info.pt < mgr->codec_desc[max].info.pt)
				{
					max = j;
				}
			}
			else
			{
				break;
			}
		}

		if (max != i)
		{
	   	swap_codec(mgr, i, max);
		}
	 }
}


/**
 * Set codec priority. The codec priority determines the order of
 * the codec in the SDP created by the endpoint. If more than one codecs
 * are found with the same codec_id prefix, then the function sets the
 * priorities of all those codecs.
 */
pj_status_t pjmedia_codec_mgr_set_codec_priority(
				pjmedia_codec_mgr *mgr, 
				const pj_str_t *codec_id,
				pj_uint8_t prio)
{
    unsigned i, found = 0;

    PJ_ASSERT_RETURN(mgr && codec_id, PJ_EINVAL);

    pj_mutex_lock(mgr->mutex);

    /* Update the priorities of affected codecs */
    for (i=0; i<mgr->codec_cnt; ++i) 
    {
	if (codec_id->slen == 0 ||
	    pj_strnicmp2(codec_id, mgr->codec_desc[i].id, 
			 codec_id->slen) == 0) 
	{
	    mgr->codec_desc[i].prio = (pjmedia_codec_priority) prio;
	    ++found;
	}
    }

    if (!found) {
	pj_mutex_unlock(mgr->mutex);
	return PJ_ENOTFOUND;
    }

    /* Re-sort codecs */
    sort_codecs(mgr);

    pj_mutex_unlock(mgr->mutex);

    return PJ_SUCCESS;
}

/* 
 * brief	by yuchuwei@20120413
 */
#	if 0
/*
 * Allocate one codec.
 */
pj_status_t pjmedia_codec_mgr_alloc_codec(pjmedia_codec_mgr *mgr, 
						  const pjmedia_codec_info *info,
						  pjmedia_codec **p_codec)
{
    pjmedia_codec_factory *factory;
    pj_status_t status;

    PJ_ASSERT_RETURN(mgr && info && p_codec, PJ_EINVAL);

    *p_codec = NULL;

    pj_mutex_lock(mgr->mutex);

    factory = mgr->factory_list.next;
    while (factory != &mgr->factory_list) {

	if ( (*factory->op->test_alloc)(factory, info) == PJ_SUCCESS ) {

	    status = (*factory->op->alloc_codec)(factory, info, p_codec);
	    if (status == PJ_SUCCESS) {
		pj_mutex_unlock(mgr->mutex);
		return PJ_SUCCESS;
	    }

	}

	factory = factory->next;
    }

    pj_mutex_unlock(mgr->mutex);

    return PJMEDIA_CODEC_EUNSUP;
}
#	endif

/*
 * Get default codec parameter.
 */
pj_status_t pjmedia_codec_mgr_get_default_param( pjmedia_codec_mgr *mgr,
							const pjmedia_codec_info *info,
							pjmedia_codec_param *param )
{
    pjmedia_codec_factory *factory;
    pj_status_t status;
    pjmedia_codec_id codec_id;
    struct pjmedia_codec_desc *codec_desc = NULL;
    unsigned i;

    PJ_ASSERT_RETURN(mgr && info && param, PJ_EINVAL);

    if (!pjmedia_codec_info_to_id(info, (char*)&codec_id, sizeof(codec_id)))
	return PJ_EINVAL;

    pj_mutex_lock(mgr->mutex);

    /* First, lookup default param in codec desc */
    for (i=0; i < mgr->codec_cnt; ++i) {
	if (pj_ansi_stricmp(codec_id, mgr->codec_desc[i].id) == 0) {
	    codec_desc = &mgr->codec_desc[i];
	    break;
	}
    }

    /* If we found the codec and its default param is set, return it */
    if (codec_desc && codec_desc->param) {
	pj_assert(codec_desc->param->param);
	pj_memcpy(param, codec_desc->param->param, 
		  sizeof(pjmedia_codec_param));

	pj_mutex_unlock(mgr->mutex);
	return PJ_SUCCESS;
    }

    /* Otherwise query the default param from codec factory */
    factory = mgr->factory_list.next;
    while (factory != &mgr->factory_list) {
	if ( (*factory->op->test_alloc)(factory, info) == PJ_SUCCESS ) {

	    status = (*factory->op->default_attr)(factory, info, param);
	    if (status == PJ_SUCCESS) {

		/* Check for invalid max_bps. */
		if (param->info.max_bps < param->info.avg_bps)
		    param->info.max_bps = param->info.avg_bps;

		pj_mutex_unlock(mgr->mutex);
		return PJ_SUCCESS;
	    }

	}

	factory = factory->next;
    }

    pj_mutex_unlock(mgr->mutex);

    return PJMEDIA_CODEC_EUNSUP;
}

/* 
 * brief	by yuchuwei@20120413
 */
#	if 0

/*
 * Set default codec parameter.
 */
pj_status_t pjmedia_codec_mgr_set_default_param( 
					    pjmedia_codec_mgr *mgr,
					    const pjmedia_codec_info *info,
					    const pjmedia_codec_param *param )
{
    unsigned i;
    pjmedia_codec_id codec_id;
    pj_pool_t *pool, *old_pool = NULL;
    struct pjmedia_codec_desc *codec_desc = NULL;
    pjmedia_codec_default_param *p;

    PJ_ASSERT_RETURN(mgr && info, PJ_EINVAL);

    if (!pjmedia_codec_info_to_id(info, (char*)&codec_id, sizeof(codec_id)))
	return PJ_EINVAL;

    pj_mutex_lock(mgr->mutex);

    /* Lookup codec desc */
    for (i=0; i < mgr->codec_cnt; ++i) {
	if (pj_ansi_stricmp(codec_id, mgr->codec_desc[i].id) == 0) {
	    codec_desc = &mgr->codec_desc[i];
	    break;
	}
    }

    /* Codec not found */
    if (!codec_desc) {
	pj_mutex_unlock(mgr->mutex);
	return PJMEDIA_CODEC_EUNSUP;
    }

    /* If codec param is previously set, reset the codec param but release
     * the codec param pool later after the new param is set (ticket #1171).
     */
    if (codec_desc->param) {
	pj_assert(codec_desc->param->pool);
        old_pool = codec_desc->param->pool;
	codec_desc->param = NULL;
    }

    /* When param is set to NULL, i.e: setting default codec param to library
     * default setting, just return PJ_SUCCESS.
     */
    if (NULL == param) {
	pj_mutex_unlock(mgr->mutex);
        if (old_pool)
	    pj_pool_release(old_pool);
	return PJ_SUCCESS;
    }

    /* Instantiate and initialize codec param */
    pool = pj_pool_create(mgr->pf, (char*)codec_id, 256, 256, NULL);
    codec_desc->param = PJ_POOL_ZALLOC_T(pool, pjmedia_codec_default_param);
    p = codec_desc->param;
    p->pool = pool;
    p->param = PJ_POOL_ZALLOC_T(pool, pjmedia_codec_param);

    /* Update codec param */
    pj_memcpy(p->param, param, sizeof(pjmedia_codec_param));
    for (i = 0; i < param->setting.dec_fmtp.cnt; ++i) {
	pj_strdup(pool, &p->param->setting.dec_fmtp.param[i].name, 
		  &param->setting.dec_fmtp.param[i].name);
	pj_strdup(pool, &p->param->setting.dec_fmtp.param[i].val, 
		  &param->setting.dec_fmtp.param[i].val);
    }
    for (i = 0; i < param->setting.enc_fmtp.cnt; ++i) {
	pj_strdup(pool, &p->param->setting.enc_fmtp.param[i].name, 
		  &param->setting.enc_fmtp.param[i].name);
	pj_strdup(pool, &p->param->setting.enc_fmtp.param[i].val, 
		  &param->setting.enc_fmtp.param[i].val);
    }

    pj_mutex_unlock(mgr->mutex);

    if (old_pool)
	pj_pool_release(old_pool);

    return PJ_SUCCESS;
}

/*
 * Dealloc codec.
 */
pj_status_t pjmedia_codec_mgr_dealloc_codec(pjmedia_codec_mgr *mgr, 
						    pjmedia_codec *codec)
{
    PJ_ASSERT_RETURN(mgr && codec, PJ_EINVAL);

    return (*codec->factory->op->dealloc_codec)(codec->factory, codec);
}
#	endif


/* 
 * brief	 ycw-pjsip.
 */
pj_status_t pjmedia_codec_mgr_codec_is_exist( const pjmedia_codec_mgr* mgr, const pj_str_t* codec_id)
{
    unsigned i;
	 pj_bool_t found = PJ_FALSE;

    PJ_ASSERT_RETURN(mgr && codec_id && codec_id->slen!=0, PJ_EINVAL);

    pj_mutex_lock(mgr->mutex);

    for (i = 0; i < mgr->codec_cnt; ++i)
	 {
		if (pj_strnicmp2(codec_id, mgr->codec_desc[i].id, codec_id->slen) == 0) 
		{
	    	found = PJ_TRUE;
			break;
		}
    }
    pj_mutex_unlock(mgr->mutex);

    return found;
}

int pjmedia_codec_mgr_get_codec_count(const pjmedia_codec_mgr* mgr)
{
	PJ_ASSERT_RETURN(mgr, PJ_EINVAL);
	return mgr->codec_cnt;
}

/*ycw-pjsip*/
/*To compare two codec param, if equal,return true, or return false*/
pj_bool_t pjmedia_codec_param_equal(pjmedia_codec_param* param1, pjmedia_codec_param* param2)
{
	int i;
	PJ_ASSERT_RETURN(param1 && param2, PJ_EINVAL);

	if (memcmp(&param1->info, &param2->info, sizeof(param2->info)))
	{
		return PJ_FALSE;
	}

	if (param1->setting.vad != param2->setting.vad ||
			param1->setting.cng != param2->setting.cng ||
			param1->setting.penh != param2->setting.penh ||
			param1->setting.plc != param2->setting.plc ||
			param1->setting.reserved != param2->setting.reserved)
	{
		return PJ_FALSE;
	}

	if (param1->setting.enc_fmtp.cnt != param2->setting.enc_fmtp.cnt ||
			param1->setting.dec_fmtp.cnt != param2->setting.dec_fmtp.cnt)
	{
		return PJ_FALSE;
	}

	for (i = 0; i < param1->setting.enc_fmtp.cnt; ++i)
	{
		if (pj_stricmp(&param1->setting.enc_fmtp.param[i].name,
								&param2->setting.enc_fmtp.param[i].name) ||
				pj_stricmp(&param1->setting.enc_fmtp.param[i].val,
								&param2->setting.enc_fmtp.param[i].val))
		{
			return PJ_FALSE;
		}
	}

	for (i = 0; i < param1->setting.dec_fmtp.cnt; ++i)
	{
		if (pj_stricmp(&param1->setting.dec_fmtp.param[i].name,
								&param2->setting.dec_fmtp.param[i].name) ||
				pj_stricmp(&param1->setting.dec_fmtp.param[i].val,
								&param2->setting.dec_fmtp.param[i].val))
		{
			return PJ_FALSE;
		}
	}

	return PJ_TRUE;
}



