#include "mtk-phy.h"

#ifdef CONFIG_A60810_SUPPORT
#include "mtk-phy-a60810.h"

PHY_INT32 phy_init_a60810(struct u3phy_info *info){

    /* for U2 hs eye diagram */
	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs_a60810->usbphyacr1)
		,A60810_RG_USB20_TERM_VREF_SEL_OFST, A60810_RG_USB20_TERM_VREF_SEL, 0x05);

	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs_a60810->usbphyacr1)
		,A60810_RG_USB20_VRT_VREF_SEL_OFST, A60810_RG_USB20_VRT_VREF_SEL, 0x05);

    /* for U2 sensitivity */
	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs_a60810->usbphyacr6)
		,A60810_RG_USB20_SQTH_OFST, A60810_RG_USB20_SQTH, 0x04);

    /* disable ssusb_p3_entry to work aroud resume form P3 bug */
	U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_regs_a60810->phyd_lfps0)
		,A60810_RG_SSUSB_P3_ENTRY_OFST, A60810_RG_SSUSB_P3_ENTRY, 0x0);

    /* force disable ssusb_p3_entry to work aroud resume form P3 bug */
	U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_regs_a60810->phyd_lfps0)
		,A60810_RG_SSUSB_P3_ENTRY_SEL_OFST, A60810_RG_SSUSB_P3_ENTRY_SEL, 0x01);

	/* fine tune SSC delta1 to let SSC min average ~0ppm */
	U3PhyWriteField32(((PHY_UINT32)&info->u3phya_da_regs_a60810->reg19)
		,A60810_RG_SSUSB_PLL_SSC_DELTA1_U3_OFST, A60810_RG_SSUSB_PLL_SSC_DELTA1_U3
		, 0x46);
	/* fine tune SSC delta to let SSC min average ~0ppm */
	U3PhyWriteField32(((PHY_UINT32)&info->u3phya_da_regs_a60810->reg21)
		,A60810_RG_SSUSB_PLL_SSC_DELTA_U3_OFST, A60810_RG_SSUSB_PLL_SSC_DELTA_U3
		, 0x40);
	/* Fine tune SYSPLL to improve phase noise */
	U3PhyWriteField32(((PHY_UINT32)&info->u3phya_da_regs_a60810->reg4)
		,A60810_RG_SSUSB_PLL_BC_U3_OFST, A60810_RG_SSUSB_PLL_BC_U3, 0x03);
	U3PhyWriteField32(((PHY_UINT32)&info->u3phya_da_regs_a60810->reg4)
		, A60810_RG_SSUSB_PLL_DIVEN_U3_OFST, A60810_RG_SSUSB_PLL_DIVEN_U3, 0x03);
	U3PhyWriteField32(((PHY_UINT32)&info->u3phya_da_regs_a60810->reg5)
		, A60810_RG_SSUSB_PLL_IC_U3_OFST, A60810_RG_SSUSB_PLL_IC_U3, 0x01);
	U3PhyWriteField32(((PHY_UINT32)&info->u3phya_da_regs_a60810->reg5)
		, A60810_RG_SSUSB_PLL_BR_U3_OFST,A60810_RG_SSUSB_PLL_BR_U3, 0x01);
	U3PhyWriteField32(((PHY_UINT32)&info->u3phya_da_regs_a60810->reg6)
		, A60810_RG_SSUSB_PLL_IR_U3_OFST, A60810_RG_SSUSB_PLL_IR_U3, 0x01);
	U3PhyWriteField32(((PHY_UINT32)&info->u3phya_da_regs_a60810->reg7)
		, A60810_RG_SSUSB_PLL_BP_U3_OFST, A60810_RG_SSUSB_PLL_BP_U3, 0x0f);
	
	/* force xtal pwd mode enable */
	U3PhyWriteField32(((PHY_UINT32)&info->spllc_regs_a60810->u3d_xtalctl_2)
		,A60810_RG_SSUSB_FORCE_XTAL_PWD_OFST, A60810_RG_SSUSB_FORCE_XTAL_PWD, 0x01);
	/* force xtal bias mode enable */
	U3PhyWriteField32(((PHY_UINT32)&info->spllc_regs_a60810->u3d_xtalctl_2)
		,A60810_RG_SSUSB_FORCE_BIAS_PWD_OFST, A60810_RG_SSUSB_FORCE_BIAS_PWD, 0x01);
	/* force xtal pwd mode enable */
	U3PhyWriteField32(((PHY_UINT32)&info->spllc_regs_a60810->u3d_xtalctl_2)
		,A60810_RG_SSUSB_XTAL_PWD_OFST, A60810_RG_SSUSB_XTAL_PWD, 0x00);
	/* force xtal pwd mode enable */
	U3PhyWriteField32(((PHY_UINT32)&info->spllc_regs_a60810->u3d_xtalctl_2)
		,A60810_RG_SSUSB_BIAS_PWD_OFST, A60810_RG_SSUSB_BIAS_PWD, 0x00);
	/* for better LPM BESL value	*/
	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs_a60810->u2phydcr1)
		, A60810_RG_USB20_SW_PLLMODE_OFST+1, A60810_RG_USB20_SW_PLLMODE, 0x1);

	return PHY_TRUE;
}

