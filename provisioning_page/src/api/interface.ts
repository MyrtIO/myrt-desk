import type {
  ApiMessage,
  Configuration,
  DeskCalibrationStatus,
  LightConfiguration,
  LightTestRequest,
  OtaUploadResult,
  SystemInformation,
} from "../models";

export interface ApiService {
  getConfiguration(): Promise<Configuration>;
  saveConfiguration(configuration: Configuration): Promise<void>;
  setLightConfiguration(light: LightConfiguration): Promise<void>;
  testColor(request: LightTestRequest): Promise<void>;
  getSystemInformation(): Promise<SystemInformation>;
  getDeskCalibrationStatus(): Promise<DeskCalibrationStatus>;
  startDeskCalibration(): Promise<ApiMessage>;
  uploadFirmware(
    file: File,
    onProgress?: (progress: number) => void
  ): Promise<OtaUploadResult>;
}
