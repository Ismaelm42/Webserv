#pragma once

#define err_first_sec_title	"<!DOCTYPE html>\n" \
"<html lang=\"es\">\n" \
"<head>\n" \
"    <meta charset=\"UTF-8\">\n" \
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n" \
"    <title>"
#define err_sec_sec_error "</title>\n" \
"    <style>\n" \
"        /* Estilos Generales */\n" \
"        body {\n" \
"            margin: 0;\n" \
"            padding: 0;\n" \
"            font-family: 'Arial', sans-serif;\n" \
"            background: #000;\n" \
"            color: #fff;\n" \
"            text-align: center;\n" \
"            overflow: hidden; /* Evitar barras de desplazamiento */\n" \
"        }\n\n" \
"        .container {\n" \
"            max-width: 800px;\n" \
"            margin: 0 auto;\n" \
"            padding: 20px;\n" \
"            position: relative;\n" \
"        }\n\n" \
"        header {\n" \
"            margin-bottom: 40px;\n" \
"            position: relative;\n" \
"            z-index: 1;\n" \
"        }\n\n" \
"        .error-code {\n" \
"            font-size: 150px;\n" \
"            font-weight: bold;\n" \
"            color: #FF0000; /* Rojo brillante para resaltar el error */\n" \
"            text-shadow: 7px 7px 15px rgba(255, 0, 0, 0.9); /* Sombra de texto dramática */\n" \
"            margin: 0;\n" \
"        }\n\n" \
"        .error-message {\n" \
"            font-size: 50px;\n" \
"            font-weight: bold;\n" \
"            color: #FFF;\n" \
"            text-shadow: 4px 4px 10px rgba(255, 255, 255, 0.7);\n" \
"            margin: 0;\n" \
"        }\n\n" \
"        main {\n" \
"            font-size: 22px;\n" \
"        }\n\n" \
"        .description {\n" \
"            margin: 20px 0;\n" \
"            color: #ddd; /* Gris más claro para la descripción */\n" \
"        }\n\n" \
"        .methods-info {\n" \
"            margin: 20px 0;\n" \
"            color: #FFCC00; /* Amarillo para destacar el mensaje */\n" \
"            font-size: 18px;\n" \
"            font-weight: bold;\n" \
"        }\n\n" \
"        .home-link {\n" \
"            display: inline-block;\n" \
"            margin-top: 20px;\n" \
"            padding: 12px 24px;\n" \
"            font-size: 20px;\n" \
"            color: #000;\n" \
"            background-color: #FF0000;\n" \
"            border: 2px solid #FFF;\n" \
"            text-decoration: none;\n" \
"            text-transform: uppercase;\n" \
"            font-weight: bold;\n" \
"            border-radius: 5px;\n" \
"            transition: background-color 0.3s, color 0.3s;\n" \
"        }\n\n" \
"        .home-link:hover {\n" \
"            background-color: #FFF;\n" \
"            color: #FF0000;\n" \
"        }\n\n" \
"        footer {\n" \
"            margin-top: 40px;\n" \
"            font-size: 14px;\n" \
"            color: #666; /* Gris oscuro para el pie de página */\n" \
"        }\n\n" \
"        .footer-text {\n" \
"            margin: 0;\n" \
"        }\n\n" \
"        /* Animaciones y efectos especiales */\n" \
"        @keyframes glitch {\n" \
"            0% { transform: translate(0, 0); }\n" \
"            25% { transform: translate(-5px, 5px); }\n" \
"            50% { transform: translate(5px, -5px); }\n" \
"            75% { transform: translate(-5px, -5px); }\n" \
"            100% { transform: translate(0, 0); }\n" \
"        }\n\n" \
"        .error-message {\n" \
"            position: relative;\n" \
"            display: inline-block;\n" \
"            animation: glitch 1s infinite;\n" \
"        }\n" \
"    </style>\n" \
"</head>\n" \
"<body>\n" \
"    <div class=\"container\">\n" \
"        <header>\n" \
"            <h1 class=\"error-code\">"
#define err_th_sec_errmsg "</h1>\n" \
"			<h2 class=\"error-message\">"
#define err_fth_sec_errdesc	"</h2>\n" \
"		</header>\n" \
"		<main>\n" \
"			<p class=\"description\">"
#define err_last_sec_end "</p>\n" \
"           	<a href=\"/\" class=\"home-link\">Go to Homepage</a>\n" \
"        	</main>\n" \
"        	<footer>\n" \
"           	<p class=\"footer-text\">2024 Imoro-sa & Alfofern Webserv. All Rights Reserved.</p>\n" \
"       	</footer>\n" \
"    	</div>\n" \
"	</body>\n" \
"</html>"
