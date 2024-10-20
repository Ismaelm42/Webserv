#!/usr/bin/env python3

print("Content-Type: text/html")    # Indica que el contenido es HTML
print()                             # Línea en blanco para separar los encabezados del contenido
print("<html>")
print("<head>")
print("<title>Welcome Page</title>")
print("<style>")
print("body { background-color: black; color: red; font-family: 'Courier New', Courier, monospace; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; }")
print(".container { text-align: center; }")
print("h1 { font-size: 6em; }")
print("h2 { font-size: 3em; }")
print("hr { color: yellow; margin-top :-60px; }")
print("</style>")
print("</head>")
print("<body>")
print("<div class='container'>")
print("<h1>Python Welcome Page</h1>")
print("<hr>")
print("<h2>Served by index.py</h2>")
print("</div>")
print("</body>")
print("</html>")