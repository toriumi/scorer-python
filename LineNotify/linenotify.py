#http://hacknote.jp/archives/19074/
import pycurl

buffer='テストだよん'
curl = pycurl.Curl()
curl.setopt(pycurl.URL, 'https://notify-api.line.me/api/notify')
curl.setopt(pycurl.HTTPHEADER, ['Authorization: Bearer AhfffLL3f1BYUJMnBnV8HZFAuvFS3ztJ65ZQv9UPo5j'])
#curl.setopt(pycurl.HTTPPOST, [('message', buffer.encode('utf-8')),('imageFile','@lena.png'),('stickerPackageId', '1'), ('stickerId', '112')])
curl.setopt(pycurl.HTTPPOST, [('message', buffer.encode('utf-8')),('imageFile', (curl.FORM_FILE, 'lena.png'))])

curl.perform()
