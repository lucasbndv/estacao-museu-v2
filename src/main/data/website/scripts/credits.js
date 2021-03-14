function setupEvents() {
    // Menu event
    let menu = document.getElementsByClassName("menu")[0];
    var menu_on = false;
    menu.addEventListener("click", () => {
        let nav = document.getElementById("nav");
        if (menu_on) {
            nav.style.display = "none";
            menu_on = false;
        }
        else {
            nav.style.display = "flex";
            menu_on = true;
        }
    });
}

function main() {
    setupEvents();
}

main();