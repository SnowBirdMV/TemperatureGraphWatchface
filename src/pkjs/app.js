var myAPIKey = 'cc1720352689e95736d730b20dfc9288';

var xhrRequest = function (url, type, callback) {
	var xhr = new XMLHttpRequest();
	xhr.onload = function () {
		callback(this.responseText);
	};
	xhr.open(type, url);
	xhr.send();
};

function locationSuccess(pos) {
	// Construct URL
	var url = "https://api.darksky.net/forecast/" + myAPIKey + "/" + pos.coords.latitude + "," + pos.coords.longitude;
	console.log(url);

	// Send request to DarkSky
	xhrRequest(url, 'GET',
						 
		function(responseText) {
            var TEMP_DATA_POINTS = 20;
            var POP_DATA_POINTS = 20;
			// responseText contains a JSON object with weather info
			var json = JSON.parse(responseText);

			var dictionary = {};
            var humidity = parseFloat(json.currently.humidity) * 100;
            dictionary["humidity"] = humidity;
            console.log("Humidity is " + humidity * 100);
            var conditions = json.currently.summary;
            dictionary["cond0"] = conditions;
            
			for (var i = 0; i < TEMP_DATA_POINTS; i++){
				var temperature = Math.round(json.hourly.data[i].temperature);
				dictionary["temp" + i] = temperature;
			}
            
            for (var j = 0; j < POP_DATA_POINTS; j++){
				var pop = parseFloat(json.hourly.data[j].precipProbability) * 100;
				dictionary["pop" + j] = pop;
			}
            dictionary["inboxHandler"] = 1;
            
			// Send to Pebble
			Pebble.sendAppMessage(dictionary,
				function(e) {
					console.log("Weather info sent to Pebble successfully!");
				},
				function(e) {
					console.log("Error sending weather info to Pebble!");
				}
			);
		}      
	);
}

function locationError(err) {
	console.log("Error requesting location!");
}

function getWeather() {
	navigator.geolocation.getCurrentPosition(
		locationSuccess,
		locationError,
		{timeout: 15000, maximumAge: 60000}
	);
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
	function(e) {
		console.log("PebbleKit JS ready!");
        Pebble.sendAppMessage({'JSReady': 1});

		// Get the initial weather
		getWeather();
	}
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
	function(e) {
		console.log("AppMessage received!");
		getWeather();
	}                     
);