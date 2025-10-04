static const char SetupPage[] PROGMEM = R"==(
<!DOCTYPE html>
<html lang="en">
    <head>
        <title>ESP32 EMF Reader</title>
    </head>
    <body>
        <h1>WiFi Setup Page</h1>
        <form method="get" action="./save">
            <label for="ssid">WiFi name (SSID): </label>
            <input type="text" name="ssid" id="ssid">
            <br>
            <label for="password">WiFi Password: </label>
            <input type="password" name="password" id="password">
            <br>
            <input type="submit" value="Save">
        </form>
    </body>
</html>)==";