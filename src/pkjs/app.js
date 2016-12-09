// Import the Clay package
var Clay = require('pebble-clay');
// Load our Clay configuration file
var clayConfig = require('./config');
// Initialize Clay
var clay = new Clay(clayConfig);

var messageKeys = require('message_keys');


var myAPIKey2 = 'a26a735eab948fcd';
var myAPIKey = '';

var xhrRequest = function (url, type, callback) {
	var xhr = new XMLHttpRequest();
	xhr.onload = function () {
		callback(this.responseText);
	};
	xhr.open(type, url);
	xhr.send();
};

function locationSuccess(pos) {
	var errorDict = {};
	var keyLocation;
	//checkMasterKey();
	
	console.log(localStorage.getItem("weatherProvider"));
	console.log(!localStorage.getItem("WetherUndergroundAPIKey"));
	console.log(localStorage.getItem("MasterKeyWUnderground"));
	if (localStorage.getItem("weatherProvider") == "WeatherUnderground"){
		if (localStorage.getItem("WetherUndergroundAPIKey") == "undefined" && localStorage.getItem("MasterKeyWUnderground") == "undefined"){
			console.log("No WUnderground API key");
				errorDict.errorCode = "No Weather Underground API key.  Please configure one in the watchface settings.";
				Pebble.sendAppMessage(errorDict,
				function(e) {
					console.log("Error log sent to Pebble successfully!");
				},
				function(e) {
					console.log("Error sending error log to Pebble!");
				}
				);
				return;
		}
	else if(localStorage.getItem("WetherUndergroundAPIKey") != "undefined" && localStorage.getItem("WetherUndergroundAPIKey")){
		console.log("Getting API key from manually defined key");
		myAPIKey = localStorage.getItem("WetherUndergroundAPIKey");
		keyLocation = 1;
	}
	else if(localStorage.getItem("MasterKeyWUnderground") != "undefined" && localStorage.getItem("MasterKeyWUnderground")){
		console.log("Getting API key from MasterKey");
		myAPIKey = localStorage.getItem("MasterKeyWUnderground");
		keyLocation = 2;
	}
		else{
			console.log("couldnt get the key from either");
			keyLocation = 3;
		}
		
		var WUndergroundurl = "http://api.wunderground.com/api/" + myAPIKey + "/hourly/geolookup/q/" + pos.coords.latitude + "," + pos.coords.longitude + ".json";
	console.log(WUndergroundurl);
		if (!localStorage.getItem("harglefarglebargle")){
			console.log("Hello there");
		}

	// Send request to OpenWeatherMap
	xhrRequest(WUndergroundurl, 'GET',
						 
		function(responseText) {
			var dictionary = {};
			if (keyLocation == 3){
				errorDict.errorCode = "Please configure a Weather Underground API key in the watchface settings";
				Pebble.sendAppMessage(errorDict,
				function(e) {
					console.log("Error log sent to Pebble successfully!");
				},
				function(e) {
					console.log("Error sending error log to Pebble!");
				}
				);
				return;
			}
			
			// responseText contains a JSON object with weather info
			var json = JSON.parse(responseText);
			
			try {
				var error = json.response.error.type;
				console.log("Invalid WUnderground API key");
				if(keyLocation == 2){
					errorDict.errorCode = "Your Weather Underground API key in your MasterKey account is invalid. Please fix and re-save settings.";
				}
				else if(keyLocation == 1){
					errorDict.errorCode = "Your manually defined Weather Underground API key is invalid.";
				}
				else{
					errorDict.errorCode = "Invalid Weather Underground API key.  Please configure one in the watchface settings.";
				}
				Pebble.sendAppMessage(errorDict,
				function(e) {
					console.log("Error log sent to Pebble successfully!");
				},
				function(e) {
					console.log("Error sending error log to Pebble!");
				}
				);
				return;
			}
			catch (e){
				console.log(e);
				console.log("Correctly formatted WUnderground URL");
			}
			
			var windSpeed = parseFloat(json.hourly_forecast[0].wspd.english);
			var windDirection = parseFloat(json.hourly_forecast[0].wdir.degrees);
			dictionary.CurentWindData = windSpeed;
			dictionary.CurentWindDirection = windDirection;

			// Temperature in Kelvin requires adjustment
			//console.log(json.hourly_forecast[0].temp.english);
			
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
		
		if(localStorage.getItem("DarkSkyAPIKey") != "undefined" && localStorage.getItem("DarkSkyAPIKey")){
			myAPIKey = localStorage.getItem("DarkSkyAPIKey");
		}
		else{
			errorDict.errorCode = "Please configure a Dark Sky API key in the watchface settings";
				Pebble.sendAppMessage(errorDict,
				function(e) {
					console.log("Error log sent to Pebble successfully!");
				},
				function(e) {
					console.log("Error sending error log to Pebble!");
				}
				);
			return;
		}
	// Construct URL
	var DarkSkyurl = "https://api.darksky.net/forecast/" + localStorage.getItem("DarkSkyAPIKey") + "/" + pos.coords.latitude + "," + pos.coords.longitude;
	console.log(DarkSkyurl);

	// Send request to DarkSky
	xhrRequest(DarkSkyurl, 'GET',
						 
		function(responseText) {
			try{
				var jsonTest = JSON.parse(responseText);
			} catch(e){
				console.log(e);
				console.log("Invalid Dark Sky API key");
				var errorDict = {};
				errorDict.errorCode = "Your manually defined Dark Sky API key is invalid.";
				Pebble.sendAppMessage(errorDict,
				function(e) {
					console.log("Error log sent to Pebble successfully!");
				},
				function(e) {
					console.log("Error sending error log to Pebble!");
				}
			);
				return;
			}
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

function checkMasterKey(){
	var xhr = new XMLHttpRequest();
    var MasterKeyurl = "https://pmkey.xyz/search/?email=" + localStorage.getItem("MasterKeyEmail") + "&pin=" + localStorage.getItem("MasterKeyPin");
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
			  //store keys
			  localStorage.setItem("MasterKeyWUnderground", result.keys.weather.wu);
		  }
      }
		
    };
  
    xhr.send();
}

 Pebble.addEventListener("webviewclosed", function(e){
    //Use the provided Master Key PIN to get relevant API keys, 
    //then store in localstorage.
    var claySettings = clay.getSettings(e.response);

    var email = claySettings[messageKeys.MasterKeyEmail];
    var PIN = claySettings[messageKeys.MasterKeyPIN];
	console.log("Email is: " + email + ", PIN is " + PIN);
	 localStorage.setItem("MasterKeyEmail", email);
	 localStorage.setItem("MasterKeyPin", PIN);
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
			  //store keys
			  localStorage.setItem("MasterKeyWUnderground", result.keys.weather.wu);
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