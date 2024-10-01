# Zootropo Antropocósmico

![Zootropo](ruta/a/la/imagen.jpg)

## Descripción

Este proyecto consiste en la creación de un zootropo interactivo de **1.2 metros** de diámetro, equipado con **12 figuras animadas** que cobran vida al girar. El zootropo cuenta con la colaboración de de tres escuelas del Tecnológico de Monterrey campus Puebla, EAAD (Escuela de Arquitectura, Arte y Diseño ) EHE (Escuela de Humanidades y Educación) y EIC(Escuela de Ingeniería y Ciencias), colaborando con el MUI (Museo Urbano Interactivo) en la residencia artística de Braulio Melendez. El zootropo incorpora un sistema electrónico avanzado que permite controlar la velocidad del motor, generar efectos estroboscópicos y detectar la presencia de personas cercanas para interactuar de manera dinámica.

## Características Principales

- **Dimensiones:** 1.2 metros de diámetro con una base inferior de 80 cm donde se aloja el motor.
- **Figuras:** 12 figuras animadas que crean una ilusión de movimiento al girar la tabla.
- **Control de Velocidad:** Ajustable mediante una interfaz web con un control deslizante.
- **Efecto Estroboscópico:** Generado por el encendido y apagado de un LED sincronizado con las revoluciones.
- **Sensores de Proximidad:** 4 sensores ultrasónicos que detectan la presencia de personas y ajustan el funcionamiento del zootropo en consecuencia.
- **Seguridad:** Apagado automático cuando una persona se acerca a menos de 30 cm.

## Estructura del Proyecto

El proyecto está organizado en las siguientes carpetas y archivos:

zootropo/ │ ├── CAD/ │ ├── base_superior.dxf # Archivo DXF de la base superior de 1.2m │ ├── base_inferior.dxf # Archivo DXF de la base inferior de 80 cm │ └── ensambles/ │ ├── soporte_tabla.stl # Ensamble de soporte de la tabla al eje del motor │ ├── bases_llantas.stl # Bases para llantas que cargan la tabla grande │ └── otros_ensambles.stl # Otros ensambles en STL │ ├── Electrónica/ │ ├── esquematico.pdf # Archivo esquemático del sistema electrónico │ └── pcb/ │ ├── zootropo_pcb.fab.gbr # Archivos de fabricación de la PCB │ └── ... │ ├── Firmware/ │ └── esp32_zootropo.ino # Código fuente de la ESP32 │ ├── Documentación/ │ └── manual_usuario.md # Manual de usuario e instrucciones de montaje │ └── README.md # Este archivo

## Componentes Electrónicos

- **Microcontrolador:** ESP32
- **Motor:** DC de 12V, 250W de corriente directa.
- **Driver de Motor:** BTS7960 de alta potencia.
- **Sensores Infrarrojos:** TCRT5000 para detección de las revoluciones.
- **LED:** LED de 12V, 50W para el efecto estroboscópico.
- **Transistor:** TIP41 utilizado como interruptor de potencia para el LED.
- **Sensores Ultrasónicos:** 4 unidades para detección de proximidad.
- **PCB:** Diseñada específicamente para integrar todos los componentes electrónicos.

## Descripción del Sistema Electrónico y de Control

El sistema electrónico está diseñado para controlar la velocidad del motor y generar efectos visuales sincronizados con las revoluciones del zootropo. A continuación, se detalla el funcionamiento de cada componente:

1. **Control de Velocidad:**

   - El motor de 12V, 250W es controlado mediante el driver BTS7960.
   - El ESP32 maneja el driver a través de PWM, permitiendo ajustar la velocidad del motor mediante una interfaz web.
   - La velocidad se controla mediante un control deslizante (slider) en una página web alojada en el ESP32.

2. **Efecto Estroboscópico:**

   - La tabla del zootropo cuenta con 12 barras equipadas con sensores infrarrojos TCRT5000.
   - Cada vez que una barra pasa por el sensor, se genera una interrupción que alterna el estado de un LED de 12V, creando un efecto estroboscópico sincronizado con las revoluciones.

