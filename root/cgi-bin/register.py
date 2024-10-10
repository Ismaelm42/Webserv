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

print(f"""
		<!DOCTYPE html>
		<html lang="es">
        <head>
	    <meta charset="UTF-8">
	    <meta name="viewport" content="width=device-width, initial-scale=1.0">
	    <title>Register success<</title>
	    <style>
	    * {{
	        box-sizing: border-box;
	        margin: 0;
	        padding: 0;
	    }}
	    body {{
	        background-color: #111;  /* Fondo negro */
	        color: #ff0000;  /* Texto rojo */
	        font-family: 'Arial Black', sans-serif;
	        text-align: center;
	        margin: 0;
	        padding: 0;
	        min-height: 100vh;
	        display: flex;
	        flex-direction: column;
	        justify-content: center;
	        align-items: center;
	        overflow-x: hidden;  /* Evitar el scroll horizontal */
	    }}
	    h1 {{
	        font-size: 3rem;
	        letter-spacing: 0.1rem;
	        margin: 0;
	    }}
		h2 {{
	        font-size: 2rem;
	        letter-spacing: 0.1rem;
	        margin: 0;
			color:#fff;
	    }}
	    p {{
	        font-size: 1.5rem;
	        margin-top: 20px;
	        color: #fff;
	    }}
	    strong {{
	        color: #ff0000;  /* Resaltar el email en rojo */
	    }}
	    header {{
			position: fixed;
	        top: 0;
	        background: #222;
	        padding: 4em;
	        width: 100%;
	        border-bottom: 2px solid #ff0000;
			margin-bottom: 2em;
	    }}
		main{{
	        padding: 2em 0;
		}}
	    footer {{
	        position: fixed;
	        bottom: 0;
	        width: 100%;
	        background: #222;
	        padding: 3em 0;
	        border-top: 2px solid #ff0000;
	        color: #fff;
	        font-size: 0.9rem;
	    }}
	    .verbenas {{
	        margin-top: 30px;
	        font-size: 1.2rem;
			font-weight: bold;
	    }}
		.verbenas a {{
		    color: red;  /* Cambia el color del texto del enlace a rojo */
		    text-decoration: underline;
		    text-decoration-color: yellow;  /* Cambia el color de la línea de subrayado a amarillo */
		}}
		</style>
		</head>
		<h1>Name: {username}</h1>
	    </header>
    	<body>
		<main>
		    <h2>Registro almacenado con éxito!</h2>
			<p class="verbenas"><a href="http://localhost:8080/private/privateindex.html">let's have a walk on the private side!</a></p>
		</main>
		</body>
	</html>
	""")

# print("<h2>Registro almacenado con éxito!</h2>")
# print(f"<p>Usuario: {username}</p>")
# print(f"<p>Email: {email}</p>")
# print(f"<p>Fecha de creación: {creation_date}</p>")
# print(f"<p>Record: {record}</p>")
# print("</body></html>")

