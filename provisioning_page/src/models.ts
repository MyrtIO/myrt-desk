export type MacAddress = [number, number, number, number, number, number];

export interface SystemInformation {
  build_version: string;
  network_mode: string;
  sta_ip: string;
  ap_ip: string;
  mac_address: MacAddress;
}

export interface OtaUploadResult {
  message: string;
  rebooting: boolean;
}

export interface ApiMessage {
  message: string;
}

export type DeskCalibrationPhase =
  | "idle"
  | "move_to_center"
  | "move_sample"
  | "settling"
  | "complete"
  | "error";

export interface DeskCalibrationStatus {
  running: boolean;
  phase: DeskCalibrationPhase;
  sample_index: number;
  sample_total: number;
  has_calibration: boolean;
  up_correction_mm: number;
  down_correction_mm: number;
  last_error: number;
}

export interface WifiConfiguration {
  ssid: string;
  password: string;
}

export interface MqttConfiguration {
  host: string;
  port: number;
  username: string;
  password: string;
}

export interface LightConfiguration {
  brightness_min: number;
  brightness_max: number;
  color_correction: number;
}

/** Request to test LED color output */
export interface LightTestRequest {
  r: number;
  g: number;
  b: number;
  brightness: number;
}

export interface Configuration {
  wifi: WifiConfiguration;
  mqtt: MqttConfiguration;
  light: LightConfiguration;
}
