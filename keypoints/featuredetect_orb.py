import cv2

# 画像ファイルの読み込み
img = cv2.imread('car.jpg')

# ORB (Oriented FAST and Rotated BRIEF)
detector_orb = cv2.ORB_create()
detector_akaze = cv2.AKAZE_create()
# AgastFeatureDetector
#detector_orb = cv2.AgastFeatureDetector_create()
# FAST
#detector_orb = cv2.FastFeatureDetector_create()
# MSER
#detector_orb = cv2.MSER_create()
# BRISK
#detector_orb = cv2.BRISK_create()
# KAZE
#detector_orb = cv2.KAZE_create()
# SimpleBlobDetector
#detector_orb = cv2.SimpleBlobDetector_create()


# 特徴検出
keypoints_orb = detector_orb.detect(img)
keypoints_akaze = detector_akaze.detect(img)
img_orb = img.copy()
img_akaze = img.copy()

# 画像への特徴点の書き込み
out_orb = cv2.drawKeypoints(img_orb, keypoints_orb, None)
out_akaze = cv2.drawKeypoints(img_akaze, keypoints_akaze, None)

# 表示
cv2.imwrite("keypoints_orb.png", out_orb)
cv2.imwrite("keypoints_akaze.png", out_akaze)