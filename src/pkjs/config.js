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
      "type": "submit",
      "defaultValue": "Save Settings"
  }
];