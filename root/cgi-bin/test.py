#!/usr/bin/python3

import cgi
import os

form = cgi.FieldStorage()

# Obtener el archivo subido
fileitem = form['filename']

# Obtener el nombre del campo de texto
nombre = form.getvalue('nombre')

# Verificar si se ha subido un archivo
if fileitem.filename:
    # Obtener el nombre base y reemplazar espacios por guiones bajos
    base_filename = os.path.basename(fileitem.filename)
    safe_filename = base_filename.replace(' ', '_')  # Reemplazar espacios

    # Guardar el archivo
    upload_path = os.path.join(os.getcwd(), 'root', 'upload', safe_filename)
    with open(upload_path, 'wb') as f:
        f.write(fileitem.file.read())

    message = f'The file "<a href=\"../upload/{safe_filename}\">{safe_filename}</a>" has been uploaded to the folder <a href="../upload">upload</a>.'
    icono = "👌"  # Icono OK
else:
    message = 'File upload failed!'
    icono = "👎"  # Icono pulgar hacia abajo
# Imprimir cabeceras HTTP y contenido HTML
print("Content-Type: text/html;charset=utf-8")
print("Content-type:text/html\r\n")
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
        <h1>Thanks for your request, {nombre} .</h1>
    </header>
    <main>
        <p><h2>{message}</h2></p>
        <div class="icon-cookie">{icono}</div>
    </main>
</body>
</html>
""")
