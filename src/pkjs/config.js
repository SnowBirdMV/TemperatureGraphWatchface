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
            "defaultValue": "0xFFFFFF",
            "label": "Grid Color"
        },
        {
            "type": "color",
            "messageKey": "BatteryChargeColor",
            "defaultValue": "0xFFFFFF",
            "label": "Battery Charge Color"
        },
        {
            "type": "color",
            "messageKey": "TempLineColor",
            "defaultValue": "0xFFFFFF",
            "label": "Temprature line color"
        },
        {
            "type": "color",
            "messageKey": "popGraphColor",
            "defaultValue": "0xFFFFFF",
            "label": "Precipitation Graph Color"
        },
        {
            "type": "color",
            "messageKey": "BatteryUnchargedColor",
            "defaultValue": "0xFFFFFF",
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