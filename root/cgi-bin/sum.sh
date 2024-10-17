#!/bin/bash

echo "Content-Type: text/html"
echo ""

if [ "$REQUEST_METHOD" == "GET" ]; then
  DATA=$QUERY_STRING
elif [ "$REQUEST_METHOD" == "POST" ]; then
  read -n "$CONTENT_LENGTH" DATA
fi

num1=""
num2=""

if [ -n "$DATA" ]; then
  DATA=$(echo "$DATA" | tr '&' '\n')
  for param in $DATA; do
    key=$(echo "$param" | cut -d '=' -f 1)
    value=$(echo "$param" | cut -d '=' -f 2 | tr '+' ' ')

    if [ "$key" == "num1" ]; then
      num1=$value
    elif [ "$key" == "num2" ]; then
      num2=$value
    fi
  done
fi

if ! [[ "$num1" =~ ^[0-9]+$ ]] || ! [[ "$num2" =~ ^[0-9]+$ ]]; then
  result="Error: num1 and num2 must be numbers!"
else
  sum=$(($num1 + $num2))
  result="Result: $num1 + $num2 = $sum"
fi

cat <<EOL
<!DOCTYPE html>
<html>
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Sum numbers</title>
    <link rel="icon" href="/favicon.ico" type="image/x-icon">
        <style>
      body {
        font-family: 'Arial', sans-serif;
        background-color: #1a1a1a;
        color: #e0e0e0;
        margin: 0;
        padding: 0;
      }
      header {
        background-color: #000000;
        border-bottom: 5px solid #ff0000;
        padding: 20px 0;
        text-align: center;
      }
      header h1 {
        color: #ff0000;
        font-size: 2.5em;
        font-weight: bold;
        text-transform: uppercase;
        letter-spacing: 5px;
        margin: 0;
        line-height: 1.5em;
      }
      nav ul {
        list-style: none;
        padding: 0;
        display: flex;
        justify-content: center;
        background-color: #ff0000;
        margin: 0;
        line-height: 2.5em;
      }
      nav ul li {
        position: relative;
        margin: 0 15px;
      }
      nav ul li a {
        color: #1a1a1a;
        text-decoration: none;
        font-size: 1.2em;
        text-transform: uppercase;
        font-weight: bold;
        letter-spacing: 2px;
      }
      nav ul li a:hover {
        color: #ffffff;
      }
      footer {
        margin-top: 40px;
        font-size: 14px;
        color: #666;
        text-align: center;
      }
      .footer-text {
        margin: 0;
      }
      .content {
        display: flex;
        justify-content: center;
        flex-wrap: wrap;
        padding: 50px 0;
      }
      .form-container {
        width: 300px;
        padding: 20px;
        background-color: #000;
        border: 5px solid #ff0000;
        border-radius: 10px;
        text-align: center;
      }
      .form-container input[type="number"],
      .form-container input[type="submit"] {
        width: calc(100% - 20px); /* Ajusta el ancho de ambos inputs y el botón */
        padding: 10px;
        margin-bottom: 15px;
        background-color: #1a1a1a;
        color: #e0e0e0;
        border: 2px solid #ff0000;
        border-radius: 5px;
      }
      .form-container input[type="number"]::-webkit-outer-spin-button,
      .form-container input[type="number"]::-webkit-inner-spin-button {
        -webkit-appearance: none; /* Oculta las flechas en Chrome, Safari, Edge */
        margin: 0;
      }
      .form-container input[type="number"] {
        -moz-appearance: textfield; /* Oculta las flechas en Firefox */
      }
      .result {
        margin-top: 20px;
        font-size: 1.5em;
        color: #ff0000;
      }
    </style>
  </head>
  <body>
    <header>
      <h1>- <span>42 - </span>WEBSERVER<span> - 42</span> -</h1>
    </header>
    <nav>
      <ul>
        <li><a href="../assets/staticindex.html">Home</a></li>
        <li><a href="../assets/forms.html">Forms</a></li>
        <li><a href="../assets/cookies.html">Cookies</a></li>
        <li><a href="../upload">Uploads</a></li>
        <li><a href="../assets/login.html">login</a></li>
        <li><a href="../private/privateindex.html">Private</a></li>
      </ul>
    </nav>
    <section class="content">
      <div class="form-container">
        <h2>SUM NUMBERS</h2>
        <form method="GET" action="../cgi-bin/sum.sh">
          <input type="number" name="num1" placeholder="Number 1" required>
          <input type="number" name="num2" placeholder="Number 2" required>
          <input type="submit" value="Calculate">
        </form>
        <div class="result">
          $result
        </div>
      </div>
    </section>
    <footer>
      <p class="footer-text">2024 Imoro-sa & Alfofern Webserv. All Rights Reserved.</p>
    </footer>
  </body>
</html>
EOL
