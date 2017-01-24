
●minagawa_license.tar.gz
皆川さん作成の車番認識プログラム関連ファイル一式

展開すると下記のファイルがある。下記が皆川さんの説明。
-----ここから-----

Conf/
設定/学習済みファイル一式

sample/
Pythonコードとサンプルコード
- LPRecogLib.py
        C++のライブラリをラップ下Python用ライブラリ
- sample.py
        LPRecogLib.py用サンプルコード
- LPRecogLib3.py
        C++のライブラリをラップ下Python3用ライブラリ
- sample3.py
        LPRecogLib3.py用サンプルコード


src/
ソースコード一式
学習機能等なしで、最低限の認識機能を動かすために必要なファイル
・LPRecog.*
・CharRecognition.*
・Exception.*
・LicensePlateInfo.*
・PlateDetection.*
・PlateSegmentation.*
・common/*
・HMAX/*
・shape/shape_predictor.*
・shape/binary_func.*

上記をPython組込み用にDLL化するためのファイル
・LPRecogLib.*

DLLのビルドに必要な情報はCMakeList.txtに記述されているので
$ cmake .
$ make
$ sudo make install
とすればビルドされてインストールできます。


また、依存するOpenCVのライブラリは以下の通りです。
opencv_core310
opencv_imgproc310
opencv_calib3d310
opencv_objdetect310
------ここまで------