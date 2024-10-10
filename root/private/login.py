#!/usr/bin/env python3
import os
import cgi
import http.cookies
import time
import tempfile
import traceback 

# Leer las variables del entorno será util si se vuelven a pasar las cookies
env = os.environ
username = env.get("REMOTE_USER", "")
password = env.get("REMOTE_PASS", "")

form = cgi.FieldStorage()
username = form.getvalue('username') 
password = form.getvalue('password')

realm = os.getenv("REALM", "Unknown")
auth_file = os.getenv("AUTH_BASIC_USER_FILE", "Unknown")
session_dir = "./root/tmp_sessions"

def authenticate(username, password, auth_file):
    try:
        with open(auth_file, 'r') as file:
            for line in file:
                # Separar los campos usando ';'
                file_username, file_password, email, creation_date, validity_date = line.strip().split(';')

                # Verificar si el username y password coinciden
                if file_username == username and file_password == password:
                    return True  # Autenticación exitosa
    except FileNotFoundError:
        print("Error: No se pudo abrir el archivo de autenticación.")
    except ValueError:
        print("Error: Datos incorrectos en el archivo de autenticación.")
    
    return False  # Fallo de autenticación

session_id = str(int(time.time()))  # Por ejemplo, un valor basado en el tiempo


def create_session_cookie():
    cookie = http.cookies.SimpleCookie()
    cookie["session"] = session_id
    cookie["session"]["path"] = "/"
    cookie["session"]["expires"] = time.strftime("%a, %d-%b-%Y %T GMT", time.gmtime(time.time() + 3600))
    return cookie


# Función para guardar la sesión en un archivo
def save_session(username, session_id, session_dir):
    try:
        # Verificar si el directorio existe, si no, crearlo
        if not os.path.exists(session_dir):
            os.makedirs(session_dir, exist_ok=True)  # Crear el directorio de forma segura

        session_file = os.path.join(session_dir, f"{session_id}.session")

        # Abrir el archivo de sesión para escribir los datos
        with open(session_file, 'w') as file:
            file.write(f"username: {username}\n")
            file.write(f"login_time: {time.strftime('%Y-%m-%d %H:%M:%S', time.gmtime())}\n")

    except OSError as e:
        # Error relacionado con permisos o acceso al sistema de archivos
        print("Content-Type: text/html\n")
        print(f"<html><body>Error al guardar la sesión: {e}</body></html>")
        traceback.print_exc()  # Mostrar el rastro de la excepción para más detalles
        return False

    except Exception as e:
        # Cualquier otro error no previstou
        print("Content-Type: text/html\n")
        print(f"<html><body>Error inesperado: {e}</body></html>")
        traceback.print_exc()  # Mostrar el rastro de la excepción para más detalles
        return False

    return True  # Sesión guardada con éxito

# htpasswd_file = "./root/.htpasswd"
# record = "hola mundo\n"

# with open(auth_file, "a") as f:
#     f.write(record)




if authenticate(username, password, auth_file):
    # Crear la cookie de sesión
    cookie = create_session_cookie()
    save_session(username, session_id, session_dir)
    # Enviar la cookie al cliente en las cabeceras HTTP
    print(cookie.output())  # Esto escribe las cabeceras de la cookie
    print("Content-Type: text/html")
    print()
    print(f"""
		<!DOCTYPE html>
		<html lang="es">
        <head>
	    <meta charset="UTF-8">
	    <meta name="viewport" content="width=device-width, initial-scale=1.0">
	    <title>Formulario Recibido</title>
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
		    <p><strong>Welcome to:</strong> {realm}</p>
			<p class="verbenas"><a href="http://localhost:8080/private/privateindex.html">let's have a walk on the private side!</a></p>
		</main>
		</body>
	</html>
	""")
else:
    print("Content-Type: text/html")
    print()
    print(f"""
		<!DOCTYPE html>
		<html lang="es">
        <head>
	    <meta charset="UTF-8">
	    <meta name="viewport" content="width=device-width, initial-scale=1.0">
	    <title>Formulario Recibido</title>
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
	    h1 {{# htpasswd_file = "./root/.htpasswd"
# record = "hola mundo\n"

# with open(auth_file, "a") as f:
#     f.write(record)



	        font-size: 3rem;
	        letter-spacing: 0.1rem;
	        margin: 0;
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
			<p><strong>Welcome to:</strong> {realm}</p>
			<p>Wrong password or user</p>
			<p class="verbenas">If you are not registered, please <a href="http://localhost:8080/assets/registration.html">register</a></p>
		</main>
		</body>
		</html>
		""")

