/*
 * $Id: stub_tx.c 66 2008-04-20 13:19:42Z hirofuchi $
 *
 * Copyright (C) 2003-2008 Takahiro Hirofuchi
 *
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 */

#include "usbip_common.h"
#include "stub.h"


static void stub_free_priv_and_urb(struct stub_priv *priv)
{
	struct urb *urb = priv->urb;

	if (urb->setup_packet)
		kfree(urb->setup_packet);

	if (urb->transfer_buffer)
		kfree(urb->transfer_buffer);

	list_del(&priv->list);
	kmem_cache_free(stub_priv_cache, priv);

	usb_free_urb(urb);
}

/* be in spin_lock_irqsave(&sdev->priv_lock, flags) */
void stub_enqueue_ret_unlink(struct stub_device *sdev, __u32 seqnum, __u32 status)
{
	struct stub_unlink *unlink;

	unlink = kzalloc(sizeof(struct stub_unlink), GFP_ATOMIC);
	if (!unlink) {
		uerr("alloc stub_unlink\n");
		usbip_event_add(&sdev->ud, VDEV_EVENT_ERROR_MALLOC);
		return;
	}

	unlink->seqnum = seqnum;
	unlink->status = status;

	list_add_tail(&unlink->list, &sdev->unlink_tx);
}

/**
 * stub_complete - completion handler of a usbip urb
 * @urb: pointer to the urb completed
 * @regs:
 *
 * When a urb has completed, the USB core driver calls this function mostly in
 * the interrupt context. To return the result of a urb, the completed urb is
 * linked to the pending list of returning.
 *
 */
void stub_complete(struct urb *urb)
{
	struct stub_priv *priv = (struct stub_priv *) urb->context;
	struct stub_device *sdev = priv->sdev;
	unsigned long flags;

	dbg_stub_tx("complete! status %d\n", urb->status);


	switch (urb->status) {
		case 0:
			/* OK */
			break;
		case -ENOENT:
			uinfo("stopped by a call of usb_kill_urb()"
					"because of cleaning up a virtual connection\n");
			return;
		case -ECONNRESET:
			uinfo("unlinked by a call of usb_unlink_urb()\n");
			break;
		case -EPIPE:
			uinfo("endpoint %d is stalled\n", usb_pipeendpoint(urb->pipe));
			break;
		case -ESHUTDOWN:
			uinfo("device removed?\n");
			break;
		default:
			uinfo("urb completion with non-zero status %d\n", urb->status);
	}

	/* link a urb to the queue of tx. */
	spin_lock_irqsave(&sdev->priv_lock, flags);

	if (priv->unlinking) {
		stub_enqueue_ret_unlink(sdev, priv->seqnum, urb->status);
		stub_free_priv_and_urb(priv);
	} else
		list_move_tail(&priv->list, &sdev->priv_tx);


	spin_unlock_irqrestore(&sdev->priv_lock, flags);

	/* wake up tx_thread */
	wake_up(&sdev->tx_waitq);
}


/*-------------------------------------------------------------------------*/
/* fill PDU */

static inline void setup_base_pdu(struct usbip_header_basic *base,
		__u32 command, __u32 seqnum)
{
	base->command = command;
	base->seqnum  = seqnum;
	base->devid   = 0;
	base->ep      = 0;
	base->direction   = 0;
}

static void setup_ret_submit_pdu(struct usbip_header *rpdu, struct urb *urb)
{
	struct stub_priv *priv = (struct stub_priv *) urb->context;

	setup_base_pdu(&rpdu->base, USBIP_RET_SUBMIT, priv->seqnum);

	usbip_pack_pdu(rpdu, urb, USBIP_RET_SUBMIT, 1);
}

static void setup_ret_unlink_pdu(struct usbip_header *rpdu,
		struct stub_unlink *unlink)
{
	setup_base_pdu(&rpdu->base, USBIP_RET_UNLINK, unlink->seqnum);

	rpdu->u.ret_unlink.status = unlink->status;
}


/*-------------------------------------------------------------------------*/
/* send RET_SUBMIT */

static struct stub_priv *dequeue_from_priv_tx(struct stub_device *sdev)
{
	unsigned long flags;
	struct stub_priv *priv, *tmp;

	spin_lock_irqsave(&sdev->priv_lock, flags);

