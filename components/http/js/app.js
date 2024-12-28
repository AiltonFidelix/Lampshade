$(document).ready(function () {

    console.log("Starting...");

    resetSlider();

    $("#sliderInput").on("input", function () {
        handleSlider($(this).val());
    });
});

function resetSlider() {
    $('#sliderInput').val(0);
    $('#sliderValue').text("0%");
}

function handleSlider(value) {
    $('#sliderValue').text(`${value}%`);
}
