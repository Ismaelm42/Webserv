#!/usr/bin/env python3

import cgi
# import os

# Obtener los datos del formulario
form = cgi.FieldStorage()
name = form.getvalue('name', 'Visitor')  # Valor predeterminado si no se proporciona
email = form.getvalue('email', 'No mail')

# Generar la cabecera de la respuesta HTTP
# print("HTTP/1.1 200 OK") # DE emomento no sé si bloque o si podemos hacerlo aquí 
print("Content-Type: text/html")	# No sé si funciona o si es lo que bloquea el get
print()
print(f"""
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Form Received</title>
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
        text-transform: uppercase;
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
    .webserv-party {{
        margin-top: 30px;
        font-size: 1.2rem;
        text-shadow: 2px 2px 4px #ff0000;
    }}
    .icon-party {{
        margin-top: 20px;
        font-size: 3rem;
    }}
	</style>
</head>
<body>
    <header>
        <h1>Thanks for your request, {name}!</h1>
    </header>
    <main>
        <p>We've saved your email <strong>{email}</strong>, so get ready to receive crucial notifications (and maybe an unexpected meme or two) from the 42 community.</p>
        <div class="webserv-party">
            <p>In the meantime, keep the code flowing, the bugs squashed, and the caffeine levels dangerously high!</p>
            <p>42 - Where coding magic blends with the madness of debugging. See you on the next push!</p>    </footer>
        </div>
        <div class="icon-party">💻🤘</div>
    </main>
    <footer>
        <p>Well, duh! This message was obviously whipped up by AI. What else did you expect?! 🤖</p>
</body>
</html>
""")
