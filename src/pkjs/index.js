// Import the Clay package
var Clay = require('pebble-clay');
// Load our Clay configuration file
var clayConfig = require('./config');
// Initialize Clay
var clay = new Clay(clayConfig);

var messageKeys = require('message_keys');


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
	console.log("Hello");
	
	var masterkeyAPIKey = localStorage.getItem("configuredKey");
	console.log(localStorage.getItem("weatherProvider"));
	if (localStorage.getItem("weatherProvider") == "WeatherUnderground"){
		
		var WUndergroundurl = "http://api.wunderground.com/api/" + myAPIKey + "/hourly/geolookup/q/" + pos.coords.latitude + "," + pos.coords.longitude + ".json";
	console.log(WUndergroundurl);

	// Send request to OpenWeatherMap
	xhrRequest(WUndergroundurl, 'GET',
						 
		function(responseText) {
			// responseText contains a JSON object with weather info
			var json = JSON.parse(responseText);

			// Temperature in Kelvin requires adjustment
			//console.log(json.hourly_forecast[0].temp.english);
			var dictionary = {};
            var humidity = parseInt(json.hourly_forecast[0].humidity);
            dictionary["humidity"] = humidity;
			for (var i = 0; i < 20; i++){
				var temperature = Math.round(json.hourly_forecast[i].temp.english);
				//console.log("Temperature is " + temperature);
				
				// Conditions
				var conditions = json.hourly_forecast[i].condition;
				//console.log(json.hourly_forecast[i].icon);
				var pop = parseInt(json.hourly_forecast[i].pop);
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
	
	
	
	else if (localStorage.getItem("weatherProvider") == "DarkSky"){
	// Construct URL
	var DarkSkyurl = "https://api.darksky.net/forecast/" + localStorage.getItem("DarkSkyAPIKey") + "/" + pos.coords.latitude + "," + pos.coords.longitude;
	console.log(DarkSkyurl);

	// Send request to DarkSky
	xhrRequest(DarkSkyurl, 'GET',
						 
		function(responseText) {
            var TEMP_DATA_POINTS = 20;
            var POP_DATA_POINTS = 20;
            var WIND_DATA_POINTS = 20;
			// responseText contains a JSON object with weather info
			var json = JSON.parse(responseText);

			var dictionary = {};
            var humidity = parseFloat(json.currently.humidity) * 100;
            dictionary.humidity = humidity;
            console.log("Humidity is " + humidity * 100);
            var conditions = json.currently.summary;
            dictionary.cond0 = conditions;
            var curentWind = parseFloat(json.hourly.data[0].windSpeed);
            dictionary.CurentWindData = curentWind;
            var CurentWindDirection1 = parseFloat(json.hourly.data[0].windBearing);
            dictionary.CurentWindDirection = CurentWindDirection1;
            console.log("Wind direction is " + CurentWindDirection1);
			var i = 0;
			var temperature;
            if(parseFloat(localStorage.getItem("celcius"))){
				for ( i = 0; i < TEMP_DATA_POINTS; i++){
				temperature = Math.round(json.hourly.data[i].temperature);
				temperature = (temperature-32) * (5/7);
				dictionary["temp" + i] = temperature;
			}
					
			}
			else{
				for (i = 0; i < TEMP_DATA_POINTS; i++){
				temperature = Math.round(json.hourly.data[i].temperature);
				dictionary["temp" + i] = temperature;
			}
			}
			
            
            for (var j = 0; j < POP_DATA_POINTS; j++){
				var pop = parseFloat(json.hourly.data[j].precipProbability) * 100;
				dictionary["pop" + j] = pop;
			}
            for (var k = 0; j < POP_DATA_POINTS; k++){
				var wind = parseFloat(json.hourly.data[k].windSpeed);
				dictionary["wind" + k] = wind;
			}
            dictionary.inboxHandler = 1;
            
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
}

 Pebble.addEventListener("webviewclosed", function(e){
    //Use the provided Master Key PIN to get relevant API keys, 
    //then store in localstorage.
    var claySettings = clay.getSettings(e.response);

    var email = claySettings[messageKeys.MasterKeyEmail];
    var PIN = claySettings[messageKeys.MasterKeyPIN];
	 console.log("Email is: " + email + ", PIN is " + PIN);
	console.log("Configured to use the " + claySettings[messageKeys.WeatherProvider] + " API");
	 localStorage.setItem("weatherProvider", claySettings[messageKeys.WeatherProvider]);
	 localStorage.setItem("DarkSkyAPIKey", claySettings[messageKeys.ManualDarkSkyAPIKey]);
	 localStorage.setItem("WetherUndergroundAPIKey", claySettings[messageKeys.ManualWUndergroundAPIKey]);
	localStorage.setItem("celcius", claySettings[messageKeys.CelciusToggle]);
    var xhr = new XMLHttpRequest();
    var MasterKeyurl = "https://pmkey.xyz/search/?email=" + email + "&pin=" + PIN;
	 console.log(MasterKeyurl);
  
    xhr.open("GET", MasterKeyurl, true);
  
    xhr.onreadystatechange = function(){
      if(xhr.readyState == 4 && xhr.status == 200){
        var result = JSON.parse(xhr.responseText);
        if(result.success && result.keys.weather.wu !== ""){
          localStorage.setItem("wuKey", result.keys.weather.wu);
            console.log(result.keys.weather.wu);
        }
		  else if (!result.success){
			console.log("Failed to fetch MasterKey data");
		}
		  if (result.success){
			  if (claySettings[messageKeys.WeatherProvider] == "WeatherUnderground"){
				  localStorage.setItem("configuredKey", result.keys.weather.wu);
			  }
			  else if(claySettings[messageKeys.WeatherProvider] == "DarkSky"){
				  //do nothing for now because masterkey does not support dark sky.
			  }
			  
		  }
      }
		
    };
  
    xhr.send();
     
  }
                         );

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
		//getWeather();
	}
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
	function(e) {
		console.log("AppMessage received!");
		getWeather();
	}                     
);