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
import { Mutex } from "../utils/mutex";
import { sleep } from "../utils/sleep";

export class FetchApiService implements ApiService {
  private readonly baseUrl: string;
  private readonly mutex = new Mutex();

  constructor(baseUrl: string) {
    this.baseUrl = baseUrl.endsWith("/") ? baseUrl.slice(0, -1) : baseUrl;
  }

  async getConfiguration(): Promise<Configuration> {
    return this.fetchGetJson<Configuration>("/configuration");
  }

  async saveConfiguration(configuration: Configuration): Promise<void> {
    return this.fetchPostJson("/configuration", configuration);
  }

  async setLightConfiguration(light: LightConfiguration): Promise<void> {
    return this.fetchPostJson("/configuration/light", light);
  }

  async testColor(request: LightTestRequest): Promise<void> {
    return this.fetchPostJson("/light/test", request);
  }

  async getSystemInformation(): Promise<SystemInformation> {
    return this.fetchGetJson<SystemInformation>("/system");
  }

  async getDeskCalibrationStatus(): Promise<DeskCalibrationStatus> {
    return this.fetchGetJson<DeskCalibrationStatus>("/desk/calibration");
  }

  async startDeskCalibration(): Promise<ApiMessage> {
    return this.fetchPostJson<ApiMessage>("/desk/calibration");
  }

  async uploadFirmware(
    file: File,
    onProgress?: (progress: number) => void
  ): Promise<OtaUploadResult> {
    return this.withLock(
      () =>
        new Promise<OtaUploadResult>((resolve, reject) => {
          const xhr = new XMLHttpRequest();
          xhr.open("POST", `${this.baseUrl}/ota`);

          xhr.upload.onprogress = (event) => {
            if (!event.lengthComputable || !onProgress) {
              return;
            }

            onProgress(Math.round((event.loaded / event.total) * 100));
          };

          xhr.onerror = () => {
            reject(new Error("Не удалось загрузить прошивку."));
          };

          xhr.onabort = () => {
            reject(new Error("Загрузка прошивки прервана."));
          };

          xhr.onload = () => {
            const response = this.parseJsonResponse<
              OtaUploadResult & { error?: string }
            >(xhr.responseText);

            if (xhr.status >= 200 && xhr.status < 300) {
              onProgress?.(100);
              resolve(
                response ?? {
                  message:
                    "Прошивка загружена. Устройство перезагрузится через несколько секунд.",
                  rebooting: true,
                }
              );
              return;
            }

            reject(
              new Error(
                response?.error ||
                  xhr.statusText ||
                  "Загрузка прошивки завершилась с ошибкой."
              )
            );
          };

          const formData = new FormData();
          formData.append("firmware", file, file.name);
          xhr.send(formData);
        })
    );
  }

  private async fetchGetJson<T>(url: string): Promise<T> {
    return this.withLock(async () => {
      const response = await fetch(`${this.baseUrl}${url}`);
      if (!response.ok) {
        throw await this.createHttpError(
          response,
          `Failed to fetch ${url}: ${response.statusText}`
        );
      }

      const raw = await response.text();
      const parsed = this.parseJsonResponse<T>(raw);
      if (parsed === null) {
        throw new Error(`Invalid JSON response from ${url}`);
      }

      return parsed;
    });
  }

  private async fetchPostJson<T = void>(
    url: string,
    body?: unknown
  ): Promise<T> {
    return this.withLock(async () => {
      const init: RequestInit = {
        method: "POST",
      };

      if (body !== undefined) {
        init.headers = {
          "Content-Type": "application/json",
        };
        init.body = JSON.stringify(body);
      }

      const response = await fetch(`${this.baseUrl}${url}`, init);
      if (!response.ok) {
        throw await this.createHttpError(
          response,
          `Failed to post to ${url}: ${response.statusText}`
        );
      }

      const raw = await response.text();
      const parsed = this.parseJsonResponse<T>(raw);
      return (parsed ?? (undefined as T));
    });
  }

  private async withLock<T>(fn: () => Promise<T>): Promise<T> {
    const unlock = await this.mutex.lock();
    try {
      const result = await fn();
      await sleep(100);
      return result;
    } finally {
      unlock();
    }
  }

  private parseJsonResponse<T>(raw: string): T | null {
    if (!raw) {
      return null;
    }

    try {
      return JSON.parse(raw) as T;
    } catch {
      return null;
    }
  }

  private async createHttpError(
    response: Response,
    fallbackMessage: string
  ): Promise<Error> {
    const raw = await response.text();
    const parsed = this.parseJsonResponse<{ error?: string }>(raw);
    const message = parsed?.error || raw || fallbackMessage;
    return new Error(message, {
      cause: response.statusText,
    });
  }
}
