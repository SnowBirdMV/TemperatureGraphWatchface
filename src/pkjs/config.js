module.exports = [
    {
      "type": "heading",
      "defaultValue": "App Configuration"
  },
  {
      "type": "text",
      "defaultValue": "Here is some introductory text."
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
            "defaultValue": false
        },
        {
            "type": "toggle",
            "messageKey": "BatteryBehindClock",
            "label": "Battery Indicator Behind Clock",
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