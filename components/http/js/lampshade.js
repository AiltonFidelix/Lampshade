let timeout;

$(document).ready(function () {

    initSlider();

    $("#sliderInput").on("input", function () {
        handleSlider($(this).val());
    });
});

function initSlider() {
    $.getJSON("/getduty.json", function (data) {

        let current = data.duty;

        $('#sliderInput').val(current);
        $('#sliderValue').text(`${current}%`);
    });
}

function handleSlider(value) {
    $('#sliderValue').text(`${value}%`);

    const json = {
        duty: Number(value)
    };

    clearTimeout(timeout);

    timeout = setTimeout(() => {
        $.ajax({
            url: "/setduty.json",
            dataType: "json",
            method: "POST",
            cache: false,
            headers: { "data": JSON.stringify(json) },
            data: ""
        });
    
    }, 1000);
}
