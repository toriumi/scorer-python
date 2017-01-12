# -*- coding: utf-8 -*-
import zmq
import cv2
import sys
from scorer import Scorer
import numpy as np

# FourCC指定
#fourcc = "DIB "
#fourcc = "XVID"

# 変換メソッド指定（？）
#warp_type = cv2.MOTION_AFFINE
#warp_type = cv2.MOTION_EUCLIDEAN
#warp_type = cv2.MOTION_TRANSLATION
warp_type = cv2.MOTION_HOMOGRAPHY

# 出力画像を縦横1/2サイズ（面積1/4）に設定
scale = 2

# WarpMatrixと変換関数を設定
if warp_type == cv2.MOTION_HOMOGRAPHY:
    warp = np.eye(3,3,dtype=np.float32)
    warpTransform = cv2.warpPerspective
else:
    warp = np.eye(2,3,dtype=np.float32)
    warpTransform = cv2.warpAffine

scorer=Scorer("Task")
# 入力動画
cap = cv2.VideoCapture(r'./test.mp4')

# キャプチャの情報取得し出力情報を作成
#fps = cap.get(cv2.CAP_PROP_FPS)
fps = 15
#size = cap.get(cv2.CAP_PROP_FRAME_WIDTH), cap.get(cv2.CAP_PROP_FRAME_HEIGHT)
size = 640, 480
size = tuple(map(lambda x: int(x / scale), size))

# 基準となる画像を取得
#ret, base = cap.read()
#base = cv2.resize(base, size)
#base = cv2.cvtColor(base, cv2.COLOR_BGR2GRAY)

scorer.poll()
rowframe = scorer.get_frame()
base = rowframe.get_gray()

# 出力先
#video1 = cv2.VideoWriter("./out1.avi", cv2.VideoWriter_fourcc(*fourcc), fps, size)

while True:
    """
    # フレーム取得
    ret, frame = cap.read()
    if not ret:
        break
    frame = cv2.resize(frame, size)
    tmp = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    """
    if scorer.poll() == False:
        continue

    rowframe = scorer.get_frame()
    if rowframe == None:
        continue
    frame = rowframe.get_bgr()
    frame = cv2.resize(frame, size)
    tmp = rowframe.get_gray()
    cv2.findTransformECC(tmp, base, warp, warp_type)

    # 出力は元画像（カラー画像）
    out = warpTransform(frame, warp, size)

    # 表示かつAVIに出力
    scorer.web_show(out,1)
    scorer.web_show(frame,2)
    #video1.write(out)

    k = cv2.waitKey(1)
    if k in [27, ord('q')]:
        # ESCかQキー押下で終了
        break

# 解放
cap.release()
video1.release()
cv2.destroyAllWindows()