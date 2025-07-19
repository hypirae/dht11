# DHT11 Temperature & Humidity Sensor App for Flipper Zero

A professional-grade DHT11 sensor application for the Flipper Zero that provides accurate temperature and humidity readings with Heat Index calculation and comprehensive debugging capabilities.

## Features

### 📊 **Core Functionality**
- **Real-time sensor readings** with DHT11 temperature and humidity sensor
- **Heat Index calculation** using the official NOAA formula
- **Temperature unit localization** - automatically displays in °C or °F based on Flipper's system preference
- **High-precision timing** using DWT cycle counter for accurate microsecond-level protocol timing
- **Comprehensive error handling** with sensor validation and reasonable range checking

### 🔧 **Advanced Features**
- **Scene-based navigation** using Flipper's standard scene manager
- **Debug mode** with detailed protocol analysis and bit-level timing information
- **Visual feedback** with LED notifications during sensor operations
- **Connection diagnostics** with wiring information and troubleshooting guides
- **Checksum verification** to ensure data integrity

### 🎨 **User Interface**
- **Clean, intuitive interface** with proper text positioning and no clipping issues
- **Right-justified Heat Index display** positioned alongside temperature and humidity readings
- **Error handling with helpful messages** and connection troubleshooting information
- **Responsive button controls** with immediate visual feedback

## Hardware Setup

### Required Components
- **Flipper Zero**
- **DHT11 Temperature & Humidity Sensor**
- **Jumper wires** (3 connections needed)

### Wiring Diagram
```
DHT11 Sensor    Flipper Zero GPIO
─────────────   ──────────────────
VCC       →     3.3V (Pin 9)
GND       →     GND (Pin 8 or 11)  
DATA      →     C0 (Pin 16)
```

### Pin Configuration
- **Power:** 3.3V (3.3V pin on GPIO header)
- **Ground:** Any GND pin on GPIO header
- **Data:** GPIO C0 (Pin 16) - configurable in `sensor.h`

## Installation

### Method 1: Pre-compiled FAP
1. Download the latest `.fap` file from the [releases page](https://github.com/Hypirae/dht11/releases)
2. Copy the `.fap` file to your Flipper Zero's `apps` folder via qFlipper or SD card
3. Restart your Flipper Zero or refresh the applications list

### Method 2: Build from Source
1. Clone the repository:
   ```bash
   git clone https://github.com/Hypirae/dht11.git
   cd dht11
   ```

2. Set up the uFBT build environment (if not already installed):
   ```bash
   pip install ufbt
   ufbt update
   ```

3. Build the application:
   ```bash
   ufbt
   ```

4. Flash to Flipper Zero:
   ```bash
   ufbt launch
   ```

## Usage

### Basic Operation
1. **Connect the DHT11 sensor** according to the wiring diagram above
2. **Launch the app** from the Flipper Zero applications menu
3. **Navigate to "Read Sensor"** and press the OK button to take a reading
4. **View results** including temperature, humidity, and calculated Heat Index

### Menu Options
- **📡 Read Sensor** - Take temperature, humidity, and Heat Index readings
- **🔧 Debug Sensor** - Advanced diagnostics with detailed timing analysis
- **ℹ️ About** - Connection information and troubleshooting guide

### Debug Mode
The debug mode provides detailed information for troubleshooting:
- **Pin state monitoring** throughout the communication process
- **Bit-level timing analysis** with microsecond precision
- **Protocol step verification** for each phase of DHT11 communication
- **Raw data display** with checksum verification details
- **Timing threshold analysis** to help optimize sensor readings

## Technical Details

### DHT11 Protocol Implementation
- **Start signal:** 20ms LOW pulse followed by 30μs HIGH
- **Response detection:** Waits for 80μs LOW + 80μs HIGH response
- **Data reading:** 40 bits (5 bytes) with precise timing measurement
- **Bit discrimination:** Logic '1' (~70μs) vs Logic '0' (~26-28μs) using 40μs threshold
- **Error handling:** Timeout detection, checksum verification, range validation

### Heat Index Calculation
Uses the official NOAA Heat Index formula:
```
HI = -42.379 + 2.04901523×T + 10.14333127×RH - 0.22475541×T×RH 
     - 6.83783×10⁻³×T² - 5.481717×10⁻²×RH² + 1.22874×10⁻³×T²×RH 
     + 8.5282×10⁻⁴×T×RH² - 1.99×10⁻⁶×T²×RH²
```
Where T = temperature in °F, RH = relative humidity %

**Note:** Heat Index is only calculated for temperatures ≥80°F (26.7°C) and humidity ≥40%. Below these thresholds, the air temperature is displayed.

### Architecture
- **Scene Management:** Uses Flipper's standard scene manager for navigation
- **Modular Design:** Separate files for each scene and sensor functionality  
- **Error Handling:** Comprehensive validation with user-friendly error messages
- **Thread Safety:** Critical sections during timing-sensitive sensor communication
- **Memory Management:** Efficient use of stack space with proper cleanup

## Troubleshooting

### Common Issues
1. **"Sensor Error" message:**
   - Check wiring connections (VCC, GND, DATA)
   - Ensure DHT11 is receiving 3.3V power
   - Verify data line is connected to GPIO C0 (Pin 16)

2. **Inconsistent readings:**
   - Wait at least 2 seconds between readings (DHT11 requirement)
   - Check for loose connections
   - Use debug mode to analyze timing issues

3. **No response from sensor:**
   - Verify power supply (3.3V)
   - Check ground connection
   - Ensure DHT11 is not damaged

### Debug Information
Use the "Debug Sensor" option to get detailed diagnostics including:
- Pin state monitoring
- Communication timing analysis  
- Raw data bytes and checksum verification
- Bit-level timing measurements

## Development

### Building
```bash
# Clean build
ufbt -c

# Build application
ufbt

# Launch on connected Flipper Zero
ufbt launch

# Flash firmware and app
ufbt flash_usb
```

### Code Structure
```
├── application.fam          # App manifest
├── dht11.c                 # Main application entry point
├── app.h                   # Application structure definitions
├── sensor.c/.h             # DHT11 sensor driver implementation
├── scenes.c/.h             # Scene management and definitions
├── main_menu.c/.h          # Main menu scene
├── read_sensor_scene.c/.h  # Sensor reading scene
├── debug_scene.c/.h        # Debug analysis scene
├── about_scene.c/.h        # About/help scene
├── dht11.png              # Application icon
└── images/                # Additional assets
```

### Contributing
1. Fork the repository
2. Create a feature branch: `git checkout -b feature-name`
3. Make your changes with proper documentation
4. Test thoroughly on hardware
5. Submit a pull request

## Version History

### v1.0.0
- Initial release with basic DHT11 functionality
- Scene-based navigation using Flipper's standard scene manager
- Heat Index calculation with NOAA formula
- Temperature unit localization (°C/°F)
- Comprehensive debug mode with microsecond-precision timing
- Professional UI with proper text positioning
- Full error handling and sensor validation

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Author

**Hypirae** - [GitHub](https://github.com/Hypirae)

## Acknowledgments

- Flipper Zero community for development tools and documentation
- NOAA for the official Heat Index calculation formula
- DHT11 sensor specifications and protocol documentation

---

## Support

If you encounter issues or have suggestions:
1. Check the troubleshooting section above
2. Use debug mode for detailed diagnostics
3. Open an issue on [GitHub](https://github.com/Hypirae/dht11/issues)

**Happy sensing! 🌡️💧**