#define PHY_DRV_SHIFT	3
#define PHY_PHASE_SHIFT	3
#define PHY_PHASE_DRV_SHIFT	1
PHY_INT32 phy_change_pipe_phase_a60810(struct u3phy_info *info, PHY_INT32 phy_drv, PHY_INT32 pipe_phase){
	PHY_INT32 drv_reg_value;
	PHY_INT32 phase_reg_value;
	PHY_INT32 temp;

	drv_reg_value = phy_drv << PHY_DRV_SHIFT;
	phase_reg_value = (pipe_phase << PHY_PHASE_SHIFT) | (phy_drv << PHY_PHASE_DRV_SHIFT);
	temp = U3PhyReadReg8(((PHY_UINT32)&info->sifslv_chip_regs_a60810->gpio_ctla)+2);
	temp &= ~(0x3 << PHY_DRV_SHIFT);
	temp |= drv_reg_value;
	U3PhyWriteReg8(((PHY_UINT32)&info->sifslv_chip_regs_a60810->gpio_ctla)+2, temp);
	temp = U3PhyReadReg8(((PHY_UINT32)&info->sifslv_chip_regs_a60810->gpio_ctla)+3);
	temp &= ~((0x3 << PHY_PHASE_DRV_SHIFT) | (0x1f << PHY_PHASE_SHIFT));
	temp |= phase_reg_value;
	U3PhyWriteReg8(((PHY_UINT32)&info->sifslv_chip_regs_a60810->gpio_ctla)+3, temp);
	return PHY_TRUE;
}

/* --------------------------------------------------------
 *    Function : fgEyeScanHelper_CheckPtInRegion()
 *  Description : Check if the test point is in a rectangle region.
 *                If it is in the rectangle, also check if this point
 *                is on the multiple of deltaX and deltaY.
 *    Parameter : strucScanRegion * prEye - the region
 *                BYTE bX
 *                BYTE bY
 *       Return : BYTE - TRUE :  This point needs to be tested
 *                       FALSE:  This point will be omitted
 *         Note : First check within the rectangle.
 *                Secondly, use modulous to check if the point will be tested.
 * --------------------------------------------------------
 */
static PHY_INT8 fgEyeScanHelper_CheckPtInRegion(struct strucScanRegion * prEye, PHY_INT8 bX, PHY_INT8 bY)
{
  PHY_INT8 fgValid = true;

	/*
  	 *  Be careful, the axis origin is on the TOP-LEFT corner.
	 * Therefore the top-left point has the minimum X and Y
	 * Botton-right point is the maximum X and Y
   	 */
  if ( (prEye->bX_tl <= bX) && (bX <= prEye->bX_br)
    && (prEye->bY_tl <= bY) && (bY <= prEye->bX_br))
  {
  	/*
    	 * With the region, now check whether or not the input test point is
    	 * on the multiples of X and Y
    	 * Do not have to worry about negative value, because we have already
    	 * check the input bX, and bY is within the region.
	 */
    if ( ((bX - prEye->bX_tl) % (prEye->bDeltaX))
      || ((bY - prEye->bY_tl) % (prEye->bDeltaY)) )
    {
	/*
        *  if the division will have remainder, that means
        *  the input test point is on the multiples of X and Y
        */
      fgValid = false;
    }
    else
    {
    }
  }
  else
  {
    
    fgValid = false;
  }
  return fgValid;
}
/*
 * --------------------------------------------------------
 *     Function : EyeScanHelper_RunTest()
 *  Description : Enable the test, and wait til it is completed
 *    Parameter : None
 *       Return : None
 *         Note : None
 * --------------------------------------------------------
 */
