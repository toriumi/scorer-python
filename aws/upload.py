# aws rekognition detect-labels --image '{"S3Object":{"Bucket":"scorer-us-east-1","Name":"car.jpg"}}' --region=us-east-1
import boto3
strfile = "car.jpg"
bucket_name = 'scorer'
s3_client = boto3.client('s3')
s3_client.upload_file('/opt/scorer/home/dev/scorer-python/aws/' + strfile ,bucket_name, strfile)