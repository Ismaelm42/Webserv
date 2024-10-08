#!/usr/bin/env python3

import cgi
import cgitb
import datetime

# Habilitar la visualización de errores en el navegador
cgitb.enable()

# Configurar el entorno de respuesta CGI
print("Content-Type: text/html")
print()
# Obtener los datos enviados desde el formulario
form = cgi.FieldStorage()

# Extraer los valores de los campos del formulario
username = form.getvalue('username')
password = form.getvalue('password')
email = form.getvalue('email')

# Definir la fecha de creación y validez	
creation_date = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
validity_date = (datetime.datetime.now() + datetime.timedelta(days=30)).strftime("%Y-%m-%d %H:%M:%S")

# Formatear los datos
record = f"{username};{password};{email};{creation_date};{validity_date}\n"

# Ruta del archivo .htpasswd (ajusta la ruta según tu estructura de directorios)
# htpasswd_file = "./.htpasswd" # lo escribe en webservers
htpasswd_file = "./root/.htpasswd"

# Abrir el archivo en modo append para agregar el registro
with open(htpasswd_file, "a") as f:
    f.write(record)

# Mensaje de éxito para el usuario

print("<!DOCTYPE html>")
print("<html lang='en'>")
print("<head>")
print("<meta charset='UTF-8'>")
print("<meta name='viewport' content='width=device-width, initial-scale=1.0'>")
print("<title>Register success</title>")
print("</head>")
print("<body>")
print("<h2>Registro almacenado con éxito!</h2>")
print(f"<p>Usuario: {username}</p>")
print(f"<p>Email: {email}</p>")
print(f"<p>Fecha de creación: {creation_date}</p>")
print(f"<p>Record: {record}</p>")
print("</body></html>")
