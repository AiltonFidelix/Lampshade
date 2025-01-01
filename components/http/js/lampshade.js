$(document).ready(function () {

    initSlider();

    $("#sliderInput").on("input", function () {
        handleSlider($(this).val());
    });
});

function initSlider() {

    // TODO: get the current value from MCU
    let current = 0;

    $('#sliderInput').val(current);
    $('#sliderValue').text(`${current}%`);
}

function handleSlider(value) {
    $('#sliderValue').text(`${value}%`);

    // Set the new value in the MCU
}
