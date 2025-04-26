# Bike Signal System - Arduino Project

This project provides an Arduino-based bike signal system with features like brake detection, turn signals, Knight Rider LED effect, and web control via WiFi.

## 1. Hardware Setup:

- **Arduino R4 WiFi** board.
- **LED Matrix**: 16x8 matrix (adjustable for your setup).
  - Connect **Data**, **Clock**, and **Load** pins for the LED matrix.
- **Accelerometer** (e.g., MPU6050):
  - Connect it to the **I2C pins** (SDA, SCL).
  - Replace the accelerometer logic with actual readings from your accelerometer.

## 2. WiFi Setup:

- This code sets up an **Arduino WiFi Access Point** named `BikeSignalNetwork`.
- You can connect your **smartphone** to this WiFi network.
- Once connected, open a **web browser** and visit the following URLs to control the bike signals:
  - `/left` for turning left.
  - `/right` for turning right.
  - `/hazard` for turning on hazard lights.

## 3. Functionality:

- **Brake Detection**:
  - Uses the accelerometer to detect hard braking.
  - Flashes the entire matrix in **red** during hard braking.
  
- **Turn Signals**:
  - Controls **left**, **right**, and **hazard** signals.
  - **Auto-off** after 5 seconds.

- **Knight Rider Effect**:
  - Runs a smooth **LED sweep** across the matrix when the bike is idle.
  
- **Power Saving**:
  - The matrix **dims** when the bike is idle.

## 4. Web Control Interface:

Once connected to `BikeSignalNetwork`, open a web browser and access the following URLs:

- `http://192.168.4.1/left` to turn on the **left signal**.
- `http://192.168.4.1/right` to turn on the **right signal**.
- `http://192.168.4.1/hazard` to turn on the **hazard lights**.

## 5. Adjustments:

- **Brake Sensitivity**:
  - Modify `brakeThreshold` and `brakeDelay` to adjust the sensitivity of brake detection and response.

- **Speed of Knight Rider Effect**:
  - Adjust the `knightDelay` value to control the speed of the moving LED bar.

## 6. Important Notes:

- **WiFi Control**:
  - The Arduino acts as a **server** that your phone connects to via WiFi.
  - If you want to create a more advanced **UI** for the web control, you'll need to create custom **HTML** for the web page.

- **Accelerometer**:
  - Replace `analogRead(A0)` in the code with the actual **accelerometer readings** from your device (like the MPU6050) to detect hard braking.

---

### Enjoy your custom **smart bike signal system**! 🚲✨
