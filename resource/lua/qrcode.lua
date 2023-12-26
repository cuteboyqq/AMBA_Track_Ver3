--[[
History:
  2022-08-17 - [Yang Cao] created file

Copyright (c) 2022 Ambarella International LP

This file and its contents ("Software") are protected by intellectual
property rights including, without limitation, U.S. and/or foreign
copyrights. This Software is also the confidential and proprietary
information of Ambarella International LP and its licensors. You may not use, reproduce,
disclose, distribute, modify, or otherwise prepare derivative works of this
Software or any portion thereof except pursuant to a signed license agreement
or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
In the absence of such an agreement, you agree to promptly notify and return
this Software to Ambarella International LP.

This file includes sample code and is only for internal testing and evaluation.  If you
distribute this sample code (whether in source, object, or binary code form), it will be
without any warranty or indemnity protection from Ambarella International LP or its affiliates.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

--]]

_nn_arm_nms_config_ = {
        conf_threshold = 0.7,    -- Confidence threshold
        nms_threshold = 0.45,     -- NMS threshold
        log_level = 2,           -- 0 none, 1 error, 2 notice, 3 debug, 4 verbose.
        keep_top_k = 100,
        nms_top_k = 100,
        background_label_id = 0,
        unnormalized = 0,
        num_class = 2,          -- class num
        thread_num = 4,
        output_score_name = "mbox_conf_flatten",
        output_loc_name = "mbox_loc",
        tf_scale_factors = {0, 0, 0, 0},         -- use tiny model, 0 no, 1 use
        ssd_debug = 0,
	bbox_enlarge_r = 0.1,     -- enlarge the detection bbox result to fully include the QR zone
	decode_res_h = 224,     -- the height resolution for QR decoding algorithm
	decode_res_w = 224,     -- the width resolution for QR decoding algorithm
}

return _nn_arm_nms_config_
