const canvas = document.getElementById('gameCanvas');
const ctx = canvas.getContext('2d');
canvas.width = 1000;
canvas.height = 750;

let carImages = [];
let carState = {
    //id: "",
    ID: 0,
    posX: 0,
    posY: 0,
    speed: 0,
    direction: 0
};

const serverUrl = 'http://localhost:8080';

// Cargar el fondo del circuito
let trackImage = new Image();
trackImage.src = `${serverUrl}/racedocs/assets/tracks/background.jpg`;

// Solicitar unirse a la carrera
async function joinRace() {
    try {
        const response = await fetch(`${serverUrl}/race4/join`);
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        const data = await response.json();

        // Asignar el estado inicial del coche desde el JSON recibido
        //carState.id = data.id;
        carState.ID = data.ID;
        carState.posX = data.posX;
        carState.posY = data.posY;
        carState.speed = data.speed;
        carState.direction = data.direction;

        console.log("Estado inicial del coche:", carState);
    } catch (error) {
        console.error("Error al unirse a la carrera:", error);
    }
}

// Cargar las imágenes de los autos al iniciar
function loadCarImages() {
    return new Promise((resolve, reject) => {
        let loadedImages = 0;
        for (let i = 1; i < 5; i++) {
            const carImg = new Image();
            carImg.src = `${serverUrl}/racedocs/assets/cars/car${i}.png`;

            // Asegurarse de que cada imagen se cargue correctamente
            carImg.onload = () => {
                console.log(`Imagen de coche ${i} cargada con éxito`);
                carImages.push(carImg);
                loadedImages++;
                if (loadedImages === 4) resolve(); // Resolver cuando todas las imágenes estén cargadas
            };


            // Manejar error de carga de imágenes
            carImg.onerror = () => {
                console.error(`Error al cargar la imagen de coche ${i}`);
                reject(`Error al cargar la imagen de coche ${i}`);
            };
        }
    });
}

// Solicitar datos iniciales del auto
async function requestCarData() {
    try {
        const response = await fetch(`${serverUrl}/race4/getState`);
        const data = await response.json();

        // Iterar sobre la lista de autos para encontrar el que coincide con el carID del cliente
        const clientCar = data.cars.find(car => car.ID === carState.ID);

        if (clientCar) {
            //carState.id = clientCar.id;
            carState.ID = clientCar.ID;
            carState.posX = clientCar.posX;
            carState.posY = clientCar.posY;
            carState.speed = clientCar.speed;
            carState.direction = clientCar.direction;
        } else {
            console.warn(`No se encontró un coche con carID: ${carState.ID}`);
        }
    } catch (error) {
        console.error("Error al solicitar el estado del auto:", error);
    }
}

// Manejar input del usuario
document.addEventListener('keydown', (event) => {
    switch (event.key) {
        case 'ArrowUp':
            if (carState.speed < 17){
                carState.speed += 2;
            }
            break;
        case 'ArrowLeft':
            carState.direction -= 0.05; // Girar a la izquierda
            break;
        case 'ArrowRight':
            carState.direction += 0.05; // Girar a la derecha
            break;
        case 'ArrowDown':
            if(carState.speed  > -15){
                carState.speed -= 1;
            }
            break;
    }
});

// Enviar el estado del auto al servidor
async function sendCarState() {
    try {
        const response = await fetch(`${serverUrl}/race4/updateState`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                //id: carState.id,
                ID: carState.ID,
                posX: carState.posX,
                posY: carState.posY,
                speed: carState.speed,
                direction: carState.direction
            })
        });
        const gameState = await response.json();
        console.log(gameState);
        updateGameState(gameState);
    } catch (error) {
        console.error("Error al enviar el estado del auto:", error);
    }
}

// Actualización del estado del juego
function updateGameState(gameState) {
    ctx.clearRect(0, 0, canvas.width, canvas.height); // Limpiar el lienzo
    ctx.drawImage(trackImage, 0, 0, canvas.width, canvas.height); // Dibujar el fondo

    for (const car of gameState.cars) {
        const carImg = carImages[car.ID - 1]; // Ajustar según el imgID
        if (!carImg) {
            console.error(`No se encontró una imagen para imgID: ${car.ID}`);
            continue; // Saltar al siguiente coche
        }
        if (carState.ID === car.ID){
            carState.posX = car.posX;
            carState.posY = car.posY;
            carState.speed = car.speed;
            carState.direction = car.direction;
        }
        if (carImg) {
            ctx.save();

           // Trasladar el contexto a la posición del coche
            ctx.translate(car.posX, car.posY); 

            // Rotar el contexto según la dirección del coche
            ctx.rotate(car.direction); 

            // Dibujar la imagen centrada en la posición del coche
            ctx.drawImage(carImg, -20, -20, 40, 40); 

            // Restaurar el estado del contexto
            ctx.restore();
        } else {
            console.error(`No se pudo encontrar la imagen para el coche con imgID: ${car.ID}`);
        }
    }
}

// Controlar el intervalo para enviar los datos
let lastUpdateTime = 0;
const updateInterval = 70; // 100ms, es decir, 10 veces por segundo

// Loop de renderizado y actualización
function gameLoop(timestamp) {
    // Solo actualiza y envía el estado si ha pasado el intervalo
    if (timestamp - lastUpdateTime >= updateInterval) {
        sendCarState(); // Enviar el estado del coche
        lastUpdateTime = timestamp; // Actualizar el tiempo de la última solicitud
    }
    requestAnimationFrame(gameLoop); // Solicitar el siguiente frame
}

// Inicializar el juego
trackImage.onload = () => {
    loadCarImages()
        .then(() => joinRace()) // Unirse a la carrera antes de iniciar
        .then(() => gameLoop(0)) // Comenzar el ciclo del juego
        .catch((error) => {
            console.error("Error durante la inicialización:", error);
        });
};
