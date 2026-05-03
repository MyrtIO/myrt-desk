import type {
  DeskCalibrationStatus,
  MacAddress,
  SystemInformation,
} from "../models";

type DeskCalibrationTone = "default" | "active" | "success" | "error";

export class SystemBlock {
  private buildVersion: HTMLDivElement;
  private networkMode: HTMLDivElement;
  private staIp: HTMLDivElement;
  private apIp: HTMLDivElement;
  private macAddress: HTMLDivElement;
  private deskCalibrationButton: HTMLButtonElement;
  private deskCalibrationStatus: HTMLDivElement;
  private block: HTMLElement;

  constructor(section: HTMLElement) {
    const $ = <T extends HTMLElement = HTMLDivElement>(selector: string): T => {
      const element = section.querySelector<T>(selector);
      if (!element) {
        throw new Error(`Element with selector ${selector} not found`);
      }
      return element;
    };

    this.block = section;
    this.buildVersion = $("#build-version");
    this.networkMode = $("#network-mode");
    this.staIp = $("#sta-ip");
    this.apIp = $("#ap-ip");
    this.macAddress = $("#mac-address");
    this.deskCalibrationButton =
      $<HTMLButtonElement>("#button-desk-calibration");
    this.deskCalibrationStatus = $("#desk-calibration-status");
  }

  public setValues(system: SystemInformation) {
    this.buildVersion.textContent = system.build_version;
    this.networkMode.textContent = system.network_mode;
    this.staIp.textContent = system.sta_ip || "нет подключения";
    this.apIp.textContent = system.ap_ip || "выключен";
    this.macAddress.textContent = formatMacAddress(system.mac_address);
  }

  public unlock() {
    this.block.classList.remove("_disabled");
  }

  public lock() {
    this.block.classList.add("_disabled");
  }

  public onDeskCalibrationStart(callback: () => void) {
    this.deskCalibrationButton.addEventListener("click", callback);
  }

  public setDeskCalibrationBusy(busy: boolean) {
    this.deskCalibrationButton.disabled = busy;
    this.deskCalibrationButton.textContent = busy
      ? "Калибровка..."
      : "Запустить";
  }

  public showDeskCalibrationMessage(
    message: string,
    tone: DeskCalibrationTone = "default"
  ) {
    this.deskCalibrationStatus.textContent = message;
    this.deskCalibrationStatus.classList.toggle("_active", tone === "active");
    this.deskCalibrationStatus.classList.toggle("_success", tone === "success");
    this.deskCalibrationStatus.classList.toggle("_error", tone === "error");
  }

  public setDeskCalibrationStatus(status: DeskCalibrationStatus) {
    this.setDeskCalibrationBusy(status.running);

    const presentation = formatDeskCalibrationStatus(status);
    this.showDeskCalibrationMessage(presentation.message, presentation.tone);
  }
}

function formatMacAddress(macAddress: MacAddress): string {
  return macAddress.map((byte) => byte.toString(16).padStart(2, "0")).join(":");
}

function formatDeskCalibrationStatus(status: DeskCalibrationStatus): {
  message: string;
  tone: DeskCalibrationTone;
} {
  switch (status.phase) {
    case "move_to_center":
      return {
        message: "Переход на базовую высоту 800 мм.",
        tone: "active",
      };
    case "move_sample":
      return {
        message: `Шаг ${status.sample_index} из ${status.sample_total}: измерение дельты.`,
        tone: "active",
      };
    case "settling":
      return {
        message:
          status.sample_index > 0
            ? `Шаг ${status.sample_index} из ${status.sample_total}: ждём полной остановки.`
            : "Ждём полной остановки на высоте 800 мм.",
        tone: "active",
      };
    case "complete":
      return {
        message: `Калибровка сохранена: вверх ${status.up_correction_mm} мм, вниз ${status.down_correction_mm} мм.`,
        tone: "success",
      };
    case "error":
      return {
        message: [
          "Калибровка прервана.",
          status.last_error !== 0
            ? `Последняя дельта: ${formatSignedMillimeters(status.last_error)}.`
            : "",
          status.has_calibration
            ? `Активна прежняя коррекция: вверх ${status.up_correction_mm} мм, вниз ${status.down_correction_mm} мм.`
            : "Коррекция не сохранена.",
        ]
          .filter(Boolean)
          .join(" "),
        tone: "error",
      };
    case "idle":
    default:
      if (!status.has_calibration) {
        return {
          message: "Калибровка ещё не запускалась.",
          tone: "default",
        };
      }

      return {
        message: `Сохранённая коррекция: вверх ${status.up_correction_mm} мм, вниз ${status.down_correction_mm} мм.`,
        tone: "default",
      };
  }
}

function formatSignedMillimeters(value: number): string {
  return `${value > 0 ? "+" : ""}${value} мм`;
}
