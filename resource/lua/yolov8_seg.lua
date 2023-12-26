--[[
History:
  2023-03-27 - [Qiangqiang Zhang] created file

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
	conf_threshold = 0.5,    -- Confidence threshold
	top_k = 200,             -- Top k on each class
	nms_threshold = 0.45,    -- NMS threshold
	class_num = 80,          -- class num
	mask_num = 32,           -- for segmentation
	enable_seg = 1,          -- enable segmentation
	log_level = 2,           -- 0 none, 1 error, 2 notice, 3 debug, 4 verbose
	disable_fsync = 0,       -- disable fsync for live mode when it is 1
	output_0 = "output0",    -- Output name for the detected objects
	output_1 = "output1",    -- Output name for segmentation, it is needed when enable_seg is 1
}
return _nn_arm_nms_config_
