var sliderPicker = new iro.ColorPicker("#sliderPicker", {
    width: 250,
    color: "rgb(255,0,0)",
    borderWidth: 1,
    borderColor: "#fff",
    layout: [
        {
            component: iro.ui.Slider,
            options: {
                sliderType: 'hue'
            }
        },
        {
            component: iro.ui.Slider,
            options: {
                sliderType: 'saturation'
            }
        },
        {
            component: iro.ui.Slider,
            options: {
                sliderType: 'value'
            }
        },
    ]
});

var lastMove = 0;
setInterval(function() {
    // Call a function repetatively with 2 Second interval
    getColorData();
    getLedData();
}, 1000); //2000mSeconds update rate

sliderPicker.on('input:change', function(color) {

    if(Date.now() - lastMove > 40) {
        var xhttp = new XMLHttpRequest();
        var hex = sliderPicker.color.hexString;
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

function getColorData() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            sliderPicker.color.hexString = "#" + this.responseText;
        }
    };
    xhttp.open("GET", "updateColor", true);
    xhttp.send();
}

function getLedData() {
    var xhttp = new XMLHttpRequest();
    var checkBox = document.getElementById("powerSwitch");
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            if(this.responseText=="1") {
                checkBox.checked = true;
            }else
            {
                checkBox.checked = false;
            }

        }
    };
    xhttp.open("GET", "updateLed", true);
    xhttp.send();
}