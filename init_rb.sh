make all
sudo insmod rbt530_drv.ko
sudo chmod 777 /dev/rbt530_dev
sudo insmod RBprobe.ko
sudo chmod 777 /dev/RBprobe
