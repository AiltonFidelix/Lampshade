$(document).ready(function () {

    displayWiFiNetworks();

    $(".wifi-item").on("click", function() {
        const id = $(this).attr("id").split("-")[1];
        togglePasswordInput(id);
    });

    $(".submit-password-btn").on("click", function() {
        const id = $(this).attr("id").split("-")[1];
        submitCredentials(id);
    });

});

// TODO: remove later, just for test
const wifiNetworks = [
    { name: "Home Network", signalStrength: "Strong" },
    { name: "Public Network", signalStrength: "Weak" },
    { name: "Office WiFi", signalStrength: "Strong" },
    { name: "Hotel WiFi", signalStrength: "Moderate" },
];

function displayWiFiNetworks() {
    
    const wifiListElement = document.getElementById("wifiList");

    let id = 0;

    // TODO: get wifiNetworks from device

    wifiNetworks.forEach(network => {
        const listItem = document.createElement("li");

        listItem.innerHTML = `
            <div id="wifiitem-${id}" class="wifi-item">
                <button id="btnWiFi-${id}" class="show-password-btn"><strong>${network.name}</strong></button>
                <span class="signal">${network.signalStrength}</span>
            </div>
            <div id="divPass-${id}" class="password-div">
                <input id="inputPass-${id}" class="password-input" type="password" placeholder="Enter WiFi password" />
                <button id="btnConnect-${id}" class="submit-password-btn">Connect</button>
            </div>
        `;

        id++;

        wifiListElement.appendChild(listItem);
    });
}

function togglePasswordInput(id) {

    $(".password-div").slideUp();

    const passwordDiv = document.getElementById(`divPass-${id}`);
    
    if (passwordDiv.style.display === "block") {
        passwordDiv.style.display = "none";
    } else {
        passwordDiv.style.display = "block";
    }
}

function submitCredentials(id) {
    const ssid = $(`#btnWiFi-${id}`).text();
    const password = $(`#inputPass-${id}`).val();

    // TODO: handle connection
}
