# Overflow: Algorithmic Tower Defense

Juego de defensa por oleadas para CI-0116 Análisis de Algoritmos y
Estructuras de Datos (ECCI, Universidad de Costa Rica). Cada torre guarda a
los enemigos de su radio en una estructura de datos, y la velocidad con que
dispara no es un número configurable: sale de contar los pasos que ejecuta
su estructura. Una lista enlazada dispara lento porque recorrerla cuesta
pasos; un AVL o una tabla hash disparan rápido porque cuestan menos.

**Integrantes:** Ashley Solano, Alejandro Cubero y Kevin Velásquez.

---

## Contenido

1. [Instalación](#instalación)
2. [Compilar](#compilar)
3. [Ejecutar](#ejecutar)
4. [Cómo se juega](#cómo-se-juega)
5. [Modo sin ventana (headless)](#modo-sin-ventana-headless)
6. [Bitácora de combate](#bitácora-de-combate)
7. [Experimentos e informe](#experimentos-e-informe)
8. [Estructura del repositorio](#estructura-del-repositorio)
9. [Problemas comunes](#problemas-comunes)
10. [Convenciones del equipo](#convenciones-del-equipo)

---

## Instalación

Probado en **Ubuntu 24.04** (nativo y en WSL con Windows 11). Todo se
instala con `apt`; no hace falta `pip`.

### Todo de una vez

```bash
sudo apt update
sudo apt install build-essential pkgconf \
  qtbase5-dev qtbase5-dev-tools \
  qtmultimedia5-dev libqt5multimedia5-plugins \
  gstreamer1.0-plugins-good gstreamer1.0-pulseaudio \
  fonts-noto-cjk \
  python3 python3-pandas python3-numpy python3-matplotlib \
  latexmk texlive-latex-recommended texlive-lang-spanish lmodern
```

### Qué es cada paquete y para qué se necesita

| Paquete | Para qué | Sin él |
|---|---|---|
| `build-essential` | Compilador `g++` y `make` | No compila nada |
| `pkgconf` | El Makefile lo usa para encontrar Qt (`pkg-config`) | El Makefile no encuentra Qt |
| `qtbase5-dev` | Qt 5 (ventana, botones, dibujo) | No compila la interfaz |
| `qtbase5-dev-tools` | `moc`, el generador de código de Qt | Errores de enlazado con `vtable` o señales |
| `qtmultimedia5-dev` | Módulo de audio de Qt (`QMediaPlayer`) | No compila (`QMediaPlayer: No such file`) |
| `libqt5multimedia5-plugins` | Complementos con los que Qt reproduce audio | El juego abre pero el audio no suena |
| `gstreamer1.0-plugins-good` | Lector de archivos WAV para GStreamer | El audio del Hollow Purple no suena |
| `gstreamer1.0-pulseaudio` | Envía el audio de GStreamer al servidor de sonido | El audio no llega a los parlantes |
| `fonts-noto-cjk` | Fuente con caracteres japoneses (texto del Hollow Purple) | Las frases salen como cuadritos vacíos |
| `python3-pandas`, `python3-numpy`, `python3-matplotlib` | Figuras y tablas del informe (`make_figures.py`) | No se generan las figuras |
| `latexmk`, `texlive-latex-recommended` | Compilar el informe en LaTeX | No se genera el PDF |
| `texlive-lang-spanish` | Idioma español para LaTeX (`babel`) | `Unknown option 'spanish'` |
| `lmodern` | Fuente Latin Modern del informe | `File 'lmodern.sty' not found` |

`run_experiments.py` y `check_combat_log.py` solo usan la biblioteca
estándar de Python: para correr experimentos no hace falta nada más que
`python3`.

### Opcionales

| Paquete | Para qué |
|---|---|
| `cpplint` | `make lint`, el revisor de estilo que usa el equipo |
| `doxygen`, `graphviz` | `make doc`, documentación a partir de los comentarios Doxygen |
| `valgrind` | `make memcheck`, detectar errores de memoria |
| `pulseaudio-utils` | `paplay` y `pactl`, para diagnosticar el audio en WSL |
| `gstreamer1.0-tools` | `gst-launch-1.0`, para probar GStreamer sin el juego |
| `ffmpeg` | Convertir audios a WAV PCM de 16 bits si hiciera falta |

El Makefile del curso también trae `make instdeps`, que instala las
herramientas del curso (lint, Doxygen, Valgrind), pero **no** instala Qt,
GStreamer, la fuente japonesa ni LaTeX: esos van con el comando de arriba.

---

## Compilar

Todo se compila con el Makefile del curso, **desde la carpeta `src/`**:

```bash
cd Proyecto1/src
make
```

El ejecutable queda en `src/bin/Overflow`. El Makefile busca solo todos los
`.cpp` del proyecto, así que al agregar un archivo nuevo no hay que tocarlo.

| Comando | Qué hace |
|---|---|
| `make` | Compila (versión de depuración) |
| `make clean` | Borra lo compilado; úsenlo antes de `make` si cambió un header o el Makefile |
| `make release` | Compila optimizado (más rápido para experimentos largos) |
| `make run` | Compila si hace falta y ejecuta el juego |
| `make lint` | Revisa el estilo con `cpplint` |
| `make doc` | Genera la documentación con Doxygen |

El proyecto compila con `-Wall -Wextra -Werror`: cualquier advertencia
detiene la compilación.

---

## Ejecutar

Desde `src/`:

```bash
./bin/Overflow                  # juego normal
./bin/Overflow --challenge      # modo desafío
```

También funciona `make run` y `make run ARGS="--challenge"`. El juego
encuentra sus imágenes y su audio a partir de dónde está el ejecutable, así
que se puede abrir desde cualquier carpeta.

---

## Cómo se juega

El mapa tiene un camino fijo con **8 ranuras** para torres. Cada oleada
tiene dos fases:

1. **Construcción** (sin límite de tiempo). Se ve la composición de la
   próxima oleada. Haciendo clic en una ranura se elige qué estructura
   instalar o por cuál reemplazarla. El reloj del juego no corre.
2. **Combate**, que empieza con **START WAVE**. Los enemigos recorren el
   camino y las estructuras quedan fijas hasta el final de la oleada.

Cada enemigo que llega a la base quita una vida. La partida termina al
perder las **20 vidas** o al superar las **20 oleadas**; cada oleada trae
un 30 % más de enemigos que la anterior.

### Estructuras

| Estructura | Precio | Comportamiento en este juego |
|---|---:|---|
| Lista enlazada | 0 | Inserta en O(1); borrar recorre toda la lista |
| Lista ordenada | 60 | Insertar recorre hasta el final |
| Arreglo dinámico | 80 | Inserta al final; borrar es búsqueda lineal |
| Arreglo ordenado | 150 | Búsqueda binaria, pero corre elementos en cada cambio |
| ABB | 220 | Los ids crecientes lo degeneran en una cadena |
| Montículo mínimo | 260 | Aquí es O(log n): el enemigo que sale siempre es el mínimo |
| AVL | 400 | Se mantiene balanceado; paga rotaciones |
| Tabla hash | 800 | O(1), salvo contra los Hive con la función original |

Cada torre dispone de 40 pasos por tick (2400 por segundo). Los gasta
primero en mantener su registro al día (insertar a quien entra al radio,
borrar a quien sale o muere) y, si le sobra, en disparar. Si su estructura
es lenta, la cola de pendientes crece y la torre deja de disparar.

### Enemigos

| Enemigo | Qué pone a prueba |
|---|---|
| Swarm | Llega en orden creciente de id: degenera el ABB |
| Wraith | Entra y sale del radio: puro costo de mantenimiento |
| Hive | Sus ids colisionan en la tabla hash |
| Decoy | Invulnerable: infla el tamaño de los registros y expira solo |
| Colossus | Mucha vida y mucha recompensa |

### Habilidad especial: 虚式「茈」 (Hollow Purple)

El botón debajo de **START WAVE**. Cuesta **100 000 créditos** y solo se
puede comprar durante el combate. Al activarla:

- Durante **18 segundos** de carga, sobre el mapa aparece un velo morado y
  las frases 〝九綱〟〝偏光〟〝烏と声明〟〝表裏の間〟, una cada 4,5 s, con el
  audio `assets/sounds/hollow_purple.wav`.
- Al terminar la carga **borra a todos los enemigos del mapa**, sin
  recompensa y sin perder vidas.
- Después queda **90 segundos en recarga**.

Precio, carga y recarga se ajustan en `include/core/CombatConstants.hpp`.
La carga debe durar lo mismo que el audio: si se cambia uno, hay que
cambiar el otro.

### Modo desafío

`--challenge` usa siempre la misma semilla (`CHALLENGE_SEED` en
`include/core/ChallengeMode.hpp`), así que todos los jugadores enfrentan
exactamente la misma partida. Al empezar pide un nombre; al terminar guarda
el resultado en `report/challenge_scores.csv` y muestra la tabla de
posiciones, que también se puede abrir con el botón **LEADERBOARD**. El
orden es por oleadas completadas, después vidas restantes y después
créditos.

---

## Modo sin ventana (headless)

Corre una partida completa sin interfaz, a máxima velocidad, y escribe la
bitácora de combate. Es el que genera los datos del informe y la prueba de
que la simulación no depende de la interfaz.

```bash
./bin/Overflow --headless --seed 42 --core avl --out resultados.csv
```

Instala la misma estructura en las 8 ranuras, empieza cada oleada sola (no
hay jugador) e imprime un resumen en la terminal.

| Opción | Valor | Qué hace |
|---|---|---|
| `--headless` | | Sin ventana (obligatoria para lo demás) |
| `--seed N` | entero | Semilla: la misma semilla da la misma partida, bit a bit |
| `--core NOMBRE` | ver abajo | Estructura para las 8 ranuras (obligatoria) |
| `--out ARCHIVO` | ruta | Dónde escribir la bitácora (por defecto `resultados.csv`) |
| `--waves M` | 1 a 20 | Jugar solo hasta la oleada M |
| `--ignore-defeat` | | Seguir jugando aunque se acaben las vidas |
| `--only CATEGORÍA` | `swarm`, `wraith`, `hive`, `decoy`, `colossus` | Todas las oleadas de una sola categoría |
| `--hash FUNCIÓN` | `default`, `mixed` | Función de dispersión de la tabla hash |
| `--buckets-out ARCHIVO` | ruta | Escribir también la distribución de cubetas de la tabla hash |

Nombres válidos para `--core`: `linked_list`, `sorted_list`,
`dynamic_array`, `sorted_array`, `bst`, `avl`, `min_heap`, `hash_table`.

`--ignore-defeat`, `--only`, `--hash` y `--buckets-out` existen para los
experimentos del informe: miden cada estructura bajo toda la carga, o
aíslan un enemigo en particular.

---

## Bitácora de combate

Un archivo CSV con **una fila por torre y por oleada**: tamaño máximo y
promedio del registro, inserciones, borrados y consultas, pasos por tipo de
operación y por categoría (comparaciones, saltos de puntero, corrimientos,
rotaciones), tiempo real en microsegundos, largo de la cola de pendientes,
ticks con la cola vacía, y disparos efectivos contra disparos a enemigos
que ya habían muerto. La semilla va en cada fila, así que las bitácoras de
varias partidas se pueden unir sin procesarlas.

Para comprobar que un archivo carga sin transformaciones:

```bash
python3 tools/check_combat_log.py resultados.csv
```

Desde `Proyecto1/`. Revisa encabezados, número de campos, que todo sea
numérico y que los pasos cuadren entre sí.

---

## Experimentos e informe

Desde `Proyecto1/`, con el juego ya compilado:

```bash
python3 tools/run_experiments.py        # 1. correr las partidas
python3 tools/make_figures.py           # 2. figuras, tablas y números
cd report && latexmk -pdf informe.tex   # 3. compilar el PDF
```

1. **`run_experiments.py`** corre 600 partidas headless (30 semillas por
   experimento) en paralelo y deja cinco CSV en `report/data/`. Tarda unos
   pocos minutos. `--seeds 5` sirve para una prueba rápida.
2. **`make_figures.py`** lee esos CSV y genera las figuras en
   `report/figures/`, las tablas y un archivo con cada número citado en el
   texto en `report/generated/`. Las constantes del juego (pasos por tick,
   crecimiento de las oleadas) las lee directo del código.
3. **`informe.tex`** incluye esas figuras, tablas y números. Si se repiten
   los experimentos y se vuelven a correr los pasos 2 y 3, el informe se
   actualiza solo, sin editar el texto.

Las partidas son deterministas: la misma semilla produce la misma bitácora
salvo la columna de tiempo real, que depende de la máquina.

El `.gitignore` del repositorio ignora todos los PDF. Para subir el informe
final: `git add -f report/informe.pdf`.

---

## Estructura del repositorio

```
Proyecto1/
├── assets/                imágenes del mapa, torres y enemigos
│   └── sounds/            audio del Hollow Purple
├── include/               headers (.hpp)
│   ├── core/              simulación, torres, economía, bitácora, CLI
│   ├── game/              oleadas y enemigos
│   ├── registries/        las 8 estructuras de datos
│   └── ui/                interfaz en Qt
├── src/                   implementaciones (.cpp) y el Makefile
│   ├── core/  game/  registries/  ui/
│   └── main.cpp
├── tools/                 scripts de experimentos, verificación y figuras
└── report/                datos, figuras, informe y tabla del modo desafío
```

La simulación (`core/`, `game/`, `registries/`) no incluye nada de Qt: la
interfaz (`ui/`) la lee, pero la simulación no sabe que existe.

---

## Problemas comunes

**`Package Qt5Widgets was not found` o `Qt5Multimedia` al compilar.**
Falta instalar `qtbase5-dev` o `qtmultimedia5-dev`. Después de instalar,
`make clean` y `make`.

**VS Code subraya en rojo los `#include` de Qt, pero compila bien.** Es
IntelliSense, no el compilador. Agreguen las rutas de Qt al `includePath`
de `.vscode/c_cpp_properties.json`:

```json
"/usr/include/x86_64-linux-gnu/qt5",
"/usr/include/x86_64-linux-gnu/qt5/QtWidgets",
"/usr/include/x86_64-linux-gnu/qt5/QtGui",
"/usr/include/x86_64-linux-gnu/qt5/QtCore",
"/usr/include/x86_64-linux-gnu/qt5/QtMultimedia"
```

Después `Ctrl+Shift+P` → *Reload Window*.

**Los caracteres japoneses salen como cuadritos.** Falta
`fonts-noto-cjk`. Instálenla y vuelvan a abrir el juego.

**El Hollow Purple no suena (WSL).** Revisen en este orden:

1. Que el archivo esté en `Proyecto1/assets/sounds/hollow_purple.wav`
   (no en `src/assets/`).
2. Que suene fuera del juego: `paplay assets/sounds/hollow_purple.wav`
   (paquete `pulseaudio-utils`).
3. Si `paplay` o `pactl info` dan `Timeout` o `Connection refused`, el
   servidor de sonido de WSL no está respondiendo. Cierren todo y, en
   PowerShell de Windows: `wsl --shutdown`.
4. Con `systemd=true` en `/etc/wsl.conf`, indiquen la ruta del servidor
   de sonido:

   ```bash
   echo 'export PULSE_SERVER=unix:/mnt/wslg/PulseServer' >> ~/.bashrc
   source ~/.bashrc
   ```

5. Si el `Timeout` vuelve:

   ```bash
   mkdir -p ~/.config/pulse
   echo "enable-shm = false" >> ~/.config/pulse/client.conf
   ```

   y otra vez `wsl --shutdown`.

En Windows 10, WSL no tiene salida de audio: el juego funciona igual, pero
en silencio. Si el audio falla, la terminal lo dice con un mensaje que
empieza con `Hollow Purple sound`.

**`QStandardPaths: wrong permissions on runtime directory`.** Es un aviso
de WSL y no afecta al juego. Se quita con `chmod 700 /run/user/1000`.

**El headless termina en unos pocos ticks sin jugar nada.** La rama no
tiene el arranque automático de oleadas en `headlessRunner.cpp`; actualicen
desde `main`.

---

## Convenciones del equipo

- Código y comentarios en **inglés**; comentarios de documentación en
  estilo **Doxygen** (`@brief`, `@param`, `@return`).
- Ramas con el formato `TASK_#_DescripcionBreve`, siempre a partir de
  `main` actualizado.
- Todo entra a `main` por **Pull Request**.
- Sin números mágicos: los valores de balance van como `constexpr` en los
  headers de constantes.
- Antes de abrir un PR: `make clean && make` (compila con `-Werror`) y
  `make lint`.
