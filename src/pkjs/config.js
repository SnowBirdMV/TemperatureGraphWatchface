module.exports = [
{
	"type": "heading",
	"defaultValue": "App Configuration"
},
{
	"type": "submit",
	"defaultValue": "Save Settings"
},
{
	"type": "section",
	"items": [
	{
		"type": "heading",
		"defaultValue": "API Settings"
	},
    {
  "type": "select",
  "messageKey": "WeatherProvider",
  "defaultValue": "Dark Sky",
  "label": "Weather Provider",
		"description": "Choose your favorite weather povider.  NOTE: you will have to provide your own API key for services other than Dark Sky.",
  "options": [
    { 
      "label": "Weather Underground", 
      "value": "WeatherUnderground" 
    },
    { 
      "label": "Dark Sky",
      "value": "DarkSky" 
    }
  ]
},
	{
		"type": "input",
		"messageKey": "ManualDarkSkyAPIKey",
		"defaultValue": "",
		"label": "Dark Sky API Key",
		"attributes": {
			"placeholder": "eg: 12ae7f328bf212248e",
			"limit": 10,
		}
	},
	{
		"type": "input",
		"messageKey": "ManualWUndergroundAPIKey ",
		"defaultValue": "",
		"label": "Weather Underground API Key",
        "description": "",
		"attributes": {
			"placeholder": "eg: 12ae7f328bf212248e",
			"limit": 10,
		}
	}
	]
},
{
	"type": "section",
	"items": [
	{
		"type": "heading",
		"defaultValue": "MasterKey Settings",
		},
	{
  		"type": "text",
  		"defaultValue" : "MasterKey is an alternate method to use your own API keys with Pebble watchfaces.  You can create an account here.  NOTE: This is completly optional.  If you have API keys defined manually above, those will be used instead."
	},
	{
		"type": "input",
		"messageKey": "MasterKeyEmail",
		"defaultValue": "",
		"label": "Email Address",
		"attributes": {
			"placeholder": "eg: name@domain.com",
			"limit": 10,
			"type": "email"
		}
	},
	{
		"type": "input",
		"messageKey": "MasterKeyPIN",
		"defaultValue": "",
		"label": "PIN",
		"attributes": {
			"placeholder": "eg: 12345",
			"limit": 10,
			"type": "number"
		}
	}
	]
},
{
	"type": "section",
	"items": [
	{
		"type": "heading",
		"defaultValue": "Colors"
	},
	{
		"type": "color",
		"messageKey": "BackgroundColor",
		"defaultValue": "0x000000",
		"label": "Background Color"
	},
	{
		"type": "color",
		"messageKey": "GridColor",
		"defaultValue": "0x00FF00",
		"label": "Grid Color"
	},
	{
		"type": "color",
		"messageKey": "BatteryChargeColor",
		"defaultValue": "0x00AA00",
		"label": "Battery Charge Color"
	},
	{
		"type": "color",
		"messageKey": "TempLineColor",
		"defaultValue": "0xFF0000",
		"label": "Temprature line color"
	},
	{
		"type": "color",
		"messageKey": "popGraphColor",
		"defaultValue": "0x0000FF",
		"label": "Precipitation Graph Color"
	},
	{
		"type": "color",
		"messageKey": "BatteryUnchargedColor",
		"defaultValue": "0xAA0000",
		"label": "Battery dead color"
	},
	{
		"type": "color",
		"messageKey": "GraphLineColor",
		"defaultValue": "0xAA0000",
		"label": "Graph Marker Color"
	},
	{
		"type": "color",
		"messageKey": "BatteryOutlineColor",
		"defaultValue": "0x555555",
		"label": "Battery Outline Color"
	},

	]
},
{
	"type": "section",
	"items": [
	{
		"type": "heading",
		"defaultValue": "More Settings"
	},
	{
		"type": "toggle",
		"messageKey": "SecondTick",
		"label": "Enable Seconds",
		"defaultValue": true
	},
	{
		"type": "slider",
		"messageKey": "SecondsCount",
		"defaultValue": 10,
		"label": "Seconds Count",
		"description": "How long to show the seconds counter after you shake the watch.",
		"min": 1,
		"max": 60,
		"step": 1
	},
	{
		"type": "toggle",
		"messageKey": "BatteryBehindClockToggle",
		"label": "Battery Indicator Behind Clock",
		"defaultValue": true
	},
	{
		"type": "toggle",
		"messageKey": "SleepModeToggle",
		"label": "Enable Automatic Sleep Mode",
		"description": "Sleep mode puts the watchface to sleep automatically while you are sleeping to conserve battery power.",
		"defaultValue": true
	}, 
	{
		"type": "toggle",
		"messageKey": "GridColorToggle",
		"label": "Change Grid Color on Disconnect",
		"defaultValue": true
	},
	{
		"type": "toggle",
		"messageKey": "CelciusToggle",
		"label": "Celcius",
		"defaultValue": false
	},
	{
		"type": "toggle",
		"messageKey": "TripleShake",
		"label": "Triple Shake to Force weather update",
		"description" : "Shake the watch three times in rapid succesion to force the watchface to get newer weather data.",
		"defaultValue": false
	}
	]
},
{
	"type": "section",
	"items": [
	{
		"type": "heading",
		"defaultValue": "Text Colors"
	},
	{
		"type": "color",
		"messageKey": "TimeColor",
		"defaultValue": "0XFFFFFF",
		"label": "Time Color"
	},
	{
		"type": "color",
		"messageKey": "DateColor",
		"defaultValue": "0XFFFFFF",
		"label": "Date Color"
	},
	{
		"type": "color",
		"messageKey": "BatteryPercentageColor",
		"defaultValue": "0XFFFFFF",
		"label": "Battery Percentage Color"
	},
	{
		"type": "color",
		"messageKey": "TempNumberColor",
		"defaultValue": "0XFFFFFF",
		"label": "Temp Number Color"
	},
	{
		"type": "color",
		"messageKey": "StepsColor",
		"defaultValue": "0XFFFFFF",
		"label": "Steps Color"
	},
	{
		"type": "color",
		"messageKey": "DataTimeColor",
		"defaultValue": "0XFFFFFF",
		"label": "Data Timer Color"
	},
	{
		"type": "color",
		"messageKey": "BatteryLifeColor",
		"defaultValue": "0XFFFFFF",
		"label": "Battery Life Color"
	},
	{
		"type": "color",
		"messageKey": "HumidityColor",
		"defaultValue": "0XFFFFFF",
		"label": "Humidity Color"
	},
	{
		"type": "color",
		"messageKey": "WeatherColor",
		"defaultValue": "0XFFFFFF",
		"label": "Weather Color"
	},
	]
},
{
	"type": "submit",
	"defaultValue": "Save Settings"
}
];