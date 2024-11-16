const canvas = document.getElementById('gameCanvas');
const ctx = canvas.getContext('2d');
canvas.width = 800;
canvas.height = 600;

let carImage = new Image();
let trackImage = new Image();
let carID;
let carState = {
    posX: 0,
    posY: 0,
    speed: 0,
    direction: 0,
    accelerating: false,
    turningLeft: false,
    turningRight: false
};

const serverUrl = 'http://localhost:8080';

// Cargar el fondo del circuito
trackImage.src = `${serverUrl}/racedocs/assets/tracks/background.jpg`;

// Solicitar la imagen del auto
async function requestCarImage() {
    const response = await fetch(`${serverUrl}/race4/getState`);
    const data = await response.json();
    carID = data.carID;
    carImage.src = `${serverUrl}/racedocs/assets/cars/car${carID}.png`;
}

// Escalar la imagen del auto
carImage.onload = () => {
    carImage.width = 20;
    carImage.height = 20;
};

// Manejar input del usuario
document.addEventListener('keydown', (event) => {
    switch (event.key) {
        case 'ArrowUp':
            carState.accelerating = true;
            break;
        case 'ArrowLeft':
            carState.turningLeft = true;
            break;
        case 'ArrowRight':
            carState.turningRight = true;
            break;
        case 'ArrowDown':
            carState.speed = Math.max(0, carState.speed - 1);
            break;
    }
});

document.addEventListener('keyup', (event) => {
    switch (event.key) {
        case 'ArrowUp':
            carState.accelerating = false;
            break;
        case 'ArrowLeft':
            carState.turningLeft = false;
            break;
        case 'ArrowRight':
            carState.turningRight = false;
            break;
    }
});

// Enviar el estado del auto al servidor
async function sendCarState() {
    const response = await fetch(`${serverUrl}/race4/update`, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(carState)
    });
    const gameState = await response.json();
    updateGameState(gameState);
}

// Actualizar el estado del juego
function updateGameState(gameState) {
    ctx.drawImage(trackImage, 0, 0, canvas.width, canvas.height);

    for (const car of gameState.cars) {
        const carImg = new Image();
        carImg.src = `${serverUrl}/assets/cars/${car.ID}.png`;
        ctx.drawImage(carImg, car.posX, car.posY, 20, 20);
    }
}

// Loop de renderizado y actualización
function gameLoop() {
    if (carState.accelerating) {
        carState.speed += 1;
    }
    if (carState.turningLeft) {
        carState.direction -= 1;
    }
    if (carState.turningRight) {
        carState.direction += 1;
    }

    sendCarState();
    requestAnimationFrame(gameLoop);
}

// Inicializar el juego
trackImage.onload = () => {
    requestCarImage().then(() => {
        gameLoop();
    });
};
