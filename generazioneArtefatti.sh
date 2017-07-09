#!/bin/bash
######################################## Operazioni da svolgere per eseguire Linux sulla Zybo ##########################################
# 1)Formattare una scheda SD (preferibilmente una SDHC) di dimensione di almeno 4 GiB con il seguente schema:
# 	4 MiB 			non allocato
#	1 GiB		FAT32	partizione di boot -> qui vanno i file generati da questo script (BOOT.bin, *.dtb e uImage)
#	> 3 GiB		ext4	partizione per il root fs -> qui va copiato il file system (linaro, xillinux, ecc.)
#
# 2) Specificare la variabile SDK_PROJECT_PATH con il path ASSOLUTO della cartella *.sdk contenuta nel progetto Vivado dal quale si
#    è esportato l'hardware;
#
# 3) Specificare il nome del progetto contenente il bitstream in BITSTREAM_PRJ_NAME ed il nome del bitstream (esclusa l'estensione) nella
#    variabile BITSTREAM_NAME;
#
# 4) Creare un progetto di tipo "board support package" in Xilinx SDK, selezionando l'opzione device_tree. Il nome del progetto
#    deve essere specificato nella variabile DTB_PRJ_NAME;
#    NOTA: modificare il file system-top.dts sostituendo la configurazione del parametro bootargs con la seguente:
#          bootargs = "console=ttyPS0,115200 root=/dev/mmcblk0p2 rw earlyprintk rootfstype=ext4 rootwait devtmpfs.mount=0";
#          ed eliminare la riga: "stdout-path = "serial0:115200n8";"
#    NOTA2: il valore di bootargs può essere definito nella finestra di configurazione del device-tree (Board support package settings),
#	    inserendo nel campo value:
#	    console=ttyPS0,115200 root=/dev/mmcblk0p2 rw earlyprintk rootfstype=ext4 rootwait devtmpfs.mount=0
#
# 5) Creare un progetto di tipo "Application project" in Xilinx SDK selezionando l'opzione Zynq FSBL e compilarlo. Il nome del progetto
#    deve essere specificato nella variabile FSBL_PRJ_NAME;
#
# 6) Eseguire lo script con il comando:
#		 ./generazioneArtefatti.sh
#    NOTA: settare prima queste variabili di ambiente prima di eseguire lo script:
#		export ARCH=arm
#	 	export CROSS_COMPILE=arm-xilinx-linux-gnueabi-
#		export PATH=$PATH:/opt/Xilinx/SDK/2016.4/gnu/arm/lin/bin:/opt/u-boot-Digilent-Dev/tools
#
#
# 7) Generare il file BOOT.BIN mediante la funzione "Create Boot Image" presente nel menù "Xilinx Tools" dell'SDK selezionando i file
#    presenti nella cartella BOOT_BIN_DIR;
#    NOTA: l'ordine di inclusione dei file DEVE essere il seguente:
#	    -fsbl.elf	(bootloader)
#	    -*.bit	(datafile)
#	    -u-boot.elf (datafile)
#
# 8) Copiare i file BOOT.BIN, uImage e devicetree.dtb nella partizione FAT32 della SD card;
#    NOTA: non utilizzare ne macOS ne Windows per la copia sull'SD card dei file. Questi OS generano file nascosti che possono impedire il
#	   corretto avviamento del kernel.
#
# 9) Copiare il file system nella partizione EXT4 della SD card;
#
# 10) Modificare la posizione del jumper 5 (JP5) sulla Zybo per l'avviamento da SD card;
#
# 11) Collegare la Zybo al PC e avviare una sessione seriale mediante il comando:
#     		screen /dev/device_name 115200
#     NOTA: device_name varia a seconda del sistema operativo: cu.usbserial-210279652574B (macOS)/ ttyUSB1 (Linux)
#
# 12) Premere il pulsante di reset PS-SRST sulla Zybo.
#
# 13) Lasciare che il processo di boot termini. Si giungerà alla fine del processo ad una shell di comando con l'utente root loggato.
#
# 14) Enjoy!
##########################################################################################################################################
######################################################### Parametri di configurazione ####################################################
# Variabili da configurare (vedi sopra)
SDK_PROJECT_PATH=/home/antonio/Scrivania/sistemiEmbedded/workbench/vivado/gpio/gpio.sdk
BITSTREAM_PRJ_NAME=design_con_interrupt_wrapper_hw_platform_0
BITSTREAM_NAME=design_con_interrupt_wrapper
DTB_PRJ_NAME=design_con_interrupt_device_tree
FSBL_PRJ_NAME=linux_fsbl

# Variabili che configurano le cartelle dove andaranno ad essere raggruppati gli artefatti
ROOT_DIR=/opt
OUTPUT_DIR=$ROOT_DIR/artefatti
BOOT_BIN_DIR=$ROOT_DIR/artefatti/bootBin
##########################################################################################################################################
# NOTE:
#	./generazioneArtefatti.sh clean_all	elimina tutti file che lo script genera (compresi gli artefatti finali)
#

# Cleaning delle risorse
if [[ $1 == clean_all ]]; then
	rm -r $ROOT_DIR/linux-Digilent-Dev/
	rm -r $ROOT_DIR/u-boot-Digilent-Dev/
	rm -r $OUTPUT_DIR
else
	cd $ROOT_DIR && mkdir artefatti && mkdir artefatti/bootBin

	################### Compilazione kernel ######################
	rm -r $ROOT_DIR/linux-Digilent-Dev/
	git clone https://github.com/Digilent/linux-Digilent-Dev.git
	cd $ROOT_DIR/linux-Digilent-Dev/
	make ARCH=arm xilinx_zynq_defconfig && make -j 2 ARCH=arm CROSS_COMPILE=arm-xilinx-linux-gnueabi- UIMAGE_LOADADDR=0x8000 uImage
	cp arch/arm/boot/uImage $OUTPUT_DIR #&& make clean

	################### Compilazione u-boot ######################
	rm -r $ROOT_DIR/u-boot-Digilent-Dev/
	git clone https://github.com/Digilent/u-boot-Digilent-Dev.git
	cd $ROOT_DIR/u-boot-Digilent-Dev/
	make CROSS_COMPILE=arm-xilinx-linux-gnueabi- zynq_zybo_config && make -j 2 CROSS_COMPILE=arm-xilinx-linux-gnueabi-
	cp u-boot $BOOT_BIN_DIR && mv $BOOT_BIN_DIR/u-boot $BOOT_BIN_DIR/u-boot.elf #&& make clean

	################### Generazione dtb #########################
	cd $SDK_PROJECT_PATH/$DTB_PRJ_NAME && dtc -I dts -O dtb -o devicetree.dtb system-top.dts
	mv devicetree.dtb $OUTPUT_DIR

	################### Copia file necessari a generare BOOT.BIN ######################
	cp $SDK_PROJECT_PATH/$FSBL_PRJ_NAME/Debug/$FSBL_PRJ_NAME.elf $BOOT_BIN_DIR
	cp $SDK_PROJECT_PATH/$BITSTREAM_PRJ_NAME/$BITSTREAM_NAME.bit $BOOT_BIN_DIR
fi
