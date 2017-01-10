#https://github.com/cassou/VL53L0X_rasp
#http://www.st.com/content/st_com/en/products/embedded-software/proximity-sensors-software/stsw-img005.html
import subprocess
res = subprocess.call('./VL53L0X_rasp/bin/vl53l0x_Ranging_Example')
#=> lsƒRƒ}ƒ“ƒh‚ÌŒ‹‰Ê
print(res)