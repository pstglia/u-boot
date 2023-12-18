#!/bin/bash

LOCALPATH=$(pwd)

export ARCH=arm
export CROSS_COMPILE=${LOCALPATH}/../prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android-

TOOLPATH=${LOCALPATH}/../rkbin/tools
PATH=$PATH:$TOOLPATH

finish() {
	echo -e "\e[31m MAKE UBOOT IMAGE FAILED.\e[0m"
	exit -1
}

trap finish ERR

#############################################################################################
make all
$TOOLPATH/loaderimage --pack --uboot ./u-boot-dtb.bin uboot.img 0x200000
#############################################################################################
echo "make idbloader.img"
tools/mkimage -n rk322x -T rksd -d ../rkbin/bin/rk32/rk322x_ddr_300MHz_v1.07.bin idbloader.img
cat ../rkbin/bin/rk32/rk322x_miniloader_at_v2.52.bin >> idbloader.img
#############################################################################################
#cp ../rkbin/bin/rk32/rk3399_loader_v1.12.112.bin .

#############################################################################################
cat >trust.ini <<EOF
[VERSION]
MAJOR=1
MINOR=0
[BL30_OPTION]
SEC=0
[BL31_OPTION]
SEC=1
PATH=../rkbin/bin/rk33/rk3399_bl31_v1.18.elf
ADDR=0x10000
[BL32_OPTION]
SEC=1
PATH=../rkbin/bin/rk33/rk3399_bl32_v1.13.bin
ADDR=0x08400000
[BL33_OPTION]
SEC=0
[OUTPUT]
PATH=trust.img
EOF

$TOOLPATH/trust_merger trust.ini
rm -f trust.ini
##############################################################################################
echo -e "\e[36m U-boot IMAGE READY! \e[0m"
