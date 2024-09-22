<?php
// Procesar un formulario enviado con método POST
$nombre = $_POST['nombre'];
$archivo = $_FILES['archivo']['name'];

echo "El nombre enviado es: " . htmlspecialchars($nombre);
echo "El archivo enviado es: " . htmlspecialchars($archivo);
?>