	list_for_each_entry_safe(priv, tmp, &sdev->priv_tx, list) {
		list_move_tail(&priv->list, &sdev->priv_free);
		spin_unlock_irqrestore(&sdev->priv_lock, flags);
		return priv;
	}

	spin_unlock_irqrestore(&sdev->priv_lock, flags);

	return NULL;
}

static int stub_send_ret_submit(struct stub_device *sdev)
{
	unsigned long flags;
	struct stub_priv *priv, *tmp;

	struct msghdr msg;
	struct kvec iov[3];
	size_t txsize;

	size_t total_size = 0;

	while ((priv = dequeue_from_priv_tx(sdev)) != NULL) {
		int ret;
		struct urb *urb = priv->urb;
		struct usbip_header pdu_header;
		void *iso_buffer = NULL;

		txsize = 0;
		memset(&pdu_header, 0, sizeof(pdu_header));
		memset(&msg, 0, sizeof(msg));
		memset(&iov, 0, sizeof(iov));

		dbg_stub_tx("setup txdata urb %p\n", urb);


		/* 1. setup usbip_header */
		setup_ret_submit_pdu(&pdu_header, urb);
		usbip_header_correct_endian(&pdu_header, 1);

		iov[0].iov_base = &pdu_header;
		iov[0].iov_len  = sizeof(pdu_header);
		txsize += sizeof(pdu_header);

		/* 2. setup transfer buffer */
		if (usb_pipein(urb->pipe) &&
		    usb_pipetype(urb->pipe) != PIPE_ISOCHRONOUS &&
		    urb->actual_length > 0) {
			iov[1].iov_base = urb->transfer_buffer;
			iov[1].iov_len  = urb->actual_length;
			txsize += urb->actual_length;
		} else if (usb_pipein(urb->pipe) &&
			   usb_pipetype(urb->pipe) == PIPE_ISOCHRONOUS) {
			/*
			 * For isochronous packets: actual length is the sum of
			 * the actual length of the individual, packets, but as
			 * the packet offsets are not changed there will be
			 * padding between the packets. To optimally use the
			 * bandwidth the padding is not transmitted.
			 */

			int i;
			for (i = 0; i < urb->number_of_packets; i++) {
				iov[1].iov_base = urb->transfer_buffer + urb->iso_frame_desc[i].offset;
				iov[1].iov_len = urb->iso_frame_desc[i].actual_length;
				txsize += urb->iso_frame_desc[i].actual_length;
			}

			if (txsize != sizeof(pdu_header) + urb->actual_length) {
				uerr("actual length of urb %d does not match iso packet sizes %lu\n",
					urb->actual_length, txsize-sizeof(pdu_header));
				kfree(iov);
				usbip_event_add(&sdev->ud,
						SDEV_EVENT_ERROR_TCP);
			   return -1;
			}
		}

		/* 3. setup iso_packet_descriptor */
		if (usb_pipetype(urb->pipe) == PIPE_ISOCHRONOUS) {
			ssize_t len = 0;

			iso_buffer = usbip_alloc_iso_desc_pdu(urb, &len);
			if (!iso_buffer) {
				usbip_event_add(&sdev->ud, SDEV_EVENT_ERROR_MALLOC);
				return -1;
			}

			iov[2].iov_base = iso_buffer;
			iov[2].iov_len  = len;
			txsize += len;
		}

		ret = kernel_sendmsg(sdev->ud.tcp_socket, &msg, iov, 3, txsize);
		if (ret != txsize) {
			uerr("sendmsg failed!, retval %d for %zd\n", ret, txsize);
			if (iso_buffer)
				kfree(iso_buffer);
			usbip_event_add(&sdev->ud, SDEV_EVENT_ERROR_TCP);
			return -1;
		}

		if (iso_buffer)
			kfree(iso_buffer);

		dbg_stub_tx("send txdata\n");

		total_size += txsize;

		/* added by tf, 110527, it is necessary to clear halt endpoint */
		if (urb->status == -EPIPE)
		{
			int ok = usb_clear_halt(urb->dev, urb->pipe);
			if (ok == 0)
				uinfo("tx: clr halt ep %d\n", usb_pipeendpoint(urb->pipe));
			else 
				uinfo("tx: clr ep %d failed\n", usb_pipeendpoint(urb->pipe));
		}
	}


	spin_lock_irqsave(&sdev->priv_lock, flags);

	list_for_each_entry_safe(priv, tmp, &sdev->priv_free, list) {
		stub_free_priv_and_urb(priv);
	}

	spin_unlock_irqrestore(&sdev->priv_lock, flags);

	return total_size;
}


