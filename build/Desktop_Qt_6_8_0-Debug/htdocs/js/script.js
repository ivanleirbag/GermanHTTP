const titleMessages = ["¡Bienvenido al Arcade", "¡Prepárate para la ganar!", "¿Listo para jugar?"];
let titleIndex = 0;

function changeTitle() {
    document.title = titleMessages[titleIndex];
    titleIndex = (titleIndex + 1) % titleMessages.length;
}


setInterval(changeTitle, 5000);