#!/usr/bin/env python3

import cgi

form = cgi.FieldStorage()
name = form.getvalue('name', 'Visitor')
email = form.getvalue('email', 'No email provided')

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
        background-color: #111;
        color: #ff0000;
        font-family: 'Arial Black', sans-serif;
        text-align: center;
        margin: 0;
        padding: 0;
        min-height: 100vh;
        display: flex;
        flex-direction: column;
        justify-content: center;
        align-items: center;
        overflow-x: hidden;
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
        color: #ff0000;
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