3. **Sensores de Proximidad:**
   - Se han integrado 4 sensores ultrasónicos que monitorean la distancia de personas cercanas.
   - Si una persona se acerca a menos de 30 cm, el sistema apaga el motor para seguridad.
   - Al alejarse la persona, el zootropo retoma su funcionamiento a la velocidad configurada.

## Descripción del Código

- **Conexión WiFi:** El ESP32 se conecta a una red WiFi con credenciales predefinidas y asigna una IP estática para facilitar el acceso al servidor web.
- **Servidor Web:** Se implementa un servidor web que sirve una página HTML con un control deslizante para ajustar la velocidad del motor. Las solicitudes para cambiar la velocidad se manejan mediante rutas específicas.
- **Control de Motor:** Utiliza PWM para ajustar la velocidad del motor a través del driver BTS7960. Si la velocidad es menor a 1, el motor se apaga.
- **Efecto Estroboscópico:** Se utiliza un encoder infrarrojo para detectar las revoluciones de la tabla. Cada revolución alterna el estado de un LED para crear un efecto estroboscópico sincronizado.
- **Sensores Ultrasónicos:** Monitorean la proximidad de personas. Si una persona se acerca a menos de 20 cm, el sistema apaga el motor por seguridad. Al alejarse, el motor retoma su funcionamiento.
- **Interrupciones:** Se utilizan interrupciones para manejar las señales del encoder infrarrojo, asegurando una respuesta rápida y precisa.

## Instrucciones de Montaje

### Impresión 3D

- **Archivos STL:** Utiliza los archivos STL en la carpeta `CAD/ensambles/` para imprimir los soportes y bases necesarios.

### Montaje Mecánico

1. **Ensambla la Base:**
   - Utiliza los archivos DXF en `CAD/` para ensamblar la base superior de 1.2 metros y la base inferior de 80 cm.
2. **Instala el Motor:**
   - Coloca el motor en la base inferior de 80 cm.
   - Asegura la tabla grande de 1.2 metros al eje del motor utilizando los soportes impresos en 3D.
3. **Monta las Figuras:**
   - Fija las 12 figuras animadas en las barras de la tabla para crear la ilusión de movimiento al girar.

### Conexión Electrónica

1. **Conecta los Componentes:**
   - Sigue el esquema eléctrico proporcionado en `Electrónica/esquematico.pdf` para conectar todos los componentes electrónicos.
2. **Fabricación de la PCB:**
   - Utiliza los archivos en `Electrónica/pcb/` para fabricar la PCB.
   - Monta los componentes en la PCB una vez fabricada.

### Programación del ESP32

1. **Configura el ESP32:**
   - Conecta el ESP32 a tu computadora mediante un cable USB.
2. **Sube el Código:**
   - Abre el archivo `Firmware/esp32_zootropo.ino` en el IDE de Arduino.
   - Asegúrate de configurar correctamente los pines y las conexiones WiFi en el código.
   - Sube el código al ESP32.

## Pruebas

1. **Enciende el Sistema:**
   - Alimenta el zootropo y asegúrate de que todos los componentes estén correctamente conectados.
2. **Accede a la Interfaz Web:**
   - Abre un navegador web y accede a la dirección IP asignada al ESP32 (por defecto: `192.168.0.251`).
3. **Ajusta la Velocidad:**
   - Utiliza el control deslizante en la página web para ajustar la velocidad del motor.
   - Verifica que el efecto estroboscópico se sincronice correctamente con las revoluciones.
4. **Prueba los Sensores de Proximidad:**
   - Acércate al zootropo para asegurarte de que los sensores ultrasónicos detectan la proximidad y apagan el motor automáticamente.
   - Al alejarte, el motor debería reanudar su funcionamiento a la velocidad configurada.

## Licencia

Este proyecto está licenciado bajo la Licencia MIT. Consulta el archivo LICENSE para más detalles.

## Contacto

Para más información, preguntas o sugerencias, por favor contacta a:

- **Nombre:** [José Ángel Balbuena]
- **Correo Electrónico:** [jose.balbuena.palma@tec.mx]
