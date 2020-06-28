var colorWheel = new iro.ColorPicker("#colorWheelDemo", {

});
var lastMove = 0;
setInterval(function() {
    // Call a function repetatively with 2 Second interval
    // getData();
}, 1000); //2000mSeconds update rate
colorWheel.on('input:change', function(color) {

    if(Date.now() - lastMove > 40) {
        var xhttp = new XMLHttpRequest();
        var hex = colorWheel.color.hexString;
        var hexCode = hex.substr(1);
        console.log(hex);
        xhttp.open("GET", "color?hex=" + hexCode, true);
        xhttp.send();
        lastMove = Date.now();

    }
});
function sendLED(led) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("LEDState").innerHTML =
                this.responseText;
        }
    };
    xhttp.open("GET", "setLED?LEDstate="+led, true);
    xhttp.send();
}
function animationState(state) {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "animationState?anim="+state, true);
    xhttp.send();

}


function powerLED() {
    var xhttp = new XMLHttpRequest();
    var checkBox = document.getElementById("powerSwitch");
    var led;
    if (checkBox.checked == true){
        led=1;
    } else {
        led=0;
    }
    xhttp.open("GET", "setLED?LEDstate="+led, true);
    xhttp.send();
}



/*function getData() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("ADCValue").innerHTML =
                this.responseText;
        }
    };
    xhttp.open("GET", "readADC", true);
    xhttp.send();
}*/
