# Portafolio de Sistemas Embebidos I

Este repositorio contiene el portafolio de pr?cticas de **Sistemas Embebidos I**, realizado con C y una Raspberry Pi Pico 2 (RP2350).

El sitio documenta ocho pr?cticas del primer parcial y nueve del segundo. Incluye las explicaciones, el c?digo de cada pr?ctica y las fotos o videos disponibles.

**Sitio web:** https://gustavobecerril.github.io/Sistemas-Embebidos/

## Contenido

- [Primer parcial](docs/parcial1/binary_counter.md): contador binario, animaciones con LEDs, compuertas l?gicas y juego Stacker.
- [Segundo parcial](docs/parcial2/adc_basic.md): ADC, temperatura, I2C, PWM, temporizador y UART.
- Fuentes en C: carpetas codigos/ y codigos_parcial2/.
- Evidencia multimedia: carpetas docs/img/ y docs/img_videos_parcial2/.

## Ver el sitio en local

Requiere Python y Git. Desde la carpeta del repositorio ejecuta:

```bash
python -m pip install -r requirements.txt
mkdocs serve
```

Abre http://127.0.0.1:8000/ en el navegador.

## Publicaci?n

GitHub Actions genera el sitio cuando se suben cambios a la rama main. En el repositorio, configura **Settings ? Pages ? Source ? GitHub Actions**. Revisa la pesta?a **Actions** para confirmar que la publicaci?n termine correctamente.
