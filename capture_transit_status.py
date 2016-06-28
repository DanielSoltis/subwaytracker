
from flask import Flask
import urllib2
import xml.etree.ElementTree as ET
import json

app = Flask(__name__)

@app.route('/nycstatus.html')
def status():
    currentStatus = subwayStatus()
    message = ""
    for k,v in currentStatus.iteritems():
       message += k + "," + v + "\n"
    return message

def subwayStatus():
    currentStatus = {}
    file = urllib2.urlopen('http://web.mta.info/status/serviceStatus.txt')
    data = file.read()
    file.close()
    root = ET.fromstring(data)

    for child in root[2]:
        currentStatus[child[0].text] = child[1].text

    return currentStatus

@app.route('/lonstatus.html')
def londonstatus():
    app_id = 'YOURID' # Register for an app id and key at https://api.tfl.gov.uk/
    app_key = 'YOURKEY'
    file = urllib2.urlopen('https://api.tfl.gov.uk/Line/Mode/tube,overground,dlr/Status?detail=False&app_id=' + app_id + '&app_key=' + app_key)
    data = file.read()
    file.close()
    parsedData = json.loads(data)
    message = ""
    for line in parsedData:
        message += line['id'] + ',' + line['lineStatuses'][0]['statusSeverityDescription'] + '\n'
    return message

if __name__ == "__main__":
    app.run()
