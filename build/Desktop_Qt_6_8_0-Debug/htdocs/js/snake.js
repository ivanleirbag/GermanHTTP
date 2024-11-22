// Seleccionamos el elemento <canvas> del HTML mediante el ID y obtenemos su contexto 2D para dibujar
const canvas = document.getElementById("canvas");
const ctx = canvas.getContext("2d");
let topScoresList = document.getElementById("topScoresList");

// Configuramos las dimensiones del canvas
canvas.width = 500;
canvas.height = 500;
const serverUrl = window.location.origin;


// Clase para representar la manzana
class apple {
    //radio va a representar el circulo de la manzana
    //color va a representar el color de la manzana
    // el contexto va a representar el canvas para dibujar
    //path va a representar la posicion de la manzana
    constructor(posicion, radio, color, contexto) {
        this.posicion = posicion;
        this.radio = radio;
        this.color = color;
        this.contexto = contexto;
    }

    // Método para dibujar la manzana en el canvas
    // vamos a utilizar lo mismo que en la serpiente
    dibujo() {
        this.contexto.beginPath();
        this.contexto.arc(this.posicion.x, this.posicion.y, this.radio, 0, 2 * Math.PI);
        this.contexto.fillStyle = this.color;
        this.contexto.fill();
        this.contexto.closePath();
    }

    // Método para verificar colisión entre la manzana y la cabeza de la serpiente
    // recibe el parametro snake
    colision(snake) {
        let distancia = Math.hypot(this.posicion.x - snake.posicion.x, this.posicion.y - snake.posicion.y);

        // Verificar si la cabeza de la serpiente colisiona con la manzana
        if (distancia < snake.radio + this.radio) {
            this.generarNuevaPosicion(snake);
            snake.agregarCabeza();

            // Incrementa el puntaje y actualiza el span, seria la puntuacion
            puntos += 10; 
            document.getElementById("puntos").textContent = puntos;
            guardarPuntajes();
            cargarPuntajes();
        }


    }

    // Genera una nueva posición para la manzana, asegurándose de que no colisione con la serpiente
    generarNuevaPosicion(snake) {
        let isValidPosition = false;
        while (!isValidPosition) {
            let nuevaPosicion = {
                x: Math.floor(Math.random() * (canvas.width - this.radio * 2) + this.radio),
                y: Math.floor(Math.random() * (canvas.height - this.radio * 2) + this.radio)
            };

            // Verificar que no colisione con el cuerpo de la serpiente
            isValidPosition = snake.body.every(segmento => {
                let distancia = Math.hypot(nuevaPosicion.x - segmento.path[0].x, nuevaPosicion.y - segmento.path[0].y);
                return distancia > this.radio + snake.radio;
            });
            // Si la nueva posicion es valida, se actualiza la posicion de la manzana
            if (isValidPosition) {
                this.posicion = nuevaPosicion;
            }
        }
    }
}

// Clase para representar los segmentos del cuerpo de la serpiente
class CuerpoSnake {
    //radio va a representar el circulo de la serpiente
    //color va a representar el color de la serpiente
    // el contexto va a representar el canvas para dibujar
    //path va a representar la posicion de la serpiente
    constructor(radio, color, contexto, path) {
        this.radio = radio;
        this.color = color;
        this.contexto = contexto;
        this.path = path;
    }
    //Metodo para dibujar el cuerpo de la serpiente
    //recibe los 4 parametros
    dibujoCirculo(x, y, radio, color) {
        this.contexto.beginPath();
        this.contexto.arc(x, y, radio, 0, 2 * Math.PI);
        this.contexto.fillStyle = color;
        this.contexto.fill();
        this.contexto.closePath();
    }

    // Metodo para dibujar el cuerpo de la serpiente
    dibujo() {
        for (let i = 0; i < this.path.length; i++) {
            this.dibujoCirculo(this.path[i].x, this.path[i].y, this.radio, this.color);
        }
    }
}

// Clase para representar la serpiente
class snake {
    constructor(posicion, radio, velocidad, color, contexto) {
        this.posicion = posicion;
        this.radio = radio;
        this.velocidad = velocidad;
        this.color = color;
        this.contexto = contexto;
        this.rotacion = 0;
        this.body = [];
        this.teclas = { A: false, D: false };
        this.tecladoPulse();
    }

