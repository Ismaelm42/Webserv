#!/usr/bin/env python3
import os
import cgi
import http.cookies
import time

# Simulación de autenticación
def authenticate(username, password):
    # Aquí validas el usuario y la contraseña con tu archivo .htpasswd o cualquier otro método
    if username == "Alfonso" and password == "1234":
        return True
    return False

def create_session_cookie():
    session_id = str(int(time.time()))  # Por ejemplo, un valor basado en el tiempo
    cookie = http.cookies.SimpleCookie()
    cookie["session"] = session_id
    cookie["session"]["path"] = "/"
    cookie["session"]["expires"] = time.strftime("%a, %d-%b-%Y %T GMT", time.gmtime(time.time() + 3600))
    return cookie

# Leer las variables del entorno
env = os.environ
username = env.get("REMOTE_USER", "")
password = env.get("REMOTE_PASS", "")

form = cgi.FieldStorage()
username = form.getvalue('username') 
password = form.getvalue('password')

if authenticate(username, password):
    # Crear la cookie de sesión
    cookie = create_session_cookie()
    
    # Enviar la cookie al cliente en las cabeceras HTTP
    print(cookie.output())  # Esto escribe las cabeceras de la cookie
    print("Content-Type: text/html")
    print()
    print("<html><body>Login successful!</body></html>")
else:
    print("Content-Type: text/html")
    print()
    print("<html><body>Invalid credentials.</body></html>")














# # Obtener los datos del formulario
# form = cgi.FieldStorage()
# nombre = form.getvalue('username') 
# password = form.getvalue('password')

# # Crear cookie de sesión
# cookie = http.cookies.SimpleCookie()
# cookie["SID"] = "unique_session_id"
# cookie["SID"]["path"] = "/"

# # Recoger variables de entorno
# username = os.getenv("REMOTE_USER", "Unknown")
# password = os.getenv("REMOTE_PASS", "Unknown")
# realm = os.getenv("REALM", "Unknown")
# auth_file = os.getenv("AUTH_BASIC_USER_FILE", "Unknown")


# # username = "Unknown"
# # password = "Unknown"
# # realm = "Unknown"
# # auth_file ="Unknown"

# # # Validar credenciales (en este caso hardcoded para simplicidad)
# # if username == "user" and password == "pass":
# # 	# Crear cookie de sesión
# # 	cookie = http.cookies.SimpleCookie()
# # 	cookie["SID"] = "unique_session_id"
# # 	cookie["SID"]["path"] = "/"

# # 	# Generar respuesta con la cookie
# # 	print("Content-Type: text/html")
# # 	print(cookie)
# # 	print()  # Fin de los encabezados
# # 	print("<html><body>Login Successful!</body></html>")
# # else:
# # 	print("Content-Type: text/html")
# # 	print()
# # 	print("<html><body>Login Failed!</body></html>")

# print("Content-Type: text/html")	# No sé si funciona o si es lo que bloquea el get
# print(cookie)
# print()
# print(f"""
# <!DOCTYPE html>
# <html lang="es">
# <head>
#     <meta charset="UTF-8">
#     <meta name="viewport" content="width=device-width, initial-scale=1.0">
#     <title>Formulario Recibido</title>
#     <style>
#     * {{
#         box-sizing: border-box;
#         margin: 0;
#         padding: 0;
#     }}
#     body {{
#         background-color: #111;  /* Fondo negro */
#         color: #ff0000;  /* Texto rojo */
#         font-family: 'Arial Black', sans-serif;
#         text-align: center;
#         margin: 0;
#         padding: 0;
#         min-height: 100vh;
#         display: flex;
#         flex-direction: column;
#         justify-content: center;
#         align-items: center;
#         overflow-x: hidden;  /* Evitar el scroll horizontal */
#     }}
#     h1 {{
#         font-size: 3rem;
#         text-transform: uppercase;
#         letter-spacing: 0.1rem;
#         margin: 0;
#     }}
#     p {{
#         font-size: 1.5rem;
#         margin-top: 20px;
#         color: #fff;
#     }}
#     strong {{
#         color: #ff0000;  /* Resaltar el email en rojo */
#     }}
#     header {{
# 		position: fixed;
#         top: 0;
#         background: #222;
#         padding: 4em;
#         width: 100%;
#         border-bottom: 2px solid #ff0000;
# 		margin-bottom: 2em;
#     }}
# 	main{{

#         padding: 2em 0;

# 	}}
#     footer {{
#         position: fixed;
#         bottom: 0;
#         width: 100%;
#         background: #222;
#         padding: 3em 0;
#         border-top: 2px solid #ff0000;
#         color: #fff;
#         font-size: 0.9rem;
#     }}
#     .verbenas {{
#         margin-top: 30px;
#         font-size: 1.2rem;
#         text-shadow: 2px 2px 4px #ff0000;
#     }}
#     .icon-metal {{
#         margin-top: 20px;
#         font-size: 3rem;
#     }}
# 	</style>
# </head>
# <body>
#     <header>
#         <h1>Name: {nombre}</h1>
#     </header>
#     <main>
#         <p>Password:  <strong>{password}</strong></p>
#         <div class="icon-metal">🤘</div>
# 	    <h1>Authentication Information</h1>
# 	    <p><strong>Username:</strong> {username}</p>
# 	    <p><strong>Password:</strong> {password}</p>
# 	    <p><strong>Realm:</strong> {realm}</p>
# 	    <p><strong>Auth Basic User File:</strong> {auth_file}</p>
# 	</main>
#     <footer>
#         <p>Campillos, Málaga - ¡Donde las verbenas y el metal se encuentran!</p>
#     </footer>
# </body>
# </html>
# """)
