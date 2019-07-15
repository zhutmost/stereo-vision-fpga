#ifndef SRC_VTC_CTRL_VTC_CTRL_H_
#define SRC_VTC_CTRL_VTC_CTRL_H_

#include "xvtc.h"

#include "../vga_mode.h"

typedef enum {
    VTC_STOPPED = 0,
    VTC_RUNNING = 1
} VtcState;

class VtcCtrl {
private:
    VideoMode vid_mode;
    XVtc vtc;
    XVtc *p_vtc;
    VtcState state;
public:
    VtcCtrl();
    ~VtcCtrl();
    int init(u16 device_id);
    int start();
    int stop();
    int set_mode(const VideoMode *new_mode);
};

#endif /* SRC_VTC_CTRL_VTC_CTRL_H_ */
