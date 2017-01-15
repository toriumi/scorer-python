import subprocess
qrcode = "sdkqr.gif"
#res = subprocess.check_output(["zbarimg", qrcode])
res = subprocess.check_output('zbarimg ' + qrcode, shell=True, universal_newlines=True)
print(res)
