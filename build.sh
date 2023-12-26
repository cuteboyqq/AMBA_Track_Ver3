#!/bin/bash

source ./env.sh

make_clean() {
	exit 0
}

make_kernel() {
 	exit 0
}

make_wnc_app() {
	./build_wnc_app.sh all
}

make_sdk() {
	exit 0
}

make_sdk_doc() {
	cd $TOPDIR/cv25_linux_sdk_3.0/ambarella/boards/cv25_hazelnut
	source ../../build/env/aarch64-linux5.4-gcc.env
	make distclean
	make sync_build_mkcfg
	make sdk_doc
}


make_distclean() {
        cd $TOPDIR/cv25_linux_sdk_3.0/ambarella/boards/cv25_hazelnut
        source ../../build/env/aarch64-linux5.4-gcc.env
        make distclean

	if [ -f "$TOPDIR/build.log" ]; then
	    rm $TOPDIR/build.log
	fi
}

make_evk() {
	#make_wnc_app
	cd $TOPDIR/cv25_linux_sdk_3.0/ambarella/boards/cv25_hazelnut
	source ../../build/env/aarch64-linux5.4-gcc.env
	make distclean
	make sync_build_mkcfg
	make cv25_ipcam_config
	make defconfig_public_linux
	make -j8 2>&1 | tee -a $TOPDIR/build.log

}


make_project() {
	#make_wnc_app all
	cd $TOPDIR/cv25_linux_sdk_3.0/ambarella/boards/cv25_hornet
	source ../../build/env/aarch64-linux5.4-gcc.env
	make distclean
	make sync_build_mkcfg
	make cv25_hornet_emmc_config
	make defconfig_public_linux
	make -j8 2>&1 | tee -a $TOPDIR/build.log

}

case "$1" in
  clean)
	make_clean
	;;
  kernel)
	make_kernel
	;;
  wnc_app)
	make_wnc_app
	;;
  sdk)
	make_sdk
	;;
  sdk_doc)
	make_sdk_doc
	;;
  distclean)
	make_distclean
	;;
  evk)
	make_evk
	;;
  *)
	make_project
	exit 0
esac
