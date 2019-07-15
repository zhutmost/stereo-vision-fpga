#include "xstatus.h"
#include "xdebug.h"
#include "xvtc.h"

#include "vtc_ctrl.h"

VtcCtrl::VtcCtrl() {
    this->p_vtc = &vtc;
}

VtcCtrl::~VtcCtrl() {
}


int VtcCtrl::init(u16 device_id) {
    int status;
    XVtc_Config *config;

    this->state = VTC_STOPPED;
    this->vid_mode = VMODE_1280x720;

    config = XVtc_LookupConfig(device_id);
    if (NULL == config) {
        return XST_FAILURE;
    }
    status = XVtc_CfgInitialize(this->p_vtc, config, config->BaseAddress);
    /* Checking status */
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    return XST_SUCCESS;
}


int VtcCtrl::start() {
    XVtc_Timing timing;
    XVtc_SourceSelect src_sel;

    xdbg_printf(XDBG_DEBUG_GENERAL, "vtc start entered\n\r");

    if (this->state == VTC_RUNNING) {
        return XST_SUCCESS;
    }

    timing.HActiveVideo = this->vid_mode.width;                        // Horizontal Active Video Size
    timing.HFrontPorch = this->vid_mode.hps - this->vid_mode.width;    // Horizontal Front Porch Size
    timing.HSyncWidth = this->vid_mode.hpe - this->vid_mode.hps;       // Horizontal Sync Width
    timing.HBackPorch = this->vid_mode.hmax - this->vid_mode.hpe + 1;  // Horizontal Back Porch Size
    timing.HSyncPolarity = this->vid_mode.hpol;                        // Horizontal Sync Polarity
    timing.VActiveVideo = this->vid_mode.height;                       // Vertical Active Video Size
    timing.V0FrontPorch = this->vid_mode.vps - this->vid_mode.height;  // Vertical Front Porch Size
    timing.V0SyncWidth = this->vid_mode.vpe - this->vid_mode.vps;      // Vertical Sync Width
    timing.V0BackPorch = this->vid_mode.vmax - this->vid_mode.vpe + 1; // Horizontal Back Porch Size
    timing.V1FrontPorch = this->vid_mode.vps - this->vid_mode.height;  // Vertical Front Porch Size
    timing.V1SyncWidth = this->vid_mode.vpe - this->vid_mode.vps;      // Vertical Sync Width
    timing.V1BackPorch = this->vid_mode.vmax - this->vid_mode.vpe + 1; // Horizontal Back Porch Size
    timing.VSyncPolarity = this->vid_mode.vpol;                        // Vertical Sync Polarity
    timing.Interlaced = 0;                                             // Interlaced / Progressive video

    /* Setup the VTC Source Select config structure. */
    /* 1=Generator registers are source */
    /* 0=Detector registers are source */
    memset((void *)&src_sel, 0, sizeof(src_sel));
    src_sel.VBlankPolSrc = 1;
    src_sel.VSyncPolSrc = 1;
    src_sel.HBlankPolSrc = 1;
    src_sel.HSyncPolSrc = 1;
    src_sel.ActiveVideoPolSrc = 1;
    src_sel.ActiveChromaPolSrc= 1;
    src_sel.VChromaSrc = 1;
    src_sel.VActiveSrc = 1;
    src_sel.VBackPorchSrc = 1;
    src_sel.VSyncSrc = 1;
    src_sel.VFrontPorchSrc = 1;
    src_sel.VTotalSrc = 1;
    src_sel.HActiveSrc = 1;
    src_sel.HBackPorchSrc = 1;
    src_sel.HSyncSrc = 1;
    src_sel.HFrontPorchSrc = 1;
    src_sel.HTotalSrc = 1;
    XVtc_SelfTest(this->p_vtc);
    XVtc_RegUpdateEnable(this->p_vtc);
    XVtc_SetGeneratorTiming(this->p_vtc, &timing);
    XVtc_SetSource(this->p_vtc, &src_sel);
    /*
     * Enable VTC core, releasing backpressure on VDMA
     */
    XVtc_EnableGenerator(this->p_vtc);

    this->state = VTC_RUNNING;

    return XST_SUCCESS;
}

int VtcCtrl::stop() {
    if (this->state == VTC_STOPPED) {
        return XST_SUCCESS;
    }

    XVtc_DisableGenerator(this->p_vtc);

    this->state = VTC_STOPPED;
    return XST_SUCCESS;
}
