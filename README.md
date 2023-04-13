# RFID-DjangoServer

## How to run this code

What to prepare:
1. ESP8266 (2)
2. RFID RFID-RC522 (2)

What to do (server):
1. Download this repo
2. Create virtual python environment
3. Install libraries in requirement.txt
4. Run Django server at 0.0.0.0:8000
5. Open localhost:8000/admin

What to do (ESP8266):
1. Search Server IP address
2. Change this line in ESP8266 code: bool res = http.begin(client, "http://<your server IP>:8000/rfid-data/");  // HTTP (USE DEVICE IP, don't use localhost)
3. Change this line too: 

(#define STASSID "<Your wifi ID>")
(#define STAPSK "<Your wifi password>")

4. Change in man2/settings.py: ALLOWED_HOSTS = ["<your server IP>", "localhost"]
5. Change this line in ESP8266 code too: int httpCode = http.POST("{\"value\":\"" + phrase_1 + " " + phrase_2 + " " + phrase_3 + "\", " + "\"reader_id\":\"" + "<your custom reader ID>" + "\"}"); 

reader ID different for each ESP8266 (create your own reader ID)
6. Upload the code 
