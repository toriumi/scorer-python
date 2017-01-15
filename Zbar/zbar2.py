import zbar
from PIL import Image
scanner = zbar.ImageScanner()
# disable qr code scanning (all symbol formats on by default)
scanner.set_config(zbar.SymbolType.QRCODE, zbar.Config.ENABLE, 0)
image = Image.open("sdkqr.gif")
codes = scanner.scan_pil_image(image)
print(codes)
print(image)