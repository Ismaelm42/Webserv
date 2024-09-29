#!/usr/bin/env python3

import cgi
# import os

# Obtener los datos del formulario
form = cgi.FieldStorage()
key = form.getvalue('key', 'default_cookie')  # Valor predeterminado si no se proporciona
value = form.getvalue('value', 'default_value')

# Generar la cabecera de la respuesta HTTP
# print("HTTP/1.1 200 OK") # DE emomento no sé si bloque o si podemos hacerlo aquí 
print("Content-Type: text/html")	# No sé si funciona o si es lo que bloquea el get
print(f"Set-Cookie: {key}={value}; SameSite=Strict")	# Set-Cookie: key=value; SameSite=Strict
print()
print(f"""
<!DOCTYPE html>
<html lang="es">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Formulario Recibido</title>
	<style>
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
		}}
		h1 {{
			font-size: 3rem;
			text-transform: uppercase;
			letter-spacing: 0.1rem;
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
			background: #222;
			padding: 20px;
			width: 100%;
			border-bottom: 2px solid #ff0000;
		}}
		footer {{
			position: fixed;
			bottom: 0;
			width: 100%;
			background: #222;
			padding: 10px 0;
			border-top: 2px solid #ff0000;
			color: #fff;
			font-size: 0.9rem;
		}}
		.verbenas {{
			margin-top: 30px;
			font-size: 1.2rem;
			text-shadow: 2px 2px 4px #ff0000;
		}}
		.icon-cookie {{
			margin-top: 20px;
			font-size: 3rem;
		}}
	</style>
</head>
<body>
	<header>
		<h1>¡Se ha guardado la cookie: {key}!</h1>
	</header>
	<main>
		<p>Guardamos el valor: {value} para futuras consultas.</p>
		<div class="icon-cookie">🍪</div>
	</main>

</body>
</html>
""")