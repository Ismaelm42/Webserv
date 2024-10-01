#!/usr/bin/python3

import cgi, os

form = cgi.FieldStorage()

# Obtener el archivo subido
fileitem = form['filename']

#nombreArchivo = form.getvalue('filename') para el campo filename no vale esta llamada
nombreArchivo = form.getvalue('trutru')  # .getvalue() es más seguro para obtener valores de formularios

# Obtener el nombre del campo de texto
nombre = form.getvalue('nombre')  # .getvalue() es más seguro para obtener valores de formularios

# Verificar si se ha subido un archivo
if fileitem.filename:
    open(os.getcwd() + '/root/upload/' + os.path.basename(fileitem.filename), 'wb').write(fileitem.file.read())
    message = 'El archivo "' + nombreArchivo + '" ha sido subido a la carpeta upload'
    icono = "👌"  # Icono OK
else:
    message = '¡La subida ha fallado!'
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
        <h1>Gracias por su solicitud, {nombre} .</h1>
    </header>
    <main>
        <p><h2>{message}</h2></p>
        <div class="icon-cookie">{icono}</div>
    </main>
</body>
</html>
""")


# #!/usr/bin/python3

# import cgi, os

# form = cgi.FieldStorage()

# # Obtener el archivo subido
# fileitem = form['filename']

# # Obtener el nombre del campo de texto
# nombre = form.getvalue('nombre')  # .getvalue() es más seguro para obtener valores de formularios

# # Verificar si se ha subido un archivo
# if fileitem.filename:
#    open(os.getcwd() + '/root/upload/' + os.path.basename(fileitem.filename), 'wb').write(fileitem.file.read())
#    message = 'El archivo "' + os.path.basename(fileitem.filename) + '" ha sido subido a la carpeta upload'
# else:
#    message = '¡a subida a fallado!'

# # Imprimir cabeceras HTTP y contenido HTML
# print("Content-Type: text/html;charset=utf-8")
# print("Content-type:text/html\r\n")
#         print(f"""
#         <!DOCTYPE html>
#         <html lang="es">
#         <head>
#             <meta charset="UTF-8">
#             <meta name="viewport" content="width=device-width, initial-scale=1.0">
#             <title>Formulario Recibido</title>
#             <style>
#                 body {{
#                     background-color: #111;  /* Fondo negro */
#                     color: #ff0000;  /* Texto rojo */
#                     font-family: 'Arial Black', sans-serif;
#                     text-align: center;
#                     margin: 0;
#                     padding: 0;
#                     min-height: 100vh;
#                     display: flex;
#                     flex-direction: column;
#                     justify-content: center;
#                     align-items: center;
#                 }}
#                 h1 {{
#                     font-size: 3rem;
#                     text-transform: uppercase;
#                     letter-spacing: 0.1rem;
#                 }}
#                 p {{
#                     font-size: 1.5rem;
#                     margin-top: 20px;
#                     color: #fff;
#                 }}
#                 strong {{
#                     color: #ff0000;  /* Resaltar el email en rojo */
#                 }}
#                 header {{
#                     background: #222;
#                     padding: 20px;
#                     width: 100%;
#                     border-bottom: 2px solid #ff0000;
#                 }}
#                 footer {{
#                     position: fixed;
#                     bottom: 0;
#                     width: 100%;
#                     background: #222;
#                     padding: 10px 0;
#                     border-top: 2px solid #ff0000;
#                     color: #fff;
#                     font-size: 0.9rem;
#                 }}
#                 .verbenas {{
#                     margin-top: 30px;
#                     font-size: 1.2rem;
#                     text-shadow: 2px 2px 4px #ff0000;
#                 }}
#                 .icon-cookie {{
#                     margin-top: 20px;
#                     font-size: 3rem;
#                 }}
#             </style>
#         </head>
#         <body>
#             <header>
# 			<H1>Gracias por su solicitud, {nombre}.</H1>")
#             <main>
#                 <p><H2>{message}</H2></p>
#                 <div class="icon-cookie">🍪</div>
#             </main>
#         </body>
#         </html>
#         """)

# # #!/usr/bin/python3

# # import cgi, os

# # form = cgi.FieldStorage()

# # # Get filename here
# # fileitem = form['filename']


# # # Test if the file was uploaded
# # if fileitem.filename:
# #    open(os.getcwd() + '/root/upload/' + os.path.basename(fileitem.filename), 'wb').write(fileitem.file.read())
# #    message = 'The file "' + os.path.basename(fileitem.filename) + '" has been uploaded to: upload/'
# # else:
# #    message = 'Uploading Failed'

# # print("Content-Type: text/html;charset=utf-8")
# # print ("Content-type:text/html\r\n")
# # print("<H2> " + message + " </H2>")

