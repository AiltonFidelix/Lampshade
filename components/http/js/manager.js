$(document).ready(function () {
    displayWiFiNetworks();
});

$(document).on("click","#btnScan", function () {
    displayWiFiNetworks();
});

$(document).on("click",".wifi-item", function () {
    const id = $(this).attr("id").split("-")[1];
    togglePasswordInput(id);
});

$(document).on("click",".submit-password-btn", function () {
    const id = $(this).attr("id").split("-")[1];
    submitCredentials(id);
});

function displayWiFiNetworks() {

    $("#subtitle").text("Loading Wi-Fi networks...");
    $("#btnScan").hide();
    $("#wifiList").empty();

    $.getJSON("/networks.json", function (data) {
        const wifiListElement = document.getElementById("wifiList");

        let wifiNetworks = data.networks;
        
        if (wifiNetworks.length > 0) {
            let id = 0;

            $("#subtitle").text("Available Wi-Fi networks");

            wifiNetworks.forEach(network => {
                const listItem = document.createElement("li");
        
                listItem.innerHTML = `
                    <div id="wifiitem-${id}" class="wifi-item">
                        <button id="btnWiFi-${id}" class="show-password-btn"><strong>${network.ssid}</strong></button>
                        <span class="signal">${network.rssi}</span>
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
        else {
            $("#subtitle").text("No one network was found!");
        }

        $("#btnScan").show();
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
    
    const json = {
        ssid: $(`#btnWiFi-${id}`).text(),
        pass: $(`#inputPass-${id}`).val()
    };

    $.ajax({
        url: "/connect.json",
        dataType: "json",
        method: "POST",
        cache: false,
        headers: { "data": JSON.stringify(json) },
        data: ""
    });

    window.alert("Credentials submitted!");
}
