import { FetchApiService } from "./api/fetch";
import { MockApiService } from "./api/mock";
import { ConfigurationBlock } from "./blocks/configuration";
import { HeaderBlock } from "./blocks/header";
import { SystemBlock } from "./blocks/system";
import "./style.scss";
import { sleep } from "./utils/sleep";

async function main() {
  const configForm = document.querySelector<HTMLFormElement>("#configForm");
  if (!configForm) {
    throw new Error("Config form not found");
  }
  const systemSection =
    document.querySelector<HTMLDivElement>(".system-section");
  if (!systemSection) {
    throw new Error("System section not found");
  }
  const header = document.querySelector<HTMLDivElement>("#header");
  if (!header) {
    throw new Error("Header not found");
  }
  const otaFileInput = document.querySelector<HTMLInputElement>("#ota-file");
  if (!otaFileInput) {
    throw new Error("OTA file input not found");
  }
  const otaSelectButton =
    document.querySelector<HTMLButtonElement>("#button-ota-select");
  if (!otaSelectButton) {
    throw new Error("OTA select button not found");
  }
  const otaUploadButton =
    document.querySelector<HTMLButtonElement>("#button-ota-upload");
  if (!otaUploadButton) {
    throw new Error("OTA upload button not found");
  }
  const otaFileName = document.querySelector<HTMLDivElement>("#ota-file-name");
  if (!otaFileName) {
    throw new Error("OTA file name block not found");
  }
  const otaStatus = document.querySelector<HTMLDivElement>("#ota-status");
  if (!otaStatus) {
    throw new Error("OTA status block not found");
  }

  const api = import.meta.env.VITE_MOCK_API
    ? new MockApiService()
    : new FetchApiService("/api");

  const systemForm = new SystemBlock(systemSection);
  const headerBlock = new HeaderBlock(header, onConfigurationSave);
  const configurationForm = new ConfigurationBlock(configForm, {
    onDirty: () => headerBlock.showSaveButton(),
    onLightChange: (light) => {
      api.setLightConfiguration(light).catch(console.error);
    },
  });

  let otaRebootPending = false;
  let deskCalibrationPollId: number | null = null;
  let deskCalibrationRequestPending = false;

  const setOtaStatus = (
    message: string,
    tone: "default" | "success" | "error" = "default"
  ) => {
    otaStatus.textContent = message;
    otaStatus.classList.toggle("_success", tone === "success");
    otaStatus.classList.toggle("_error", tone === "error");
  };

  const updateOtaControls = (busy: boolean) => {
    otaSelectButton.disabled = busy || otaRebootPending;
    otaUploadButton.disabled =
      busy || otaRebootPending || !(otaFileInput.files && otaFileInput.files[0]);
  };

  const updateSelectedFirmware = () => {
    const file = otaFileInput.files?.[0];
    otaFileName.textContent = file ? file.name : "Файл не выбран";

    if (!file) {
      setOtaStatus(
        "Выберите .bin из .pio/build/<env>/firmware.bin и загрузите его в устройство."
      );
      updateOtaControls(false);
      return;
    }

    setOtaStatus(
      `Готово к загрузке: ${formatFileSize(file.size)}. Обновление запишется прямо во flash.`
    );
    updateOtaControls(false);
  };

  otaSelectButton.addEventListener("click", () => {
    otaFileInput.click();
  });

  otaFileInput.addEventListener("change", () => {
    updateSelectedFirmware();
  });

  otaUploadButton.addEventListener("click", async () => {
    const file = otaFileInput.files?.[0];
    if (!file) {
      setOtaStatus("Сначала выберите файл прошивки.", "error");
      updateOtaControls(false);
      return;
    }

    updateOtaControls(true);
    setOtaStatus("Загрузка прошивки: 0%");

    try {
      const result = await api.uploadFirmware(file, (progress) => {
        setOtaStatus(`Загрузка прошивки: ${progress}%`);
      });

      otaRebootPending = result.rebooting;
      setOtaStatus(result.message, "success");
      updateOtaControls(true);
    } catch (error) {
      const message =
        error instanceof Error
          ? error.message
          : "Загрузка прошивки завершилась с ошибкой.";
      setOtaStatus(message, "error");
      updateOtaControls(false);
    }
  });

  const stopDeskCalibrationPolling = () => {
    if (deskCalibrationPollId !== null) {
      window.clearInterval(deskCalibrationPollId);
      deskCalibrationPollId = null;
    }
  };

  const refreshDeskCalibrationStatus = async () => {
    if (deskCalibrationRequestPending) {
      return;
    }

    deskCalibrationRequestPending = true;
    try {
      const status = await api.getDeskCalibrationStatus();
      systemForm.setDeskCalibrationStatus(status);

      if (!status.running) {
        stopDeskCalibrationPolling();
      }
    } catch (error) {
      const message =
        error instanceof Error
          ? error.message
          : "Не удалось получить статус калибровки.";
      systemForm.showDeskCalibrationMessage(message, "error");
      systemForm.setDeskCalibrationBusy(false);
      stopDeskCalibrationPolling();
    } finally {
      deskCalibrationRequestPending = false;
    }
  };

  const startDeskCalibrationPolling = () => {
    if (deskCalibrationPollId !== null) {
      return;
    }

    deskCalibrationPollId = window.setInterval(() => {
      void refreshDeskCalibrationStatus();
    }, 1200);
  };

  systemForm.onDeskCalibrationStart(() => {
    systemForm.setDeskCalibrationBusy(true);
    systemForm.showDeskCalibrationMessage("Запуск калибровки...", "active");

    void api
      .startDeskCalibration()
      .then((result) => {
        systemForm.showDeskCalibrationMessage(result.message, "active");
        startDeskCalibrationPolling();
        return refreshDeskCalibrationStatus();
      })
      .catch((error) => {
        const message =
          error instanceof Error
            ? error.message
            : "Не удалось запустить калибровку.";
        systemForm.showDeskCalibrationMessage(message, "error");
        systemForm.setDeskCalibrationBusy(false);
      });
  });

  // Wire up test color buttons
  const testRed = document.getElementById("test-red");
  const testGreen = document.getElementById("test-green");
  const testBlue = document.getElementById("test-blue");
  const testWhite = document.getElementById("test-white");

  const testColor = (r: number, g: number, b: number, brightness: number) => {
    api.testColor({ r, g, b, brightness }).catch(console.error);
  };

  const testBrightness = 128;

  testRed?.addEventListener("click", () =>
    testColor(255, 0, 0, testBrightness)
  );
  testGreen?.addEventListener("click", () =>
    testColor(0, 255, 0, testBrightness)
  );
  testBlue?.addEventListener("click", () =>
    testColor(0, 0, 255, testBrightness)
  );
  testWhite?.addEventListener("click", () =>
    testColor(255, 255, 255, testBrightness)
  );

  async function onConfigurationSave(e: Event) {
    e.preventDefault();
    if (!configurationForm.validate()) return;
    const values = configurationForm.getValues();
    headerBlock.showLoader();
    headerBlock.hideSaveButton();
    try {
      await api.saveConfiguration(values);
    } catch (error) {
      console.error(error);
    }
    headerBlock.hideLoader();
    configurationForm.markClean();
  }

  // Load initial state
  await sleep(100);
  const configuration = await api.getConfiguration();
  const system = await api.getSystemInformation();
  const deskCalibration = await api.getDeskCalibrationStatus();

  configurationForm.setValues(configuration);
  systemForm.setValues(system);
  systemForm.setDeskCalibrationStatus(deskCalibration);
  if (deskCalibration.running) {
    startDeskCalibrationPolling();
  }
  updateSelectedFirmware();

  configurationForm.unlock();
  systemForm.unlock();

  headerBlock.hideLoader();
}

function formatFileSize(size: number): string {
  if (size < 1024 * 1024) {
    return `${(size / 1024).toFixed(1)} KB`;
  }

  return `${(size / (1024 * 1024)).toFixed(2)} MB`;
}

main().catch(console.error);
