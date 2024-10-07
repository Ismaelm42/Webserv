#!/usr/bin/env python3

import cgi
import os

def parse_multipart_form():
    environ = os.environ.copy()
    environ['REQUEST_METHOD'] = 'POST'
    environ['CONTENT_TYPE'] = os.environ['CONTENT_TYPE']
    form = cgi.FieldStorage(fp=sys.stdin, environ=environ, keep_blank_values=True)

    fields = {}
    for field in form.keys():
        if form[field].filename:
            filename = form[field].filename
            file_content = form[field].file.read()
            fields[field] = {'filename': filename, 'content': file_content}
        else:
            fields[field] = form[field].value
    return fields

def save_file(filename, content):
    with open(filename, 'wb') as f:
        f.write(content)

if __name__ == "__main__":
    # Procesar el formulario y guardar el archivo
    fields = parse_multipart_form()
    for field_name, field_value in fields.items():
        if isinstance(field_value, dict) and 'filename' in field_value:
            save_file(field_value['filename'], field_value['content'])
    
    # Generar la cabecera de la respuesta HTTP
    print("Content-Type: text/html")
    print()

    # Generar el HTML con el mensaje de éxito
    print(f"""
    <!DOCTYPE html>
    <html lang="es">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Archivo Subido</title>
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
        </style>
    </head>
    <body>
        <header>
            <h1>¡Tu archivo se ha subido correctamente!</h1>
        </header>
    </body>
    </html>
    """)