    // 
    InicioJuego() {
        // Vacía el cuerpo de la serpiente
        this.body = [];

        for (let i = 0; i < 5; i++) { // Crear 5 segmentos iniciales
            let path = [];
            for (let j = 0; j < 15; j++) { //define el tamaño de la serpiente
                path.push({
                    x: this.posicion.x - j * this.radio * 2 - i * this.radio * 2, // Ajustamos la posición inicial
                    y: this.posicion.y
                });
            }
            this.body.push(new CuerpoSnake(this.radio, this.color, this.contexto, path));
        }
        this.dibujoCuerpo();
    }
    

    dibujoCirculo(x, y, radio, color) {
        this.contexto.beginPath();
        this.contexto.arc(x, y, radio, 0, 2 * Math.PI);
        this.contexto.fillStyle = color;
        this.contexto.fill();
        this.contexto.closePath();
    }

    agregarCabeza() {
        let path = [];
        for (let j = 0; j < 5; j++) {
            path.push({
                x: this.body.slice(-1)[0].path.slice(-1)[0].x,
                y: this.body.slice(-1)[0].path.slice(-1)[0].y
            });
        }
        this.body.push(new CuerpoSnake(this.radio, this.color, this.contexto, path));
    }

    dibujoCabeza() {
        this.dibujoCirculo(this.posicion.x, this.posicion.y, this.radio, this.color);

        // Dibujar ojos
        this.dibujoCirculo(this.posicion.x, this.posicion.y - 9, this.radio - 5, "#fff");
        this.dibujoCirculo(this.posicion.x + 1, this.posicion.y - 9, this.radio - 7, "#000");
        this.dibujoCirculo(this.posicion.x + 5, this.posicion.y - 8, this.radio - 11, "#fff");

        this.dibujoCirculo(this.posicion.x, this.posicion.y + 9, this.radio - 5, "#fff");
        this.dibujoCirculo(this.posicion.x + 1, this.posicion.y + 9, this.radio - 7, "#000");
        this.dibujoCirculo(this.posicion.x + 5, this.posicion.y + 8, this.radio - 11, "#fff");
    }

    actualizar() {
        
        this.dibujoCuerpo();
        this.dibujo();
        if (this.teclas.A) this.rotacion -= 0.04;
        if (this.teclas.D) this.rotacion += 0.04;

        this.posicion.x += Math.cos(this.rotacion) * this.velocidad;
        this.posicion.y += Math.sin(this.rotacion) * this.velocidad;

        this.colision();
    }

    dibujoCuerpo() {
        
        this.body.forEach((cuerpo) => {
            cuerpo.color = this.color; 
            cuerpo.dibujo();
          });

        this.body[0].path.unshift({ x: this.posicion.x, y: this.posicion.y });
        if (this.body[0].path.length > 20) this.body[0].path.pop();

        this.body[0].dibujo();

        for (let i = 1; i < this.body.length; i++) {
            this.body[i].path.unshift(this.body[i - 1].path.pop());
            if (this.body[i].path.length > 20) this.body[i].path.pop();
            this.body[i].dibujo();
        }

        ctx.font = '24px Arial';
        ctx.fillStyle = '#fff';
        ctx.textAlign = 'left';
        ctx.textBaseline = 'top';
        ctx.fillText(inputTexto.value, this.posicion.x + 20, this.posicion.y + 20);
    }

    dibujo() {
        this.contexto.save();
        this.contexto.translate(this.posicion.x, this.posicion.y);
        this.contexto.rotate(30 * Math.PI / 180);
        this.contexto.translate(-this.posicion.x, -this.posicion.y);
        this.dibujoCabeza();
        this.contexto.restore();
    }

    tecladoPulse() {
        document.addEventListener("keydown", (e) => {
            if (e.key.toLowerCase() === 'a' || e.key === 'ArrowLeft') this.teclas.A = true;
            if (e.key.toLowerCase() === 'd' || e.key === 'ArrowRight') this.teclas.D = true;
        });

        document.addEventListener("keyup", (e) => {
            if (e.key.toLowerCase() === 'a' || e.key === 'ArrowLeft') this.teclas.A = false;
            if (e.key.toLowerCase() === 'd' || e.key === 'ArrowRight') this.teclas.D = false;
        });
    }

    colision() {
        for (let i = 1; i < this.body.length; i++) {
            if (
                Math.hypot(
                    this.posicion.x - this.body[i].path[0].x,
                    this.posicion.y - this.body[i].path[0].y
                ) < this.radio * 2
            ) {
                this.reiniciarJuego();
                return;
            }
        }

        if (this.posicion.x + this.radio > canvas.width) this.posicion.x = this.radio;
        if (this.posicion.x - this.radio < 0) this.posicion.x = canvas.width - this.radio;
        if (this.posicion.y + this.radio > canvas.height) this.posicion.y = this.radio;
        if (this.posicion.y - this.radio < 0) this.posicion.y = canvas.height - this.radio;
    }


