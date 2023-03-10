#include <pjmedia-codec/g721.h>
#include <pjmedia/codec.h>
#include <pjmedia/endpoint.h>
#include <pjmedia/errno.h>
/* 
 * brief	by yuchuwei@20120413
 */
#	if 0
#include <pjmedia/plc.h>
#include <pjmedia/silencedet.h>
#	endif
#include <pj/pool.h>
#include <pj/string.h>
#include <pj/assert.h>
#include <pj/os.h>
#include "cmsip_assert.h"

#if defined(PJMEDIA_HAS_G721_CODEC) && PJMEDIA_HAS_G721_CODEC!=0


#define G721_BPS 32000
#define G721_CODEC_CNT	    0	/* number of codec to preallocate in memory */
#define PTIME		    10	/* basic frame size is 10 msec	    */
#define FRAME_SIZE	    (8000 * PTIME / 1000)   /* 80 bytes	    */
#define SAMPLES_PER_FRAME   (8000 * PTIME / 1000)   /* 80 samples   */

/* Prototypes for G721 factory */
static pj_status_t g721_test_alloc( pjmedia_codec_factory *factory, 
				    const pjmedia_codec_info *id );
static pj_status_t g721_default_attr( pjmedia_codec_factory *factory, 
				      const pjmedia_codec_info *id, 
				      pjmedia_codec_param *attr );
static pj_status_t g721_enum_codecs (pjmedia_codec_factory *factory, 
				     unsigned *count, 
				     pjmedia_codec_info codecs[]);
/* 
 * brief	yuchuwei@20120413
 */
#	if 0
static pj_status_t g721_alloc_codec( pjmedia_codec_factory *factory, 
				     const pjmedia_codec_info *id, 
				     pjmedia_codec **p_codec);
static pj_status_t g721_dealloc_codec( pjmedia_codec_factory *factory, 
				       pjmedia_codec *codec );

/* Prototypes for G721 implementation. */
static pj_status_t  g721_init( pjmedia_codec *codec, 
			       pj_pool_t *pool );
static pj_status_t  g721_open( pjmedia_codec *codec, 
			       pjmedia_codec_param *attr );
static pj_status_t  g721_close( pjmedia_codec *codec );
static pj_status_t  g721_modify(pjmedia_codec *codec, 
			        const pjmedia_codec_param *attr );
static pj_status_t  g721_parse(pjmedia_codec *codec,
			       void *pkt,
			       pj_size_t pkt_size,
			       const pj_timestamp *timestamp,
			       unsigned *frame_cnt,
			       pjmedia_frame frames[]);
static pj_status_t  g721_encode( pjmedia_codec *codec, 
				 const struct pjmedia_frame *input,
				 unsigned output_buf_len, 
				 struct pjmedia_frame *output);
static pj_status_t  g721_decode( pjmedia_codec *codec, 
				 const struct pjmedia_frame *input,
				 unsigned output_buf_len, 
				 struct pjmedia_frame *output);
#if !PLC_DISABLED
static pj_status_t  g721_recover( pjmedia_codec *codec,
				  unsigned output_buf_len,
				  struct pjmedia_frame *output);
#endif

/* Definition for G711 codec operations. */
static pjmedia_codec_op g721_op = 
{
    &g721_init,
    &g721_open,
    &g721_close,
    &g721_modify,
    &g721_parse,
    &g721_encode,
    &g721_decode,
    NULL
};
#	endif

/* Definition for G711 codec factory operations. */
static pjmedia_codec_factory_op g721_factory_op =
{
    &g721_test_alloc,
    &g721_default_attr,
    &g721_enum_codecs,
/* 
 * brief	yuchuwei@20120413
 */
#	if 0
    &g721_alloc_codec,
    &g721_dealloc_codec
#	endif
};

/* G711 factory private data */
static struct g721_factory
{
    pjmedia_codec_factory	base;
    pjmedia_endpt	       *endpt;
    pj_pool_t		       *pool;
    pj_mutex_t		       *mutex;
    pjmedia_codec		codec_list;
} g721_factory;

/* 
 * brief	by yuchuwei@20120413
 */
#	if 0

/* G711 codec private data. */
struct g721_private
{
    unsigned		 pt;
#if !PLC_DISABLED
    pj_bool_t		 plc_enabled;
    pjmedia_plc		*plc;
#endif
    pj_bool_t		 vad_enabled;
    pjmedia_silence_det *vad;
    pj_timestamp	 last_tx;
};
#	endif

