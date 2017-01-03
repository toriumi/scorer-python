import cv2
import sys
import scorer_sdk.Scorer
import pycurl

def send2line_on():
    buffer='on.png saved'
    curl = pycurl.Curl()
    curl.setopt(pycurl.URL, 'https://notify-api.line.me/api/notify')
    curl.setopt(pycurl.HTTPHEADER, ['Authorization: Bearer AhfffLL3f1BYUJMnBnV8HZFAuvFS3ztJ65ZQv9UPo5j'])
    curl.setopt(pycurl.HTTPPOST, [('message', buffer.encode('utf-8')),('imageFile', (curl.FORM_FILE, '/opt/scorer/home/dev/SDK/roi/on.png'))])
    curl.perform()

def send2line_off():
    buffer='off.png saved'
    curl = pycurl.Curl()
    curl.setopt(pycurl.URL, 'https://notify-api.line.me/api/notify')
    curl.setopt(pycurl.HTTPHEADER, ['Authorization: Bearer AhfffLL3f1BYUJMnBnV8HZFAuvFS3ztJ65ZQv9UPo5j'])
    curl.setopt(pycurl.HTTPPOST, [('message', buffer.encode('utf-8')),('imageFile', (curl.FORM_FILE, '/opt/scorer/home/dev/SDK/roi/off.png'))])
    curl.perform()

args = sys.argv
scorer=scorer_sdk.Scorer.Scorer("Task")
x1,y1,x2,y2 = scorer.get_roi_rect_by_index(0)


if scorer.poll() == False:
    sys.exit()

frame = scorer.get_frame()
    
if frame == None:
    sys.exit()

bgr = frame.get_bgr()
img_b, img_g, img_r = cv2.split(bgr)
cv2.imshow("img_b", img_b)
cv2.imshow("img_g", img_g)
cv2.imshow("img_r", img_r)
gray = cv2.cvtColor(bgr, cv2.COLOR_BGR2GRAY)
img1=gray.copy()
#img1 = gray[y1:y2, x1:x2]

#img = cv2.imread('messi5.jpg',0)
img2 = img1.copy()
template1 = cv2.imread('/opt/scorer/home/dev/SDK/roi/on.png',0)
template2 = cv2.imread('/opt/scorer/home/dev/SDK/roi/off.png',0)
w1, h1 = template1.shape[::-1]
w2, h2 = template2.shape[::-1]

# All the 6 methods for comparison in a list
#methods = ['cv2.TM_CCOEFF', 'cv2.TM_CCOEFF_NORMED', 'cv2.TM_CCORR',
#            'cv2.TM_CCORR_NORMED', 'cv2.TM_SQDIFF', 'cv2.TM_SQDIFF_NORMED']


method = eval('cv2.TM_CCORR_NORMED')

# Apply template Matching
res1 = cv2.matchTemplate(img1,template1,method)
res2 = cv2.matchTemplate(img2,template2,method)
min_val1, max_val1, min_loc1, max_loc1 = cv2.minMaxLoc(res1)
min_val2, max_val2, min_loc2, max_loc2 = cv2.minMaxLoc(res2)

# If the method is TM_SQDIFF or TM_SQDIFF_NORMED, take minimum
if method in [cv2.TM_SQDIFF, cv2.TM_SQDIFF_NORMED]:
    top_left1 = min_loc1
    top_left2 = min_loc2
else:
    top_left1 = max_loc1
    top_left2 = max_loc2
bottom_right1 = (top_left1[0] + w1, top_left1[1] + h1)
bottom_right2 = (top_left2[0] + w2, top_left2[1] + h2)

cv2.rectangle(img1,top_left1, bottom_right1, 255, 2)
cv2.rectangle(img2,top_left2, bottom_right2, 255, 2)
#cv2.imshow("res1.jpg",res1)
#cv2.imshow("result1.png",img1)
#cv2.imshow("template1.png",template1)
#cv2.imshow("res2.jpg",res2)
#cv2.imshow("result2.png",img2)
#cv2.imshow("template2.png",template2)
print(min_val1)
print(max_val1)
print(min_val2)
print(max_val2)
cv2.waitKey(10000)