static void EyeScanHelper_RunTest(struct u3phy_info *info)
{
	/* 
	 * Disable the test
	 *  RG_SSUSB_RX_EYE_CNT_EN = 0 
	 */
	U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_regs_a60810->eq_eye0)
		, A60810_RG_SSUSB_EQ_EYE_CNT_EN_OFST, A60810_RG_SSUSB_EQ_EYE_CNT_EN, 0);	

	/* 
	 * Run the test
	 * RG_SSUSB_RX_EYE_CNT_EN = 1 
	 */
	U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_regs_a60810->eq_eye0)
  		, A60810_RG_SSUSB_EQ_EYE_CNT_EN_OFST, A60810_RG_SSUSB_EQ_EYE_CNT_EN, 1);

	/* 
	 * Wait til it's done
	 * RGS_SSUSB_RX_EYE_CNT_RDY 
	 */
	while(!U3PhyReadField32(((PHY_UINT32)&info->u3phyd_regs_a60810->phya_rx_mon5)
  		, A60810_RGS_SSUSB_EQ_EYE_CNT_RDY_OFST, A60810_RGS_SSUSB_EQ_EYE_CNT_RDY));
}

/* 
 * --------------------------------------------------------
 *    Function : fgEyeScanHelper_CalNextPoint()
 *   Description : Calcualte the test point for the measurement
 *   Parameter : None
 *      Return : BOOL - TRUE :  the next point is within the
 *                              boundaryof HW limit
 *                      FALSE:  the next point is out of the HW limit
 *        Note : The next point is obtained by calculating
 *               from the bottom left of the region rectangle
 *               and then scanning up until it reaches the upper
 *               limit. At this time, the x will increment, and
 *               start scanning downwards until the y hits the
 *               zero.
 * --------------------------------------------------------
 */
static PHY_INT8 fgEyeScanHelper_CalNextPoint(void)
{
  if ( ((_bYcurr == MAX_Y) && (_eScanDir == SCAN_DN))
    || ((_bYcurr == MIN_Y) && (_eScanDir == SCAN_UP))
        )
  {
    /* 
    	 * Reaches the limit of Y axis
    	 * Increment X 
    	 */
    _bXcurr++;
    _fgXChged = true;
    _eScanDir = (_eScanDir == SCAN_UP) ? SCAN_DN : SCAN_UP;

    if (_bXcurr > MAX_X)
    {
      return false;
    }
  }
  else
  {
    _bYcurr = (_eScanDir == SCAN_DN) ? _bYcurr + 1 : _bYcurr - 1;
    _fgXChged = false;
  }
  return PHY_TRUE;
}

PHY_INT32 eyescan_init_a60810 (struct u3phy_info *info){
	/* initial PHY setting */
	U3PhyWriteField32(((PHY_UINT32)&info->u3phya_regs_a60810->reg9)
		, A60810_RG_SSUSB_CDR_EPEN_OFST, A60810_RG_SSUSB_CDR_EPEN, 1);
	U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_regs_a60810->phyd_mix3)
		, A60810_RG_SSUSB_FORCE_CDR_PI_PWD_OFST, A60810_RG_SSUSB_FORCE_CDR_PI_PWD, 1);
	
	return PHY_TRUE;
}

