const char MAIN_page[] = R"=====(<!DOCTYPE html>

<html>
<head>
    <h1>WIFI LED Control</h1>
</head>
<link rel="stylesheet" type="text/css" href="/cssCode">
<body>
<script src="https://cdn.jsdelivr.net/npm/@jaames/iro@5"></script>
<script src="/javaScript"></script>
<div class="power-switch">
    <input type="checkbox" id="powerSwitch" onclick="powerLED()" />
    <div class="button">
        <svg class="power-off">
            <use xlink:href="#line" class="line" />
            <use xlink:href="#circle" class="circle" />
        </svg>
        <svg class="power-on">
            <use xlink:href="#line" class="line" />
            <use xlink:href="#circle" class="circle" />
        </svg>
    </div>
</div>
<!-- SVG -->
<svg xmlns="http://www.w3.org/2000/svg" style="display: none;">
    <symbol xmlns="http://www.w3.org/2000/svg" viewBox="0 0 150 150" id="line">
        <line x1="75" y1="34" x2="75" y2="58"/>
    </symbol>
    <symbol xmlns="http://www.w3.org/2000/svg" viewBox="0 0 150 150" id="circle">
        <circle cx="75" cy="80" r="35"/>
    </symbol>
</svg>

<div class="wheel" id="colorWheelDemo"></div>

<div>
    <label class="container">Still
        <input type="radio" checked="checked" name="radio" value="none" onchange="animationState(this.value)">
        <span class="checkmark"></span>
    </label>
    <label class="container">Rainbow
        <input type="radio" name="radio" value="rain" onchange="animationState(this.value)">
        <span class="checkmark"></span>
    </label>
    <label class="container">Fade IN OUT
        <input type="radio" name="radio" value="fade" onchange="animationState(this.value)">
        <span class="checkmark"></span>
    </label>
    <label class="container">Color wipe
        <input type="radio" name="radio" value="wipe" onchange="animationState(this.value)">
        <span class="checkmark"></span>
    </label>
</div>
</body>
</html>)=====";
const char javaScript[] = R"=====(var colorWheel = new iro.ColorPicker("#colorWheelDemo", {

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
)=====";
const char cssCode[] = R"=====(.container {
    display: block;
    position: relative;
    padding-left: 35px;
    margin-bottom: 12px;
    cursor: pointer;
    font-size: 22px;
    -webkit-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
}

/* Hide the browser's default radio button */
.container input {
    position: absolute;
    opacity: 0;
    cursor: pointer;
    height: 0;
    width: 0;
}

/* Create a custom radio button */
.checkmark {
    position: absolute;
    top: 0;
    left: 0;
    height: 25px;
    width: 25px;
    background-color: #eee;
    border-radius: 50%;
}

/* On mouse-over, add a grey background color */
.container:hover input ~ .checkmark {
    background-color: #ccc;
}

/* When the radio button is checked, add a blue background */
.container input:checked ~ .checkmark {
    background-color: #2196F3;
}

/* Create the indicator (the dot/circle - hidden when not checked) */
.checkmark:after {
    content: "";
    position: absolute;
    display: none;
}

/* Show the indicator (dot/circle) when checked */
.container input:checked ~ .checkmark:after {
    display: block;
}

/* Style the indicator (dot/circle) */
.container .checkmark:after {
    top: 9px;
    left: 9px;
    width: 8px;
    height: 8px;
    border-radius: 50%;
    background: white;
}


.power-switch {
    --color-invert: #ffffff;
    --width: 150px;
    --height: 150px;
    position: relative;
    display: flex;
    justify-content: center;
    align-items: center;
    width: var(--width);
    height: var(--height);
}
.power-switch .button {
    width: 100%;
    height: 100%;
    display: flex;
    justify-content: center;
    align-items: center;
    position: relative;
}
.power-switch .button:after {
    content: "";
    width: 100%;
    height: 100%;
    position: absolute;
    background: radial-gradient(circle closest-side, var(--color-invert), transparent);
    filter: blur(20px);
    opacity: 0;
    transition: opacity 1s ease, transform 1s ease;
    transform: perspective(1px) translateZ(0);
    backface-visibility: hidden;
}
.power-switch .button .power-on, .power-switch .button .power-off {
    height: 100%;
    width: 100%;
    position: absolute;
    z-index: 1;
    fill: none;
    stroke: var(--color-invert);
    stroke-width: 8px;
    stroke-linecap: round;
    stroke-linejoin: round;
}
.power-switch .button .power-on .line, .power-switch .button .power-off .line {
    opacity: 0.2;
}
.power-switch .button .power-on .circle, .power-switch .button .power-off .circle {
    opacity: 0.2;
    transform: rotate(-58deg);
    transform-origin: center 80px;
    stroke-dasharray: 220;
    stroke-dashoffset: 40;
}
.power-switch .button .power-on {
    filter: drop-shadow(0px 0px 6px rgba(255, 255, 255, 0.8));
}
.power-switch .button .power-on .line {
    opacity: 0;
    transition: opacity 0.3s ease 1s;
}
.power-switch .button .power-on .circle {
    opacity: 1;
    stroke-dashoffset: 220;
    transition: transform 0s ease, stroke-dashoffset 1s ease 0s;
}
.power-switch input {
    position: absolute;
    height: 100%;
    width: 100%;
    z-index: 2;
    cursor: pointer;
    opacity: 0;
}
.power-switch input:checked + .button:after {
    opacity: 0.15;
    transform: scale(2) perspective(1px) translateZ(0);
    backface-visibility: hidden;
    transition: opacity 0.5s ease, transform 0.5s ease;
}
.power-switch input:checked + .button .power-on, .power-switch input:checked + .button .power-off {
    animation: click-animation 0.3s ease forwards;
    transform: scale(1);
}
.power-switch input:checked + .button .power-on .line, .power-switch input:checked + .button .power-off .line {
    animation: line-animation 0.8s ease-in forwards;
}
.power-switch input:checked + .button .power-on .circle, .power-switch input:checked + .button .power-off .circle {
    transform: rotate(302deg);
}
.power-switch input:checked + .button .power-on .line {
    opacity: 1;
    transition: opacity 0.05s ease-in 0.55s;
}
.power-switch input:checked + .button .power-on .circle {
    transform: rotate(302deg);
    stroke-dashoffset: 40;
    transition: transform 0.4s ease 0.2s, stroke-dashoffset 0.4s ease 0.2s;
}
@keyframes line-animation {
    0% {
        transform: translateY(0);
    }
    10% {
        transform: translateY(10px);
    }
    40% {
        transform: translateY(-25px);
    }
    60% {
        transform: translateY(-25px);
    }
    85% {
        transform: translateY(10px);
    }
    100% {
        transform: translateY(0px);
    }
}
@keyframes click-animation {
    0% {
        transform: scale(1);
    }
    50% {
        transform: scale(0.9);
    }
    100% {
        transform: scale(1);
    }
}
body {
    background: #232323;
    height: 85vh;
    font: 400 16px 'Poppins', sans-serif;
    display: flex;
    flex-direction: column;
    justify-content: center;
    align-items: center;
}
)=====";