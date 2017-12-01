# CSE530_TEAM29

1.To compile use :
make

(Note, even after passing the pthread flag, warnings are shown. Also for the kernel module, compiling for the first time shows warnings. After Recompiling warnings are not shown)

2. to initialize the tree driver, use
./init_rb.sh


3.to run the test user program run,
./rb_test


4. TO remove the driver, run:
./exit_rb.sh


ASUAD\mahmad11@en4061184l:~/Downloads/CSE530_TEAM29$ ./rb_test
Thread ID accessing the tree = 140464342124288
Thread ID accessing the tree = 140464350516992
Thread ID accessing the tree = 140464325338880
Thread ID accessing the tree = 140464333731584
data entered : key =  83, data = 1 
---------------------------------------
data entered : key =  86, data = 1 
---------------------------------------
data entered : key =  77, data = 1 
---------------------------------------
data entered : key =  15, data = 1 
---------------------------------------
pid = 6152, address = 0xc07a3024, timestamp (TSC) = 2725257578672, rb_object->key = 77
Thread ID accessing the tree = 140464342124288
data entered : key =  93, data = 1 
---------------------------------------
pid = 6151, address = 0xc07a3024, timestamp (TSC) = 2735897463560, rb_object->key = 86
Thread ID accessing the tree = 140464350516992
data entered : key =  35, data = 1 
---------------------------------------
pid = 6150, address = 0xc07a3024, timestamp (TSC) = 2741217695520, rb_object->key = 15
Thread ID accessing the tree = 140464333731584
data entered : key =  86, data = 1 
---------------------------------------
Thread ID accessing the tree = 140464342124288
data entered : key =  92, data = 1 
---------------------------------------
pid = 6151, address = 0xc07a3024, timestamp (TSC) = 2746537883512, rb_object->key = 93
Thread ID accessing the tree = 140464350516992
Thread ID accessing the tree = 140464342124288
data entered : key =  49, data = 1 
---------------------------------------
data entered : key =  21, data = 1 
---------------------------------------
pid = 6151, address = 0xc07a3024, timestamp (TSC) = 2757178496736, rb_object->key = 15
Thread ID accessing the tree = 140464325338880
data entered : key =  62, data = 1 
---------------------------------------
pid = 6153, address = 0xc07a3024, timestamp (TSC) = 2759837782696, rb_object->key = 49
Thread ID accessing the tree = 140464333731584
data entered : key =  27, data = 1 
---------------------------------------
pid = 6152, address = 0xc07a3024, timestamp (TSC) = 2762498285112, rb_object->key = 21
Thread ID accessing the tree = 140464342124288
data entered : key =  90, data = 1 
---------------------------------------
pid = 6151, address = 0xc07a3024, timestamp (TSC) = 2767818901072, rb_object->key = 86
Thread ID accessing the tree = 140464350516992
data entered : key =  59, data = 1 
---------------------------------------
pid = 6150, address = 0xc07a3024, timestamp (TSC) = 2773138903776, rb_object->key = 49
Thread ID accessing the tree = 140464342124288
data entered : key =  63, data = 1 
---------------------------------------
pid = 6151, address = 0xc07a3024, timestamp (TSC) = 2778459390440, rb_object->key = 77
Thread ID accessing the tree = 140464333731584
data entered : key =  26, data = 1 
---------------------------------------
pid = 6152, address = 0xc07a3024, timestamp (TSC) = 2781118685488, rb_object->key = 27
Thread ID accessing the tree = 140464350516992
data entered : key =  40, data = 1 
---------------------------------------
Thread ID accessing the tree = 140464342124288
data entered : key =  26, data = 1 
---------------------------------------
pid = 6150, address = 0xc07a3024, timestamp (TSC) = 2789099298392, rb_object->key = 49
Thread ID accessing the tree = 140464325338880
IOCTL Id is 2147773185 , cmd :1 
Ioctl set. It returned : 0
Reading maximum value : 93
---------------------------------------
Thread ID accessing the tree = 140464333731584
IOCTL Id is 2147773185 , cmd :1 
Ioctl set. It returned : 0
Reading maximum value : 93
---------------------------------------
Read the tree of size : 16
ALL THE TREE DATA : 
83,35,92,21,62,86,93,15,27,49,77,90,26,40,59,63,


This program has been tested and worked correctly on Ubuntu Xenial 16.04 x86_64 OS

ASUAD\mahmad11@en4061184l:~/Downloads/CSE530_TEAM29$ uname -a 
Linux en4061184l 4.10.0-35-generic #39~16.04.1-Ubuntu SMP Wed Sep 13 09:02:42 UTC 2017 x86_64 x86_64 x86_64 GNU/Linux
ASUAD\mahmad11@en4061184l:~/Downloads/CSE530_TEAM29$ lsb_release -a
No LSB modules are available.
Distributor ID:	Ubuntu
Description:	Ubuntu 16.04.3 LTS
Release:	16.04
Codename:	xenial



