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

	// Send request to OpenWeatherMap
	xhrRequest(url, 'GET',
						 
		function(responseText) {
			// responseText contains a JSON object with weather info
			var json = JSON.parse(responseText);

			// Temperature in Kelvin requires adjustment
			//console.log(json.hourly_forecast[0].temp.english);
			var dictionary = {};
            var humidity = parseFloat(json.currently.humidity) * 100;
            dictionary["humidity"] = humidity;
            console.log("Humidity is " + humidity * 100);
			for (var i = 0; i < 20; i++){
				var temperature = Math.round(json.hourly.data[i].temperature);
				//console.log("Temperature is " + temperature);
				
				// Conditions
				var conditions = json.hourly.data[i].summary;
				//console.log(json.hourly_forecast[i].icon);
				var pop = parseFloat(json.hourly.data[i].precipProbability) * 100;
					//console.log(pop);
				dictionary["temp" + i] = temperature;
				dictionary["cond" + i] = conditions;
				dictionary["pop" + i] = pop;
                //console.log(dictionary["pop" + i]);
				//console.log("Conditions are " + conditions);
			}
			
			
			
			// Assemble dictionary using our keys
			

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