PHY_INT32 phy_eyescan_a60810(struct u3phy_info *info, PHY_INT32 x_t1, PHY_INT32 y_t1, PHY_INT32 x_br, PHY_INT32 y_br, PHY_INT32 delta_x, PHY_INT32 delta_y
		, PHY_INT32 eye_cnt, PHY_INT32 num_cnt, PHY_INT32 PI_cal_en, PHY_INT32 num_ignore_cnt){
	PHY_INT32 cOfst = 0;
	PHY_UINT8 bIdxX = 0;
	PHY_UINT8 bIdxY = 0;
	PHY_UINT8 bIdxCycCnt = 0;
	PHY_INT8 fgValid;
	PHY_INT8 cX;
	PHY_INT8 cY;
	PHY_UINT8 bExtendCnt;
	PHY_INT8 isContinue;
	PHY_UINT32 wErr0 = 0, wErr1 = 0;

	_rEye1.bX_tl = x_t1;
	_rEye1.bY_tl = y_t1;
	_rEye1.bX_br = x_br;
	_rEye1.bY_br = y_br;
	_rEye1.bDeltaX = delta_x;
	_rEye1.bDeltaY = delta_y;

	_rEye2.bX_tl = x_t1;
	_rEye2.bY_tl = y_t1;
	_rEye2.bX_br = x_br;
	_rEye2.bY_br = y_br;
	_rEye2.bDeltaX = delta_x;
	_rEye2.bDeltaY = delta_y;

	_rTestCycle.wEyeCnt = eye_cnt;
	_rTestCycle.bNumOfEyeCnt = num_cnt;
	_rTestCycle.bNumOfIgnoreCnt = num_ignore_cnt;
	_rTestCycle.bPICalEn = PI_cal_en;	

	_bXcurr = 0;
	_bYcurr = 0;
	_eScanDir = SCAN_DN;
	_fgXChged = false;

	printk("x_t1: %x, y_t1: %x, x_br: %x, y_br: %x, delta_x: %x, delta_y: %x, \
		eye_cnt: %x, num_cnt: %x, PI_cal_en: %x, num_ignore_cnt: %x\n", \
		x_t1, y_t1, x_br, y_br, delta_x, delta_y, eye_cnt, num_cnt, PI_cal_en, num_ignore_cnt);		

	/* force SIGDET to OFF */
	U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_bank2_regs_a60810->b2_phyd_misc0)
		, A60810_RG_SSUSB_RX_SIGDET_EN_SEL_OFST, A60810_RG_SSUSB_RX_SIGDET_EN_SEL, 1);
	U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_bank2_regs_a60810->b2_phyd_misc0)
		, A60810_RG_SSUSB_RX_SIGDET_EN_OFST, A60810_RG_SSUSB_RX_SIGDET_EN, 0);
	U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_regs_a60810->eq_eye1)
		, A60810_RG_SSUSB_EQ_SIGDET_OFST, A60810_RG_SSUSB_EQ_SIGDET, 0);

	/*  RX_TRI_DET_EN to Disable */
	U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_regs_a60810->eq3)
		, A60810_RG_SSUSB_EQ_TRI_DET_EN_OFST, A60810_RG_SSUSB_EQ_TRI_DET_EN, 0);

	U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_regs_a60810->eq_eye0)
		, A60810_RG_SSUSB_EQ_EYE_MON_EN_OFST, A60810_RG_SSUSB_EQ_EYE_MON_EN, 1);
	U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_regs_a60810->eq_eye0)
		, A60810_RG_SSUSB_EQ_EYE_XOFFSET_OFST, A60810_RG_SSUSB_EQ_EYE_XOFFSET, 0);
	U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_regs_a60810->eq_eye0)
		, A60810_RG_SSUSB_EQ_EYE0_Y_OFST, A60810_RG_SSUSB_EQ_EYE0_Y, 0);
	U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_regs_a60810->eq_eye0)
		, A60810_RG_SSUSB_EQ_EYE1_Y_OFST, A60810_RG_SSUSB_EQ_EYE1_Y, 0);


	if (PI_cal_en){
		/* PI Calibration */
		U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_bank2_regs_a60810->b2_phyd_misc0)
			, A60810_RG_SSUSB_RX_PI_CAL_EN_SEL_OFST, A60810_RG_SSUSB_RX_PI_CAL_EN_SEL, 1);
		U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_bank2_regs_a60810->b2_phyd_misc0)
			, A60810_RG_SSUSB_RX_PI_CAL_EN_OFST, A60810_RG_SSUSB_RX_PI_CAL_EN, 0);
		U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_bank2_regs_a60810->b2_phyd_misc0)
			, A60810_RG_SSUSB_RX_PI_CAL_EN_OFST, A60810_RG_SSUSB_RX_PI_CAL_EN, 1);

		DRV_UDELAY(20);

		U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_bank2_regs_a60810->b2_phyd_misc0)
			, A60810_RG_SSUSB_RX_PI_CAL_EN_OFST, A60810_RG_SSUSB_RX_PI_CAL_EN, 0);
		
		_bPIResult = U3PhyReadField32(((PHY_UINT32)&info->u3phyd_regs_a60810->phya_rx_mon5)
			, A60810_RGS_SSUSB_EQ_PILPO_OFST, A60810_RGS_SSUSB_EQ_PILPO);

		printk(KERN_ERR "PI result: %d\n", _bPIResult);
	}
	/*
	 * Read Initial DAC
	 *  Set CYCLE
	 */
	U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_regs_a60810->eq_eye3)
		,A60810_RG_SSUSB_EQ_EYE_CNT_OFST, A60810_RG_SSUSB_EQ_EYE_CNT, eye_cnt);	
	/* Eye Monitor Feature */
	U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_regs_a60810->eq_eye1)
		, A60810_RG_SSUSB_EQ_EYE_MASK_OFST, A60810_RG_SSUSB_EQ_EYE_MASK, 0x3ff);
	U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_regs_a60810->eq_eye0)
		, A60810_RG_SSUSB_EQ_EYE_MON_EN_OFST, A60810_RG_SSUSB_EQ_EYE_MON_EN, 1);

	/* Move X,Y to the top-left corner */
	for (cOfst = 0; cOfst >= -64; cOfst--)
	{
		U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_regs_a60810->eq_eye0)
			,A60810_RG_SSUSB_EQ_EYE_XOFFSET_OFST, A60810_RG_SSUSB_EQ_EYE_XOFFSET, cOfst);
	}
	for (cOfst = 0; cOfst < 64; cOfst++)
	{
		U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_regs_a60810->eq_eye0)
			, A60810_RG_SSUSB_EQ_EYE0_Y_OFST, A60810_RG_SSUSB_EQ_EYE0_Y, cOfst);
		U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_regs_a60810->eq_eye0)
			, A60810_RG_SSUSB_EQ_EYE1_Y_OFST, A60810_RG_SSUSB_EQ_EYE1_Y, cOfst);
	}
	/* ClearErrorResult */
	for(bIdxCycCnt = 0; bIdxCycCnt < CYCLE_COUNT_MAX; bIdxCycCnt++){
		for(bIdxX = 0; bIdxX < ERRCNT_MAX; bIdxX++)
		{
			for(bIdxY = 0; bIdxY < ERRCNT_MAX; bIdxY++){
				pwErrCnt0[bIdxCycCnt][bIdxX][bIdxY] = 0;
				pwErrCnt1[bIdxCycCnt][bIdxX][bIdxY] = 0;
			}
		}
	}
	isContinue = true;
	while(isContinue){
		printk(KERN_ERR "_bXcurr: %d, _bYcurr: %d\n", _bXcurr, _bYcurr);
		/*
		 *  The point is within the boundary, then let's check if it is within
		 *  the testing region.
		 * The point is only test-able if one of the eye region
		 * includes this point.
		 */
	    fgValid = fgEyeScanHelper_CheckPtInRegion(&_rEye1, _bXcurr, _bYcurr)
           || fgEyeScanHelper_CheckPtInRegion(&_rEye2, _bXcurr, _bYcurr);
        /*
		 * Translate bX and bY to 2's complement from where the origin was on the
		 *  top left corner.
		 *  0x40 and 0x3F needs a bit of thinking!!!! >"<
		 */
		cX = (_bXcurr ^ 0x40);
		cY = (_bYcurr ^ 0x3F);

		/* Set X if necessary */
		if (_fgXChged == true)
		{
			U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_regs_a60810->eq_eye0)
				, A60810_RG_SSUSB_EQ_EYE_XOFFSET_OFST, A60810_RG_SSUSB_EQ_EYE_XOFFSET, cX);		
		}
		/* Set Y */
		U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_regs_a60810->eq_eye0)
			, A60810_RG_SSUSB_EQ_EYE0_Y_OFST, A60810_RG_SSUSB_EQ_EYE0_Y, cY);			
		U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_regs_a60810->eq_eye0)
			, A60810_RG_SSUSB_EQ_EYE1_Y_OFST, A60810_RG_SSUSB_EQ_EYE1_Y, cY);	

		/* Test this point! */
		if (fgValid){
			for (bExtendCnt = 0; bExtendCnt < num_ignore_cnt; bExtendCnt++)
			{
				/* run test */
				EyeScanHelper_RunTest(info);
			}
			for (bExtendCnt = 0; bExtendCnt < num_cnt; bExtendCnt++)
			{
				EyeScanHelper_RunTest(info);
				wErr0 = U3PhyReadField32(((PHY_UINT32)&info->u3phyd_regs_a60810->phya_rx_mon3)
					, A60810_RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_0_OFST, A60810_RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_0);
				wErr1 = U3PhyReadField32(((PHY_UINT32)&info->u3phyd_regs_a60810->phya_rx_mon4)
					, A60810_RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_1_OFST, A60810_RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_1);

				pwErrCnt0[bExtendCnt][_bXcurr][_bYcurr] = wErr0;
				pwErrCnt1[bExtendCnt][_bXcurr][_bYcurr] = wErr1;

			}
		}
		else{
			
		}
		if (fgEyeScanHelper_CalNextPoint() == false){
			printk(KERN_ERR "Xcurr [0x%x] Ycurr [0x%x]\n", _bXcurr, _bYcurr);
		 	printk(KERN_ERR "XcurrREG [0x%x] YcurrREG [0x%x]\n", cX, cY);
			printk(KERN_ERR "end of eye scan\n");
		  	isContinue = false;
		}
	}
	printk(KERN_ERR "CurX [0x%x] CurY [0x%x]\n"
		, U3PhyReadField32(((PHY_UINT32)&info->u3phyd_regs_a60810->eq_eye0), A60810_RG_SSUSB_EQ_EYE_XOFFSET_OFST, A60810_RG_SSUSB_EQ_EYE_XOFFSET)
		, U3PhyReadField32(((PHY_UINT32)&info->u3phyd_regs_a60810->eq_eye0), A60810_RG_SSUSB_EQ_EYE0_Y_OFST, A60810_RG_SSUSB_EQ_EYE0_Y));

	/*  Move X,Y to the top-left corner */
	for (cOfst = 63; cOfst >= 0; cOfst--)
	{
		U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_regs_a60810->eq_eye0)
			, A60810_RG_SSUSB_EQ_EYE_XOFFSET_OFST, A60810_RG_SSUSB_EQ_EYE_XOFFSET, cOfst);
	}
	for (cOfst = 63; cOfst >= 0; cOfst--)
	{
		U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_regs_a60810->eq_eye0)
			, A60810_RG_SSUSB_EQ_EYE0_Y_OFST, A60810_RG_SSUSB_EQ_EYE0_Y, cOfst);
		U3PhyWriteField32(((PHY_UINT32)&info->u3phyd_regs_a60810->eq_eye0)
			, A60810_RG_SSUSB_EQ_EYE1_Y_OFST, A60810_RG_SSUSB_EQ_EYE1_Y, cOfst);

	}
	printk(KERN_ERR "CurX [0x%x] CurY [0x%x]\n"
		, U3PhyReadField32(((PHY_UINT32)&info->u3phyd_regs_a60810->eq_eye0), A60810_RG_SSUSB_EQ_EYE_XOFFSET_OFST, A60810_RG_SSUSB_EQ_EYE_XOFFSET)
		, U3PhyReadField32(((PHY_UINT32)&info->u3phyd_regs_a60810->eq_eye0), A60810_RG_SSUSB_EQ_EYE0_Y_OFST,A60810_RG_SSUSB_EQ_EYE0_Y));

	printk(KERN_ERR "PI result: %d\n", _bPIResult);
	printk(KERN_ERR "pwErrCnt0 addr: 0x%x\n", (PHY_UINT32)pwErrCnt0);
	printk(KERN_ERR "pwErrCnt1 addr: 0x%x\n", (PHY_UINT32)pwErrCnt1);
	return PHY_TRUE;
}

