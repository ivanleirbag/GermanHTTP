


const titleMessages = ["¡Bienvenido a la viborita!", "¡Prepárate para la ganar!", "¿Listo para jugar?"];
let titleIndex = 0;

function changeTitle() {
    document.title = titleMessages[titleIndex];
    titleIndex = (titleIndex + 1) % titleMessages.length;
}


setInterval(changeTitle, 5000);

var canvas = document.getElementById('game');
var context = canvas.getContext('2d');

var grid = 16;
var count = 0;

var snake = {
    x: 160,
    y: 160,
    dx: grid,
    dy: 0,
    cells: [],
    maxCells: 4
};

var apple = {
    x: 320,
    y: 320
};

var score = 0;
var bestScore = 0;
var lastScoreIncrement = 0;
var gamePaused = false;
var gameOver = false;
var gameOverTransition = 0;
var baseSpeed = 100;
var currentSpeed = baseSpeed;
var speedIncrement = 50;
var gameOverTimer = 0;
var gameOverDisplayTime = 2000;
var transitionStartTime = 0;
var pauseImage = new Image();
pauseImage.src = 'assets/pausasnake.png'; // Cambia esto a la ruta de tu imagen


var gameOverImage = new Image();
gameOverImage.src = '/assets/gameover.png';

function getRandomInt(min, max) {
    return Math.floor(Math.random() * (max - min)) + min;
}

function drawScore() {
    context.fillStyle = 'white';
    context.font = '10px Arial';
    context.fillText('Best: ' + bestScore + ' pts', 10, 20);
    context.fillText('Now: ' + score + ' pts', 10, 40);
}

function drawGameOver() {
    context.drawImage(gameOverImage, 0, 0, canvas.width, canvas.height);
    context.fillStyle = 'rgba(255, 0, 0, ' + gameOverTransition + ')';
    context.fillRect(0, 0, canvas.width, canvas.height);
    context.fillStyle = 'white';
    context.font = '20px Arial';
    context.fillText('Perdiste', canvas.width / 2 - 50, canvas.height / 2 - 10);
    context.fillText('R para reiniciar', canvas.width / 2 - 80, canvas.height / 2 + 10);
}

function drawPauseMenu() {
    context.fillStyle = 'rgba(0, 0, 0, 0.8)';
    context.fillRect(0, 0, canvas.width, canvas.height);
    
    context.drawImage(pauseImage, canvas.width / 2 - 100, canvas.height / 2 - 80, 200, 100); // Ajusta las coordenadas y el tamaño según necesites

    context.fillStyle = 'white';
    context.font = '30px Arial';
    context.fillText('PAUSA', canvas.width / 2 - 50, canvas.height / 2 - 10);
    context.font = '20px Arial';
    context.fillText('C para Continuar', canvas.width / 2 - 100, canvas.height / 2 + 10);
    context.fillText('R para reiniciar', canvas.width / 2 - 100, canvas.height / 2 + 40);
    context.fillText('Q para Salir', canvas.width / 2 - 80, canvas.height / 2 + 70);
}

function loop() {
    requestAnimationFrame(loop);
    if (++count < 4) {
        return;
    }

    count = 0;

    if (gamePaused) {
        drawPauseMenu();
        return;
    }

    if (gameOver) {
        gameOverTimer += 100;
        if (gameOverTimer >= gameOverDisplayTime) {
            if (gameOverTransition < 1) {
                gameOverTransition += 0.05;
                if (gameOverTransition >= 1) {
                    gameOverTransition = 1;
                }
            }
        }
        drawGameOver();
        return;
    }

    if (score > lastScoreIncrement && score % 5 === 0) {
        lastScoreIncrement = score;
        currentSpeed += speedIncrement;
    }

    context.fillStyle = 'black';
    context.fillRect(0, 0, canvas.width, canvas.height);

    snake.x += snake.dx;
    snake.y += snake.dy;

    if (snake.x < 0) {
        snake.x = canvas.width - grid;
    } else if (snake.x >= canvas.width) {
        snake.x = 0;
    }

    if (snake.y < 0) {
        snake.y = canvas.height - grid;
    } else if (snake.y >= canvas.height) {
        snake.y = 0;
    }

    snake.cells.unshift({ x: snake.x, y: snake.y });

    if (snake.cells.length > snake.maxCells) {
        snake.cells.pop();
    }

    context.fillStyle = 'red';
    context.fillRect(apple.x, apple.y, grid - 1, grid - 1);

    context.fillStyle = 'green';
    snake.cells.forEach(function(cell, index) {
        context.fillRect(cell.x, cell.y, grid - 1, grid - 1);

        if (cell.x === apple.x && cell.y === apple.y) {
            snake.maxCells++;
            score++;

            if (score > bestScore) {
                bestScore = score;
            }

            apple.x = getRandomInt(0, 25) * grid;
            apple.y = getRandomInt(0, 25) * grid;
        }

        for (var i = index + 1; i < snake.cells.length; i++) {
            if (cell.x === snake.cells[i].x && cell.y === snake.cells[i].y) {
                gameOver = true;
                gameOverTransition = 0;
                gameOverTimer = 0;
            }
        }
    });

    drawScore();
}

function restartGame() {
    snake.x = 160;
    snake.y = 160;
    snake.cells = [];
    snake.maxCells = 4;
    snake.dx = grid;
    snake.dy = 0;
    score = 0;
    gamePaused = false;
    gameOver = false;
    gameOverTransition = 0;
    currentSpeed = baseSpeed;
    lastScoreIncrement = 0;
    gameOverTimer = 0;

    apple.x = getRandomInt(0, 25) * grid;
    apple.y = getRandomInt(0, 25) * grid;
}

document.addEventListener('keydown', function(e) {
    if (gameOver) {
        if (e.which === 82) {
            restartGame();
        }
        return;
    }

    if (gamePaused) {
        if (e.which === 67) {
            gamePaused = false;
        } else if (e.which === 82) {
            restartGame();
        } else if (e.which === 81) {
            console.log('Salir del juego');
        }
        return;
    }

    if (e.which === 27) {
        gamePaused = true;
        return;
    }

    if (!gameOver) {
        if (e.which === 37 && snake.dx === 0) {
            snake.dx = -grid;
            snake.dy = 0;
        } else if (e.which === 38 && snake.dy === 0) {
            snake.dy = -grid;
            snake.dx = 0;
        } else if (e.which === 39 && snake.dx === 0) {
            snake.dx = grid;
            snake.dy = 0;
        } else if (e.which === 40 && snake.dy === 0) {
            snake.dy = grid;
            snake.dx = 0;
        }
    }
});

loop();



