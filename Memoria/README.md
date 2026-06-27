## Estructura de ficheros

### 1. Raíz

#### 1.1 Carpetas

**capitulos**: aquí están los ficheros Latex correspondientes a los capítulos.

**portada**: aquí están los ficheros Latex correspondientes a las primeras páginas del documento.

**figs**: aquí deberás meter todas las figuras que enlaces en el documento.

#### 1.2 Ficheros que tendrás que editar

**memoria.tex**: fichero principal. Desde él se acceden al estilo, páginas de portada, capítulos y bibliografía (estilo establecido: APA).

**estilo.tex**: define el estilo/apariencia que tendrá el documento final.

**bibliografia.bib**: incluye las referencias bibliográficas.

#### 1.3 Ficheros auxiliares que no tendrás que editar

**Makefile**: archivo de compilación.

**listings.sty**, **lstmisc.sty**: paquetes de definición y estilo de "listings".

### 2. capitulos ###

Los ficheros Latex aquí contenidos siguen la siguiente estructura:

**capitulo1.tex**: introducción.

**capitulo2.tex**: objetivos.

**capitulo3.tex**: plataforma de desarrollo.

**capitulo4.tex**: diseño.

**capitulo5.tex**: conclusiones.

### 3. portada ###

En esta carpeta se encuentran los ficheros Latex correspondientes a las primeras páginas del documento.

**indice.tex**: fichero principal. Desde él se acceden al resto de ficheros de esta carpeta en el siguiente orden. Además, recoge también la tabla de contenidos, lista de figuras, lista de códigos, lista de ecuaciones y lista de tablas (o cuadros).

**portada.tex**: página de portada del documento.

**licencia.tex**: página dedicada a especificar la licencia del documento. Por defecto está bajo licencia CC-BY-NC-SA.

**agradecimientos.tex**: página de agradecimientos.

**resumen.tex**: resumen del trabajo.

**acronimos.tex**: donde se especifican los acrónimos usados en el documento.
