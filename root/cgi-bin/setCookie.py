#!/usr/bin/env python3

import cgi

form = cgi.FieldStorage()
key = form.getvalue('key', 'default_cookie')
value = form.getvalue('value', 'default_value')

print("Content-Type: text/html")
print(f"Set-Cookie: {key}={value}; SameSite=Strict")
print()
print(f"""
<!DOCTYPE html>
<html lang="es">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Form Received</title>
	<style>
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
			color: #ff0000;
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
		.webserv-party {{
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
		<h1>Cookie has been saved: {key}!</h1>
	</header>
	<main>
		<p>We save the value: {value} for future reference.</p>
		<div class="icon-cookie">🍪</div>
	</main>

</body>
</html>
""")