PHY_INT32 u2_connect_a60810(struct u3phy_info *info){
	/* for better LPM BESL value	 */
	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs_a60810->u2phydcr1)
		, A60810_RG_USB20_SW_PLLMODE_OFST, A60810_RG_USB20_SW_PLLMODE, 0x1);
	return PHY_TRUE;
}

PHY_INT32 u2_disconnect_a60810(struct u3phy_info *info){
	/* for better LPM BESL value	 */
	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs_a60810->u2phydcr1)
		, A60810_RG_USB20_SW_PLLMODE_OFST, A60810_RG_USB20_SW_PLLMODE, 0x0);
	return PHY_TRUE;
}

PHY_INT32 u2_save_cur_en_a60810(struct u3phy_info *info){
	return PHY_TRUE;
}

PHY_INT32 u2_save_cur_re_a60810(struct u3phy_info *info){
	return PHY_TRUE;
}

PHY_INT32 u2_slew_rate_calibration_a60810(struct u3phy_info *info){
	PHY_INT32 i=0;
	PHY_INT32 fgRet = 0;	
	PHY_INT32 u4FmOut = 0;	
	PHY_INT32 u4Tmp = 0;

	/* 
	 * => RG_USB20_HSTX_SRCAL_EN = 1
	 * enable HS TX SR calibration 
	 */
	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs_a60810->usbphyacr5)
		, A60810_RG_USB20_HSTX_SRCAL_EN_OFST, A60810_RG_USB20_HSTX_SRCAL_EN, 1);
	DRV_MSLEEP(1);	

	/* 
	 * => RG_FRCK_EN = 1
	 * Enable free run clock
	 */
	U3PhyWriteField32(((PHY_UINT32)&info->sifslv_fm_regs_a60810->fmmonr1)
		, A60810_RG_FRCK_EN_OFST, A60810_RG_FRCK_EN, 0x1);
	/* 
	 *  => RG_CYCLECNT = 0x400
	 *  Setting cyclecnt = 0x400 
	 */
	U3PhyWriteField32(((PHY_UINT32)&info->sifslv_fm_regs_a60810->fmcr0)
		, A60810_RG_CYCLECNT_OFST, A60810_RG_CYCLECNT, 0x400);
	/* 
	 *  => RG_FREQDET_EN = 1
	 *  Enable frequency meter 
	 */
	U3PhyWriteField32(((PHY_UINT32)&info->sifslv_fm_regs_a60810->fmcr0)
		, A60810_RG_FREQDET_EN_OFST, A60810_RG_FREQDET_EN, 0x1);

	/* wait for FM detection done, set 10ms timeout */
	for(i=0; i<10; i++){
		/*
		 * => u4FmOut = USB_FM_OUT
		 *  read FM_OUT 
		 */
		u4FmOut = U3PhyReadReg32(((PHY_UINT32)&info->sifslv_fm_regs_a60810->fmmonr0));
		printk("FM_OUT value: u4FmOut = %d(0x%08X)\n", u4FmOut, u4FmOut);

		/* check if FM detection done  */
		if (u4FmOut != 0)
		{
			fgRet = 0;
			printk("FM detection done! loop = %d\n", i);
			
			break;
		}

		fgRet = 1;
		DRV_MSLEEP(1);
	}
	/*
	 * => RG_FREQDET_EN = 0
	 * disable frequency meter 
	 */
	U3PhyWriteField32(((PHY_UINT32)&info->sifslv_fm_regs_a60810->fmcr0)
		, A60810_RG_FREQDET_EN_OFST, A60810_RG_FREQDET_EN, 0);
	/* 
	 *  => RG_FRCK_EN = 0
	 *  disable free run clock 
	 */
	U3PhyWriteField32(((PHY_UINT32)&info->sifslv_fm_regs_a60810->fmmonr1)
		, A60810_RG_FRCK_EN_OFST, A60810_RG_FRCK_EN, 0);
	/* 
	 *  => RG_USB20_HSTX_SRCAL_EN = 0
	 *  disable HS TX SR calibration 
	 */
	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs_a60810->usbphyacr5)
		, A60810_RG_USB20_HSTX_SRCAL_EN_OFST, A60810_RG_USB20_HSTX_SRCAL_EN, 0);
	DRV_MSLEEP(1);

	if(u4FmOut == 0){
		U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs_a60810->usbphyacr5)
				, A60810_RG_USB20_HSTX_SRCTRL_OFST, A60810_RG_USB20_HSTX_SRCTRL, 0x4);

		fgRet = 1;
	}
	else{
		u4Tmp = (((1024 * REF_CK * U2_SR_COEF_A60810) / u4FmOut) + 500) / 1000;
		printk("SR calibration value u1SrCalVal = %d\n", (PHY_UINT8)u4Tmp);
		U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs_a60810->usbphyacr5)
				, A60810_RG_USB20_HSTX_SRCTRL_OFST, A60810_RG_USB20_HSTX_SRCTRL, u4Tmp);
	}

	return fgRet;
}
#endif
