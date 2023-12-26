--[[
History:
  2022-08-31 - [Yingjie Xue] created file

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
	use_multi_cls = 0,       -- 0: use the best class for one object; 1: use multi classes for one object; > 1: Invalid
	top_k = 200,             -- Top k on each class
	conf_threshold = 0.2,    -- Confidence threshold
	nms_threshold = 0.3,     -- NMS threshold
	log_level = 2,           -- 0 none, 1 error, 2 notice, 3 debug, 4 verbose.
	feature_map_32 = "422",  -- Output name for the feature map with 32 down sampling for big objects
	feature_map_16 = "360",  -- Output name for the feature map with 16 down sampling for medium objects
	feature_map_8 = "298",   -- Output name for the feature map with 8 down sampling for small objects
	class_num = 80,          -- class num
	tiny_enable = 0,         -- use tiny model, 0 no, 1 use
	anchors_value = {10, 13, 16, 30, 33, 23, 30, 16, 62, 45, 59, 119, 116, 90, 156, 198, 373, 326}, -- Anchors for above three feature maps
}
return _nn_arm_nms_config_
