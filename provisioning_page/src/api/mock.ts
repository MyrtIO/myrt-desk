import type {
  ApiMessage,
  Configuration,
  DeskCalibrationStatus,
  LightConfiguration,
  LightTestRequest,
  OtaUploadResult,
  SystemInformation,
} from "../models";
import type { ApiService } from "./interface";

export class MockApiService implements ApiService {
  private calibrationStartedAt: number | null = null;

  async getConfiguration(): Promise<Configuration> {
    console.log(`[mock] getting configuration`);
    await simulateNetworkDelay();
    return {
      wifi: {
        ssid: "MySSID",
        password: "My Password",
      },
      mqtt: {
        host: "mqtt-balancer.lan",
        port: 1883,
        username: "myrtio",
        password: "myrtio",
      },
      light: {
        brightness_min: 0,
        brightness_max: 255,
        color_correction: 0xf7e4ff,
      },
    };
  }

  async saveConfiguration(configuration: Configuration): Promise<void> {
    console.log(`[mock] saving configuration`, {
      configuration,
    });
    await simulateNetworkDelay();
    return;
  }

  async setLightConfiguration(light: LightConfiguration): Promise<void> {
    console.log(`[mock] setting light configuration`, { light });
    await simulateNetworkDelay();
    return;
  }

  async testColor(request: LightTestRequest): Promise<void> {
    console.log(`[mock] testing color`, { request });
    await simulateNetworkDelay();
    return;
  }

  async getSystemInformation(): Promise<SystemInformation> {
    console.log(`[mock] getting system information`);
    await simulateNetworkDelay();
    return {
      build_version: "0d35914-2025-12-23T11:16:09+0000",
      network_mode: "AP + STA",
      sta_ip: "192.168.1.34",
      ap_ip: "192.168.4.1",
      mac_address: [160, 183, 101, 22, 48, 84],
    };
  }

  async getDeskCalibrationStatus(): Promise<DeskCalibrationStatus> {
    console.log(`[mock] getting desk calibration status`);
    await simulateNetworkDelay(120, 120);
    return this.getCurrentCalibrationStatus();
  }

  async startDeskCalibration(): Promise<ApiMessage> {
    console.log(`[mock] starting desk calibration`);
    await simulateNetworkDelay(180, 120);

    const status = this.getCurrentCalibrationStatus();
    if (status.running) {
      throw new Error("Калибровка уже выполняется.");
    }

    this.calibrationStartedAt = Date.now();
    return {
      message: "Калибровка стола запущена.",
    };
  }

  async uploadFirmware(
    file: File,
    onProgress?: (progress: number) => void,
  ): Promise<OtaUploadResult> {
    console.log(`[mock] uploading firmware`, {
      file,
    });

    onProgress?.(5);
    await simulateNetworkDelay(200, 200);
    onProgress?.(41);
    await simulateNetworkDelay(250, 250);
    onProgress?.(83);
    await simulateNetworkDelay(200, 200);
    onProgress?.(100);

    return {
      message:
        "Прошивка загружена. Устройство перезагрузится через несколько секунд.",
      rebooting: true,
    };
  }

  private getCurrentCalibrationStatus(): DeskCalibrationStatus {
    if (this.calibrationStartedAt === null) {
      return {
        running: false,
        phase: "idle",
        sample_index: 0,
        sample_total: 8,
        has_calibration: true,
        up_correction_mm: 12,
        down_correction_mm: 11,
        last_error: 0,
      };
    }

    const elapsed = Date.now() - this.calibrationStartedAt;
    const stepDuration = 1100;
    const totalDuration = stepDuration * 10;

    if (elapsed >= totalDuration) {
      this.calibrationStartedAt = null;
      return {
        running: false,
        phase: "complete",
        sample_index: 8,
        sample_total: 8,
        has_calibration: true,
        up_correction_mm: 12,
        down_correction_mm: 12,
        last_error: 1,
      };
    }

    if (elapsed < stepDuration) {
      return {
        running: true,
        phase: "move_to_center",
        sample_index: 0,
        sample_total: 8,
        has_calibration: true,
        up_correction_mm: 12,
        down_correction_mm: 11,
        last_error: 0,
      };
    }

    const sampleIndex = Math.min(
      8,
      Math.floor((elapsed - stepDuration) / stepDuration) + 1,
    );
    const phase =
      elapsed % stepDuration < stepDuration / 2 ? "move_sample" : "settling";

    return {
      running: true,
      phase,
      sample_index: sampleIndex,
      sample_total: 8,
      has_calibration: true,
      up_correction_mm: 12,
      down_correction_mm: 11,
      last_error: sampleIndex % 2 === 0 ? -1 : 1,
    };
  }
}

function simulateNetworkDelay(
  min: number = 500,
  delta: number = 1000,
): Promise<void> {
  let delay = Math.random() * delta + min;
  return sleep(delay);
}

const sleep = (ms: number): Promise<void> =>
  new Promise((resolve) => setTimeout(resolve, ms));
