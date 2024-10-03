// web_pages.h
const char PAGE_SUCCESS[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Wi-Fi Setup Success</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
        }
        .container {
            background: white;
            padding: 30px;
            border-radius: 8px;
            box-shadow: 0 0 10px rgba(0,0,0,0.1);
            text-align: center;
        }
        h1 {
            font-size: 24px;
            color: #28a745;
            margin-bottom: 20px;
        }
        p {
            font-size: 18px;
            color: #333;
        }
        .icon {
            font-size: 50px;
            color: #28a745;
            margin-bottom: 20px;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="icon">✔️</div>
        <h1>Wi-Fi Setup Successful!</h1>
        <p>Your Wi-Fi has been configured successfully.</p>
    </div>
</body>
</html>
)rawliteral";

const char PAGE_INDEX[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Harja WiFi</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
        }
        .container {
            background: white;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 0 10px rgba(0,0,0,0.1);
            width: 300px;
            max-width: 100%;
        }
        h1 {
            margin-bottom: 20px;
            font-size: 24px;
            text-align: center;
        }
        .form-group {
            margin-bottom: 15px;
        }
        label {
            display: block;
            margin-bottom: 5px;
        }
        input {
            width: 100%;
            padding: 10px;
            border: 1px solid #ddd;
            border-radius: 4px;
            box-sizing: border-box;
        }
        button {
            width: 100%;
            padding: 10px;
            border: none;
            border-radius: 4px;
            background-color: #28a745;
            color: white;
            font-size: 16px;
            cursor: pointer;
        }
        button:hover {
            background-color: #218838;
        }
        .message {
            margin-top: 10px;
            font-size: 14px;
            color: red;
            text-align: center;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>LOGIN TOOL</h1>
        <form action="/save" method="POST">
        <div class="form-group">
            <label for="ssid">WiFi Name</label>
            <input type="text" name="ssid" required id="ssid" placeholder="Enter SSID">
        </div>
        <div class="form-group">
            <label for="password">Password</label>
            <input type="password" name="password" required id="password" placeholder="Enter Password">
        </div>
        <div class="form-group">
            <label for="token">Token</label>
            <input type="text" id="token" name="mqtt_token" required placeholder="Enter Token">
        </div>
        <button>Submit</button>
    </form>
    </div>
</body>
</html>
)rawliteral";