pj_status_t pjmedia_codec_g721_init(pjmedia_endpt *endpt)
{
	/*ycw-pjsip-codec*/
	#if 0
    pjmedia_codec_mgr *codec_mgr;
	#else
	pjmedia_codec_mgr* full_codec_mgr;
	#endif
    pj_status_t status;

    if (g721_factory.endpt != NULL)
	 {
		/* Already initialized. */
		return PJ_SUCCESS;
    }

    /* Init factory */
    g721_factory.base.op = &g721_factory_op;
    g721_factory.base.factory_data = NULL;
    g721_factory.endpt = endpt;

    pj_list_init(&g721_factory.codec_list);

    /* Create pool */
    g721_factory.pool = pjmedia_endpt_create_pool(endpt, "g721", 4000, 4000);
    if (!g721_factory.pool)
    {
		return PJ_ENOMEM;
    }

    /* Create mutex. */
    status = pj_mutex_create_simple(g721_factory.pool, "g621", &g721_factory.mutex);
    if (status != PJ_SUCCESS)
    {
		goto on_error;
    }

	/*ycw-pjsip-codec*/
	#if 0
    /* Get the codec manager. */
    codec_mgr = pjmedia_endpt_get_codec_mgr(endpt);
    if (!codec_mgr)
	 {
		return PJ_EINVALIDOP;
    }

    /* Register codec factory to endpoint. */
    status = pjmedia_codec_mgr_register_factory(codec_mgr, &g726_factory.base);
    if (status != PJ_SUCCESS)
    {
		return status;
    }
	 #else
	     /* Get the codec manager. */
    full_codec_mgr = pjmedia_endpt_get_full_codec_mgr(endpt);
    if (!full_codec_mgr)
	 {
		return PJ_EINVALIDOP;
    }

    /* Register codec factory to endpoint. */
    status = pjmedia_codec_mgr_register_factory(full_codec_mgr, &g721_factory.base);
    if (status != PJ_SUCCESS)
    {
		return status;
    }
	 #endif


    return PJ_SUCCESS;

on_error:
    if (g721_factory.mutex)
	 {
		pj_mutex_destroy(g721_factory.mutex);
		g721_factory.mutex = NULL;
    }
	 
    if (g721_factory.pool)
	 {
		pj_pool_release(g721_factory.pool);
		g721_factory.pool = NULL;
    }
    return status;
}

pj_status_t pjmedia_codec_g721_deinit(void)
{
	/*ycw-pjsip*/
	#if 0
    pjmedia_codec_mgr *codec_mgr;
	#else
	pjmedia_codec_mgr* full_codec_mgr;
	#endif
    pj_status_t status;

    if (g721_factory.endpt == NULL)
	 {
		/* Not registered. */
		return PJ_SUCCESS;
    }

    /* Lock mutex. */
    pj_mutex_lock(g721_factory.mutex);

	/*ycw-pjsip-codec*/
	#if 0
    /* Get the codec manager. */
    codec_mgr = pjmedia_endpt_get_codec_mgr(g726_factory.endpt);
    if (!codec_mgr)
	 {
		g726_factory.endpt = NULL;
		pj_mutex_unlock(g726_factory.mutex);
		return PJ_EINVALIDOP;
    }

    /* Unregister G711 codec factory. */
    status = pjmedia_codec_mgr_unregister_factory(codec_mgr,
						  &g726_factory.base);
	 #else
	 /* Get the codec manager. */
    full_codec_mgr = pjmedia_endpt_get_full_codec_mgr(g721_factory.endpt);
    if (!full_codec_mgr)
	 {
		g721_factory.endpt = NULL;
		pj_mutex_unlock(g721_factory.mutex);
		return PJ_EINVALIDOP;
    }

    /* Unregister G711 codec factory. */
    status = pjmedia_codec_mgr_unregister_factory(full_codec_mgr, &g721_factory.base);
	 #endif
    g721_factory.endpt = NULL;

    /* Destroy mutex. */
    pj_mutex_destroy(g721_factory.mutex);
    g721_factory.mutex = NULL;


    /* Release pool. */
    pj_pool_release(g721_factory.pool);
    g721_factory.pool = NULL;


    return status;
}

static pj_status_t g721_test_alloc(pjmedia_codec_factory *factory, 
				   const pjmedia_codec_info *id )
{
    PJ_UNUSED_ARG(factory);

	 if (PJMEDIA_RTP_PT_G721 == id->pt)
	 {
	 	return PJ_SUCCESS;
	 }

	 return PJMEDIA_CODEC_EUNSUP;
}