    // utilizamos reiniciarJuego para cuando colisione, reubicando el snake en las posiciones dadas
    //  posicion en x: 15, y: 80, direccion yendo a la derecha, cuerpo vacio --body-- y llamando a InicioJuego
    reiniciarJuego() {
        this.posicion = { x: 15, y: 80 }; 
        this.direccion = { x: 1, y: 0 }; 
        this.body = []; 
        this.InicioJuego();

        puntos = 0;
        document.getElementById("puntos").textContent = puntos;
    }
}

//proceda a declarar las variables
let puntos = 0;


//procedemos a declarar las constantes 
const colorOptions = document.querySelectorAll('.color-option');
const inputTexto = document.getElementById('input-texto');
const puntosActual = document.getElementById('puntos');




// vamos a obtener el valor de los divs
colorOptions.forEach((option) => {
    option.addEventListener('click', () => {
    const selectedColor = option.getAttribute('data-color');
    SnakeFrost.color = selectedColor;
    });
});



// vamos a obtener la informacion del input para utilizarlo en la fn dibujoCuerpo
inputTexto.addEventListener('input', (e) => {
    const texto = e.target.value;
    // Actualiza el texto en el canvas
    actualizarTexto(texto);
})




// Instancia de la serpiente y la manzana
const SnakeFrost = new snake({ x: 10, y: 40 }, 11, 2, "#ff0094", ctx);
SnakeFrost.InicioJuego();
const manzanaFood = new apple({ x: 200, y: 200 }, 8, "red", ctx);



// declaramos una variable para almacenar los puntajes, en este caso sera un array
let topScores = [];

async function cargarPuntajes() {
    try {
        // Intentar cargar los puntajes actuales desde el servidor
        const response = await fetch("/json/scores.json");
        if (response.ok) {
            const data = await response.json();
            topScores = data.puntajes || [];
        } else {
            console.warn("No se pudieron cargar los puntajes del servidor.");
        }
    } catch (error) {
        console.error("Error al cargar los puntajes del servidor:", error);
    }
    // Renderizar la lista inicial en el HTML
    const listaHTML = document.getElementById("topScoresList");
    listaHTML.innerHTML = ""; // Limpiar la lista actual

    topScores.forEach((jugador) => {
        const li = document.createElement("li");
        li.textContent = `${jugador.name}: ${jugador.score}`;
        listaHTML.appendChild(li);
    });
}

cargarPuntajes();


// Procedemos a guardarlo en el json
async function guardarPuntajes() {
    const jugadorActual = inputTexto.value.trim(); // Obtener el nombre del jugador desde el input
    if (!jugadorActual) {
        console.warn("El nombre del jugador está vacío, no se puede guardar el puntaje.");
        return;
    }

    // Verificar si el jugador ya está en la lista
    let jugadorExistenteIndex = topScores.findIndex((jugador) => jugador.name === jugadorActual);

    if (jugadorExistenteIndex !== -1) {
        // Actualizar el puntaje solo si el nuevo es mayor
        if (puntos > topScores[jugadorExistenteIndex].score) {
            topScores[jugadorExistenteIndex].score = puntos;
        }
    } else {
        // Agregar un nuevo jugador a la lista
        topScores.push({ name: jugadorActual, score: puntos });
    }

    // Ordenar la lista por puntaje en orden descendente
    topScores.sort((a, b) => b.score - a.score);
    topScores = topScores.slice(0, 5);


    // Crear el objeto con la estructura solicitada
    const puntajesJSON = {
        puntajes: topScores
    };

    // Enviar el JSON al servidor
    try {
        const response = await fetch("/json/scores.json", {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
            },
            body: JSON.stringify(puntajesJSON),
        });

        if (!response.ok) {
            console.error("Error al guardar los puntajes en el servidor.");
        }
    } catch (error) {
        console.error("No se pudo enviar los datos al servidor:", error);
    }
}




// Fondo del juego
function fondoSnake() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.fillStyle = "#1B1C30";
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    for (let i = 0; i < canvas.height; i += 90) {
        for (let j = 0; j < canvas.width; j += 90) {
            ctx.fillStyle = "#23253C";
            ctx.fillRect(j, i, 70, 70);
        }
    }
}

// Lógica principal del juego
function InitGame() {
    fondoSnake();
    SnakeFrost.actualizar();
    manzanaFood.dibujo();
    manzanaFood.colision(SnakeFrost);
    requestAnimationFrame(InitGame);
}

InitGame();
