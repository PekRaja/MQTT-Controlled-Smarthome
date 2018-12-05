import paho.mqtt.client as mqtt
import MySQLdb
db = MySQLdb.connect(host="localhost", user="Pi", passwd="Ytrewq123", db="Smarthome")
cal = db.cursor()

# Don't forget to change the variables for the MQTT broker!
mqtt_username = "raspi"
mqtt_password = "Ytrewq"
mqtt_topic = "info"
mqtt_topic2 = "dht"

mqtt_broker_ip = "172.20.248.132"

client = mqtt.Client(client_id="raspi_sub")
# Set the username and password for the MQTT client
client.username_pw_set(mqtt_username, mqtt_password)

# These functions handle what happens when the MQTT client connects
# to the broker, and what happens then the topic receives a message
def on_connect(client, userdata, flags, rc):
    # rc is the error code returned when connecting to the broker
    print ("Connected! INFO:\n Client: {}\n Userdata:{}\n Error code(RC):{}\n".format(client, userdata, rc))
    
    # Once the client has connected to the broker, subscribe to the topic
    client.subscribe(mqtt_topic)
    print("Subscribed to {}".format(mqtt_topic))
    client.subscribe(mqtt_topic2)
    print("Subscribed to {}".format(mqtt_topic2))
    
def on_message(client, userdata, msg):
    # This function is called everytime the topic is published to.
    # If you want to check each message, and do something depending on
    # the content, the code to do this should be run in this function
    
    print ("Topic: {}\nMessage: {}".format(msg.topic, msg.payload))
    a = msg.payload
    t = msg.topic
    b = ["Light On","Light Off","Fan On","Fan Off","tripped","23.00"]
    # Lights on/off
    if a.find(b[0]) != -1:
        cal.callproc('Lightson')
        db.commit()
        
    if a.find(b[1]) != -1:
        cal.callproc('Lightsoff')
        db.commit()
        
    # Fan On/Off
    if a.find(b[2]) != -1:
        cal.callproc('Fanon')
        db.commit()
        
    if a.find(b[3]) != -1:
        cal.callproc('Fanoff')
        db.commit()
        
    # Alarm tripped
    if a.find(b[4]) != -1:
        cal.callproc('Alarm')
        db.commit()
    
    # Database entry for temperature change
    if  t == "dht":
        cal.callproc('Tempe', [a,])
        db.commit()
        if a > b[5]:
            print ("fan on")
            client.publish("fan","1")
    # The message itself is stored in the msg variable
    # and details about who sent it are stored in userdata

# Here, we are telling the client which functions are to be run
# on connecting, and on receiving a message
client.on_connect = on_connect
client.on_message = on_message

# Once everything has been set up, we can (finally) connect to the broker
# 1883 is the listener port that the MQTT broker is using
print("Attempting connection to {}...".format(mqtt_broker_ip))
client.connect(mqtt_broker_ip, 1883)

# Once we have told the client to connect, let the client object run itself
client.loop_forever()
client.disconnect()
