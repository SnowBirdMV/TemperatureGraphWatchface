var myAPIKey = 'a26a735eab948fcd';

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
	var url = "http://api.wunderground.com/api/" + myAPIKey + "/hourly/geolookup/q/" + pos.coords.latitude + "," + pos.coords.longitude + ".json";
	console.log(url);

	// Send request to OpenWeatherMap
	xhrRequest(url, 'GET',
						 
		function(responseText) {
			// responseText contains a JSON object with weather info
			var json = JSON.parse(responseText);

			// Temperature in Kelvin requires adjustment
			console.log(json.hourly_forecast[0].temp.english);
			var dictionary = {};
            var humidity = parseInt(json.hourly_forecast[0].humidity);
            dictionary["humidity"] = humidity;
			for (var i = 0; i < 20; i++){
				var temperature = Math.round(json.hourly_forecast[i].temp.english);
				//console.log("Temperature is " + temperature);
				
				// Conditions
				var conditions = json.hourly_forecast[i].icon;
				console.log(json.hourly_forecast[i].icon);
				var pop = parseInt(json.hourly_forecast[i].pop);
					console.log(pop);
				dictionary["temp" + i] = temperature;
				dictionary["cond" + i] = conditions;
				dictionary["pop" + i] = pop;
                console.log(dictionary["pop" + i]);
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