/*-------------------------------------------------------------------------*/
/* send RET_UNLINK */

static struct stub_unlink *dequeue_from_unlink_tx(struct stub_device *sdev)
{
	unsigned long flags;
	struct stub_unlink *unlink, *tmp;

	spin_lock_irqsave(&sdev->priv_lock, flags);

	list_for_each_entry_safe(unlink, tmp, &sdev->unlink_tx, list) {
		list_move_tail(&unlink->list, &sdev->unlink_free);
		spin_unlock_irqrestore(&sdev->priv_lock, flags);
		return unlink;
	}

	spin_unlock_irqrestore(&sdev->priv_lock, flags);

	return NULL;
}


static int stub_send_ret_unlink(struct stub_device *sdev)
{
	unsigned long flags;
	struct stub_unlink *unlink, *tmp;

	struct msghdr msg;
	struct kvec iov[1];
	size_t txsize;

	size_t total_size = 0;

	while ((unlink = dequeue_from_unlink_tx(sdev)) != NULL) {
		int ret;
		struct usbip_header pdu_header;

		txsize = 0;
		memset(&pdu_header, 0, sizeof(pdu_header));
		memset(&msg, 0, sizeof(msg));
		memset(&iov, 0, sizeof(iov));

		dbg_stub_tx("setup ret unlink %lu\n", unlink->seqnum);

		/* 1. setup usbip_header */
		setup_ret_unlink_pdu(&pdu_header, unlink);
		usbip_header_correct_endian(&pdu_header, 1);

		iov[0].iov_base = &pdu_header;
		iov[0].iov_len  = sizeof(pdu_header);
		txsize += sizeof(pdu_header);

		ret = kernel_sendmsg(sdev->ud.tcp_socket, &msg, iov, 1, txsize);
		if (ret != txsize) {
			uerr("sendmsg failed!, retval %d for %zd\n", ret, txsize);
			usbip_event_add(&sdev->ud, SDEV_EVENT_ERROR_TCP);
			return -1;
		}


		dbg_stub_tx("send txdata\n");

		total_size += txsize;
	}


	spin_lock_irqsave(&sdev->priv_lock, flags);

	list_for_each_entry_safe(unlink, tmp, &sdev->unlink_free, list) {
		list_del(&unlink->list);
		kfree(unlink);
	}

	spin_unlock_irqrestore(&sdev->priv_lock, flags);

	return total_size;
}


/*-------------------------------------------------------------------------*/

void stub_tx_loop(struct usbip_task *ut)
{
	struct usbip_device *ud = container_of(ut, struct usbip_device, tcp_tx);
	struct stub_device *sdev = container_of(ud, struct stub_device, ud);
	
	while (1) {
		if (signal_pending(current)) {
			dbg_stub_tx("signal catched\n");
			break;
		}

		if (usbip_event_happend(ud))
			break;

		/*
		 * send_ret_submit comes earlier than send_ret_unlink.  stub_rx
		 * looks at only priv_init queue. If the completion of a URB is
		 * earlier than the receive of CMD_UNLINK, priv is moved to
		 * priv_tx queue and stub_rx does not find the target priv. In
		 * this case, vhci_rx receives the result of the submit request
		 * and then receives the result of the unlink request. The
		 * result of the submit is given back to the usbcore as the
		 * completion of the unlink request. The request of the
		 * unlink is ignored. This is ok because a driver who calls
		 * usb_unlink_urb() understands the unlink was too late by
		 * getting the status of the given-backed URB which has the
		 * status of usb_submit_urb().
		 */
		if (stub_send_ret_submit(sdev) < 0)
			break;

		if (stub_send_ret_unlink(sdev) < 0)
			break;

		wait_event_interruptible(sdev->tx_waitq,
				(!list_empty(&sdev->priv_tx) ||
				 !list_empty(&sdev->unlink_tx)));
	}
}