static pj_status_t g721_default_attr (pjmedia_codec_factory *factory, 
				      const pjmedia_codec_info *id, 
				      pjmedia_codec_param *attr )
{
    PJ_UNUSED_ARG(factory);

    pj_bzero(attr, sizeof(pjmedia_codec_param));
    attr->info.clock_rate = 8000;
    attr->info.channel_cnt = 1;
    attr->info.avg_bps = G721_BPS;
    attr->info.max_bps = G721_BPS;
    attr->info.pcm_bits_per_sample = 16;
    attr->info.frm_ptime = PTIME;
    attr->info.pt = (pj_uint8_t)id->pt;

	/*ycw-pjsip-ptime*/
	#if 0
    /* Set default frames per packet to 2 (or 20ms) */
    attr->setting.frm_per_pkt = 2;
	#endif

#if !PLC_DISABLED
    /* Enable plc by default. */
    attr->setting.plc = 1;
#endif

    /* Enable VAD by default. */
    attr->setting.vad = 1;

    /* Default all other flag bits disabled. */

    return PJ_SUCCESS;
}

static pj_status_t g721_enum_codecs(pjmedia_codec_factory *factory, 
				    unsigned *max_count, 
				    pjmedia_codec_info codecs[])
{
    unsigned count = 0;

    PJ_UNUSED_ARG(factory);

    if (count < *max_count) {
	codecs[count].type = PJMEDIA_TYPE_AUDIO;
	codecs[count].pt = PJMEDIA_RTP_PT_G721;
	codecs[count].encoding_name = pj_str("G721");
	codecs[count].clock_rate = 8000;
	codecs[count].channel_cnt = 1;
	++count;
    }
	 
    *max_count = count;

    return PJ_SUCCESS;
}

/* 
 * brief	yuchuwei@20120413
 */
#	if 0

static pj_status_t g721_alloc_codec( pjmedia_codec_factory *factory, 
				     const pjmedia_codec_info *id,
				     pjmedia_codec **p_codec)
{
	PJ_UNUSED_ARG(factory);
	PJ_UNUSED_ARG(id);
	PJ_UNUSED_ARG(p_codec);
   return PJ_SUCCESS;
}

static pj_status_t g721_dealloc_codec(pjmedia_codec_factory *factory, 
				      pjmedia_codec *codec )
{
	PJ_UNUSED_ARG(factory);
	PJ_UNUSED_ARG(codec);
    return PJ_SUCCESS;
}

static pj_status_t g721_init( pjmedia_codec *codec, pj_pool_t *pool )
{
    /* There's nothing to do here really */
    PJ_UNUSED_ARG(codec);
    PJ_UNUSED_ARG(pool);

    return PJ_SUCCESS;
}

static pj_status_t g721_open(pjmedia_codec *codec, pjmedia_codec_param *attr )
{
	PJ_UNUSED_ARG(codec);
	PJ_UNUSED_ARG(attr);
   return PJ_SUCCESS;
}

static pj_status_t g721_close( pjmedia_codec *codec )
{
    PJ_UNUSED_ARG(codec);
    /* Nothing to do */
    return PJ_SUCCESS;
}

static pj_status_t  g721_modify(pjmedia_codec *codec, const pjmedia_codec_param *attr )
{
	PJ_UNUSED_ARG(codec);
	PJ_UNUSED_ARG(attr);
   return PJ_SUCCESS;
}

static pj_status_t  g721_parse( pjmedia_codec *codec,
				void *pkt,
				pj_size_t pkt_size,
				const pj_timestamp *ts,
				unsigned *frame_cnt,
				pjmedia_frame frames[])
{
	PJ_UNUSED_ARG(codec);
	PJ_UNUSED_ARG(pkt);
	PJ_UNUSED_ARG(pkt_size);
	PJ_UNUSED_ARG(ts);
	PJ_UNUSED_ARG(frame_cnt);
	PJ_UNUSED_ARG(frames);

   return PJ_SUCCESS;
}

static pj_status_t  g721_encode(pjmedia_codec *codec, 
				const struct pjmedia_frame *input,
				unsigned output_buf_len, 
				struct pjmedia_frame *output)
{

	PJ_UNUSED_ARG(codec);
	PJ_UNUSED_ARG(input);
	PJ_UNUSED_ARG(output_buf_len);
	PJ_UNUSED_ARG(output);

   return PJ_SUCCESS;
}

static pj_status_t  g721_decode(pjmedia_codec *codec, 
				const struct pjmedia_frame *input,
				unsigned output_buf_len, 
				struct pjmedia_frame *output)
{

	PJ_UNUSED_ARG(codec);
	PJ_UNUSED_ARG(input);
	PJ_UNUSED_ARG(output_buf_len);
	PJ_UNUSED_ARG(output);

    return PJ_SUCCESS;
}

#	endif /*end if 0*/

#endif	/* PJMEDIA_HAS_G721_CODEC */





