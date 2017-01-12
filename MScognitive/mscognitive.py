# Variables
import time
import requests
import wave
import pygame
import time
import zmq
import cv2
from scorer import Scorer


_url = 'https://api.projectoxford.ai/vision/v1.0/analyze/'
_key = '7733a2c4fe314dd590a1c1d8d9503eee' #Here you have to paste your primary key
_maxNumRetries = 10

#Note: The way to get api key:
#Free: https://www.microsoft.com/cognitive-services/en-us/subscriptions?productId=/products/Bing.Speech.Preview
#Paid: https://portal.azure.com/#create/Microsoft.CognitiveServices/apitype/Bing.Speech/pricingtier/S0
#apiKey = "Your api key goes here"
apiKey = "38cff4c6e50e41c482f612089b5ab8e0"

scorer=Scorer("Task")


def saveCameraImage(img_file_path):
    scorer.poll()
    frame = scorer.get_frame()
    bgr = frame.get_bgr()
    cv2.imwrite(img_file_path, bgr)


def processRequest(json, data, headers, params):
    retries = 0
    result = None

    while True:
        response = requests.request( 'POST', _url, json = json, data = data, headers = headers, params = params )
        if response.status_code == 429: 
            print( "Message: %s" % ( response.json()['error']['message'] ) )
            if retries <= _maxNumRetries: 
                time.sleep(1) 
                retries += 1
                continue
            else: 
                print( 'Error: failed after retrying!' )
                break
        elif response.status_code == 200 or response.status_code == 201:
            if 'content-length' in response.headers and int(response.headers['content-length']) == 0: 
                result = None 
            elif 'content-type' in response.headers and isinstance(response.headers['content-type'], str): 
                if 'application/json' in response.headers['content-type'].lower(): 
                    result = response.json() if response.content else None 
                elif 'image' in response.headers['content-type'].lower(): 
                    result = response.content
        else:
            print( "Error code: %d" % ( response.status_code ) )
            print( "Message: %s" % ( response.json()['error']['message'] ) )
        break
    return result['description']['captions'][0]['text']


def analyze_stored_image(file_path):
    with open(file_path, 'rb') as f:
        data = f.read()
    # Computer Vision parameters
    params = { 'visualFeatures' : 'Description'}
    headers = dict()
    headers['Ocp-Apim-Subscription-Key'] = _key
    headers['Content-Type'] = 'application/octet-stream'
    json = None
    return processRequest( json, data, headers, params )


def analyze_url_image(urlImage):
    # Computer Vision parameters
    params = { 'visualFeatures' : 'Description'}
    headers = dict()
    headers['Ocp-Apim-Subscription-Key'] = _key
    headers['Content-Type'] = 'application/json'
    json = {'url': urlImage}
    data = None
    return processRequest( json, data, headers, params )


def get_access_token():
    AccessTokenUri = "https://api.cognitive.microsoft.com/sts/v1.0/issueToken"
    headers = {"Ocp-Apim-Subscription-Key": apiKey}
    res = requests.request('POST', AccessTokenUri, headers=headers)
    return res.text

def request_audio_file(speechText, accessToken):
    data = "<speak version='1.0' xml:lang='en-us'><voice xml:lang='en-us' xml:gender='Female' name='Microsoft Server Speech Text to Speech Voice (en-US, ZiraRUS)'>%s</voice></speak>" % speechText
    headers = {"Content-type": "application/ssml+xml", 
            "X-Microsoft-OutputFormat": "riff-16khz-16bit-mono-pcm", 
            "Authorization": "Bearer " + accessToken, 
            "X-Search-AppId": "07D3234E49CE426DAA29772419F436CA", 
            "X-Search-ClientID": "1ECFAE91408841A480F00935DC390960", 
            "User-Agent": "TTSForPython"}
    speechUri = "https://speech.platform.bing.com/synthesize"
    res = requests.request("POST", speechUri, data=data, headers=headers)
    return res.content

def save_audio(data, file_path):
    f = open(file_path, 'wb')
    f.write(data)
    f.close()

def play_audio(file_path):
    audio_info = get_audio_info(file_path)
    pygame.mixer.init(frequency=audio_info['frame_rate'])
    pygame.mixer.music.load(file_path)
    pygame.mixer.music.play(1) # loop count
    time.sleep(audio_info['time'])   
    pygame.mixer.music.stop()  

def get_audio_info(file_path):
    wf = wave.open(file_path , "r" )
    info = {}
    info['frame_rate'] = wf.getframerate()
    info['frame_num'] =  wf.getframerate()
    info['time'] = float(wf.getnframes()) / wf.getframerate()
    return info


if __name__ == '__main__':

    img_file_path = "./image.jpg"
    wave_file_path = "./speech.wav"

    saveCameraImage(img_file_path)
    text = analyze_stored_image(img_file_path)
    print(text)

    accesstoken = get_access_token()
    wav_data = request_audio_file(text, accesstoken)
    save_audio(wav_data, wave_file_path)
    play_audio(wave_file_path)


#    result2 = analyzeURLImage('https://oxfordportal.blob.core.windows.net/vision/Analysis/3.jpg')
#    print(result2)