#if CLIENT > 2
// added by tf, 110530
static int heartbeat_timeout(unsigned long data)
{
	struct stub_device * sdev = (struct stub_device *)data;
	char tmp[]= "server: How are you?";
	int ret;

	udbg("heartbeat_timeout Enter...\n");

	if (NULL == sdev || NULL == sdev->ud.tcp_socket)
	{
       	 return -1;
	}  
    
	if (sdev->heartbeat_timer_running == 0)
	{

		uinfo("heartbeat not running (heartbeat_timer_running == 0)?!\n");
		sdev->heartbeat_timer_running = 1;
		return -2;
	}
	
	if (sdev->heartbeat_pending > HEARTBEAT_FAILED_TIMES)
	{
	/*  ?????????????? */
		sdev->heartbeat_timer_running = 0;
		sdev->heartbeat_pending = 0;
		uinfo("HeartBeat timeout!\n");
		usbip_event_add(&sdev->ud, SDEV_EVENT_ERROR_TCP);	/* remove it temporary, 110711 */
		return -3;
	}
	else
	{
		struct msghdr msg;
		struct kvec iov[1];
		struct usbip_header pdu_header;
		size_t txsize = sizeof(pdu_header);

		memset(&pdu_header, 0, sizeof(pdu_header));
		memset(&msg, 0, sizeof(msg));
		memset(&iov, 0, sizeof(iov));
	    
		if(sdev->heartbeat_seqNum == 0xffff)
		{
			sdev->heartbeat_seqNum = 1;
		}
		
		 /* setup usbip_header */
		pdu_header.base.command = USBIP_HEARTBEAT_ECHO;
		pdu_header.base.seqnum = sdev->heartbeat_seqNum++;
		memcpy(&pdu_header.u.cmd_submit, tmp, sizeof(tmp));
		usbip_header_correct_endian(&pdu_header, 1);

		iov[0].iov_base = &pdu_header;
		iov[0].iov_len  = sizeof(pdu_header);

		udbg("send HEARTBEAT  :%d-----pending:%d\n", 
		        sdev->heartbeat_seqNum - 1, sdev->heartbeat_pending + 1);

		ret =kernel_sendmsg(sdev->ud.tcp_socket, &msg, iov, 1, txsize);
		if (ret != txsize) {
			uerr("sendmsg failed!, retval %d for %d\n", ret, txsize);
			/* usbip_event_add(&sdev->ud, SDEV_EVENT_ERROR_TCP); */
			return 0;
		}
	
		 /*
		ret = kernel_sendmsg(sdev->ud.tcp_socket, &msg, iov, 1, txsize);
		if (ret != txsize) {
			uerr("sendmsg failed!, retval %d for %d\n", ret, txsize);
			usbip_event_add(&sdev->ud, SDEV_EVENT_ERROR_TCP);
			return -1;
		}
		*/
		
		sdev->heartbeat_pending++;
		sdev->heartbeat_timer_running = 1;  
		return 0;
	}
	return 0;
 }
// end --- added

/*
 * send heartbeat packets periodly.
 */
void heartbeat_tx_loop(struct usbip_task *ut)
{
	struct usbip_device *ud = container_of(ut, struct usbip_device, heartbeat_tx);
	struct stub_device *sdev = container_of(ud, struct stub_device, ud);
	int ret;

	uinfo("HeartBeat Thread --- %s start!\n", __func__);
	
	while (1) {
		set_current_state(TASK_INTERRUPTIBLE);
		
		/* if SIGKILL signal catched, exit */
		if (signal_pending(current)) {
			uinfo("SIGKILL signal catched\n");
			break;
		}

#if 0
		uinfo("before sleep!\n");
		/* sleep 5s at the first time or sleep 2s later. */
		if (sdev->heartbeat_seqNum == 1) {
			uinfo("Sleep 5s .....\n");
			schedule_timeout_interruptible( HEARTBEAT_FIRST_INTER * HZ);
		} else {
#endif
		udbg("Sleep 2s ..\n");
		schedule_timeout_interruptible( HEARTBEAT_INTERVAL * HZ);
#if 0
		}
#endif

		set_current_state(TASK_RUNNING);
		
		/* send heartbeat pkt */
		ret = heartbeat_timeout(sdev);
		if (ret != 0)
		{
			uinfo("%s, L%d, heartbeat_timeout = %d\n", __func__, __LINE__, ret);
			break;
		}

		ret = usbip_event_happend(ud);
		if (ret)
		{
			uinfo("%s, ud->event == %.4x\n",  __func__, ud->event );
			break; 
		}
	}

	set_current_state(TASK_RUNNING);
	uinfo("HeartBeat Thread --- %s exit!\n", __func__);
}
#endif

