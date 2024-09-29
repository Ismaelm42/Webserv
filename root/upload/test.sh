#!/bin/bash
# Hacer que el script funcione como CGI
echo "Content-type: text/html"
echo ""

# Leer los datos enviados por el formulario
read FORM_DATA

# Extraer las variables del formulario
NOMBRE=$(echo "$FORM_DATA" | grep -oP '(?<=nombre=)[^&]*' | tr '+' ' ' | sed 's/%20/ /g')
CONTENIDO=$(echo "$FORM_DATA" | grep -oP '(?<=contenido=).*' | tr '+' ' ' | sed 's/%20/ /g')

# Decodificar caracteres URL si es necesario
NOMBRE=$(echo "$NOMBRE" | sed 's/%20/ /g')

# Definir el nombre base del archivo
ARCHIVO="${NOMBRE}.txt"

# Verificar si el archivo ya existe
CONTADOR=1
while [ -e "$ARCHIVO" ]; do
    # Si existe, agregar sufijo incremental
    ARCHIVO="${NOMBRE}($CONTADOR).txt"
    CONTADOR=$((CONTADOR+1))
done

# Guardar el contenido en el archivo
echo "$CONTENIDO" > "$ARCHIVO"

# Mostrar mensaje de éxito
echo "<html><body>"
echo "<h3>Archivo guardado exitosamente: $ARCHIVO</h3>"
echo "</body></html>"
